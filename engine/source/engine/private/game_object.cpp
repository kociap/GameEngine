#include "game_object.hpp"

Game_Object::Game_Object(): Object(), transform(*this) {}

void Game_Object::update() {
    for (auto& [key, vector] : components) {
        for (auto& component_ptr : vector) {
            component_ptr->update();
        }
    }
}