#pragma once
#include <cstdint>
#include <concepts>
#include <memory>

namespace common
{
template <typename Reader>
concept a_reader = requires (Reader r)
{
    { r.read(static_cast<typename Reader::addr_t>(0)) } -> std::same_as<typename Reader::data_t>;
};

template<typename Writer>
concept a_writer = requires (Writer w)
{
    { w.write(static_cast<typename Writer::addr_t>(0), static_cast<typename Writer::data_t>(0)) } -> std::same_as<void>;
};
}

namespace common::memory
{

namespace ref
{

template<typename AddrT, typename DataT>
struct ReadBase
{
    using addr_t = AddrT;
    using data_t = DataT;

    virtual data_t read(const addr_t) = 0;
    virtual ~ReadBase() = default;
};

template<typename AddrT, typename DataT>
struct WriteBase
{
    using addr_t = AddrT;
    using data_t = DataT;

    virtual void write(const addr_t, const data_t) = 0;
    virtual ~WriteBase() = default;
};

template<typename T, typename AddrT = typename T::addr_t, typename DataT = typename T::data_t>
class ReadModel
:   public ReadBase<AddrT, DataT>
{
private:
    T & m_repr;
public:
    using addr_t = AddrT;
    using data_t = DataT;

    explicit ReadModel(T & repr)
    :   m_repr(repr)
    {}

    data_t read(const addr_t addr) override
    {
        return m_repr.read(addr);
    }
};

template<typename T, typename AddrT = typename T::addr_t, typename DataT = typename T::data_t>
class WriteModel
:   public WriteBase<AddrT, DataT>
{
private:
    T & m_repr;
public:
    using addr_t = AddrT;
    using data_t = DataT;

    explicit WriteModel(T & repr)
    :   m_repr(repr)
    {}

    void write(const addr_t addr, const data_t data) override
    {
        m_repr.write(addr, data);
    }
};

template<typename AddrT, typename DataT>
struct Memory
{
    using addr_t = AddrT;
    using data_t = DataT;

    ReadBase<addr_t, data_t> & m_reader;
    WriteBase<addr_t, data_t> & m_writer;

    Memory(ReadBase<addr_t, data_t>& reader, WriteBase<addr_t, data_t>& writer)
    :   m_reader{reader}
    ,   m_writer{writer}
    {}

    data_t read(const addr_t addr)
    {
        return m_reader.read(addr);
    }
    void write(const addr_t addr, const data_t data)
    {
        return m_writer.write(addr, data);
    }
};

} // namespace ref

namespace value
{

template<typename AddrT, typename DataT>
struct ReadBase
{
    using addr_t = AddrT;
    using data_t = DataT;

    virtual std::unique_ptr<ReadBase> clone() const = 0;
    virtual data_t read(const addr_t) = 0;
    virtual ~ReadBase() = default;
};

template<typename AddrT, typename DataT>
struct WriteBase
{
    using addr_t = AddrT;
    using data_t = DataT;

    virtual std::unique_ptr<WriteBase> clone() const = 0;
    virtual void write(const addr_t, const data_t) = 0;
    virtual ~WriteBase() = default;
};

template<typename T, typename AddrT = typename T::addr_t, typename DataT = typename T::data_t>
class ReadModel
:   public ReadBase<AddrT, DataT>
{
private:
    T m_repr;
public:
    using addr_t = AddrT;
    using data_t = DataT;

    explicit ReadModel(T repr)
    :   m_repr{std::move(repr)}
    {}

    std::unique_ptr<ReadBase<addr_t, data_t>> clone() const override
    {
        return std::make_unique<ReadModel>(m_repr);
    }

    data_t read(const addr_t addr) override
    {
        return m_repr.read(addr);
    }
};

template<typename T, typename AddrT = typename T::addr_t, typename DataT = typename T::data_t>
class WriteModel
:   public WriteBase<AddrT, DataT>
{
private:
    T m_repr;
public:
    using addr_t = AddrT;
    using data_t = DataT;

    explicit WriteModel(T repr)
    :   m_repr{std::move(repr)}
    {}

    std::unique_ptr<WriteBase<addr_t, data_t>> clone() const override
    {
        return std::make_unique<WriteModel>(m_repr);
    }

    void write(const addr_t addr, const data_t data) override
    {
        return m_repr.write(addr, data);
    }
};

template<typename AddrT, typename DataT>
struct Memory
{
    using addr_t = AddrT;
    using data_t = DataT;

    std::unique_ptr<ReadBase<addr_t, data_t>> m_reader;
    std::unique_ptr<WriteBase<addr_t, data_t>> m_writer;

    template<a_reader R, a_writer W>
    Memory(R r, W w)
    :   m_reader{std::make_unique<ReadBase<addr_t, data_t>(std::move(r))}
    ,   m_writer{std::make_unique<WriteBase<W>>(std::move(w))}
    {}
    Memory(const Memory& other)
    :   m_reader{other.m_reader->clone()}
    ,   m_writer{other.m_writer->clone()}
    {}
    Memory(Memory&&) noexcept = default;
    Memory& operator=(Memory rhs)
    {
        std::swap(m_reader, rhs.m_reader);
        std::swap(m_writer, rhs.m_writer);
        return *this;
    }

    ~Memory() = default;

    data_t read(const addr_t addr)
    {
        return m_reader.read(addr);
    }
    void write(const addr_t addr, const data_t data)
    {
        return m_writer.write(addr, data);
    }
};


} // namespace value


} // namespace common::memory