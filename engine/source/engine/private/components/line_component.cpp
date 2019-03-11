#include "components/line_component.hpp"

#include <utility>

Line_Component::Line_Component(Entity const& entity) : Renderable_Component(entity), mesh_handle(0), shader_handle(0) {}