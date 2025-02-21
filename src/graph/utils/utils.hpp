/*******************************************************************************
* Copyright 2020-2022 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef GRAPH_UTILS_UTILS_HPP
#define GRAPH_UTILS_UTILS_HPP

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <type_traits>

#include "common/utils.hpp"

#include "graph/interface/c_types_map.hpp"

namespace dnnl {
namespace impl {
namespace graph {
namespace utils {

// import common utilities
using namespace dnnl::impl::utils;

#ifndef assertm
#define assertm(exp, msg) assert(((void)(msg), (exp)))
#endif

#ifndef NDEBUG
#define DEBUG_PRINT_ERROR(message) \
    do { \
        std::cerr << "ERROR: " << message << std::endl; \
    } while (0)
#else
#define DEBUG_PRINT_ERROR(message)
#endif

inline static size_t size_of(data_type_t dtype) {
    switch (dtype) {
        case data_type::f32:
        case data_type::s32: return 4U;
        case data_type::s8:
        case data_type::u8: return 1U;
        case data_type::f16:
        case data_type::bf16: return 2U;
        default: return 0;
    }
}

inline static size_t prod(const std::vector<dim_t> &shape) {
    if (shape.empty()) return 0;

    size_t p = (std::accumulate(
            shape.begin(), shape.end(), size_t(1), std::multiplies<dim_t>()));

    return p;
}

inline static size_t size_of(
        const std::vector<dim_t> &shape, data_type_t dtype) {
    return prod(shape) * size_of(dtype);
}

template <typename T>
inline bool any_le(const std::vector<T> &v, T i) {
    return std::any_of(v.begin(), v.end(), [i](T k) { return k <= i; });
}

// solve the Greatest Common Divisor
inline size_t gcd(size_t a, size_t b) {
    size_t temp = 0;
    while (b != 0) {
        temp = a;
        a = b;
        b = temp % b;
    }
    return a;
}

// solve the Least Common Multiple
inline size_t lcm(size_t a, size_t b) {
    return a * b / gcd(a, b);
}

int getenv_int_internal(const char *name, int default_value);
bool check_verbose_string_user(const char *name, const char *expected);

inline std::string thread_id_to_str(std::thread::id id) {
    std::stringstream ss;
    ss << id;
    return ss.str();
}

inline int div_and_ceil(float x, float y) {
    return std::ceil(x / y);
}

inline int div_and_floor(float x, float y) {
    return std::floor(x / y);
}

} // namespace utils
} // namespace graph
} // namespace impl
} // namespace dnnl

#endif
