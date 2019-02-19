#include "id.hpp"

#include "guid.hpp"

ID::ID(Guid const& guid) : _value(guid.value()) {}

uint64_t ID::value() const {
    return _value;
}

bool operator==(ID const& a, ID const& b) {
    return a.value() == b.value();
}

bool operator==(Guid const& a, ID const& b) {
    return a.value() == b.value();
}

bool operator==(ID const& a, Guid const& b) {
    return a.value() == b.value();
}