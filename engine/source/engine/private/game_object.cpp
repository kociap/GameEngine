#include "game_object.hpp"

void Game_Object::update() {
    for (auto& [key, vector] : components) {
        for (auto& component_ptr : vector) {
            component_ptr->update();
        }
    }
}