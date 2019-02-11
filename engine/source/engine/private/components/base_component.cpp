#include "components/base_component.hpp"
#include "game_object.hpp"

Base_Component::Base_Component(Game_Object& go) : game_object(go) {}

Base_Component::~Base_Component() {}

Transform& Base_Component::get_transform() {
    return game_object.get_component<Transform>();
}
