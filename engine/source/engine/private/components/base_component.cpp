#include "components/base_component.hpp"
#include "game_object.hpp"

Base_Component::Base_Component(Game_Object& go) : game_object(go), transform(go.transform) {
    // acquire id
}

Base_Component::~Base_Component() {
    // free id
}

void Base_Component::register_component() {}

void Base_Component::unregister_component() {}

// Events
void Base_Component::on_register() {}
void Base_Component::on_unregister() {}
void Base_Component::update() {}
