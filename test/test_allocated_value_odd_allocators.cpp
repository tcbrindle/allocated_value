
#include <tcb/allocated_value.hpp>

#include "catch.hpp"
#include "test_allocators.hpp"
#include "test_types.hpp"

using tcb::allocated_value;



TEST_CASE("POCCA copy-assign strong guarantee", "[odd-allocators]")
{
    using alloc_t = pocca_allocator<throw_on_copy_assign>;
    const auto a = allocated_value<throw_on_copy_assign, alloc_t>{tcb::in_place, "1", 2};
    auto b = allocated_value<throw_on_copy_assign, alloc_t>{tcb::in_place, "3", 4};

    REQUIRE_THROWS_AS(b = a, test_error);
    REQUIRE(b->str == "3"); // Unchanged
    REQUIRE(b->i == 4); // Unchanged
}

TEST_CASE("Non-POCMA move-assign", "[odd-allocators]")
{
    using alloc_t = non_pocma_allocator<throw_on_move_assign>;
    auto a = allocated_value<throw_on_move_assign, alloc_t>{tcb::in_place, "1", 2};
    auto b = allocated_value<throw_on_move_assign, alloc_t>{tcb::in_place, "3", 4};

    REQUIRE_NOTHROW(b = std::move(a));
    REQUIRE(b->str == "1");
    REQUIRE(b->i == 2);
}

TEST_CASE("POCS member swap", "[odd-allocators]")
{
    using alloc_t = pocs_allocator<throw_on_move_assign>;
    const auto t = throw_on_move_assign{"1", 2};
    auto a = allocated_value<throw_on_move_assign, alloc_t>{tcb::in_place, "1", 2};
    auto b = allocated_value<throw_on_move_assign, alloc_t>{tcb::in_place, "3", 4};

    REQUIRE_NOTHROW(a.swap(b));
    REQUIRE(a->str == "3");
    REQUIRE(a->i == 4);
    REQUIRE(b->str == "1");
    REQUIRE(b->i == 2);
}

TEST_CASE("POCS non-member swap", "[odd-allocators]")
{
    using alloc_t = pocs_allocator<throw_on_move_assign>;
    const auto t = throw_on_move_assign{"1", 2};
    auto a = allocated_value<throw_on_move_assign, alloc_t>{tcb::in_place, "1", 2};
    auto b = allocated_value<throw_on_move_assign, alloc_t>{tcb::in_place, "3", 4};

    REQUIRE_NOTHROW(a.swap(b));
    REQUIRE(a->str == "3");
    REQUIRE(a->i == 4);
    REQUIRE(b->str == "1");
    REQUIRE(b->i == 2);
}

TEST_CASE("Unequal allocator move-construct", "[odd-allocators]")
{

}