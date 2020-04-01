#include <windowing/window.hpp>

#include <core/anton_crt.hpp>
#include <core/assert.hpp>
#include <core/atl/vector.hpp>
#include <core/diagnostic_macros.hpp>
#include <core/exception.hpp>
#include <core/atl/fixed_array.hpp>

#include <mimas/mimas_gl.h>

namespace anton_engine::windowing {
    class Window {
    public:
        Mimas_Window* mimas_window = nullptr;
        struct {
            window_resize_function window_resize = nullptr;
            window_activate_function window_activate = nullptr;
            cursor_pos_function cursor_pos = nullptr;
            mouse_button_function mouse_button = nullptr;
            scroll_function scroll = nullptr;
            key_function key = nullptr;
            void* window_resize_data = nullptr;
            void* window_activate_data = nullptr;
            void* cursor_pos_data = nullptr;
            void* mouse_button_data = nullptr;
            void* scroll_data = nullptr;
            void* key_data = nullptr;
        } callbacks;
    };

    class Windowing {
    public:
        struct {
            joystick_function joystick;
            void* joystick_data;
        } callbacks;
        atl::Vector<Window*> windows;
    };

    static Windowing windowing;

    bool init() {
        if (!mimas_init_with_gl()) {
            return false;
        }

        windowing.callbacks.joystick = nullptr;
        windowing.callbacks.joystick_data = nullptr;

        return true;
    }

    void terminate() {
        windowing.callbacks.joystick = nullptr;
        windowing.callbacks.joystick_data = nullptr;

        for (Window* window: windowing.windows) {
            destroy_window(window);
        }

        mimas_terminate();
    }

    void poll_events() {
        mimas_poll_events();
    }

    Display* get_primary_display() {
        return reinterpret_cast<Display*>(mimas_get_primary_display());
    }

    void fullscreen_window(Window* window, Display* display) {
        mimas_fullscreen_window(window->mimas_window, reinterpret_cast<Mimas_Display*>(display));
    }

    void _window_activate_callback(Mimas_Window* const, mimas_i32 const activated, void* const data){
        Window* const window = reinterpret_cast<Window*>(data);
        if(window->callbacks.window_activate) {
            window->callbacks.window_activate(window, activated, window->callbacks.window_activate_data);
        }
    }

    static atl::Fixed_Array<Key, 256> create_mimas_to_key_map() {
        atl::Fixed_Array<Key, 256> key_map(256, Key::none);
        key_map[MIMAS_MOUSE_LEFT_BUTTON] = Key::left_mouse_button;
        key_map[MIMAS_MOUSE_RIGHT_BUTTON] = Key::right_mouse_button;
        key_map[MIMAS_MOUSE_MIDDLE_BUTTON] = Key::middle_mouse_button;
        key_map[MIMAS_MOUSE_THUMB_BUTTON_1] = Key::thumb_mouse_button_1;
        key_map[MIMAS_MOUSE_THUMB_BUTTON_2] = Key::thumb_mouse_button_2;
        key_map[MIMAS_MOUSE_WHEEL] = Key::mouse_scroll;
        key_map[MIMAS_MOUSE_HORIZ_WHEEL] = Key::mouse_horiz_scroll;
        key_map[MIMAS_KEY_0] = Key::zero;
        key_map[MIMAS_KEY_1] = Key::one;
        key_map[MIMAS_KEY_2] = Key::two;
        key_map[MIMAS_KEY_3] = Key::three;
        key_map[MIMAS_KEY_4] = Key::four;
        key_map[MIMAS_KEY_5] = Key::five;
        key_map[MIMAS_KEY_6] = Key::six;
        key_map[MIMAS_KEY_7] = Key::seven;
        key_map[MIMAS_KEY_8] = Key::eight;
        key_map[MIMAS_KEY_9] = Key::nine;
        key_map[MIMAS_KEY_A] = Key::a;
        key_map[MIMAS_KEY_B] = Key::b;
        key_map[MIMAS_KEY_C] = Key::c;
        key_map[MIMAS_KEY_D] = Key::d;
        key_map[MIMAS_KEY_E] = Key::e;
        key_map[MIMAS_KEY_F] = Key::f;
        key_map[MIMAS_KEY_G] = Key::g;
        key_map[MIMAS_KEY_H] = Key::h;
        key_map[MIMAS_KEY_I] = Key::i;
        key_map[MIMAS_KEY_J] = Key::j;
        key_map[MIMAS_KEY_K] = Key::k;
        key_map[MIMAS_KEY_L] = Key::l;
        key_map[MIMAS_KEY_M] = Key::m;
        key_map[MIMAS_KEY_N] = Key::n;
        key_map[MIMAS_KEY_O] = Key::o;
        key_map[MIMAS_KEY_P] = Key::p;
        key_map[MIMAS_KEY_Q] = Key::q;
        key_map[MIMAS_KEY_R] = Key::r;
        key_map[MIMAS_KEY_S] = Key::s;
        key_map[MIMAS_KEY_T] = Key::t;
        key_map[MIMAS_KEY_U] = Key::u;
        key_map[MIMAS_KEY_V] = Key::v;
        key_map[MIMAS_KEY_W] = Key::w;
        key_map[MIMAS_KEY_X] = Key::x;
        key_map[MIMAS_KEY_Y] = Key::y;
        key_map[MIMAS_KEY_Z] = Key::z;
        key_map[MIMAS_KEY_SLASH] = Key::slash;
        key_map[MIMAS_KEY_COMMA] = Key::comma;
        key_map[MIMAS_KEY_DOT] = Key::dot;
        key_map[MIMAS_KEY_SEMICOLON] = Key::semicolon;
        key_map[MIMAS_KEY_LEFT_BRACKET] = Key::left_bracket;
        key_map[MIMAS_KEY_RIGHT_BRACKET] = Key::right_bracket;
        key_map[MIMAS_KEY_MINUS] = Key::minus;
        key_map[MIMAS_KEY_EQUALS] = Key::equals;
        key_map[MIMAS_KEY_APOSTROPHE] = Key::apostrophe;
        key_map[MIMAS_KEY_TICK] = Key::tick;
        key_map[MIMAS_KEY_BACKWARD_SLASH] = Key::backward_slash;
        key_map[MIMAS_KEY_F1] = Key::f1;
        key_map[MIMAS_KEY_F2] = Key::f2;
        key_map[MIMAS_KEY_F3] = Key::f3;
        key_map[MIMAS_KEY_F4] = Key::f4;
        key_map[MIMAS_KEY_F5] = Key::f5;
        key_map[MIMAS_KEY_F6] = Key::f6;
        key_map[MIMAS_KEY_F7] = Key::f7;
        key_map[MIMAS_KEY_F8] = Key::f8;
        key_map[MIMAS_KEY_F9] = Key::f9;
        key_map[MIMAS_KEY_F10] = Key::f10;
        key_map[MIMAS_KEY_F11] = Key::f11;
        key_map[MIMAS_KEY_F12] = Key::f12;
        key_map[MIMAS_KEY_LEFT] = Key::left;
        key_map[MIMAS_KEY_RIGHT] = Key::right;
        key_map[MIMAS_KEY_UP] = Key::up;
        key_map[MIMAS_KEY_DOWN] = Key::down;
        key_map[MIMAS_KEY_CAPS_LOCK] = Key::caps_lock;
        key_map[MIMAS_KEY_TAB] = Key::tab;
        key_map[MIMAS_KEY_PAGE_UP] = Key::page_up;
        key_map[MIMAS_KEY_PAGE_DOWN] = Key::page_down;
        key_map[MIMAS_KEY_HOME] = Key::home;
        key_map[MIMAS_KEY_END] = Key::end;
        key_map[MIMAS_KEY_INSERT] = Key::insert;
        key_map[MIMAS_KEY_DELETE] = Key::del;
        key_map[MIMAS_KEY_BACKSPACE] = Key::backspace;
        key_map[MIMAS_KEY_SPACE] = Key::spacebar;
        key_map[MIMAS_KEY_ENTER] = Key::enter;
        key_map[MIMAS_KEY_ESCAPE] = Key::escape;
        key_map[MIMAS_KEY_LEFT_SHIFT] = Key::left_shift;
        key_map[MIMAS_KEY_RIGHT_SHIFT] = Key::right_shift;
        key_map[MIMAS_KEY_LEFT_CONTROL] = Key::left_control;
        key_map[MIMAS_KEY_RIGHT_CONTROL] = Key::right_control;
        key_map[MIMAS_KEY_LEFT_ALT] = Key::left_alt;
        key_map[MIMAS_KEY_RIGHT_ALT] = Key::right_alt;
        key_map[MIMAS_KEY_NUMPAD_0] = Key::numpad_0;
        key_map[MIMAS_KEY_NUMPAD_1] = Key::numpad_1;
        key_map[MIMAS_KEY_NUMPAD_2] = Key::numpad_2;
        key_map[MIMAS_KEY_NUMPAD_3] = Key::numpad_3;
        key_map[MIMAS_KEY_NUMPAD_4] = Key::numpad_4;
        key_map[MIMAS_KEY_NUMPAD_5] = Key::numpad_5;
        key_map[MIMAS_KEY_NUMPAD_6] = Key::numpad_6;
        key_map[MIMAS_KEY_NUMPAD_7] = Key::numpad_7;
        key_map[MIMAS_KEY_NUMPAD_8] = Key::numpad_8;
        key_map[MIMAS_KEY_NUMPAD_9] = Key::numpad_9;
        key_map[MIMAS_KEY_NUMPAD_ADD] = Key::numpad_add;
        key_map[MIMAS_KEY_NUMPAD_SUBTRACT] = Key::numpad_subtract;
        key_map[MIMAS_KEY_NUMPAD_DIVIDE] = Key::numpad_divide;
        key_map[MIMAS_KEY_NUMPAD_MULTIPLY] = Key::numpad_multiply;
        key_map[MIMAS_KEY_NUMPAD_DECIMAL] = Key::numpad_decimal;
        key_map[MIMAS_KEY_NUMPAD_ENTER] = Key::numpad_enter;
        key_map[MIMAS_KEY_NUMLOCK] = Key::numlock;
        return key_map;
    }

    static atl::Fixed_Array<i32, 256> create_key_to_mimas_map() {
        atl::Fixed_Array<i32, 256> key_map{256, MIMAS_KEY_UNKNOWN};
        key_map[static_cast<i32>(Key::left_mouse_button)] = MIMAS_MOUSE_LEFT_BUTTON;
        key_map[static_cast<i32>(Key::right_mouse_button)] = MIMAS_MOUSE_RIGHT_BUTTON;
        key_map[static_cast<i32>(Key::middle_mouse_button)] = MIMAS_MOUSE_MIDDLE_BUTTON;
        key_map[static_cast<i32>(Key::thumb_mouse_button_1)] = MIMAS_MOUSE_THUMB_BUTTON_1;
        key_map[static_cast<i32>(Key::thumb_mouse_button_2)] = MIMAS_MOUSE_THUMB_BUTTON_2;
        key_map[static_cast<i32>(Key::mouse_scroll)] = MIMAS_MOUSE_WHEEL;
        key_map[static_cast<i32>(Key::mouse_horiz_scroll)] = MIMAS_MOUSE_HORIZ_WHEEL;
        key_map[static_cast<i32>(Key::zero)] = MIMAS_KEY_0;
        key_map[static_cast<i32>(Key::one)] = MIMAS_KEY_1;
        key_map[static_cast<i32>(Key::two)] = MIMAS_KEY_2;
        key_map[static_cast<i32>(Key::three)] = MIMAS_KEY_3;
        key_map[static_cast<i32>(Key::four)] = MIMAS_KEY_4;
        key_map[static_cast<i32>(Key::five)] = MIMAS_KEY_5;
        key_map[static_cast<i32>(Key::six)] = MIMAS_KEY_6;
        key_map[static_cast<i32>(Key::seven)] = MIMAS_KEY_7;
        key_map[static_cast<i32>(Key::eight)] = MIMAS_KEY_8;
        key_map[static_cast<i32>(Key::nine)] = MIMAS_KEY_9;
        key_map[static_cast<i32>(Key::a)] = MIMAS_KEY_A;
        key_map[static_cast<i32>(Key::b)] = MIMAS_KEY_B;
        key_map[static_cast<i32>(Key::c)] = MIMAS_KEY_C;
        key_map[static_cast<i32>(Key::d)] = MIMAS_KEY_D;
        key_map[static_cast<i32>(Key::e)] = MIMAS_KEY_E;
        key_map[static_cast<i32>(Key::f)] = MIMAS_KEY_F;
        key_map[static_cast<i32>(Key::g)] = MIMAS_KEY_G;
        key_map[static_cast<i32>(Key::h)] = MIMAS_KEY_H;
        key_map[static_cast<i32>(Key::i)] = MIMAS_KEY_I;
        key_map[static_cast<i32>(Key::j)] = MIMAS_KEY_J;
        key_map[static_cast<i32>(Key::k)] = MIMAS_KEY_K;
        key_map[static_cast<i32>(Key::l)] = MIMAS_KEY_L;
        key_map[static_cast<i32>(Key::m)] = MIMAS_KEY_M;
        key_map[static_cast<i32>(Key::n)] = MIMAS_KEY_N;
        key_map[static_cast<i32>(Key::o)] = MIMAS_KEY_O;
        key_map[static_cast<i32>(Key::p)] = MIMAS_KEY_P;
        key_map[static_cast<i32>(Key::q)] = MIMAS_KEY_Q;
        key_map[static_cast<i32>(Key::r)] = MIMAS_KEY_R;
        key_map[static_cast<i32>(Key::s)] = MIMAS_KEY_S;
        key_map[static_cast<i32>(Key::t)] = MIMAS_KEY_T;
        key_map[static_cast<i32>(Key::u)] = MIMAS_KEY_U;
        key_map[static_cast<i32>(Key::v)] = MIMAS_KEY_V;
        key_map[static_cast<i32>(Key::w)] = MIMAS_KEY_W;
        key_map[static_cast<i32>(Key::x)] = MIMAS_KEY_X;
        key_map[static_cast<i32>(Key::y)] = MIMAS_KEY_Y;
        key_map[static_cast<i32>(Key::z)] = MIMAS_KEY_Z;
        key_map[static_cast<i32>(Key::slash)] = MIMAS_KEY_SLASH;
        key_map[static_cast<i32>(Key::comma)] = MIMAS_KEY_COMMA;
        key_map[static_cast<i32>(Key::dot)] = MIMAS_KEY_DOT;
        key_map[static_cast<i32>(Key::semicolon)] = MIMAS_KEY_SEMICOLON;
        key_map[static_cast<i32>(Key::left_bracket)] = MIMAS_KEY_LEFT_BRACKET;
        key_map[static_cast<i32>(Key::right_bracket)] = MIMAS_KEY_RIGHT_BRACKET;
        key_map[static_cast<i32>(Key::minus)] = MIMAS_KEY_MINUS;
        key_map[static_cast<i32>(Key::equals)] = MIMAS_KEY_EQUALS;
        key_map[static_cast<i32>(Key::apostrophe)] = MIMAS_KEY_APOSTROPHE;
        key_map[static_cast<i32>(Key::tick)] = MIMAS_KEY_TICK;
        key_map[static_cast<i32>(Key::backward_slash)] = MIMAS_KEY_BACKWARD_SLASH;
        key_map[static_cast<i32>(Key::f1)] = MIMAS_KEY_F1;
        key_map[static_cast<i32>(Key::f2)] = MIMAS_KEY_F2;
        key_map[static_cast<i32>(Key::f3)] = MIMAS_KEY_F3;
        key_map[static_cast<i32>(Key::f4)] = MIMAS_KEY_F4;
        key_map[static_cast<i32>(Key::f5)] = MIMAS_KEY_F5;
        key_map[static_cast<i32>(Key::f6)] = MIMAS_KEY_F6;
        key_map[static_cast<i32>(Key::f7)] = MIMAS_KEY_F7;
        key_map[static_cast<i32>(Key::f8)] = MIMAS_KEY_F8;
        key_map[static_cast<i32>(Key::f9)] = MIMAS_KEY_F9;
        key_map[static_cast<i32>(Key::f10)] = MIMAS_KEY_F10;
        key_map[static_cast<i32>(Key::f11)] = MIMAS_KEY_F11;
        key_map[static_cast<i32>(Key::f12)] = MIMAS_KEY_F12;
        key_map[static_cast<i32>(Key::left)] = MIMAS_KEY_LEFT;
        key_map[static_cast<i32>(Key::right)] = MIMAS_KEY_RIGHT;
        key_map[static_cast<i32>(Key::up)] = MIMAS_KEY_UP;
        key_map[static_cast<i32>(Key::down)] = MIMAS_KEY_DOWN;
        key_map[static_cast<i32>(Key::caps_lock)] = MIMAS_KEY_CAPS_LOCK;
        key_map[static_cast<i32>(Key::tab)] = MIMAS_KEY_TAB;
        key_map[static_cast<i32>(Key::page_up)] = MIMAS_KEY_PAGE_UP;
        key_map[static_cast<i32>(Key::page_down)] = MIMAS_KEY_PAGE_DOWN;
        key_map[static_cast<i32>(Key::home)] = MIMAS_KEY_HOME;
        key_map[static_cast<i32>(Key::end)] = MIMAS_KEY_END;
        key_map[static_cast<i32>(Key::insert)] = MIMAS_KEY_INSERT;
        key_map[static_cast<i32>(Key::del)] = MIMAS_KEY_DELETE;
        key_map[static_cast<i32>(Key::backspace)] = MIMAS_KEY_BACKSPACE;
        key_map[static_cast<i32>(Key::spacebar)] = MIMAS_KEY_SPACE;
        key_map[static_cast<i32>(Key::enter)] = MIMAS_KEY_ENTER;
        key_map[static_cast<i32>(Key::escape)] = MIMAS_KEY_ESCAPE;
        key_map[static_cast<i32>(Key::left_shift)] = MIMAS_KEY_LEFT_SHIFT;
        key_map[static_cast<i32>(Key::right_shift)] = MIMAS_KEY_RIGHT_SHIFT;
        key_map[static_cast<i32>(Key::left_control)] = MIMAS_KEY_LEFT_CONTROL;
        key_map[static_cast<i32>(Key::right_control)] = MIMAS_KEY_RIGHT_CONTROL;
        key_map[static_cast<i32>(Key::left_alt)] = MIMAS_KEY_LEFT_ALT;
        key_map[static_cast<i32>(Key::right_alt)] = MIMAS_KEY_RIGHT_ALT;
        key_map[static_cast<i32>(Key::numpad_0)] = MIMAS_KEY_NUMPAD_0;
        key_map[static_cast<i32>(Key::numpad_1)] = MIMAS_KEY_NUMPAD_1;
        key_map[static_cast<i32>(Key::numpad_2)] = MIMAS_KEY_NUMPAD_2;
        key_map[static_cast<i32>(Key::numpad_3)] = MIMAS_KEY_NUMPAD_3;
        key_map[static_cast<i32>(Key::numpad_4)] = MIMAS_KEY_NUMPAD_4;
        key_map[static_cast<i32>(Key::numpad_5)] = MIMAS_KEY_NUMPAD_5;
        key_map[static_cast<i32>(Key::numpad_6)] = MIMAS_KEY_NUMPAD_6;
        key_map[static_cast<i32>(Key::numpad_7)] = MIMAS_KEY_NUMPAD_7;
        key_map[static_cast<i32>(Key::numpad_8)] = MIMAS_KEY_NUMPAD_8;
        key_map[static_cast<i32>(Key::numpad_9)] = MIMAS_KEY_NUMPAD_9;
        key_map[static_cast<i32>(Key::numpad_add)] = MIMAS_KEY_NUMPAD_ADD;
        key_map[static_cast<i32>(Key::numpad_subtract)] = MIMAS_KEY_NUMPAD_SUBTRACT;
        key_map[static_cast<i32>(Key::numpad_divide)] = MIMAS_KEY_NUMPAD_DIVIDE;
        key_map[static_cast<i32>(Key::numpad_multiply)] = MIMAS_KEY_NUMPAD_MULTIPLY;
        key_map[static_cast<i32>(Key::numpad_decimal)] = MIMAS_KEY_NUMPAD_DECIMAL;
        key_map[static_cast<i32>(Key::numpad_enter)] = MIMAS_KEY_NUMPAD_ENTER;
        key_map[static_cast<i32>(Key::numlock)] = MIMAS_KEY_NUMLOCK;
        return key_map;
    }

    bool get_key(Key const k) {
        static atl::Fixed_Array<i32, 256> key_map = create_key_to_mimas_map();
        Mimas_Key key = static_cast<Mimas_Key>(key_map[static_cast<u32>(k)]);
        return mimas_get_key(key);
    }

    void _window_key_callback(Mimas_Window* const, Mimas_Key const key, Mimas_Key_Action const action, void* data){
        static atl::Fixed_Array<Key, 256> key_map = create_mimas_to_key_map();
        Key const mapped_key = key_map[key];
        Window* const window = reinterpret_cast<Window*>(data);
        if(mapped_key != Key::none && window->callbacks.key) {
            window->callbacks.key(window, key_map[key], action, window->callbacks.key_data);
        }
    }

    void _window_mouse_button_callback(Mimas_Window* const, Mimas_Key key, Mimas_Mouse_Button_Action action, void* data) {
        static atl::Fixed_Array<Key, 256> key_map = create_mimas_to_key_map();
        Key const mapped_key = key_map[key];
        Window* const window = reinterpret_cast<Window*>(data);
        if(mapped_key != Key::none && window->callbacks.mouse_button) {
            window->callbacks.mouse_button(window, key_map[key], action, window->callbacks.key_data);
        }
    }

    void _window_scroll_callback(Mimas_Window* const, mimas_i32 const d_x, mimas_i32 const d_y, void* data) {
        Window* const window = reinterpret_cast<Window*>(data);
        if(window->callbacks.scroll) {
            window->callbacks.scroll(window, d_x, d_y, window->callbacks.scroll_data);
        }
    }

    void _cursor_callback(Mimas_Window* const, mimas_i32 const x, mimas_i32 const y, void* data){
        Window* const window = reinterpret_cast<Window*>(data);
        if(window->callbacks.cursor_pos) {
            window->callbacks.cursor_pos(window, x, y, window->callbacks.cursor_pos_data);
        }
    }
    
    Window* create_window(f32 const width, f32 const height, bool const decorated) {
        ANTON_ASSERT(width > 0 && height > 0, "Window dimensions may not be 0");

        Mimas_Window_Create_Info info;
        info.width = width;
        info.height = height;
        info.title = "GameEngine";
        info.decorated = decorated;
        Mimas_Window* const mimas_window = mimas_create_window(info);
        if (mimas_window) {
            mimas_show_window(mimas_window);
            Window* window = new Window;
            window->mimas_window = mimas_window;
            windowing.windows.emplace_back(window);
            mimas_set_window_activate_callback(mimas_window, _window_activate_callback, window);
            mimas_set_window_key_callback(mimas_window, _window_key_callback, window);
            mimas_set_window_cursor_pos_callback(mimas_window, _cursor_callback, window);
            mimas_set_window_mouse_button_callback(mimas_window, _window_mouse_button_callback, window);
            mimas_set_window_scroll_callback(mimas_window, _window_scroll_callback, window);
            return window;
        } else {
            throw Exception("Could not create window.");
        }
    }

    void destroy_window(Window* const window) {
        for (i64 i = 0; i < windowing.windows.size(); ++i) {
            if (windowing.windows[i] == window) {
                windowing.windows.erase_unsorted_unchecked(i);
                break;
            }
        }

        mimas_destroy_window(window->mimas_window);
        delete window;
    }

    void clip_cursor(Window* window, Rect const* rect) {
        mimas_clip_cursor(window->mimas_window, reinterpret_cast<Mimas_Rect const*>(rect));
    }

    void set_cursor(Window* window, Cursor* cursor) {
        mimas_set_cursor(window->mimas_window, reinterpret_cast<Mimas_Cursor*>(cursor));
    }

    Cursor* create_standard_cursor(Standard_Cursor standard_cursor) {
        Mimas_Cursor* const cursor = mimas_create_standard_cursor(static_cast<Mimas_Standard_Cursor>(standard_cursor));
        return reinterpret_cast<Cursor*>(cursor);
    }

    void destroy_cursor(Cursor* cursor) {
        mimas_destroy_cursor(reinterpret_cast<Mimas_Cursor*>(cursor));
    }

    void set_window_resize_callback(Window* const window, window_resize_function const cb, void* const user_data) {
        window->callbacks.window_resize = cb;
        window->callbacks.window_resize_data = user_data;
    }

    void set_window_activate_callback(Window* const window, window_activate_function const cb, void* const user_data) {
        window->callbacks.window_activate =  cb;
        window->callbacks.window_activate_data = user_data;
    }

    void set_cursor_pos_callback(Window* const window, cursor_pos_function const cb, void* user_data) {
        window->callbacks.cursor_pos =  cb;
        window->callbacks.cursor_pos_data= user_data;
    }

    void set_mouse_button_callback(Window* const window, mouse_button_function const cb, void* user_data) {
        window->callbacks.mouse_button =  cb;
        window->callbacks.mouse_button_data= user_data;
    }

    void set_scroll_callback(Window* const window, scroll_function const cb, void* user_data) {
        window->callbacks.scroll =  cb;
        window->callbacks.scroll_data = user_data;
    }

    void set_key_callback(Window* const window, key_function const cb, void* user_data) {
        window->callbacks.key =  cb;
        window->callbacks.key_data= user_data;
    }

    void set_joystick_callback(joystick_function const cb, void* user_data) {
        windowing.callbacks.joystick = cb;
        windowing.callbacks.joystick_data = user_data;
    }

    Vector2 get_cursor_pos() {
        i32 x;
        i32 y;
        mimas_get_cursor_pos(&x, &y);
        return Vector2(x, y);
    }

    bool close_requested(Window* const window) {
        return mimas_close_requested(window->mimas_window);
    }

    void resize(Window* const window, f32 width, f32 height) {
        mimas_set_window_content_size(window->mimas_window, width, height);
    }

    Vector2 get_window_size(Window* const window) {
        i32 width = 0;
        i32 height = 0;
        mimas_get_window_content_size(window->mimas_window, &width, &height);
        return {(f32)width, (f32)height};
    }

    Vector2 get_window_pos(Window* const window) {
        i32 x;
        i32 y;
        mimas_get_window_content_pos(window->mimas_window, &x, &y);
        return {(f32)x, (f32)y};
    }

    void set_size(Window* const window, Vector2 const size) {
        mimas_set_window_content_size(window->mimas_window, size.x, size.y);
    }

    void set_window_pos(Window* const window, Vector2 const pos) {
        mimas_set_window_content_pos(window->mimas_window, pos.x, pos.y);
    }

    OpenGL_Context* create_context(i32 const major, i32 const minor, OpenGL_Profile const profile) {
        Mimas_GL_Context* const ctx = mimas_create_gl_context(major, minor, static_cast<Mimas_GL_Profile>(profile));
        return reinterpret_cast<OpenGL_Context*>(ctx);
    }

    void destroy_context(OpenGL_Context* const ctx) {
        mimas_destroy_gl_context(reinterpret_cast<Mimas_GL_Context*>(ctx));
    }

    void make_context_current(OpenGL_Context* const ctx, Window* const window) {
        mimas_make_context_current(window->mimas_window, reinterpret_cast<Mimas_GL_Context*>(ctx));
    }

    void swap_buffers(Window* const window) {
        mimas_swap_buffers(window->mimas_window);
    }

    void enable_vsync(bool const enabled) {
        mimas_set_swap_interval(enabled);
    }
} // namespace anton_engine::windowing
