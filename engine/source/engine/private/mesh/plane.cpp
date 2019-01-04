#include "mesh/plane.hpp"

Plane::Plane()
    : Mesh({Vertex(Vector3(-1.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f)),
            Vertex(Vector3(-1.0f, -1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f)),
            Vertex(Vector3(1.0f, -1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f)),
            Vertex(Vector3(-1.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f)),
            Vertex(Vector3(1.0f, -1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f)),
            Vertex(Vector3(1.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f))},
           {0, 1, 2, 3, 4, 5}, {}) {}