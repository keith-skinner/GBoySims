#pragma once

#include <cstdint>
#include <array>

#include <utility/interval.hpp>

#include "SystemTypes.hpp"
#include "state.hpp"

namespace LR35902
{

namespace MMU
{

namespace MemoryRegions
{
    // https://gbdev.io/pandocs/


    template<Addr BEGIN, Addr END>
    using Span = utility::interval::Span<Addr, BEGIN, END>;
    template<Addr ADDR>
    using Singular = utility::interval::Singular<Addr, ADDR>;
    
    // General Regions
    using ROM_0     = Span<0x0000, 0x4000>; // Static ROM
    using ROM_N     = Span<0x4000, 0x8000>; // Switchable ROM
    using VRAM      = Span<0x8000, 0xA000>; // Video RAM
    using SRAM      = Span<0xA000, 0xC000>; // Switchable RAM Bank (saves mostly)
    using IRAM_0    = Span<0xC000, 0xD000>; // Internal / Working RAM
    using IRAM_N    = Span<0xD000, 0xE000>; // Internal / Working Ram (switchable 1-7 in CGB Mode)
    using Shadow    = Span<0xE000, 0xFE00>; // Shadow RAM (same as (0xC00-0xDDFF))
    using NotUsable = Span<0xFE00, 0xFF00>; // Not Usable
    using IOPorts   = Span<0xFF00, 0xFF80>; // IO Ports
    using HRAM      = Span<0xFF80, 0xFFFF>; // High RAM
    
    // Memory Mapped Registers
    using JPAD      = Singular<0xFF00>; // Joy-pad buttons

    using SerTxRx   = Singular<0xFF01>; // Serial Transfer Data
    using SerCtrl   = Singular<0xFF02>; // Serial Control
    
    using DIV       = Singular<0xFF04>; // Divider Register
    using TIMA      = Singular<0xFF05>; // Timer Counter
    using TMA       = Singular<0xFF06>; // Timer Modulo
    using TAC       = Singular<0xFF07>; // Timer Control
    
    using IF        = Singular<0xFF0F>; // Interrupt Flag
    
    using NR10      = Singular<0xFF10>; // Sweep
    using NR11      = Singular<0xFF11>; // Length Timer & Duty Cycle
    using NR12      = Singular<0xFF12>; // Volume and Envelope
    using NR13      = Singular<0xFF13>; // Period Low
    using NR14      = Singular<0xFF14>; // Period High & Control
    using NR21      = Singular<0xFF16>; // Length Timer & Duty Cycle
    using NR22      = Singular<0xFF17>; // Volume and Envelope
    using NR23      = Singular<0xFF18>; // Period Low
    using NR24      = Singular<0xFF19>; // Period High & Control
    using NR30      = Singular<0xFF1A>; // DAC Enable
    using NR31      = Singular<0xFF1B>; // Length Timer
    using NR32      = Singular<0xFF1C>; // Output Level
    using NR33      = Singular<0xFF1D>; // Period Low
    using NR34      = Singular<0xFF1E>; // Period High & Control
    using NR41      = Singular<0xFF20>; // Length Timer
    using NR42      = Singular<0xFF21>; // Volume & Envelope
    using NR43      = Singular<0xFF22>; // Frequency & Randomness
    using NR44      = Singular<0xFF23>; // Control
    using NR50      = Singular<0xFF24>; // Master Volume & VIN Panning
    using NR51      = Singular<0xFF25>; // Sound Panning
    using NR52      = Singular<0xFF26>; // Sound On/Off
    using WRAM      = Span<0xFF30, 0xFF40>; // Wave RAM
    
    using LCDC      = Singular<0xFF40>; // LCD Control Register
    using STAT      = Singular<0xFF41>; // LCD Status
    
    using SCY       = Singular<0xFF42>; // Viewport Y Position
    using SCX       = Singular<0xFF43>; // Viewport X Position
    
    using LY        = Singular<0xFF44>; // LCD Y Position
    using LY        = Singular<0xFF45>; // LY Compare
    
    using DMA       = Singular<0xFF46>; // OAM DMA Source Address & Start
    using BGP       = Singular<0xFF47>; // BG Palette Data
    
    using OBP0      = Singular<0xFF48>; // OBJ Palette 0 Data
    using OBP0      = Singular<0xFF49>; // OBJ Palette 1 Data
    
    using WY        = Singular<0xFF4A>; // Window Y Position
    using WX        = Singular<0xFF4B>; // Window X Position
    
    using KEY1      = Singular<0xFF4D>; // Prepare Speed Switch
    
    using VBK       = Singular<0xFF4F>; // VRAM Bank
    using HDMA1     = Singular<0xFF51>; // VRAM DMA Source High
    using HDMA2     = Singular<0xFF52>; // VRAM DMA Source High
    using HDMA3     = Singular<0xFF53>; // VRAM DMA Source High
    using HDMA4     = Singular<0xFF54>; // VRAM DMA Source High
    using HDMA5     = Singular<0xFF55>; // VRAM DMA Source High
    
    using RP        = Singular<0xFF56>; // Infrared Communications Port
    
    using BGPI      = Singular<0xFF68>; // Background Color Palette Index
    using BGPD      = Singular<0xFF69>; // Background Color Palette Data
    
    using OPRI      = Singular<0xFF6C>; // Object Priority Mode
    
    using SVBK      = Singular<0xFF70>; // WRAM Bank
    
    using PCM12     = Singular<0xFF76>; // Audio Digital Outputs 1&2
    using PCM34     = Singular<0xFF77>; // Audio Digital Outputs 3&4
    
    using IE        = Singular<0xFFFF>; // Interrupt Enable Register
}

/** @brief LCDC - LCD Control Register (0=OFF; 1=ON) 
 * @details
 * ---------------------
 * ADDR     BIT     OPTION(0/1)     Description
 * ---------------------
 * FF40     0       OFF/ON          BG Display (for CGB)
 *          1       OFF/ON          OBJ (sprite) display enable
 *          2       8x8/8x16        OBJ (sprite) size
 *          3       Low/High        BG Tile Map Display Select - TODO define Low and High
 *          4       Low/High        BG and Window Tile Map Display Select
 *          5       OFF/ON          Window Display Enable
 *          6       Low/High        Window Tile Map Display Select
 *          7       OFF/ON          LCD Display Enable (1->0 durring VBlank only. White when disabled)
 * 
 * LCDC[0] Performs different actions for different GameBoy Types
 *  - SGB and set to 0
 *      - background becomes blank (white)
 *      - window may still be displayed (LCDC.5)
 *      - sprites may still be displayed (LCDC.1)
 *  - CGB (SGB mode)
 *      - background becomes blank (white)
 *      - window becomes blank(white) LCDC.5 is ignored
 *      - sprites may still be displayed (LCDC.1)
 *  - CGB (CGB mode)
 *      - Background and window lose priority
 *      - sprites will always be displayed on top of background and window
 *          - ignores priority of OAM and BG Map attributes
 */
struct LCDC
{
    using Region = MemoryRegions::LCDC;
    inline constexpr bool for_me(const Addr addr) noexcept
    {
        return Region::isMember(addr);
    }

    inline constexpr void write(const Addr, const Data data) noexcept
    {
        systemState.LCDC.set(data);
    }

    inline constexpr Data read(const Addr) noexcept
    {
        return systemState.LCDC.get();
    }

    LR35902::SystemState& systemState;
};



template<typename RegionT, std::size_t CountV>
class SwitchedRAM
{
    using Region = RegionT;
    static constexpr std::size_t COUNT = CountV;

    void setSwitchIdx(const std::size_t index)
    {
        if (index < COUNT)
            m_index = index;
    }

    std::uint8_t& at(const std::size_t pos)
    {
        if (pos < Region::DISTANCE)
            return m_data[m_index][pos];
        else throw std::out_of_range();
    }

    std::uint8_t& at(const std::size_t pos) const
    {
        if (pos < Region::DISTANCE)
            return m_data[m_index][pos];
        else throw std::out_of_range();
    }

private:
    std::size_t m_index;
    std::array<std::uint8_t, Region::DISTANCE> m_data;
};




template<typename ... MemoryObjects>
struct MMU
{
    [[nodiscard]] inline constexpr bool for_me(const Addr addr) noexcept
    {
        return false;
    }

    inline constexpr void write(const Addr addr, const Data data) noexcept
    {
        return ;
    }
    
    inline constexpr Data read(const Addr addr) noexcept
    {
        return 0;
    }


};


} // namespace MMU
} // namespace LR35902