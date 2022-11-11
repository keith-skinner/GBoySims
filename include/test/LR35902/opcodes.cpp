#include <gtest/gtest.h>
#include <LR35902/opcodes.hpp>
#include <array>

TEST(LR35902_opcodes, OpCodes)
{
    [[maybe_unused]]
    const std::array<LR35902::Opcode, 256>& opcodes = LR35902::opcodes;
}