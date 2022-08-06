#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <stdexcept>
#include <functional>
#include <utility/hex.hpp>


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
    Component(T t, const std::uint16_t begin, const std::uint16_t end)
    : component{std::make_unique<ComponentModel<T>>(std::move(t))}
    , begin_{begin}
    , end_{end}
    {}

    Component(const Component& other)
    : component{other.component->clone()}
    , begin_{other.begin_}
    , end_{other.end_}
    {}

    Component& operator=(const Component& other)
    {
        component.reset(other.component->clone().release());
        begin_ = other.begin_;
        end_ = other.end_;
        return *this;
    }

    Component(Component&&) noexcept = default;
    Component& operator=(Component&&) noexcept = default;

    std::uint8_t read(std::uint16_t addr)
    {
        if (!is_valid(addr))
            throw std::invalid_argument("addr needs to be between 0x" + to_hex(begin()) + " and 0x" + to_hex(end()) + ". addr: 0x" + to_hex(addr));
        return component->read(addr);
    }
    void write(std::uint16_t addr, std::uint8_t value)
    {
        if (!is_valid(addr))
            throw std::invalid_argument("addr needs to be between 0x" + to_hex(begin()) + " and 0x" + to_hex(end()) + ". addr: 0x" + to_hex(addr));
        return component->write(addr, value);
    }

    bool is_valid(const std::uint16_t addr) const
    {
        return begin() <= addr && addr <= end();
    }
    std::uint16_t begin() const { return begin_; }
    std::uint16_t end() const { return end_; }

private:
    std::unique_ptr<ComponentConcept> component;
    std::uint16_t begin_;
    std::uint16_t end_;
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