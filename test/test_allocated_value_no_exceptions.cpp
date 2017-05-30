
#include <tcb/allocated_value.hpp>

// Force assertions even in release mode
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>

namespace {

void test_construct()
{
    const auto a = tcb::allocated_value<int>{3};
    assert(*a == 3);
}

void test_copy_construct()
{
    const auto a = tcb::allocated_value<int>{3};
    const auto b = a;
    assert(*b == 3);
}

void test_move_construct()
{
    auto a = tcb::allocated_value<int>{3};
    const auto b = std::move(a);
    assert(*b == 3);
}

void test_copy_assign()
{
    const auto a = tcb::allocated_value<int>{3};
    auto b = tcb::allocated_value<int>{4};

    b = a;
    assert(*b == 3);
}

void test_copy_assign_from_value()
{
    auto a = tcb::allocated_value<int>{3};
    int i = 4;
    a = i;

    assert(*a == 4);
}

void test_move_assign()
{
    auto a = tcb::allocated_value<int>{3};
    auto b = tcb::allocated_value<int>{4};

    b = std::move(a);
    assert(*b == 3);
}

void test_move_assign_from_value()
{
    auto a = tcb::allocated_value<int>{3};
    a = 4;

    assert(*a == 4);
}

void test_member_swap()
{
    auto a = tcb::allocated_value<int>{3};
    auto b = tcb::allocated_value<int>{4};

    a.swap(b);
    assert(*a == 4);
    assert(*b == 3);
}

void test_nonmember_swap()
{
    auto a = tcb::allocated_value<int>{3};
    auto b = tcb::allocated_value<int>{4};

    swap(a, b);
    assert(*a == 4);
    assert(*b == 3);
}

void test_comparisons()
{
    constexpr auto i = 3;
    constexpr auto j = i + 1;
    const auto a = tcb::allocated_value<int>{i};
    const auto b = tcb::allocated_value<int>{j};

    // Allocated <-> allocated comparisons
    assert(a == a);
    assert(a != b);
    assert(a < b);
    assert(a <= a);
    assert(b > a);
    assert(a >= a);

    // Allocated <-> T comparisons
    assert(a == i);
    assert(a != j);
    assert(a < j);
    assert(a <= i);
    assert(b > i);
    assert(b >= j);

    // T <-> allocated comparisons
    assert(i == a);
    assert(i != b);
    assert(i < b);
    assert(i <= a);
    assert(j > a);
    assert(i >= a);
}

}

int main()
{
    test_construct();
    test_copy_construct();
    test_move_construct();
    test_copy_assign();
    test_copy_assign_from_value();
    test_move_assign();
    test_move_assign_from_value();
    test_member_swap();
    test_nonmember_swap();
    test_comparisons();
}