#ifndef ENGINE_ID_HPP_INCLUDE
#define ENGINE_ID_HPP_INCLUDE

#include <cstdint>

class Guid;

// Non-unique identifier class
// value is never equal 0
class ID {
public:
    ID(Guid const&);

    uint64_t value() const;

private:
    uint64_t _value;
};

bool operator==(ID const&, ID const&);
bool operator==(Guid const&, ID const&);
bool operator==(ID const&, Guid const&);

#endif //!ENGINE_ID_HPP_INCLUDE