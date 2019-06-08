#include "ecs/component_container.hpp"
#include "ecs/component_registry.hpp"

// clang-format off
#include "components/transform.hpp"
#include "components/static_mesh_component.hpp"
#include "components/point_light_component.hpp"
#include "components/line_component.hpp"
#include "components/directional_light_component.hpp"
#include "components/spot_light_component.hpp"
#include "components/line_component.hpp"
#include "components/camera.hpp"
#include "camera_movement.hpp"
#include "debug_hotkeys.hpp"


Component_Registry::Component_Registry(): 
    serialization_funcs{
        {Type_Family::family_id<Transform>(), {&Component_Container<Transform>::serialize, &Component_Container<Transform>::deserialize}},
        {Type_Family::family_id<Static_Mesh_Component>(), {&Component_Container<Static_Mesh_Component>::serialize, &Component_Container<Static_Mesh_Component>::deserialize}},
        {Type_Family::family_id<Line_Component>(), {&Component_Container<Line_Component>::serialize, &Component_Container<Line_Component>::deserialize}},
        {Type_Family::family_id<Point_Light_Component>(), {&Component_Container<Point_Light_Component>::serialize, &Component_Container<Point_Light_Component>::deserialize}},
        {Type_Family::family_id<Directional_Light_Component>(), {&Component_Container<Directional_Light_Component>::serialize, &Component_Container<Directional_Light_Component>::deserialize}},
        {Type_Family::family_id<Spot_Light_Component>(), {&Component_Container<Spot_Light_Component>::serialize, &Component_Container<Spot_Light_Component>::deserialize}},
        {Type_Family::family_id<Camera>(), {&Component_Container<Camera>::serialize, &Component_Container<Camera>::deserialize}},
        {Type_Family::family_id<Camera_Movement>(), {&Component_Container<Camera_Movement>::serialize, &Component_Container<Camera_Movement>::deserialize}},
        {Type_Family::family_id<Debug_Hotkeys>(), {&Component_Container<Debug_Hotkeys>::serialize, &Component_Container<Debug_Hotkeys>::deserialize}}
    } {}
