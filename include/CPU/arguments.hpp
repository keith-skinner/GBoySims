#pragma once
#include <concepts>


namespace CPU::Arguments
{

namespace Source
{
    struct HardCoded{};
    struct Register {};
} // namespace Source

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

// Immediate HardCoded  - Get the number thats hardcoded into the opcode
// Immediate Register   - Get the number that's stored in a register (addr + offset = real addr)
// Reference HardCoded  - Dereference address 3 thats hardcoded into the opcode
// Reference Register   - Dereference the address that's stored in a register (addr + offset = real addr)

template<typename NameT, typename SourceT, typename AccessT>
class Argument {
public:
    using Name = NameT;
    using Source = SourceT;
    using Access = AccessT;
    using Data = typename AccessT::Data;

    constexpr Data operator*() { return v + Access::OFFSET; }

private:
    Data v = {};
};

} // namespace CPU::Arguments