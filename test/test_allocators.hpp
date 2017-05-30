
#pragma once

#include <cstddef>
#include <memory>
#include <utility>

struct allocator_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// As the name suggests, this allocator will always throw in its
// allocate method
template <typename T>
struct throw_on_allocate_allocator : std::allocator<T> {
    using std::allocator<T>::allocator;

    T* allocate(std::size_t)
    {
        throw allocator_error{"throw_on_allocate_allocator"};
    }
};

template <typename T>
struct throw_on_construct_allocator : std::allocator<T> {
    using std::allocator<T>::allocator;

    template <typename... Args>
    void construct(T*, Args&&...)
    {
        throw allocator_error{"throw_on_construct_allocator"};
    }
};

template <typename T>
struct pocca_allocator : std::allocator<T>
{
    using std::allocator<T>::allocator;
    using propagate_on_container_copy_assignment = std::true_type;
};

template <typename T>
struct non_pocma_allocator : std::allocator<T>
{
    using std::allocator<T>::allocator;
    using propagate_on_container_move_assignment = std::false_type;
};

template <typename T>
struct pocs_allocator : std::allocator<T>
{
    using std::allocator<T>::allocator;
    using propagate_on_container_swap = std::true_type;
};

template <typename T>
struct never_equal_allocator : std::allocator<T>
{
    using std::allocator<T>::allocator;
};

template <typename T>
bool operator==(never_equal_allocator<T>, never_equal_allocator<T>)
{
    return false;
}

template <typename T>
bool operator!=(never_equal_allocator<T>, never_equal_allocator<T>)
{
    return true;
}