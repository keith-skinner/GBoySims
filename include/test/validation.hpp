#pragma once
#include <cstdint>
#include <string_view>
#include <vector>

#include <sc/string_constant.hpp>
#include <LR35902/registers.hpp>
#include <common/components/simple.hpp>

static constexpr auto s_read = "READ"_sc;
static constexpr auto s_write = "WRITE"_sc;

template<typename addr_t, typename data_t>
struct mem_record_t
{    
    std::string_view oper;
    addr_t addr;
    data_t data;
};

using MemRecord = mem_record_t<uint16_t, uint8_t>;

constexpr bool operator==(const MemRecord& lhs, const MemRecord& rhs)
{
    return lhs.addr == rhs.addr && lhs.data == rhs.data && lhs.oper == rhs.oper;
}

template<typename stream_t>
static stream_t& operator<<(stream_t & stream, const MemRecord r)
{
    return stream << "(" << r.oper << ":0x" << std::hex << r.addr << ":" << std::dec << r.data << ")";
}

template<typename Base, typename Base::addr_t EndV, typename Base::addr_t BeginV>
struct MemRecorder
{
    using base_t = Base;
    using addr_t = typename Base::addr_t;
    using data_t = typename Base::data_t;
    static constexpr addr_t end = EndV;
    static constexpr addr_t begin = BeginV;
    static constexpr std::size_t size = end - begin + 1;
    using MemRecord = mem_record_t<typename Base::addr_t, typename Base::data_t>;

    std::vector<MemRecord> records = {};
    base_t base = {};
    
    data_t read(const addr_t addr) {
        data_t data = base.read(addr);
        records.push_back({s_read(), addr, data});
        return data;
    }

    auto write(const addr_t addr, const data_t data) {
        records.push_back({s_write(), addr, data});
        return base.write(addr, data);
    }

    auto get(const addr_t addr)
    {
        return base.read(addr);
    }
    auto set(const addr_t addr, const data_t data)
    {
        return base.write(addr, data);
    }
};

struct RegRecord
{
    std::string_view oper;
    std::string_view reg;
    std::uint16_t val;
};

constexpr bool operator==(const RegRecord& lhs, const RegRecord& rhs)
{
    return  lhs.oper == rhs.oper
        &&  lhs.reg == rhs.reg
        &&  lhs.val == rhs.val;
}

template <typename stream_t>
stream_t& operator<<(stream_t& stream, const RegRecord& obj)
{
    return stream << "(" << obj.oper << ":" << obj.reg << ":0x" << std::hex << obj.val << ")";
}

struct RegRecorder 
:   public LR35902::RegisterFile
{
    using Base = LR35902::RegisterFile;

    std::vector<RegRecord> records;

    template<typename SubRegister>
    constexpr auto write(SubRegister reg = SubRegister{})
    {
        records.push_back({s_write(), reg.name, reg.value});
        return Base::write(reg);
    }

    template<typename SubRegister>
    constexpr auto read(SubRegister reg = SubRegister{})
    {
        auto value = Base::read(reg);
        records.push_back({s_read(), reg.name(), value});
        return value;
    }

    /**
     * @brief Set a register without recording that it was changed.
     * 
     * @details The motivation for this function is to reset the system to a specified state.
     * 
     * @tparam SubRegister 
     * @param reg 
     * @return constexpr auto 
     */
    template<typename SubRegister>
    constexpr auto set(SubRegister reg)
    {
        return Base::write(reg);
    }

    /**
     * @brief Get the value of a register without recording the register access.
     * @details The motivation for this function is to validate that the system is in a specified state.
     * @tparam SubRegister 
     * @param reg 
     * @return constexpr auto 
     */
    template<typename SubRegister>
    constexpr auto get(SubRegister reg = SubRegister{})
    {
        return Base::read(reg);
    }
};

template<typename A, typename D, A E, A B>
using Simple = common::components::Simple<A, D, E, B>;
template<typename A, typename D, A End, A Begin>
using SimpleMemRecorder = MemRecorder<Simple<A, D, End, Begin>, End, Begin>;