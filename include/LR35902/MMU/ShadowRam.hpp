#ifndef LR35902_MMU_SHADOWRAM_HPP
#define LR35902_MMU_SHADOWRAM_HPP

template<typename RegionT, typename ShadowedT, Data OffsetV = Data{0}>
class ShadowedRAM
{
public:
    using Region = RegionT;
    using Shadowed = ShadowedT;
    using ShadowedRegion = Shadowed::typename Region;
    static constexpr Data OFFSET = OffsetV;

    static_assert(
        Shadowed::for_me(OFFSET),
        "Offset must be a value inside the shadowed region.");

    static_assert(
        Shadowed::for_me(toShadowedAddr(Region::min()))
    &&  Shadowed::for_me(toShadowedAddr(Region::max())),
        "All values of Region must map to Shadowed::Region.");

    
    [[nodiscard]] inline static constexpr
    Addr toShadowedAddr(const Addr addr) noexcept
    {
        return addr - Region::min() + OFFSET;
    }
    
    [[nodiscard]] inline static constexpr
    bool for_me(const Addr addr) noexcept
    {
        return Region::contains(addr);
    }
    
    inline constexpr 
    void write(const Addr addr, const Data data) noexcept
    {
        m_shadowed.write(toShadowedAddr(addr), data);
    }
    
    [[nodiscard]] inline constexpr 
    Data read(const Addr addr) noexcept
    {
        return m_shadowed.read(toShadowedAddr(addr));
    }

private:
    Shadowed m_shadowed;
};

#endif // LR35902_MMU_SHADOWRAM_HPP