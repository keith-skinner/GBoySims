#include <gtest/gtest.h>

#include <sc/string_constant.hpp>
#include <common/components/simple.hpp>

#include <LR35902/micro.hpp>
#include <LR35902/mmu.hpp>
#include <LR35902/registers.hpp>

#include "../validation.hpp"

TEST(LR35902_micro, NOP)
{
    RegRecorder registers{};
    SimpleMemRecorder<std::uint16_t, std::uint8_t, 0xFFFF, 0x0000> memory{};
    auto reader = LR35902::ReadModel<decltype(memory)>{memory};
    auto writer = LR35902::WriteModel<decltype(memory)>{memory};
    LR35902::MMU mmu{reader, writer};
    LR35902::Micro micro{registers, mmu};

    EXPECT_TRUE(memory.records.size() == 0);
    EXPECT_TRUE(registers.records.size() == 0);

    micro.NOP();

    EXPECT_TRUE(memory.records.size() == 0);
    EXPECT_TRUE(registers.records.size() == 0);
};

