#include "components/base_component.hpp"
#include "game_object.hpp"

Base_Component::Base_Component(Game_Object& go) : game_object(go), transform(go.transform) {}

Base_Component::~Base_Component() {}

// Events
void Base_Component::on_register() {}
void Base_Component::on_unregister() {}
