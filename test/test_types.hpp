
#pragma once

#include <stdexcept>
#include <string>

struct test_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct test_struct {
    test_struct() = default;

    test_struct(const std::string& s, int i)
            : str(s), i(i)
    {}

    std::string str;
    int i;
};

inline bool operator==(const test_struct& lhs, const test_struct& rhs)
{
    return std::tie(lhs.str, lhs.i) == std::tie(lhs.str, lhs.i);
}

inline bool operator!=(const test_struct& lhs, const test_struct& rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(const test_struct& lhs, const test_struct& rhs)
{
    return std::tie(lhs.str, lhs.i) < std::tie(lhs.str, lhs.i);
}

inline bool operator<=(const test_struct& lhs, const test_struct& rhs)
{
    return !(rhs < lhs);
}

inline bool operator>(const test_struct& lhs, const test_struct& rhs)
{
    return rhs < lhs;
}

inline bool operator>=(const test_struct& lhs, const test_struct& rhs)
{
    return !(lhs < rhs);
}

struct move_only : test_struct {
    using test_struct::test_struct;
    move_only(move_only&&) = default;
    move_only& operator=(const move_only&) = default;
};

struct throw_on_default_construct : test_struct {
    using test_struct::test_struct;
    throw_on_default_construct() {
        throw test_error{"throw_on_default_construct"};
    }
};

struct throw_on_value_construct : test_struct {
    using test_struct::test_struct;
    throw_on_value_construct(const std::string&, int)
    {
        throw test_error("throw_on_value_construct");
    }
};

struct throw_on_copy_construct : test_struct {
    using test_struct::test_struct;
    throw_on_copy_construct(const throw_on_copy_construct&)
    {
        throw test_error{"throw_on_copy_construct"};
    }
    throw_on_copy_construct& operator=(const throw_on_copy_construct&) = default;
    throw_on_copy_construct(throw_on_copy_construct&&) noexcept = default;
    throw_on_copy_construct& operator=(throw_on_copy_construct&&) noexcept = default;
};

struct throw_on_move_construct : test_struct {
    using test_struct::test_struct;

    throw_on_move_construct(const throw_on_move_construct&) = default;
    throw_on_move_construct& operator=(const throw_on_move_construct&) = default;

    throw_on_move_construct(throw_on_move_construct&&)
    {
        throw test_error{"throw_on_move_construct"};
    }
    throw_on_move_construct& operator=(throw_on_move_construct&&) noexcept = default;
};

struct throw_on_copy_assign : test_struct {
    using test_struct::test_struct;

    throw_on_copy_assign(const throw_on_copy_assign&) = default;
    throw_on_copy_assign& operator=(const throw_on_copy_assign&)
    {
        throw test_error{"throw_on_copy_assign"};
    }
    throw_on_copy_assign(throw_on_copy_assign&&) noexcept = default;
    throw_on_copy_assign& operator=(throw_on_copy_assign&&) noexcept = default;
};

struct throw_on_move_assign : test_struct {
    using test_struct::test_struct;

    throw_on_move_assign(const throw_on_move_assign&) = default;
    throw_on_move_assign& operator=(const throw_on_move_assign&) = default;
    throw_on_move_assign(throw_on_move_assign&&) = default;

    throw_on_move_assign& operator=(throw_on_move_assign&&)
    {
        throw test_error{"throw_on_move_assign"};
    }
};