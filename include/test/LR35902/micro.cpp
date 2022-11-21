#include <gtest/gtest.h>
#include <vector>
#include <array>
#include <sc/string_constant.hpp>
#include <LR35902/micro.hpp>
#include <LR35902/mmu.hpp>

[[maybe_unused]]
static constexpr auto s_read = "READ"_sc;
[[maybe_unused]]
static constexpr auto s_write = "WRITE"_sc;

template<typename addr_t, typename data_t>
struct record
{    
    addr_t addr;
    data_t data;
    std::string_view oper;
};

template<typename stream_t, typename addr_t, typename data_t>
stream_t& operator<<(stream_t & stream, record<addr_t, data_t> r)
{
    return stream << "{" << "addr: 0x" << std::hex << r.addr << " data: 0x" << std::hex << r.data << " " << oper << "}";
}

template<typename addr_t, typename data_t>
bool operator==(const record<addr_t, data_t>& lhs, const record<addr_t, data_t>& rhs)
{
    return lhs.addr == rhs.addr && lhs.data == rhs.data && lhs.oper == rhs.oper;
}

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

};

template<typename Base, typename Base::addr_t EndV, typename Base::addr_t BeginV>
struct Recorder
{
    using base_t = Base;
    using addr_t = typename Base::addr_t;
    using data_t = typename Base::data_t;
    static constexpr addr_t end = EndV;
    static constexpr addr_t begin = BeginV;
    static constexpr std::size_t size = end - begin + 1;

    std::vector<record<addr_t, data_t>> records = {};
    base_t base = {};
    
    data_t read(const addr_t addr) {
        data_t data = base.read(addr);
        records.push_back({addr, data, s_read()});
        return data;
    }

    void write(const addr_t addr, const data_t data) {
        records.push_back({addr, data, s_write()});
        base.write(addr, data);
    }

};

template<typename A, typename D, A End, A Begin>
using SimpleRecorder = Recorder<Simple<A, D, End, Begin>, End, Begin>;

TEST(LR35902_micro, NOP)
{
    LR35902::RegisterFile registers{};
    SimpleRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    auto & records = memory.records;
    EXPECT_TRUE(records.size() == 0);
    EXPECT_EQ(0, registers.read<LR35902::A>());
    EXPECT_EQ(0, registers.read<LR35902::BC>());
    EXPECT_EQ(0, registers.read<LR35902::DE>());
    EXPECT_EQ(0, registers.read<LR35902::HL>());
    EXPECT_EQ(0, registers.read<LR35902::SP>());
    EXPECT_EQ(0, registers.read<LR35902::PC>());
    EXPECT_EQ(0, registers.read<LR35902::F>());


    micro.NOP();

    // No writes to memory
    EXPECT_TRUE(records.size() == 0);

    // No writes to registers
    EXPECT_EQ(0, registers.read<LR35902::A>());
    EXPECT_EQ(0, registers.read<LR35902::BC>());
    EXPECT_EQ(0, registers.read<LR35902::DE>());
    EXPECT_EQ(0, registers.read<LR35902::HL>());
    EXPECT_EQ(0, registers.read<LR35902::SP>());
    EXPECT_EQ(0, registers.read<LR35902::PC>());

    // No writes to flags
    EXPECT_EQ(0, registers.read<LR35902::F>());
};