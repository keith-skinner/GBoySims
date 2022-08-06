#pragma once
#include <cstdint>
#include <array>
#include <stdexcept>
#include <string>
#include <charconv>
#include <algorithm>
#include "rom.hpp"
#include "component.hpp"

struct Mmu
{
    std::uint8_t read(std::uint16_t addr)
    {
        Component& component = *std::find_if(std::begin(components), std::end(components),
        [=](const Component& c){
            return c.is_valid(addr);
        });
        return component.read(addr);
    }
    void write(std::uint16_t addr, std::uint8_t value)
    {
        Component& component = *std::find_if(std::begin(components), std::end(components),
        [=](const Component& c){
            return c.is_valid(addr);
        });
        return component.write(addr, value);
    }

    template<typename T>
    void addComponent(T t, const std::uint16_t begin, const std::uint16_t end)
    {
        
    }

    void addComponent(Component component)
    {
        
    }
    std::vector<Component> components;
};
