#ifndef LR35902_MMU_RAM_HPP
#define LR35902_MMU_RAM_HPP

template<typename RegionT>
class RAM
{
public:
    using Region = RegionT;

    [[nodiscard]] inline constexpr static bool for_me(const Addr addr) { return Region::contains(addr); }

    inline constexpr void write(const Addr addr, const Data data) noexcept {
        m_data[addr-Region::min()] = data;
    }

    [[nodiscard]] inline constexpr Data read(const Addr addr) noexcept {
        return m_data[addr-Region::min()];
    }

private:
    std::array<Data, Region::DISTANCE> m_data;
};

#endif // LR35902_MMU_RAM_HPP