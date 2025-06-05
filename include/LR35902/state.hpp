#include <cstdint>

namespace LR35902
{

namespace State
{

class LCDControl
{
public:
    constexpr uint8_t get() { return value; }
    constexpr void set(const uint8_t v) { value = v; }

    constexpr bool getBGDisplay       () { return value & 0b0000'0001; };
    constexpr bool getOBJEnable       () { return value & 0b0000'0010; };
    constexpr bool getOBJSize         () { return value & 0b0000'0100; };
    constexpr bool getBGTileMapSelect () { return value & 0b0000'1000; };
    constexpr bool getBGTileDataSelect() { return value & 0b0001'0000; };
    constexpr bool getWinDisplayEnable() { return value & 0b0010'0000; };
    constexpr bool getWinTileMapSelect() { return value & 0b0100'0000; };
    constexpr bool getLCDDisplayEnable() { return value & 0b1000'0000; };

    constexpr void setBGDisplay       (const bool v) { value &= uint8_t{0b0000'0001} * v; } 
    constexpr void setOBJEnable       (const bool v) { value &= uint8_t{0b0000'0010} * v; }
    constexpr void setOBJSize         (const bool v) { value &= uint8_t{0b0000'0100} * v; }
    constexpr void setBGTileMapSelect (const bool v) { value &= uint8_t{0b0000'1000} * v; }
    constexpr void setBGTileDataSelect(const bool v) { value &= uint8_t{0b0001'0000} * v; }
    constexpr void setWinDisplayEnable(const bool v) { value &= uint8_t{0b0010'0000} * v; }
    constexpr void setWinTileMapSelect(const bool v) { value &= uint8_t{0b0100'0000} * v; }
    constexpr void setLCDDisplayEnable(const bool v) { value &= uint8_t{0b1000'0000} * v; }

private:
    uint8_t value;
};

} // namespace State

struct SystemState
{
    State::LCDControl LCDC;
};

} // namespace LR35902