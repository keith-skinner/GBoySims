#pragma once
#include <cstddef>
#include <type_traits>


template<typename T, typename ... Ts>
static constexpr 
bool is_member_of_v = (false || ... || std::is_same_v<T, Ts>);

namespace detail 
{
template<typename T>
static consteval 
std::size_t find_index() {
    return -1ULL;
}

template<typename T, typename T1, typename ... Ts>
static consteval
std::size_t find_index() {
    if (std::is_same_v<T, T1>)
        return 0;
    return 1 + get_index<T, Ts...>();
}
}


template<typename T, typename ... Ts>
static consteval 
std::size_t find_index() {
    using U = std::remove_cvref_t<T>;
    static_assert(is_subregister<U, Ts...>());
    return detail::find_index<U, Ts...>();
}