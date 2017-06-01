
#ifndef TCB_ALLOCATED_HPP_INCLUDED
#define TCB_ALLOCATED_HPP_INCLUDED

#include <memory>
#include <type_traits>

namespace tcb {

#ifndef TCB_ALLOCATED_VALUE_NO_EXCEPTIONS
# ifndef __cpp_exceptions
#   define TCB_ALLOCATED_VALUE_NO_EXCEPTIONS
# endif
#endif

#ifdef TCB_ALLOCATED_VALUE_NO_EXCEPTIONS
#define TRY
#define CATCH(X) if (false)
#define THROW
#else
#define TRY try
#define CATCH catch
#define THROW throw
#endif

template <typename T, typename A>
class allocated_value;

template <typename T>
struct is_allocated_value : std::false_type {};

template <typename T, typename A>
struct is_allocated_value<allocated_value<T, A>> : std::true_type {};

#if defined(__cpp_variable_templates) && (__cpp_variable_templates >= 201304)
template <typename T>
constexpr bool is_allocated_value_v = is_allocated_value<T>::value;
#endif

struct in_place_t { explicit in_place_t() = default; };

constexpr in_place_t in_place{};

namespace detail {

template <typename T>
struct ebo_store : private T {
    ebo_store() = default;
    ebo_store(const T& t) : T(t) {}
    ebo_store(T&& t) : T{std::move(t)} {}

    T& get_ebo_value() { return *this; }
    const T& get_ebo_value() const { return *this; }
};

}

template <typename T, typename Alloc = std::allocator<T>>
class allocated_value : private detail::ebo_store<Alloc> {

    using traits = std::allocator_traits<Alloc>;
    using ebo_base = detail::ebo_store<Alloc>;

    using is_pocca_t = typename traits::propagate_on_container_copy_assignment;
    using is_pocma_t = typename traits::propagate_on_container_move_assignment;
    using is_pocs_t = typename traits::propagate_on_container_swap;

    template <typename...> using void_t = void;

    template <typename A, typename = void>
    struct always_equal_helper : std::false_type {};
    template <typename A>
    struct always_equal_helper<A, void_t<typename std::allocator_traits<A>::is_always_equal>>
        : traits::is_always_equal
    {};

    static constexpr bool is_always_equal_v = always_equal_helper<Alloc>::value;

public:
    using value_type = T;
    using allocator_type = Alloc;
    using pointer = typename traits::pointer;
    using const_pointer = typename traits::const_pointer;
    using reference = value_type&;
    using const_reference = const value_type&;

    static_assert(!std::is_reference<value_type>::value,
        "An allocated_value cannot be used to store reference types.\n"
        "Use allocated_value<std::reference_wrapper<T>>."
    );

    /**
     * Default constructor.
     *
     * Constructs an allocated_value holding a default-constructed value_type.
     *
     * This constructor is available only if both the value_type and
     * allocator_type are DefaultConstructible.
     */
    template <typename V = value_type, typename A = allocator_type,
              typename = typename std::enable_if<
                      std::is_default_constructible<V>::value &&
                      std::is_default_constructible<A>::value>::type>
    explicit allocated_value()
    {
        do_construct();
    }

    /**
     * Allocator constructor.
     *
     * Constructs an allocated_value<T> holding a default-constructed T, using
     * the supplied allocator.
     *
     * This constructor is available only if the value_type is DefaultConstructible.
     */
    template <typename V = value_type,
              typename = typename std::enable_if<
                    std::is_default_constructible<V>::value>::type>
    explicit allocated_value(const allocator_type& allocator)
        : ebo_base{allocator}
    {
        do_construct();
    }

    /**
     * Converting constructor.
     *
     * Constructs an allocated_value holding a copy of value.
     *
     * This constructor is available only if the allocator_type is DefaultConstructible.
     */
    template <typename A = allocator_type,
              typename = typename std::enable_if<
                    std::is_default_constructible<A>::value>::type>
    explicit allocated_value(const value_type& value)
    {
        do_construct(value);
    }

    /**
     * Converting constructor.
     *
     * Constructs an allocated_value<T>, with the contents of value moved into it.
     *
     * This constructor is available only if the allocator_type is DefaultConstructible.
     */
    template <typename A = allocator_type,
              typename = typename std::enable_if<
                    std::is_default_constructible<A>::value>::type>
    explicit allocated_value(value_type&& value)
    {
        do_construct(std::move(value));
    }

    /**
     * Converting constructor.
     *
     * Constructs an allocated_value holding a copy of value, using the
     * supplied allocator.
     */
    allocated_value(const value_type& value, const allocator_type& allocator)
        : ebo_base{allocator}
    {
        do_construct(value);
    }

    /**
     * Converting constructor.
     *
     * Constructs an allocated_value holding the moved value, using the
     * supplied allocator.
     */
    allocated_value(value_type&& value, const allocator_type& allocator)
        : ebo_base{allocator}
    {
        do_construct(std::move(value));
    }

    /**
     * In-place constructor
     *
     * Constructs an allocated_value<T> with T constructed in-place from the
     * given arguments.
     *
     * This constructor is available only if the value_type is construcible
     * from the given arguments, and the allocator_type is DefaultConstructible.
     */
    template <typename... Args, typename A = allocator_type,
              typename = typename std::enable_if<
                    std::is_constructible<T, Args...>::value &&
                    std::is_default_constructible<A>::value>::type>
    explicit allocated_value(in_place_t, Args&&... args)
    {
        do_construct(std::forward<Args>(args)...);
    }

    /**
     * In-place constructor.
     *
     * Constructs an allocated_value<T> with T constructed in-place from the
     * given arguments, using the supplied allocator.
     *
     * This constructor is available only if the value_type is constructible
     * from the given arguments.
     */
    template <typename... Args,
              typename = typename std::enable_if<
                    std::is_constructible<T, Args...>::value>::type>
    allocated_value(std::allocator_arg_t, const allocator_type& allocator,
                    in_place_t, Args&&... args)
        : ebo_base{allocator}
    {
        do_construct(std::forward<Args>(args)...);
    }

    /**
     * Copy Constructor.
     *
     * Constructs a new allocated_value holding a copy of other's value.
     *
     * The new allocator is selected by
     *
     * std::allocator_traits::select_on_copy_construction(other.get_allocator())
     */
    allocated_value(const allocated_value& other)
        : allocated_value(other,
                          traits::select_on_container_copy_construction(other.get_allocator()))
    {}

    /**
     * Copy constructor.
     *
     * Constructs a new allocated_value holding a copy of other's value, using
     * the supplied allocator.
     */
    allocated_value(const allocated_value& other, const allocator_type& allocator)
            : ebo_base{allocator}
    {
        do_construct(other.get());
    }

    /**
     * Move constructor.
     *
     * Constructs a new allocated_value holding the contents of other. Performs
     * no allocations.
     *
     * After using this constructor, other is in an invalid state, and can only
     * be assigned to or destructed.
     *
     * This constructor will not throw.
     */
    allocated_value(allocated_value&& other) noexcept
        : ebo_base(std::move(other)),
          ptr(std::move(other.ptr))
    {
        other.ptr = nullptr;
    }

    /**
     * Move constructor.
     *
     * Move-constructs a new allocated_value with the contents of other,
     * using the supplied allocator.
     *
     * If the supplied allocator compares equal to other.get_allocator(), then
     * no allocation will be performed and other will be in a moved-from state.
     *
     * Otherwise, this is equivalent to calling
     *
     * allocated_value(std::move(other.get(), allocator),
     *
     * that is, an allocation will be performed, and other's *contained value*
     * will be in a moved-from state.
     *
     * This constructor will not throw if the supplied allocator compares equal
     * to other.get_allocator().
     */
    allocated_value(allocated_value&& other, const allocator_type& allocator)
        noexcept(is_always_equal_v)
        : ebo_base{allocator}
    {
        // If the provided allocator equals other's allocator, we can just
        // steal its contents
        if (get_allocator() == other.get_allocator()) {
            ptr = std::move(other.ptr);
            other.ptr = nullptr;
        } else {  // Otherwise we have to allocate and then move-construct the value
            do_construct(std::move(other.get()));
        }
    }

    /**
     * Copy-assignment operator.
     *
     * @post: get() == other.get().
     *
     * If an exception is thrown during allocation or assignment, *this is unchanged.
     */
    allocated_value& operator=(const allocated_value& other)
        noexcept(!is_pocca_t::value && std::is_nothrow_copy_assignable<value_type>::value)
    {
        if (this != std::addressof(other)) {
            do_copy_assign(is_pocca_t{}, other);
        }
        return *this;
    }

    /**
     * Copy-assignment from value.
     *
     * @post: get() == value.
     *
     * If an exception is thrown during assignment, *this is unchanged.
     */
    allocated_value& operator=(const value_type& value)
        noexcept(std::is_nothrow_copy_assignable<value_type>::value &&
                 std::is_nothrow_move_constructible<value_type>::value)
    {
        value_type temp{std::move(get())};
        TRY {
            get() = value;
        } CATCH (...) {
            get() = std::move(temp);
            THROW;
        }
        return *this;
    }

    // Move assignment
    allocated_value& operator=(allocated_value&& other)
        noexcept(is_pocma_t::value || is_always_equal_v)
    {
        do_move_assign(is_pocma_t{}, std::move(other));
        return *this;
    }

    allocated_value& operator=(value_type&& value)
        noexcept(std::is_nothrow_move_assignable<value_type>::value)
    {
        get() = std::move(value);
        return *this;
    }

    /**
     * Destructor.
     *
     * Releases all resources held by this object.
     */
    ~allocated_value()
    {
        noexcept_release();
    }

    /**
     * Swaps the contained values of *this and other.
     */
    void swap(allocated_value& other) noexcept
    {
        do_swap(is_pocs_t{}, other);
    }

    /**
     * Replaces the contents of *this with a new value constructed
     * in-place from the given arguments.
     *
     * This function is available only if value_type is constructible from Args.
     */
    template <typename... Args,
              typename = typename
                  std::enable_if<std::is_constructible<T, Args...>::value>::type>
    void emplace(Args&&... args)
    {
        // If constructing the new value fails, move the old value back in place.
        auto temp = std::move(get());
        auto a = get_allocator();
        TRY {
            traits::destroy(a, ptr);
            traits::construct(a, ptr, std::forward<Args>(args)...);
        } CATCH(...) {
            get() = std::move(temp);
        }

    }

    /// Access the contained value.
    reference get() noexcept { return *ptr; }
    /// @overload
    const_reference get() const noexcept { return *ptr; }

    /// Returns a copy of the contained allocator.
    allocator_type get_allocator() const noexcept { return as_allocator(); }

    /// Returns get().
    reference operator*() noexcept { return get(); }
    /// @overload
    const_reference operator*() const noexcept { return get(); }

    /// Member access.
    pointer operator->() noexcept { return ptr; }
    /// @overload
    const_pointer operator->() const noexcept { return ptr; }

private:
    template <typename... Args>
    void do_construct(Args&&... args)
    {
        auto a = get_allocator();
        ptr = traits::allocate(a, 1);
        TRY {
            traits::construct(a, ptr, std::forward<Args>(args)...);
        } CATCH (...) {
            traits::deallocate(a, ptr, 1);
            THROW;
        }
    }

    void do_copy_assign(std::true_type /*is_pocca*/, const allocated_value& other)
    {
        // Keep a copy of our old contents around for safekeeping
        auto temp = std::move(*this);
        ptr = nullptr;
        // We are now empty, so copy-assign the allocator, allocate, then
        // (try to) copy-assign the value.
        as_allocator() = other.get_allocator();
        auto a = get_allocator();
        TRY {
            ptr = traits::allocate(a, 1);
            TRY {
                get() = other.get();
            } CATCH(...) {
                traits::deallocate(a, ptr, 1);
                ptr = nullptr;
                THROW;
            }
        } CATCH (...) {
            // Either allocate or copy-assign has failed, so restore our old
            // allocator and value then re-throw
            as_allocator() = std::move(temp.as_allocator());
            ptr = std::move(temp.ptr);
            temp.ptr = nullptr;
            THROW;
        }
    }

    void do_copy_assign(std::false_type /*is_pocca*/, const allocated_value& other)
        noexcept(std::is_nothrow_copy_assignable<value_type>::value)
    {
        get() = other.get();
    }

    void do_move_assign(std::true_type /*is_pocma*/, allocated_value&& other) noexcept
    {
        // Destroy what we have, then steal the contents of other
        noexcept_release();
        this->as_allocator() = std::move(other.as_allocator());
        this->ptr = other.ptr;
        other.ptr = nullptr;
    }

    void do_move_assign(std::false_type /*is_pocma*/, allocated_value&& other)
        noexcept(is_always_equal_v)
    {
        if (get_allocator() == other.get_allocator()) {
            // Destroy what we have, then steal the allocated pointer from other
            noexcept_release();
            ptr = std::move(other.ptr);
            other.ptr = nullptr;
        } else {
            // Move-assign our value from other
            get() = std::move(other.get());
        }
    }

    void do_swap(std::true_type /*is_pocs*/, allocated_value& other) noexcept
    {
        // Need to swap both allocator and pointer
        using std::swap;
        swap(as_allocator(), other.as_allocator());
        swap(ptr, other.ptr);
    }

    void do_swap(std::false_type /*is_pocs*/, allocated_value& other) noexcept
    {
        // Only need to swap pointer values
        using std::swap;
        swap(ptr, other.ptr);
    }

    void noexcept_release() noexcept
    {
        // This is horrible. We call destroy then deallocate,
        // swallowing all exceptions that may occur
        auto a = get_allocator();
        if (ptr) {
            TRY {
                traits::destroy(a, ptr);
            } CATCH (...) {}
            TRY {
                traits::deallocate(a, ptr, 1);
            } CATCH (...) {}
        }
    }

    allocator_type& as_allocator() { return this->get_ebo_value(); }
    const allocator_type& as_allocator() const { return this->get_ebo_value(); }

    pointer ptr = nullptr;
};

template <typename T, typename Alloc = std::allocator<T>, typename... Args>
allocated_value<T, Alloc>
make_allocated_value(Args&&... args)
{
    return allocated_value<T, Alloc>(in_place, std::forward<Args>(args)...);
}

/* FIXME: The naming is consistent with allocate_shared(), but... urgh. */
template <typename T, typename Alloc, typename... Args>
allocated_value<T, Alloc>
allocate_allocated_value(const Alloc& allocator, Args&&... args)
{
    return allocated_value<T, Alloc>(std::allocator_arg, allocator,
                                     in_place, std::forward<Args>(args)...);
}

// Non-member swap
template <typename T, typename A>
void swap(allocated_value<T, A>& first, allocated_value<T, A>& second)
{
    first.swap(second);
}

// Comparison between two allocated_values (possibly with different allocators)
template <typename T, typename A, typename B>
bool operator==(const allocated_value<T, A>& lhs, const allocated_value<T, B>& rhs)
{
    return lhs.get() == rhs.get();
}

template <typename T, typename A, typename B>
bool operator!=(const allocated_value<T, A>& lhs, const allocated_value<T, B>& rhs)
{
    return lhs.get() != rhs.get();
}

template <typename T, typename A, typename B>
bool operator<(const allocated_value<T, A>& lhs, const allocated_value<T, B>& rhs)
{
    return lhs.get() < rhs.get();
}

template <typename T, typename A, typename B>
bool operator<=(const allocated_value<T, A>& lhs, const allocated_value<T, B>& rhs)
{
    return lhs.get() <= rhs.get();
}

template <typename T, typename A, typename B>
bool operator>(const allocated_value<T, A>& lhs, const allocated_value<T, B>& rhs)
{
    return lhs.get() > rhs.get();
}

template <typename T, typename A, typename B>
bool operator>=(const allocated_value<T, A>& lhs, const allocated_value<T, B>& rhs)
{
    return lhs.get() >= rhs.get();
}

// Comparison between T and allocated_value<T>
template <typename T, typename A>
bool operator==(const T& lhs, const allocated_value<T, A>& rhs)
{
    return lhs == rhs.get();
}

template <typename T, typename A>
bool operator!=(const T& lhs, const allocated_value<T, A>& rhs)
{
    return lhs != rhs.get();
}

template <typename T, typename A>
bool operator<(const T& lhs, const allocated_value<T, A>& rhs)
{
    return lhs < rhs.get();
}

template <typename T, typename A>
bool operator<=(const T& lhs, const allocated_value<T, A>& rhs)
{
    return lhs <= rhs.get();
}

template <typename T, typename A>
bool operator>(const T& lhs, const allocated_value<T, A>& rhs)
{
    return lhs > rhs.get();
}

template <typename T, typename A>
bool operator>=(const T& lhs, const allocated_value<T, A>& rhs)
{
    return lhs >= rhs.get();
}

// Comparison between allocated_value<T> and T

template <typename T, typename A>
bool operator==(const allocated_value<T, A>& lhs, const T& rhs)
{
    return lhs.get() == rhs;
}

template <typename T, typename A>
bool operator!=(const allocated_value<T, A>& lhs, const T& rhs)
{
    return lhs.get() != rhs;
}

template <typename T, typename A>
bool operator<(const allocated_value<T, A>& lhs, const T& rhs)
{
    return lhs.get() < rhs;
}

template <typename T, typename A>
bool operator<=(const allocated_value<T, A>& lhs, const T& rhs)
{
    return lhs.get() <= rhs;
}

template <typename T, typename A>
bool operator>(const allocated_value<T, A>& lhs, const T& rhs)
{
    return lhs.get() > rhs;
}

template <typename T, typename A>
bool operator>=(const allocated_value<T, A>& lhs, const T& rhs)
{
    return lhs.get() >= rhs;
}

#undef TRY
#undef CATCH
#undef THROW

} // namespace tcb

#endif
