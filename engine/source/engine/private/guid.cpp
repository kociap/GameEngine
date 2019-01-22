#include "guid.hpp"

#include <queue>

static uint64_t highest_unused = 0;

Guid::Guid(): value(highest_unused++) {}

bool operator==(Guid const& lhs, Guid const& rhs) {
    return lhs.value == rhs.value;
}