#pragma once
#include <cstddef>
#include <type_traits>

namespace meta
{

template<template<typename> class Pred, typename... Candidates>
struct find_if;

// no more candidate types to test
template<template<typename> class Pred>
struct find_if<Pred> : std::integral_constant<std::size_t, 0> {};

template<template<typename> class Pred, typename Candidate, typename... Candidates>
struct find_if<Pred, Candidate, Candidates...>
: std::conditional_t<
    Pred<Candidate>::value, // found?
    std::integral_constant<std::size_t, 0>, // found
    std::integral_constant<std::size_t, 1 + find_if<Pred, Candidates...>::value> // not found
>
{};

template<template<typename> class Pred, typename Candidate, typename... Candidates>
constexpr auto find_if_v = find_if<Pred, Candidate, Candidates...>::value;

template<typename Target>
struct Find
{
    // bind T to is_same
    template<typename Candidate>
    struct is_same : std::is_same<Target, Candidate> {};
    
    template<typename... Candidates>
    static constexpr std::size_t index = find_if<is_same, Candidates...>::value;
    
    
    template<typename... Candidates>
    static constexpr bool member = index<Candidates...> < sizeof...(Candidates);
};

template<typename T, typename... Ts>
constexpr bool is_member_of_v = Find<T>::member<Ts...>;

} // namespace meta