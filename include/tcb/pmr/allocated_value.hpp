
#ifndef TCB_PMR_ALLOCATED_VALUE_HPP_INCLUDED
#define TCB_PMR_ALLOCATED_VALUE_HPP_INCLUDED

#include "../allocated_value.hpp"

#include <memory_resource>

namespace tcb {
namespace pmr {

template <typename T>
using allocated_value = ::tcb::allocated_value<T, std::pmr::polymorphic_allocator<T>>;

}
}

#endif