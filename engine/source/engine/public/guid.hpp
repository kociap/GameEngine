#ifndef ENGINE_GUID_HPP_INCLUDE
#define ENGINE_GUID_HPP_INCLUDE

#include <cstdint>

struct Guid {
public:
    Guid();
    Guid(Guid const&) = delete;
    Guid& operator=(Guid const&) = delete;
    Guid(Guid&&) noexcept = default;
    Guid& operator=(Guid&&) noexcept = default;
    ~Guid() = default;

    uint64_t value() const;

private:
    uint64_t _value;
};

bool operator==(Guid const&, Guid const&);

#endif //!ENGINE_GUID_HPP_INCLUDE