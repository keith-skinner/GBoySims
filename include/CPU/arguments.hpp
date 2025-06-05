#pragma once
#include <concepts>


namespace CPU::Arguments
{

namespace Access
{
template<std::integral DataT>
struct Immediate{
    using Data = DataT;
    static constexpr Data OFFSET = 0;
};

template<std::integral DataT, DataT OffsetV = 0>
struct Reference {
    using Data = DataT;
    static constexpr Data OFFSET = OffsetV;
};
} // namespace Access

namespace Source
{
    struct HardCoded{};
    struct Register {};
} // namespace Source

// Immediate HardCoded  - Get the number thats hardcoded into the opcode
// Immediate Register   - Get the number that's stored in a register
// Reference HardCoded  - Dereference address 3 thats hardcoded into the opcode
// Reference Register   - Dereference the address that's stored in a register

template<typename NameT, typename SourceT, typename AccessT>
struct Argument {
    using Name = NameT;
    using Source = SourceT;
    using Access = AccessT;
    using Data = typename AccessT::Data;

    constexpr Data operator*() { return v + Access::OFFSET; }
    Data v = {};
};

} // namespace CPU::Arguments