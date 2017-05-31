
#include <tcb/allocated_value.hpp>

#include "catch.hpp"
#include "test_types.hpp"

using tcb::allocated_value;

/*
 * A few compile-time tests
 */
static_assert(sizeof(allocated_value<int>) == sizeof(int*), "");
static_assert(std::uses_allocator<allocated_value<int>, std::allocator<int>>::value, "");
static_assert(std::is_constructible<allocated_value<int>, int>::value, "");
static_assert(!std::is_constructible<allocated_value<int>, std::nullptr_t>::value, "");
static_assert(std::is_nothrow_move_constructible<allocated_value<int>>::value, "");
static_assert(std::is_nothrow_move_assignable<allocated_value<int>>::value, "");

/*
 * Basic tests
 *
 * These functions test the basic functionality, using std::allocator. No
 * tricks or funny business here.
 */


TEST_CASE("Default construction", "[basic]")
{
    const auto a = allocated_value<test_struct>{};
    REQUIRE(*a == test_struct{});
}

TEST_CASE("Default construction with allocator", "[basic]")
{
    const auto a = allocated_value<test_struct>{std::allocator<test_struct>{}};
    REQUIRE(*a == test_struct{});
}

TEST_CASE("Basic construction from lvalue", "[basic]")
{
    const test_struct t{"1", 2};
    const auto a = allocated_value<test_struct>(t);
    REQUIRE(*a == t);
}

TEST_CASE("Basic construction from lvalue with allocator", "[basic]")
{
    const test_struct t{"1", 2};
    const auto a = allocated_value<test_struct>(t, std::allocator<test_struct>{});
    REQUIRE(*a == t);
}

TEST_CASE("Basic construction from rvalue", "[basic]")
{
    const auto a = allocated_value<test_struct>(test_struct{"1", 2});
    const auto t = test_struct{"1", 2};
    REQUIRE(*a == t);
}

TEST_CASE("Basic construction from rvalue with allocator", "[basic]")
{
    const auto a = allocated_value<test_struct>(test_struct{"1", 2}, std::allocator<test_struct>{});
    const auto t = test_struct{"1", 2};
    REQUIRE(*a == t);
}

TEST_CASE("Basic copy construct", "[basic]")
{
    const auto a = allocated_value<test_struct>(test_struct{"1", 2});
    const auto b = a;
    REQUIRE(*a == *b);
}

TEST_CASE("Basic copy construct with allocator", "[basic]")
{
    const auto t = test_struct{"1", 2};
    const auto a = allocated_value<test_struct>(t);
    const allocated_value<test_struct> b(a, std::allocator<test_struct>{});
    REQUIRE(*a == *b);
    REQUIRE(*a == t);
    REQUIRE(*b == t);
}

TEST_CASE("Basic move construct", "[basic]")
{
    const auto t = test_struct{"1", 2};
    auto a = allocated_value<test_struct>(t);
    const auto b = std::move(a);
    REQUIRE(*b == t);
}

TEST_CASE("Basic move construct with allocator", "[basic]")
{
    const auto t = test_struct{"1", 2};
    auto a = allocated_value<test_struct>(t);
    const allocated_value<test_struct> b(std::move(a), std::allocator<test_struct>{});
    REQUIRE(*b == t);
}

TEST_CASE("Basic in-place construct", "[basic]")
{
    const auto a = allocated_value<test_struct>(tcb::in_place, "1", 2);
    REQUIRE((*a).str == "1");
    REQUIRE((*a).i == 2);
}

TEST_CASE("Basic copy assign", "[basic]")
{
    const auto t = test_struct{"1", 2};
    const auto a = allocated_value<test_struct>(t);
    auto b = allocated_value<test_struct>{};
    REQUIRE_NOTHROW(b = a);
    REQUIRE(*b == *a);
    REQUIRE(*b == t);
}

TEST_CASE("Basic copy assign from value", "[basic]")
{
    auto a = allocated_value<test_struct>{};
    const auto t = test_struct{"1", 2};
    REQUIRE_NOTHROW(a = t);
    REQUIRE(*a == t);
}

TEST_CASE("Basic move assign", "[basic]")
{
    const auto t = test_struct{"1", 2};
    auto a = allocated_value<test_struct>(t);
    auto b = allocated_value<test_struct>{};
    b = std::move(a);
    REQUIRE(*b == t);
}

TEST_CASE("Basic move assign from value", "[basic]")
{
    auto a = allocated_value<test_struct>{};
    a = test_struct{"1", 2};
    REQUIRE(a->str == "1");
    REQUIRE(a->i == 2);
}

TEST_CASE("Basic emplace", "[basic]")
{
    auto a = allocated_value<test_struct>{};
    a.emplace("1", 2);
    REQUIRE(a->str == "1");
    REQUIRE(a->i == 2);
}

TEST_CASE("Basic member swap", "[basic]")
{
    test_struct t1 = {"1", 2};
    test_struct t2 = {"3", 4};
    auto a = allocated_value<test_struct>(t1);
    auto b = allocated_value<test_struct>(t2);
    a.swap(b);
    REQUIRE(*a == t2);
    REQUIRE(*b == t1);
}

TEST_CASE("Basic non-member swap", "[basic]")
{
    const test_struct t1 = {"1", 2};
    const test_struct t2 = {"3", 4};
    auto a = allocated_value<test_struct>(t1);
    auto b = allocated_value<test_struct>(t2);
    swap(a, b);
    REQUIRE(*a == t2);
    REQUIRE(*b == t1);
}

TEST_CASE("Basic member access", "[basic]")
{
    const test_struct t{"1", 2};
    auto a = allocated_value<test_struct>(t);
    REQUIRE(a->str == "1");
    REQUIRE(a->i == 2);

    REQUIRE_NOTHROW(a->str = "3");
    REQUIRE(a->str == "3");
    REQUIRE_NOTHROW(a->i = 4);
    REQUIRE(a->i == 4);
}

TEST_CASE("Basic make_allocated_value()", "[basic]")
{
    const auto a = tcb::make_allocated_value<test_struct>("1", 2);
    REQUIRE(a->str == "1");
    REQUIRE(a->i == 2);
}

TEST_CASE("Basic comparisons", "[basic]")
{
    const auto t = test_struct{"1", 2};
    const auto a = allocated_value<test_struct>(t);

    SECTION("allocated<->allocated comparisons") {
        REQUIRE(a == a);
        REQUIRE_FALSE(a != a);
        REQUIRE_FALSE(a < a);
        REQUIRE(a <= a);
        REQUIRE_FALSE(a > a);
        REQUIRE(a >= a);
    }

    SECTION("T<->allocated comparisons") {
        REQUIRE(a == t);
        REQUIRE_FALSE(a != t);
        REQUIRE_FALSE(a < t);
        REQUIRE(a <= t);
        REQUIRE_FALSE(a > t);
        REQUIRE(a >= t);
    }

    SECTION("allocated<->T comparisons") {
        REQUIRE(t == a);
        REQUIRE_FALSE(t != a);
        REQUIRE_FALSE(t < a);
        REQUIRE(t <= a);
        REQUIRE_FALSE(t > a);
        REQUIRE(t >= a);
    }
}
