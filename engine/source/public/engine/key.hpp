#ifndef ENGINE_KEY_HPP_INCLUDE
#define ENGINE_KEY_HPP_INCLUDE

#include <core/atl/string_view.hpp>

namespace anton_engine {
    enum class Key {
        none,
        any_key,

        mouse_x,
        mouse_y,
        mouse_scroll,
        mouse_horiz_scroll,
        left_mouse_button,
        right_mouse_button,
        middle_mouse_button,
        thumb_mouse_button_1,
        thumb_mouse_button_2,
        mouse_button_6,
        mouse_button_7,
        mouse_button_8,

        escape,
        enter,
        tab,
        caps_lock,
        spacebar,
        backspace,
        left_alt,
        right_alt,
        left_shift,
        right_shift,
        left_control,
        right_control,

        numpad_0,
        numpad_1,
        numpad_2,
        numpad_3,
        numpad_4,
        numpad_5,
        numpad_6,
        numpad_7,
        numpad_8,
        numpad_9,
        numpad_add,
        numpad_subtract,
        numpad_divide,
        numpad_multiply,
        numpad_decimal,
        numpad_enter,
        numlock,

        left,
        right,
        up,
        down,

        insert,
        home,
        page_up,
        page_down,
        end,
        del,

        zero,
        one,
        two,
        three,
        four,
        five,
        six,
        seven,
        eight,
        nine,

        a,
        b,
        c,
        d,
        e,
        f,
        g,
        h,
        i,
        j,
        k,
        l,
        m,
        n,
        o,
        p,
        q,
        r,
        s,
        t,
        u,
        v,
        w,
        x,
        y,
        z,

        slash,
        comma,
        dot,
        semicolon,
        left_bracket,
        right_bracket,
        minus,
        equals,
        apostrophe,
        tick,
        backward_slash,

        f1,
        f2,
        f3,
        f4,
        f5,
        f6,
        f7,
        f8,
        f9,
        f10,
        f11,
        f12,

        pause,

        gamepad_left_stick_x_axis,
        gamepad_left_stick_y_axis,
        gamepad_right_stick_x_axis,
        gamepad_right_stick_y_axis,
        gamepad_left_trigger,
        gamepad_right_trigger,

        gamepad_button_0,
        gamepad_button_1,
        gamepad_button_2,
        gamepad_button_3,
        gamepad_button_4,
        gamepad_button_5,
        gamepad_button_6,
        gamepad_button_7,
        gamepad_button_8,
        gamepad_button_9,
        gamepad_button_10,
        gamepad_button_11,
        gamepad_button_12,
        gamepad_button_13,
        gamepad_button_14,
        gamepad_button_15,
        gamepad_button_16,
        gamepad_button_17,
        gamepad_button_18,
        gamepad_button_19,
    };

    [[nodiscard]] inline bool is_mouse_axis(Key const k) {
        return k == Key::mouse_x | k == Key::mouse_y | k == Key::mouse_scroll;
    }

    [[nodiscard]] inline bool is_gamepad_axis(Key const k) {
        return k == Key::gamepad_right_stick_x_axis | k == Key::gamepad_right_stick_y_axis | k == Key::gamepad_left_stick_x_axis |
               k == Key::gamepad_left_stick_y_axis | k == Key::gamepad_left_trigger | k == Key::gamepad_right_trigger;
    }

    [[nodiscard]] inline bool is_gamepad_stick(Key const k) {
        return k == Key::gamepad_right_stick_x_axis | k == Key::gamepad_right_stick_y_axis | k == Key::gamepad_left_stick_x_axis |
               k == Key::gamepad_left_stick_y_axis;
    }

    atl::String_View key_to_string(Key);
    Key key_from_string(atl::String_View);
} // namespace anton_engine

#endif // !ENGINE_KEY_HPP_INCLUDE
