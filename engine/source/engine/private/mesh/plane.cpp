#include <mesh/plane.hpp>

// clang-format off
Plane::Plane()
    : Mesh({anton_stl::variadic_construct,
            Vertex({-1, 1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {0, 1}), Vertex({-1, -1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {0, 0}),
            Vertex({1, -1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {1, 0}), Vertex({-1, 1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {0, 1}),
            Vertex({1, -1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {1, 0}), Vertex({1, 1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, {1, 1})},
           {anton_stl::variadic_construct, 0, 1, 2, 3, 4, 5}) {}
// clang-format on
