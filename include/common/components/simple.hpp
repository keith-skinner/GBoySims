#pragma once
#include <array>

namespace common::components
{

template<typename AddrT, typename DataT, AddrT E, AddrT B>
struct Simple
{
    using addr_t = AddrT;
    using data_t = DataT;
    static constexpr addr_t end = E;
    static constexpr addr_t begin = B;
    static constexpr std::size_t size = end - begin + 1;

    std::array<data_t, size> m_data;

    data_t read(const addr_t addr) {
        return m_data[addr - begin];
    }

    void write(const addr_t addr, const data_t data) {
        m_data[addr - begin] = data;
    }

    data_t  operator[](std::size_t idx) const { return m_data[idx]; }
    data_t& operator[](std::size_t idx)       { return m_data[idx]; }
};

} // namespace common::components