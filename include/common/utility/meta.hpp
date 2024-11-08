#pragma once
#include <cstddef>
#include <type_traits>



template<typename T, typename ... Ts>
static constexpr
bool is_any_same_v = (false || ... || std::is_same_v<T, Ts>);

template<typename T, typename ... Ts>
static constexpr 
bool is_member_of_v = is_any_same_v<T, Ts...>;

template<typename T, typename ... Ts>
static constexpr
bool is_all_same_v = (true && ... && std::is_same_v<T, Ts>);

template<typename T, typename ...Ts>
std::size_t find_index()
{
    return detail::find_index<T, Ts...>();
}

namespace detail 
{
template<typename T>
static consteval 
std::size_t find_index() {
    return 0;
}

template<typename T, typename T1, typename ... Ts>
static consteval
std::size_t find_index() {
    if (std::is_same_v<T, T1>)
        return 0;
    return 1 + find_index<T, Ts...>();
}
}