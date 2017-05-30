
#include <tcb/allocated_value.hpp>

#include "catch.hpp"
#include "hh_short_alloc.h"

constexpr int arena_size = 1024;
template <typename T>
using stack_value = tcb::allocated_value<T, hh::short_alloc<T, arena_size>>;
using arena_t = hh::arena<arena_size>;

TEST_CASE("short_alloc construction", "[short-alloc]")
{
    arena_t arena;

    auto a = stack_value<int>(3, arena);
    REQUIRE(*a == 3);
}

TEST_CASE("short alloc copy construction", "[short-alloc]")
{
    arena_t arena;

    const auto a = stack_value<int>(3, arena);
    const auto b = a;

    REQUIRE(*b == 3);
}

TEST_CASE("short alloc move construction", "[short-alloc]")
{
    arena_t arena;

    auto a = stack_value<int>(3, arena);
    const auto b = std::move(a);
    REQUIRE(b == 3);
}

TEST_CASE("short alloc copy-assignment, same arena", "[short-alloc]")
{
    arena_t arena;

    const auto a = stack_value<int>(3, arena);
    auto b = stack_value<int>(4, arena);

    REQUIRE_NOTHROW(b = a);
    REQUIRE(*b == 3);
}

TEST_CASE("short alloc copy-assignment, different arena", "[short-alloc]")
{
    arena_t arena1;
    arena_t arena2;

    const auto a = stack_value<int>(3, arena1);
    auto b = stack_value<int>(4, arena2);

    REQUIRE_NOTHROW(b = a);
    REQUIRE(*b == 3);
}

TEST_CASE("short alloc copy-assignment from value", "[short-alloc]")
{
    arena_t arena;

    auto a = stack_value<int>(3, arena);
    const int i = 4;

    REQUIRE_NOTHROW(a = i);
    REQUIRE(*a == i);
}

TEST_CASE("short alloc move-assignment, same arena", "[short-alloc]")
{
    arena_t arena;

    auto a = stack_value<int>(3, arena);
    auto b = stack_value<int>(4, arena);

    REQUIRE_NOTHROW(b = std::move(a));
    REQUIRE(*b == 3);
}

TEST_CASE("short alloc move-assignment, different arena", "[short-alloc]")
{
    arena_t arena1;
    arena_t arena2;

    auto a = stack_value<int>(3, arena1);
    auto b = stack_value<int>(4, arena2);

    REQUIRE_NOTHROW(b = std::move(a));
    REQUIRE(*b == 3);
}

TEST_CASE("short alloc move-assignment from value", "[short-alloc]")
{
    arena_t arena;

    auto a = stack_value<int>(3, arena);

    REQUIRE_NOTHROW(a = 4);
    REQUIRE(*a == 4);
}