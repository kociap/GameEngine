#include "components/static_mesh_component.hpp"

#include "components/transform.hpp"
#include "containers/pool.hpp"
#include "engine.hpp"
#include "mesh/mesh.hpp"
#include "mesh/mesh_manager.hpp"

Static_Mesh_Component::Static_Mesh_Component(Entity const& entity) : Renderable_Component(entity), mesh_handle(0), shader_handle(0) {}
