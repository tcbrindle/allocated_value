
#include "test_pimpl.hpp"

struct test_pimpl::private_ {
    int i;
};

// private_ is an aggregate, so we need to use use aggregate construction +
// move in the member initializer list here.
test_pimpl::test_pimpl(int val)
    : priv_(private_{val})
{}

// Defaulted special members
test_pimpl::test_pimpl() = default;
test_pimpl::test_pimpl(const test_pimpl&) = default;
test_pimpl::test_pimpl(test_pimpl&&) noexcept = default;
test_pimpl::~test_pimpl() = default;
test_pimpl& test_pimpl::operator=(const test_pimpl&) = default;
test_pimpl& test_pimpl::operator=(test_pimpl&&) = default;

int test_pimpl::get_value() const
{
    return priv_->i;
}