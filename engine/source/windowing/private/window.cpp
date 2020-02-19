#include <window.hpp>

#include <anton_assert.hpp>
#include <anton_stl/vector.hpp>
#include <diagnostic_macros.hpp>
#include <exception.hpp>
#include <window_internal.hpp>
#include <anton_stl/fixed_array.hpp>

namespace anton_engine::windowing {
    class Windowing {
    public:
        joystick_function joystick_callback;
        void* joystick_callback_user_data;
        anton_stl::Vector<Window*> windows;
    };

    static Windowing windowing;

    bool init() {
        if (!mimas_init_with_gl()) {
            return false;
        }

        windowing.joystick_callback = nullptr;
        windowing.joystick_callback_user_data = nullptr;

        return true;
    }

    void terminate() {
        windowing.joystick_callback = nullptr;
        windowing.joystick_callback_user_data = nullptr;

        for (Window* window: windowing.windows) {
            destroy_window(window);
        }

        mimas_terminate();
    }

    void poll_events() {
        mimas_poll_events();
        poll_user_input_devices_state();
    }

    void _window_activate_callback(Mimas_Window* const mimas_window, mimas_i32 const activated, void* const data){
        Window* const window = reinterpret_cast<Window*>(data);
        if(window->callbacks.window_activate) {
            window->callbacks.window_activate(window, activated, window->callbacks.window_activate_data);
        }
    }

    static anton_stl::Fixed_Array<Key, 256> create_key_map() {
        anton_stl::Fixed_Array<Key, 256> key_map(256, Key::none);
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
        key_map[MIMAS_KEY_A] = Key::zero;
        key_map[MIMAS_KEY_0] = Key::zero;
        key_map[MIMAS_KEY_0] = Key::zero;

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
        return key_map;
    }

    void _window_key_callback(Mimas_Window* const mimas_window, Mimas_Key const key, Mimas_Key_Action const action, void* data){
        static anton_stl::Fixed_Array<Key, 256> key_map = create_key_map();
        Key const mapped_key = key_map[key];
        Window* const window = reinterpret_cast<Window*>(data);
        if(mapped_key != Key::none && window->callbacks.key) {
            window->callbacks.key(window, key_map[key], action, window->callbacks.key_data);
        }
    }

    void _cursor_callback(Mimas_Window* const mimas_window, mimas_i32 const x, mimas_i32 const y, void* data){
        Window* const window = reinterpret_cast<Window*>(data);
        if(window->callbacks.cursor_pos) {
            window->callbacks.cursor_pos(window, x, y, window->callbacks.cursor_pos_data);
        }
    }
    
    Window* create_window(f32 const width, f32 const height, Window* const share, bool const create_context, bool const decorated) {
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
            return window;
        } else {
            return nullptr;
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

    void set_cursor_mode(Window* const window, Cursor_Mode const mode) {
        // switch (mode) {
        // case Cursor_Mode::normal: {
        //     glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // } break;

        // case Cursor_Mode::hidden: {
        //     glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        // } break;

        // case Cursor_Mode::captured: {
        //     glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        // } break;

        // case Cursor_Mode::locked: {
        //     glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // } break;
        // }
    }

    Cursor_Mode get_cursor_mode(Window* const window) {
        // i32 const glfw_mode = glfwGetInputMode(window->glfw_window, GLFW_CURSOR);
        // switch (glfw_mode) {
        // case GLFW_CURSOR_NORMAL: {
        //     return Cursor_Mode::normal;
        // } break;

        // case GLFW_CURSOR_HIDDEN: {
        //     return Cursor_Mode::hidden;
        // } break;

        // case GLFW_CURSOR_DISABLED: {
        //     return Cursor_Mode::locked;
        // } break;

        // case GLFW_CURSOR_CAPTURED:
        //     return Cursor_Mode::captured;
        // }

        return Cursor_Mode::normal;
    }

    void set_window_resize_callback(Window* const window, window_resize_function const cb, void* const user_data) {
        // window->window_resize_callback = cb;
        // window->window_resize_callback_user_data = user_data;
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
        // window->callbacks.mouse_button =  cb;
        // window->callbacks.mouse_button_data= user_data;
    }

    void set_scroll_callback(Window* const window, scroll_function const cb, void* user_data) {
        // window->callbacks.scroll =  cb;
        // window->callbacks.scroll_data= user_data;
    }

    void set_key_callback(Window* const window, key_function const cb, void* user_data) {
        window->callbacks.key =  cb;
        window->callbacks.key_data= user_data;
    }

    void set_joystick_callback(joystick_function const cb, void* user_data) {
        // windowing.joystick_callback = cb;
        // windowing.joystick_data= user_data;
    }

    Vector2 get_cursor_pos() {
        i32 x;
        i32 y;
        mimas_get_cursor_pos(&x, &y);
        return Vector2(x, y);
    }

    bool should_close(Window* const window) {
        return false;
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

    void set_window_pos(Window* const window, Vector2 const pos) {
        mimas_set_window_content_pos(window->mimas_window, pos.x, pos.y);
    }

    void set_opacity(Window* const window, float const opacity) {
        // glfwSetWindowOpacity(window->glfw_window, opacity);
    }

    void focus_window(Window* const window) {
        // glfwFocusWindow(window->glfw_window);
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
