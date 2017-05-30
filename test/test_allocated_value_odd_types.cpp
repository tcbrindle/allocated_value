
#include <tcb/allocated_value.hpp>

#include "catch.hpp"

#include "test_types.hpp"
#include "test_allocators.hpp"

using tcb::allocated_value;

/*
 * Tests with move-only types
 */
TEST_CASE("Move-only construction", "[move-only]")
{
    const auto a = allocated_value<move_only>{move_only{"1", 2}};
    REQUIRE(a->str == "1");
    REQUIRE(a->i == 2);
}

TEST_CASE("Move-only move construction", "[move-only]")
{
    auto a = allocated_value<move_only>{tcb::in_place, "1", 2};
    const auto b = std::move(a);
    REQUIRE(b->str == "1");
    REQUIRE(b->i == 2);
}

TEST_CASE("Move-only move assignment", "[move-only]")
{
    auto a = allocated_value<move_only>{tcb::in_place, "1", 2};
    auto b = allocated_value<move_only>{tcb::in_place, "3", 4};
    b = std::move(a);
    REQUIRE(b->str == "1");
    REQUIRE(b->i == 2);
}

TEST_CASE("Move-only move assignment from value", "[move-only]")
{
    auto a = allocated_value<move_only>{tcb::in_place, "1", 2};
    a = move_only{"3", 4};
    REQUIRE(a->str == "3");
    REQUIRE(a->i == 4);
}

/*
 * Tests with throwing constructors
 */
TEST_CASE("Throw on default construct")
{
    REQUIRE_THROWS_AS(allocated_value<throw_on_default_construct>{}, test_error);
}

TEST_CASE("Throw on value construct")
{
    REQUIRE_THROWS_AS(allocated_value<throw_on_value_construct>(tcb::in_place, "3", 4), test_error);
}

TEST_CASE("Throw on copy construct")
{
    const auto a = allocated_value<throw_on_copy_construct>{};
    const auto do_copy = [&] { auto b = a; };
    REQUIRE_THROWS_AS(do_copy(), test_error);
}

// Note that when move-constructing an allocated_value<T>, we never actually
// call T's move constructor or allocate anything, but just steal the contents
// of the RHS. So this test is actually a little redundant, as the move
// constructor is marked noexcept so will actually terminate if something
// does end up throwing somehow.
TEST_CASE("Throw on move construct")
{
    throw_on_move_construct m{};
    auto a = allocated_value<throw_on_move_construct>{m};
    const auto do_move = [&] { auto b{std::move(a)}; };
    REQUIRE_NOTHROW(do_move());
}

TEST_CASE("Throw on copy assign")
{
    const auto a = allocated_value<throw_on_copy_assign>({"1", 2});
    auto b = allocated_value<throw_on_copy_assign>({"3", 4});

    REQUIRE_THROWS_AS(b = a, test_error);
    REQUIRE(b->str == "3");
    REQUIRE(b->i == 4);
}

TEST_CASE("Throw on copy assign (from value)")
{
    auto a = allocated_value<throw_on_copy_assign>({"1", 2});
    const auto c = throw_on_copy_assign{"3", 4};
    REQUIRE_THROWS_AS(a = c, test_error);
    REQUIRE(a->str == "1");
    REQUIRE(a->i == 2);
}

// Contrary to expectations, this should NOT throw with the default allocator,
// because we never actually move-assign the underlying value, just the
// contained ptr
TEST_CASE("(No) throw on move assign")
{
    auto a = allocated_value<throw_on_move_assign>({"1", 2});
    auto b = allocated_value<throw_on_move_assign>({"3", 4});

    REQUIRE_NOTHROW(b = std::move(a));
    REQUIRE(b->str == "1");
    REQUIRE(b->i == 2);
}

TEST_CASE("Throw on move assign (from value)")
{
    auto a = allocated_value<throw_on_move_assign>({"1", 2});

    REQUIRE_THROWS_AS(a = throw_on_move_assign("3", 4), test_error);
    REQUIRE(a->str == "1");
    REQUIRE(a->i == 2);
}


