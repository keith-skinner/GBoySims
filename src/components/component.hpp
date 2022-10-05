#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <stdexcept>
#include <functional>
#include <utility/hex.hpp>

struct bad_read_address : std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
    bad_read_address(std::string_view component, uint16_t addr)
    : invalid_argument(std::string(component.size(), component.length()) + " invalid read at address: " + to_hex(addr))
    {}
};

struct bad_write_address : std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
    bad_write_address(std::string_view component, uint16_t addr)
    : invalid_argument(std::string(component.size(), component.length()) + " invalid write at address: " + to_hex(addr))
    {}
};

struct ComponentConcept
{
    virtual std::uint8_t read(std::uint16_t addr) = 0;
    virtual void write(std::uint16_t addr, std::uint8_t value) = 0;
    virtual std::unique_ptr<ComponentConcept> clone() = 0;
    virtual ~ComponentConcept() = default;
};
template<typename T>
struct ComponentModel : public ComponentConcept
{
    explicit ComponentModel(T t) : component(std::move(t)) {}
    std::unique_ptr<ComponentConcept> clone() override
    {
        return std::make_unique<ComponentModel>(component);
    }
    std::uint8_t read(const std::uint16_t addr) override
    {
        return component.read(addr);
    }
    void write(const std::uint16_t addr, const std::uint8_t value) override
    {
        return component.write(addr, value);
    }
    T component;
};

class Component
{
public:
    template<typename T>
    Component(T t)
    : component{std::make_unique<ComponentModel<T>>(std::move(t))}
    {}

    Component(const Component& other)
    : component{other.component->clone()}
    {}

    Component& operator=(const Component& other)
    {
        component.reset(other.component->clone().release());
        return *this;
    }

    Component(Component&&) noexcept = default;
    Component& operator=(Component&&) noexcept = default;

    std::uint8_t read(std::uint16_t addr) const
    {
        return component->read(addr);
    }
    void write(std::uint16_t addr, std::uint8_t value)
    {
        return component->write(addr, value);
    }

private:
    std::unique_ptr<ComponentConcept> component;
};

namespace Components {
struct DefaultComponent
{
    virtual std::uint8_t read(const std::uint16_t) {
        throw std::runtime_error("Invalid component");
    }
    virtual void write(const std::uint16_t, const std::uint8_t) {
        throw std::runtime_error("Invalid component");
    }
};
}