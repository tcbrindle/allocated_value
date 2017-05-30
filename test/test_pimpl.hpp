
#pragma once

#include <tcb/allocated_value.hpp>

class test_pimpl {
public:
    test_pimpl();
    explicit test_pimpl(int val);

    // Special member declarations (defaulted in implementation file)
    test_pimpl(const test_pimpl&);
    test_pimpl(test_pimpl&&) noexcept;
    ~test_pimpl();
    test_pimpl& operator=(const test_pimpl&);
    test_pimpl& operator=(test_pimpl&&);

    int get_value() const;

private:
    struct private_;
    tcb::allocated_value<private_> priv_;
};