#include "guid.hpp"

#include <queue>

static uint64_t highest_unused = 1;

Guid::Guid() : _value(highest_unused++) {}

uint64_t Guid::value() const {
    return _value;
}

bool operator==(Guid const& lhs, Guid const& rhs) {
    return lhs.value() == rhs.value();
}