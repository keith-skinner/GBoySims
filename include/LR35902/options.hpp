#ifndef LR35902_OPTIONS_HPP
#define LR35902_OPTIONS_HPP

#ifndef FORCE_FLAGS_REG_READ
#define FORCE_FLAGS_REG_READ false
#endif // FORCE_FLAGS_REG_READ


namespace options
{
static constexpr bool ForceFlagsRegRead = FORCE_FLAGS_REG_READ;

}

#endif // LR35902_OPTIONS_HPP