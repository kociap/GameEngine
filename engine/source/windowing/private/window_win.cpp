#include <window_internal.hpp>

#include <utils/enum.hpp>

#define GLFW_EXPOSE_NATIVE_WIN32 1
#define GLFW_EXPOSE_NATIVE_WGL 1
#include <GLFW/glfw3native.h>

#define NOMINMAX 1
#define WIN_LEAN_AND_MEAN 1
#include <Windows.h>

#include <unordered_map>

namespace anton_engine::windowing {
    class Win_Platform {
    public:
        u8 key_state[256];
    };

    static Win_Platform win_platform;

    OpenGL_Context* get_window_context(Window* window) {
        HGLRC const context = glfwGetWGLContext(window->glfw_window);
        return reinterpret_cast<OpenGL_Context*>(context);
    }

    void make_context_current_with_window(OpenGL_Context* context, Window* window) {
        HWND const hwnd = glfwGetWin32Window(window->glfw_window);
        HDC const hdc = GetDC(hwnd);
        HGLRC const hglrc = reinterpret_cast<HGLRC>(context);
        wglMakeCurrent(hdc, hglrc);
    }

    void poll_user_input_devices_state() {
        GetKeyboardState(win_platform.key_state);
    }

    // Returns -1 if the parameter key doesn't name a known key.
    static i32 map_key_to_win_virtual_key(Key const key) {
        static std::unordered_map<Key, u8> vk_map{
            {Key::left_mouse_button, VK_LBUTTON},
            {Key::right_mouse_button, VK_RBUTTON},
            {Key::middle_mouse_button, VK_MBUTTON},
            {Key::backspace, VK_BACK},
            {Key::tab, VK_TAB},
            {Key::enter, VK_RETURN},
            {Key::left_shift, VK_LSHIFT},
            {Key::right_shift, VK_RSHIFT},
            {Key::left_control, VK_LCONTROL},
            {Key::right_control, VK_RCONTROL},
            {Key::pause, VK_PAUSE},
            {Key::left_alt, VK_MENU},
            {Key::caps_lock, VK_CAPITAL},
            {Key::escape, VK_ESCAPE},
            {Key::spacebar, VK_SPACE},
            {Key::page_up, VK_PRIOR},
            {Key::page_down, VK_NEXT},
            {Key::delete_key, VK_DELETE},
            {Key::insert, VK_INSERT},
            {Key::end, VK_END},
            {Key::home, VK_HOME},
            {Key::left, VK_LEFT},
            {Key::up, VK_UP},
            {Key::right, VK_RIGHT},
            {Key::down, VK_DOWN},
            {Key::f1, VK_F1},
            {Key::f2, VK_F2},
            {Key::f3, VK_F3},
            {Key::f4, VK_F4},
            {Key::f5, VK_F5},
            {Key::f6, VK_F6},
            {Key::f7, VK_F7},
            {Key::f8, VK_F8},
            {Key::f9, VK_F9},
            {Key::f10, VK_F10},
            {Key::f11, VK_F11},
            {Key::f12, VK_F12},
            {Key::num_lock, VK_NUMLOCK},
            {Key::numpad_0, VK_NUMPAD0},
            {Key::numpad_1, VK_NUMPAD1},
            {Key::numpad_2, VK_NUMPAD2},
            {Key::numpad_3, VK_NUMPAD3},
            {Key::numpad_4, VK_NUMPAD4},
            {Key::numpad_5, VK_NUMPAD5},
            {Key::numpad_6, VK_NUMPAD6},
            {Key::numpad_7, VK_NUMPAD7},
            {Key::numpad_8, VK_NUMPAD8},
            {Key::numpad_8, VK_NUMPAD8},
            {Key::numpad_9, VK_NUMPAD9},
        };

        auto const key_value = utils::enum_to_value(key);
        if (key_value >= utils::enum_to_value(Key::a) && key_value <= utils::enum_to_value(Key::z)) {
            return key_value - utils::enum_to_value(Key::a) + 0x41;
        }

        if (key_value >= utils::enum_to_value(Key::zero) && key_value <= utils::enum_to_value(Key::nine)) {
            return key_value - utils::enum_to_value(Key::zero) + 0x30;
        }

        if (auto iter = vk_map.find(key); iter != vk_map.end()) {
            return iter->second;
        } else {
            return -1;
        }
    }

    bool get_key(Key const k) {
        i32 const index = map_key_to_win_virtual_key(k);
        return index != -1 && (win_platform.key_state[index] & 0x80);
    }
} // namespace anton_engine::windowing
