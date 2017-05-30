
#include "test_pimpl.hpp"

#include "catch.hpp"

/*
 * Pimpl Tests
 *
 * These ensure that basic pimpl functionality works as desired.
 * Uses the test_pimpl class, whose implementation is separately compiled.
 */
TEST_CASE("pimpl construction", "[pimpl]")
{
    REQUIRE_NOTHROW(test_pimpl{});
}

TEST_CASE("pimpl copy construction", "[pimpl]")
{
    const auto a = test_pimpl{1};
    const auto b = a;
    REQUIRE(b.get_value() == 1);
}

TEST_CASE("pimpl move construction", "[pimpl]")
{
    auto a = test_pimpl{1};
    const auto b = std::move(a);
    REQUIRE(b.get_value() == 1);
}

TEST_CASE("pimpl copy assignment", "[pimpl]")
{
    const auto a = test_pimpl{1};
    auto b = test_pimpl{2};
    REQUIRE_NOTHROW(b = a);
    REQUIRE(b.get_value() == 1);
}

TEST_CASE("pimpl move assignment", "[pimpl]")
{
    auto a = test_pimpl{1};
    auto b = test_pimpl{2};
    REQUIRE_NOTHROW(b = std::move(a));
    REQUIRE(b.get_value() == 1);
}