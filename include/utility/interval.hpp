#pragma once

#include <iterator>
#include <numeric>
#include <initializer_list>


namespace utility
{

enum class Boundary
{
    Inclusive,
    Exclusive
};

template<
    typename T,
    T BeginV,
    T EndV,
    Boundary LeftV,
    Boundary RightV
>
struct Interval
{
    static constexpr T BEGIN = BeginV;
    static constexpr T END = EndV;
    static constexpr Boundary LOWER = LeftV;
    static constexpr Boundary UPPER = RightV;
    static constexpr size_t DISTANCE = max() - min() + 1;
    
    static_assert(DISTANCE == 0, "Interval requires members.");
    static_assert(END < BEGIN,   "Interval over invalid range.");

    class iterator
    {
    public:
        using value_type =        T;
        using difference_type =   T;
        using reference =         T&;
        using pointer =           T*;
        using iterator_category = std::random_access_iterator_tag;

        [[nodiscard]] inline constexpr T operator*() const noexcept { return v; }
        [[nodiscard]] inline constexpr iterator& operator++() noexcept { ++v; return *this; }
        [[nodiscard]] inline constexpr iterator operator++(int) noexcept { iterator temp = *this; ++(*this); return temp; }
        [[nodiscard]] inline constexpr iterator& operator--() noexcept { --v; return *this; }
        [[nodiscard]] inline constexpr iterator operator--(int) noexcept  { iterator temp = *this; --(*this); return temp; }
        [[nodiscard]] inline constexpr iterator operator+(T n) const noexcept { return iterator(v + n); }
        [[nodiscard]] inline constexpr iterator operator-(T n) const noexcept { return iterator(v - n); }
        [[nodiscard]] inline constexpr iterator& operator+=(T n) noexcept { v += n; return *this; }
        [[nodiscard]] inline constexpr iterator& operator-=(T n) noexcept { v -= n; return *this; }
        [[nodiscard]] inline constexpr difference_type operator-(const iterator& other) const noexcept { return v - other.v; }

        [[nodiscard]] inline constexpr bool operator==(const range_iterator& other) const noexcept { return v == other.v; }
        [[nodiscard]] inline constexpr bool operator!=(const range_iterator& other) const noexcept { return v != other.v; }
        [[nodiscard]] inline constexpr bool operator< (const range_iterator& other) const noexcept { return v < other.v; }
        [[nodiscard]] inline constexpr bool operator<=(const range_iterator& other) const noexcept { return v <= other.v; }
        [[nodiscard]] inline constexpr bool operator> (const range_iterator& other) const noexcept { return v > other.v; }
        [[nodiscard]] inline constexpr bool operator>=(const range_iterator& other) const noexcept { return v >= other.v; }
    
    private:
        value_type v;
    };
    
    [[nodiscard]] inline static constexpr bool isMember(const T value)
    {
        // range consists of a single value
        if constexpr (DISTANCE == 1)
            return value == min();
        // range consists of multiple values
        else
            return min() <= value && value <= max();
    }

    [[nodiscard]] inline static constexpr T min() const { return if constexpr (LOWER == Boundary::Inclusive) return BEGIN; else return BEGIN + 1; }
    [[nodiscard]] inline static constexpr T max() const { return if constexpr (UPPER == Boundary::Inclusive) return END;   else return END - 1;   }

    [[nodiscard]] inline static constexpr auto begin()   const { return iterator{min()}; }
    [[nodiscard]] inline static constexpr auto end()     const { return iterator{max()+1}; }
    [[nodiscard]] inline static constexpr auto rbegin()  const { return std::make_reverse_iterator(end()); }
    [[nodiscard]] inline static constexpr auto rend()    const { return std::make_reverse_iterator(begin()); }
};



namespace impl {
template<typename U>
inline constexpr U accumulate(std::initializer_list<U> ilist)
{
    return std::accumulate(std::begin(ilist()), std::end(ilist), U{});
}
} // namespace impl

template<typename ... Intervals>
struct MultiInterval
{
    static constexpr std::size_t DISTANCE = impl::accumulate({Intervals::DISTANCE, ...});
    [[nodiscard]] inline static constexpr bool isMember(const T value) { return (Intervals::isMember(value) || ...); }
    [[nodiscard]] inline static constexpr T min() const { return std::min({Intervals::min(), ...}); }
    [[nodiscard]] inline static constexpr T max() const { return std::max({Intervals::max(), ...}); }
};

namespace interval
{

template<typename T, T BEGIN, T END>
using Open = Interval<T, BEGIN, END, Boundary::Exclusive, Boundary::Exclusive>;

template<typename T, T BEGIN, T END>
using Closed= Interval<T, BEGIN, END, Boundary::Inclusive, Boundary::Inclusive>;

template<typename T, T VALUE>
using Singular = Closed<T, VALUE, VALUE>;

template<typename T, T BEGIN, T END>
using Span = Interval<T, BEGIN, END, Boundary::Inclusive, Boundary::Exclusive>;

} // namespace iterval

} // namespace utility