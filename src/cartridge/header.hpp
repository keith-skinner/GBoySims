#include <tuple>
#include <array>
#include <string>
#include <string_view>

#include <utility/defines.hpp>
#include <utility/range.hpp>

namespace GBOYSIM
{
namespace Header
{
using header_view = std::basic_string_view<data_t>;

constexpr auto RangeEntry = InclusiveRange{0x0100_a, 0x0103_a}; // usually NOP; JMP 0x0150;
constexpr auto RangeLogo = InclusiveRange{0x0134_a, 0x0133_a};
constexpr auto RangeTitle = InclusiveRange{0x0134_a, 0x0143_a};
constexpr auto RangeManufacturer = InclusiveRange{0x013F_a, 0x0142_a};
constexpr auto RangeCgbFlag = InclusiveRange{0x0143_a};
constexpr auto RangeNewLiscense = InclusiveRange{0x0144_a, 0x0145_a};
constexpr auto RangeSgbFlag = InclusiveRange{0x146_a};
constexpr auto RangeCartridgeType = InclusiveRange{0x0147_a};
constexpr auto RangeRomSize = InclusiveRange{0x0148_a};
constexpr auto RangeRamSize = InclusiveRange{0x0149_a};
constexpr auto RangeDestCode = InclusiveRange{0x014A_a};
constexpr auto RangeOldLiscense = InclusiveRange{0x014B_a}; //0x33 means check new liscensee code (0x0144-0x0145); SGB functions won't work if 0x33
constexpr auto RangeRomVersion = InclusiveRange{0x014C_a}; // usually 0x00
constexpr auto RangeHeaderChecksum = InclusiveRange{0x014D_a};
constexpr auto RangeGlobalChecksum = InclusiveRange{0x014E_a, 0x014F_a};

// Global checksum is a 16-bit checksum across the whole cartridge ROM.
//   Produced by adding all bytes of the cartridge (except the 2 checksum bytes).
//   Gameboy does not verify this checksum.

constexpr std::array<data_t, 48> NintendoLogo
{
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
};

constexpr std::array<std::tuple<data_t, std::string_view>, 3> ColorFlag
{{
    {0x00, "GameBoy"},
    {0x80, "Compatibility"},
    {0xC0, "GameBoy Color"},
}};

constexpr std::array<std::tuple<data_t, std::string_view>, 3> SuperFlag
{{
    {0x00, "GameBoy"},
    {0x30, "GameBoy Super"},
    // Super Gameboy disables Super Gameboy functions if not set to 0x30
}};

constexpr std::array<std::tuple<data_t, std::string_view>, 61> NewLiscenseeCode
{{
    {0x00, "none"},
    {0x01, "Nintendo R&D1"},
    {0x08, "Capcom"},
    {0x13, "Electronic Arts"},
    {0x18, "Hudson Soft"},
    {0x19, "b-ai"},
    {0x20, "kss"},
    {0x22, "pow"},
    {0x24, "PCM Complete"},
    {0x25, "san-x"},
    {0x28, "Kemco Japan"},
    {0x29, "seta"},
    {0x30, "Viacom"},
    {0x31, "Nintendo"},
    {0x32, "Bandai"},
    {0x33, "Ocean/Acclaim"},
    {0x34, "Konami"},
    {0x35, "Hector"},
    {0x37, "Taito"},
    {0x38, "Hudson"},
    {0x39, "Banpresto"},
    {0x41, "Ubi Soft"},
    {0x42, "Atlus"},
    {0x44, "Malibu"},
    {0x46, "angel"},
    {0x47, "Bullet-Proof"},
    {0x49, "irem"},
    {0x50, "Absolute"},
    {0x51, "Acclaim"},
    {0x52, "Activision"},
    {0x53, "American sammy"},
    {0x54, "Konami"},
    {0x55, "Hi tech entertainment"},
    {0x56, "LJN"},
    {0x57, "Matchbox"},
    {0x58, "Mattel"},
    {0x59, "Milton Bradley"},
    {0x60, "Titus"},
    {0x61, "Virgin"},
    {0x64, "LucasArts"},
    {0x67, "Ocean"},
    {0x69, "Electronic Arts"},
    {0x70, "Infogrames"},
    {0x71, "Interplay"},
    {0x72, "Broderbund"},
    {0x73, "sculptured"},
    {0x75, "sci"},
    {0x78, "THQ"},
    {0x79, "Accolade"},
    {0x80, "misawa"},
    {0x83, "lozc"},
    {0x86, "tokuma shoten i*"},
    {0x87, "tsukuda ori*"},
    {0x91, "Chunsoft"},
    {0x92, "Video system"},
    {0x93, "Ocean/Acclaim"},
    {0x95, "Varie"},
    {0x96, "Yonezawa/s\'pal"},
    {0x97, "Kaneko"},
    {0x99, "Pack in soft"},
    {0xA4, "Konami (Yu-Gi-Oh!)"},
}};

constexpr std::array<std::tuple<data_t, std::string_view>, 29> CartridgeType
{{
    {0x00, "ROM_ONLY"}, // 32KB ROM only
    {0x01, "MBC1"},
    {0x02, "MBC1_RAM"},
    {0x03, "MBC1_RAM_BATTERY"},
    {0x05, "MBC2"},
    {0x06, "MBC2_BATTERY"},
    {0x08, "ROM_RAM"},
    {0x09, "ROM_RAM_BATTERY"},
    {0x0B, "MMM01"},
    {0x0C, "MMM01_RAM"},
    {0x0D, "MMM01_RAM_BATTERY"},
    {0x0F, "MBC3_TIMER_BATTERY"},
    {0x10, "MBC3_TIMER_RAM_BATTERY"},
    {0x11, "MBC3"},
    {0x12, "MBC3_RAM"},
    {0x13, "MBC3_RAM_BATTERY"},
    {0x15, "MBC4"},
    {0x16, "MBC4_RAM"},
    {0x17, "MBC4_RAM_BATTERY"},
    {0x19, "MBC5"},
    {0x1A, "MBC5_RAM"},
    {0x1B, "MBC5_RAM_BATTERY"},
    {0x1C, "MBC5_RUMBLE"},
    {0x1D, "MBC5_RUMBLE_RAM"},
    {0x1E, "MBC5_RUMBLE_RAM_BATTERY"},
    {0xFC, "POCKET_CAMERA"},
    {0xFD, "BANDAI_TAMA5"},
    {0xFE, "HUC3"},
    {0xFF, "HUC1_RAM_BATTERY"},
}};

constexpr std::array<std::tuple<data_t, std::string_view>, 11> RomSize
{{
    {0x00, " 32 KB;   0 Banks"},
    {0x01, " 64 KB;   4 Banks"},
    {0x02, "128 KB;   8 banks"},
    {0x03, "256 KB;  16 banks"},
    {0x04, "512 KB;  32 banks"},
    {0x05, "  1 MB;  63 banks"},
    {0x06, "  2 MB; 125 banks"},
    {0x07, "  4 MB; 256 banks"},
    {0x52, "1.1 MB;  72 banks"},
    {0x53, "1.2 MB;  80 banks"},
    {0x54, "1.5 MB;  96 banks"},
}};

constexpr std::array<std::tuple<data_t, std::string_view>, 4> RamSize
{{
    {0x00, " 0 KB"},
    {0x01, " 2 KB"},
    {0x02, " 8 KB"},
    {0x03, "32 KB"},
}};

constexpr std::array<std::tuple<data_t, std::string_view>, 2> DestinationCode
{{
    {0x00, "Japanese"},
    {0x01, "Non-Japanese"},
}};

}
}