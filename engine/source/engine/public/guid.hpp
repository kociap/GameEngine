#ifndef ENGINE_GUID_HPP_INCLUDE
#define ENGINE_GUID_HPP_INCLUDE

struct Guid {
public:
    Guid();
    Guid(Guid const&) = delete;
    Guid& operator=(Guid const&) = delete;
    Guid(Guid&&) = default;
    Guid& operator=(Guid&&) = default;
    ~Guid() = default;

public:
    uint64_t const value;
};

bool operator==(Guid const&, Guid const&);

#endif //!ENGINE_GUID_HPP_INCLUDE