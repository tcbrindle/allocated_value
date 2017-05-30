
#include <tcb/allocated_value.hpp>

#include "catch.hpp"
#include "test_allocators.hpp"

using tcb::allocated_value;

TEST_CASE("Nested allocated values can be constructed", "[nested]")
{
    using nested_t = allocated_value<allocated_value<int>>;

    nested_t a{allocated_value<int>{3}};
    REQUIRE(**a == 3);
}

TEST_CASE("Nested allocated values can be copy constructed", "[nested]")
{
    using nested_t = allocated_value<allocated_value<int>>;

    nested_t a{allocated_value<int>{3}};
    nested_t b{a};
    REQUIRE(**b == 3);
}