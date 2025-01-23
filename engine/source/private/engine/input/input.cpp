#include <engine/input.hpp>
#include <engine/input/input_internal.hpp>

#include <anton/array.hpp>
#include <anton/flat_hash_map.hpp>
#include <anton/math/math.hpp>
#include <anton/math/vec2.hpp>
#include <anton/string.hpp>
#include <core/logging.hpp>
#include <core/paths.hpp>
#include <engine/time.hpp>

// TODO: Add support for multiple gamepads.
// TODO: Fix any_key.
// TODO: Clean up events.

namespace anton_engine::input {
  struct Action_Mapping {
    anton::String action;
    Key key;
  };

  struct Axis_Mapping {
    anton::String axis;
    Key key;
    // Scale by which to multiply raw value
    f32 raw_value_scale;
    // How fast to accumulate value in units/s
    f32 accumulation_speed;
    f32 raw_value = 0.0f;
    // Smoothed binding value
    f32 value = 0.0f;

    // If raw value changes sign, should we reset to 0 or continue from current value?
    i8 snap;
  };

  struct Event {
    Key key;
    f32 value;

    Event(Key k, f32 v): key(k), value(v) {}
  };

  struct Mouse_Event {
    f32 mouse_x;
    f32 mouse_y;
    f32 wheel;

    Mouse_Event(f32 m_x = 0.0f, f32 m_y = 0.0f, f32 w = 0.0f)
      : mouse_x(m_x), mouse_y(m_y), wheel(w)
    {
    }
  };

  struct Gamepad_Event {
    i32 pad_index;
    Key key;
    f32 value;

    Gamepad_Event(i32 pad_inx, Key k, f32 val)
      : pad_index(pad_inx), key(k), value(val)
    {
    }
  };

  struct Axis {
    anton::String axis;
    f32 value = 0.0f;
    f32 raw_value = 0.0f;

    Axis(anton::String_View const a): axis(a) {}
  };

  struct Action {
    anton::String action;
    Key captured_key = Key::none;
    bool down = false;
    bool pressed = false;
    bool released = false;
    bool bind_press_event = true;
    bool bind_release_event = true;

    Action(anton::String_View const a): action(a) {}
  };

  static anton::Flat_Hash_Map<Key, Key_State> key_states;
  // TODO redesign events
  static anton::Array<Key> key_events_queue;
  static anton::Array<Event> input_event_queue;
  static anton::Array<Mouse_Event> mouse_event_queue;
  static anton::Array<Gamepad_Event> gamepad_event_queue;
  static anton::Array<Gamepad_Event> gamepad_stick_event_queue;

  static anton::Array<Axis_Mapping> axis_mappings;
  static anton::Array<Action_Mapping> action_mappings;
  static anton::Array<Axis> axes;
  static anton::Array<Action> actions;

  // Use radial dead zone for gamepad sticks?
  // Turned on by default
  // If off, axial dead zone will be used instead
  static bool use_radial_deadzone_for_gamepad_sticks = true;

  // TODO TEMPORARY hardcoded deadzone for gamepad sticks
  static f32 gamepad_dead_zone = 0.25f;

  void add_gamepad_event(i32 const pad_index, Key const k, f32 const value)
  {
    if(is_gamepad_stick(k)) {
      gamepad_stick_event_queue.push_back({pad_index, k, value});
    } else {
      gamepad_event_queue.push_back({pad_index, k, value});
    }

    // TODO: Differentiate between pads.
    key_events_queue.push_back(k);
  }

  void add_event(Key const k, f32 const value)
  {
    if(k == Key::mouse_x || k == Key::mouse_y || k == Key::mouse_scroll) {
      mouse_event_queue.push_back({value * f32(k == Key::mouse_x),
                                   value * f32(k == Key::mouse_y),
                                   value * f32(k == Key::mouse_scroll)});
    } else {
      input_event_queue.push_back({k, value});
    }
    key_events_queue.push_back(k);
  }

  static Action_Mapping const*
  find_mapping_with_key(anton::Array<Action_Mapping> const& mappings,
                        anton::String_View const action, Key key)
  {
    for(auto& mapping: mappings) {
      if(mapping.key == key && mapping.action == action) {
        return &mapping;
      }
    }
    return nullptr;
  }

  static void process_mouse_events()
  {
    Mouse_Event current_frame_mouse;
    for(auto [mouse_x, mouse_y, wheel]: mouse_event_queue) {
      current_frame_mouse.mouse_x += mouse_x;
      current_frame_mouse.mouse_y += mouse_y;
      current_frame_mouse.wheel += wheel;
    }
    mouse_event_queue.clear();

    Key_State& mouse_x_key_state =
      key_states.find_or_emplace(Key::mouse_x)->value;
    mouse_x_key_state.value = mouse_x_key_state.raw_value =
      current_frame_mouse.mouse_x;
    Key_State& mouse_y_key_state =
      key_states.find_or_emplace(Key::mouse_y)->value;
    mouse_y_key_state.value = mouse_y_key_state.raw_value =
      current_frame_mouse.mouse_y;
    Key_State& wheel_key_state =
      key_states.find_or_emplace(Key::mouse_scroll)->value;
    wheel_key_state.value = wheel_key_state.raw_value =
      current_frame_mouse.wheel;
  }

  // TODO: Radial dead zone makes axes never reach 1 (values are slightly less than 1, e.g. 0.99996).
  [[nodiscard]] static Vec2 apply_deadzone(Vec2 const stick, f32 const deadzone,
                                           bool const use_radial)
  {
    if(use_radial) {
      f32 const stick_length = math::min(math::length(stick), 1.0f);
      if(stick_length > deadzone) {
        return math::normalize(stick) * (stick_length - deadzone) /
               (1.0f - deadzone);
      } else {
        return Vec2{0.0f, 0.0f};
      }
    } else {
      Vec2 out;
      if(math::abs(stick.x) > deadzone) {
        out.x = (stick.x - math::sign(stick.x) * deadzone) / (1.0f - deadzone);
      } else {
        out.x = 0.0f;
      }

      if(math::abs(stick.y) > deadzone) {
        out.y = (stick.y - math::sign(stick.y) * deadzone) / (1.0f - deadzone);
      } else {
        out.y = 0.0f;
      }
      return out;
    }
  }

  static void process_gamepad_events()
  {
    Vec2 left_stick;
    Vec2 right_stick;

    Key_State& left_stick_x_state =
      key_states.find_or_emplace(Key::gamepad_left_stick_x_axis)->value;
    Key_State& left_stick_y_state =
      key_states.find_or_emplace(Key::gamepad_left_stick_y_axis)->value;
    Key_State& right_stick_x_state =
      key_states.find_or_emplace(Key::gamepad_right_stick_x_axis)->value;
    Key_State& right_stick_y_state =
      key_states.find_or_emplace(Key::gamepad_right_stick_y_axis)->value;

    for(Gamepad_Event const& stick_event: gamepad_stick_event_queue) {
      if(stick_event.key == Key::gamepad_right_stick_x_axis) {
        right_stick.x = stick_event.value;
        right_stick_x_state.raw_value = stick_event.value;
      } else if(stick_event.key == Key::gamepad_right_stick_y_axis) {
        right_stick.y = stick_event.value;
        right_stick_y_state.raw_value = stick_event.value;
      } else if(stick_event.key == Key::gamepad_left_stick_x_axis) {
        left_stick.x = stick_event.value;
        left_stick_x_state.raw_value = stick_event.value;
      } else if(stick_event.key == Key::gamepad_left_stick_y_axis) {
        left_stick.y = stick_event.value;
        left_stick_y_state.raw_value = stick_event.value;
      }
    }

    left_stick = apply_deadzone(left_stick, gamepad_dead_zone,
                                use_radial_deadzone_for_gamepad_sticks);
    right_stick = apply_deadzone(right_stick, gamepad_dead_zone,
                                 use_radial_deadzone_for_gamepad_sticks);

    left_stick_x_state.value = left_stick.x;
    left_stick_y_state.value = left_stick.y;
    right_stick_x_state.value = right_stick.x;
    right_stick_y_state.value = right_stick.y;

    for(auto [pad_index, key, value]: gamepad_event_queue) {
      Key_State& key_state = key_states.find_or_emplace(key)->value;
      key_state.value = key_state.raw_value = value;
      key_state.down = value != 0.0f;
      key_state.up_down_transitioned = true;
    }

    gamepad_stick_event_queue.clear();
    gamepad_event_queue.clear();
  }

  void process_events()
  {
    // TODO add any_key support

    for(auto& [key, key_state]: key_states) {
      key_state.up_down_transitioned = false;
    }

    process_mouse_events();
    process_gamepad_events();

    // Update key_state
    for(auto [key, value]: input_event_queue) {
      Key_State& key_state = key_states.find_or_emplace(key)->value;
      key_state.value = key_state.raw_value = value;
      key_state.up_down_transitioned = (value != 0.0f) != key_state.down;
      key_state.down = value != 0.0f;
    }
    input_event_queue.clear();

    f32 delta_time = get_delta_time();
    for(auto& mapping: axis_mappings) {
      Key_State const& key_state =
        key_states.find_or_emplace(mapping.key)->value;
      if(is_mouse_axis(mapping.key) || is_gamepad_axis(mapping.key)) {
        // Force raw for those axes
        mapping.raw_value = key_state.value;
        mapping.value = math::sign(mapping.accumulation_speed) *
                        mapping.raw_value_scale * key_state.value;
      } else {
        mapping.raw_value = key_state.value;
        f32 raw_scaled = mapping.raw_value_scale * mapping.raw_value;
        if(mapping.snap && raw_scaled != 0 &&
           math::sign(mapping.value) != math::sign(mapping.raw_value)) {
          mapping.value = 0;
        }

        f32 value_delta = math::abs(mapping.accumulation_speed) * delta_time;
        mapping.value = math::step_to_value(
          mapping.value, math::sign(mapping.accumulation_speed) * raw_scaled,
          value_delta);
      }
    }

    for(auto& axis: axes) {
      axis.raw_value = 0;
      axis.value = 0;
      for(auto& mapping: axis_mappings) {
        if(axis.axis == mapping.axis) {
          axis.raw_value += mapping.raw_value_scale * mapping.raw_value;
          axis.value += mapping.value;
        }
      }
    }

    for(auto& action: actions) {
      ANTON_ASSERT(action.bind_press_event || action.bind_release_event,
                   "Action is not bound to any event");
      bool paired_action = action.bind_press_event && action.bind_release_event;
      action.down =
        paired_action &&
        action
          .down; // If not paired, reset every time because we don't know the state
      action.pressed = false;
      action.released = false;
      for(Key const k: key_events_queue) {
        if(action.captured_key == k) {
          // If the captured key is not none, then we have a mapping that allowed us to capture the key
          Key_State const& key_state = key_states.find_or_emplace(k)->value;
          action.down = key_state.down;
          action.pressed = key_state.up_down_transitioned && key_state.down;
          action.released = key_state.up_down_transitioned && !key_state.down;
          if(action.released) {
            action.captured_key = Key::none;
          }
        }

        // Sort of fallthrough because we might have set key to none in the previous if-clause
        if(action.captured_key == Key::none) {
          Action_Mapping const* action_mapping =
            find_mapping_with_key(action_mappings, action.action, k);
          if(action_mapping) {
            Key_State const& key_state = key_states.find_or_emplace(k)->value;
            if(paired_action) {
              action.down = key_state.down;
              action.pressed = key_state.up_down_transitioned && key_state.down;
              action.captured_key = k;
            } else if(action.bind_release_event) {
              // If another unpaired key has been released, keep the relase state
              action.released =
                action.released ||
                (key_state.up_down_transitioned && !key_state.down);
            } else if(action.bind_press_event) {
              // If another unpaired key has been pressed, keep the press state
              action.down = action.down || key_state.down;
              action.pressed =
                action.pressed ||
                (key_state.up_down_transitioned && key_state.down);
            }
          }
        }
      }
    }
    key_events_queue.clear();
  }

  // PUBLIC INTERFACE

  void add_axis(anton::String_View const name, Key const k,
                f32 const raw_value_scale, f32 const accumulation_speed,
                bool const snap)
  {
    Axis_Mapping const new_binding{anton::String(name),
                                   k,
                                   raw_value_scale,
                                   accumulation_speed,
                                   0.0f,
                                   0.0f,
                                   snap};
    bool duplicate = false;
    for(Axis_Mapping& axis_binding: axis_mappings) {
      duplicate = duplicate || (axis_binding.axis == new_binding.axis &&
                                axis_binding.key == new_binding.key);
    }

    if(!duplicate) {
      axis_mappings.push_back(new_binding);
    }

    duplicate = false;
    for(Axis& axis: axes) {
      duplicate = duplicate || (axis.axis == new_binding.axis);
    }

    if(!duplicate) {
      axes.emplace_back(new_binding.axis);
    }
  }

  void add_action(anton::String_View const name, Key const k)
  {
    Action_Mapping const new_binding{anton::String(name), k};
    bool duplicate = false;
    for(Action_Mapping& action_binding: action_mappings) {
      duplicate = duplicate || (action_binding.action == new_binding.action &&
                                action_binding.key == new_binding.key);
    }
    if(!duplicate) {
      action_mappings.push_back(new_binding);
    }

    duplicate = false;
    for(Action& action: actions) {
      duplicate = duplicate || (action.action == new_binding.action);
    }
    if(!duplicate) {
      actions.emplace_back(new_binding.action);
    }
  }

  f32 get_axis(anton::String_View const axis_name)
  {
    for(Axis& axis: axes) {
      if(axis_name == axis.axis) {
        return axis.value;
      }
    }
    ANTON_LOG_WARNING("Unknown axis " + anton::String(axis_name.data()));
    return 0;
  }

  f32 get_axis_raw(anton::String_View const axis_name)
  {
    for(Axis& axis: axes) {
      if(axis_name == axis.axis) {
        return axis.raw_value;
      }
    }
    ANTON_LOG_WARNING("Unknown axis " + anton::String(axis_name.data()));
    return 0;
  }

  Action_State get_action(anton::String_View const action_name)
  {
    for(Action& action: actions) {
      if(action_name == action.action) {
        return {action.down, action.pressed, action.released};
      }
    }
    ANTON_LOG_WARNING("Unknown action " + anton::String(action_name.data()));
    return {};
  }

  Key_State get_key_state(Key k)
  {
    ANTON_ASSERT(k != Key::none && k != Key::any_key,
                 "Key may not be none or any_key");
    return key_states.find_or_emplace(k)->value;
  }

  // Any_Key_State get_any_key_state() {
  //     Manager& input_manager = get_input_manager();
  //     return input_manager.any_key_state;
  // }
} // namespace anton_engine::input
