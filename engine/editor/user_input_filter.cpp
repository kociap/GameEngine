#include <user_input_filter.hpp>

#include <debug_macros.hpp>
#include <editor.hpp>
#include <engine.hpp>
#include <input/input_core.hpp>
#include <key.hpp>

ANTON_DISABLE_WARNINGS()
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
ANTON_RESTORE_WARNINGS()

User_Input_Filter::User_Input_Filter(int32_t const initial_x, int32_t const initial_y): last_cursor_pos_x(initial_x), last_cursor_pos_y(initial_y) {}

bool User_Input_Filter::eventFilter(QObject* const object, QEvent* const event) {
    static std::unordered_map<Qt::Key, Key> keyboard_buttons{
        {Qt::Key_Escape, Key::escape},
        {Qt::Key_Tab, Key::tab},
        {Qt::Key_Backspace, Key::backspace},
        {Qt::Key_Return, Key::enter}, // TODO differentiate between enter and return (maybe)
        {Qt::Key_Enter, Key::enter},
        {Qt::Key_Insert, Key::insert},
        {Qt::Key_Delete, Key::delete_key},
        //{Qt::Key_Pause, },
        {Qt::Key_Home, Key::home},
        {Qt::Key_End, Key::end},
        {Qt::Key_Left, Key::left},
        {Qt::Key_Up, Key::up},
        {Qt::Key_Right, Key::right},
        {Qt::Key_Down, Key::down},
        {Qt::Key_PageUp, Key::page_up},
        {Qt::Key_PageDown, Key::page_down},
        {Qt::Key_Shift, Key::left_shift}, // TODO which shift
        //{Qt::Key_Control, },
        // {Qt::Key_Meta,}, Windows/Control key
        {Qt::Key_Alt, Key::left_alt},   // TODO which alt
        {Qt::Key_AltGr, Key::left_alt}, // TODO which alt
        {Qt::Key_CapsLock, Key::caps_lock},
        {Qt::Key_NumLock, Key::num_lock},
        //{Qt::Key_ScrollLock,},
        {Qt::Key_F1, Key::f1},
        {Qt::Key_F2, Key::f2},
        {Qt::Key_F3, Key::f3},
        {Qt::Key_F4, Key::f4},
        {Qt::Key_F5, Key::f5},
        {Qt::Key_F6, Key::f6},
        {Qt::Key_F7, Key::f7},
        {Qt::Key_F8, Key::f8},
        {Qt::Key_F9, Key::f9},
        {Qt::Key_F10, Key::f10},
        {Qt::Key_F11, Key::f11},
        {Qt::Key_F12, Key::f12},
        {Qt::Key_Space, Key::spacebar},
        {Qt::Key_Any, Key::any_key},
        //{Qt::Key_Exclam,},
        //{Qt::Key_QuoteDbl,},
        //{Qt::Key_Dollar,},
        //{Qt::Key_Percent,},
        //{Qt::Key_Ampersand,},
        //{Qt::Key_Apostrophe,},
        //{Qt::Key_ParenLeft,},
        //{Qt::Key_ParenRight,},
        //{Qt::Key_Asterisk,},
        //{Qt::Key_Plus,},
        //{Qt::Key_Comma,},
        //{Qt::Key_Minus,},
        //{Qt::Key_Period,},
        //{Qt::Key_Slash,},
        {Qt::Key_0, Key::zero},
        {Qt::Key_1, Key::one},
        {Qt::Key_2, Key::two},
        {Qt::Key_3, Key::three},
        {Qt::Key_4, Key::four},
        {Qt::Key_5, Key::five},
        {Qt::Key_6, Key::six},
        {Qt::Key_7, Key::seven},
        {Qt::Key_8, Key::eight},
        {Qt::Key_9, Key::nine},
        //{Qt::Key_Colon,},
        //{Qt::Key_Semicolon,},
        //{Qt::Key_Less,},
        //{Qt::Key_Equal,},
        //{Qt::Key_Greater,},
        //{Qt::Key_Question,},
        //{Qt::Key_At,},
        {Qt::Key_A, Key::a},
        {Qt::Key_B, Key::b},
        {Qt::Key_C, Key::c},
        {Qt::Key_D, Key::d},
        {Qt::Key_E, Key::e},
        {Qt::Key_F, Key::f},
        {Qt::Key_G, Key::g},
        {Qt::Key_H, Key::h},
        {Qt::Key_I, Key::i},
        {Qt::Key_J, Key::j},
        {Qt::Key_K, Key::k},
        {Qt::Key_L, Key::l},
        {Qt::Key_M, Key::m},
        {Qt::Key_N, Key::n},
        {Qt::Key_O, Key::o},
        {Qt::Key_P, Key::p},
        {Qt::Key_Q, Key::q},
        {Qt::Key_R, Key::r},
        {Qt::Key_S, Key::s},
        {Qt::Key_T, Key::t},
        {Qt::Key_U, Key::u},
        {Qt::Key_V, Key::v},
        {Qt::Key_W, Key::w},
        {Qt::Key_X, Key::x},
        {Qt::Key_Y, Key::y},
        {Qt::Key_Z, Key::z},
        //{Qt::Key_BracketLeft,},
        //{Qt::Key_Backslash,},
        //{Qt::Key_BracketRight,},
        //{Qt::Key_Underscore,},
        //{Qt::Key_QuoteLeft,},
        //{Qt::Key_BraceLeft,},
        //{Qt::Key_Bar,},
        //{Qt::Key_BraceRight,},
        //{Qt::Key_AsciiTilde,}
    };

    static std::unordered_map<Qt::MouseButton, Key> mouse_buttons{
        {Qt::MouseButton::LeftButton, Key::left_mouse_button},     {Qt::MouseButton::RightButton, Key::right_mouse_button},
        {Qt::MouseButton::MiddleButton, Key::middle_mouse_button}, {Qt::MouseButton::XButton1, Key::thumb_mouse_button_1},
        {Qt::MouseButton::XButton2, Key::thumb_mouse_button_2},    {Qt::MouseButton::ExtraButton3, Key::mouse_button_6},
        {Qt::MouseButton::ExtraButton4, Key::mouse_button_7},      {Qt::MouseButton::ExtraButton5, Key::mouse_button_8},
    };

    // TODO consider adding debug warnings to all the cases that we do not handle
    Input::Manager& input_manager = get_input_manager();
    switch (event->type()) {
        case QEvent::Type::KeyPress:
        case QEvent::Type::KeyRelease: {
            QKeyEvent* const key_event = static_cast<QKeyEvent*>(event);
            auto iter = keyboard_buttons.find(static_cast<Qt::Key>(key_event->key()));
            if (iter != keyboard_buttons.end()) {
                float value = event->type() == QEvent::Type::KeyPress;
                input_manager.add_event(Input::Event{iter->second, value});
                return true;
            } else {
                return false;
            }
        }

        case QEvent::Type::MouseButtonPress:
        case QEvent::Type::MouseButtonRelease: {
            QMouseEvent* const mouse_event = static_cast<QMouseEvent*>(event);
            auto iter = mouse_buttons.find(static_cast<Qt::MouseButton>(mouse_event->button()));
            if (iter != mouse_buttons.end()) {
                float value = event->type() == QEvent::Type::MouseButtonPress;
                input_manager.add_event(Input::Event(iter->second, value));
                return true;
            } else {
                return false;
            }
        }

        case QEvent::Type::MouseMove: {
            QMouseEvent* const mouse_event = static_cast<QMouseEvent*>(event);
            int32_t delta_x = mouse_event->globalX() - last_cursor_pos_x;
            int32_t delta_y = last_cursor_pos_y - mouse_event->globalY();
            last_cursor_pos_x = mouse_event->globalX();
            last_cursor_pos_y = mouse_event->globalY();
            input_manager.add_event(Input::Mouse_Event(delta_x, delta_y, 0.0f));
            return true;
        }

        case QEvent::Type::Wheel: {
            QWheelEvent* const wheel_event = static_cast<QWheelEvent*>(event);
            float delta_y = wheel_event->pixelDelta().y();
            input_manager.add_event(Input::Mouse_Event(0.0f, 0.0f, delta_y));
            return true;
        }

        case QEvent::Type::Close: {
            Editor::quit();
            return true;
        }

        default:
            return QObject::eventFilter(object, event); // We do not process the event, so we let Qt do it for us
    }
}
