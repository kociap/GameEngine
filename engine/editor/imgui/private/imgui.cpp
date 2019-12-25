#include <imgui.hpp>

#include <anton_assert.hpp>
#include <anton_stl/vector.hpp>
#include <exception.hpp>
#include <hashing/murmurhash2.hpp>

#include <unordered_map>

namespace anton_engine::imgui {
    class Window {
    public:
        i64 id;
        Vector2 position;
        Vector2 dimensions;
        Style style;
        bool hovered;
        bool clicked;
    };

    class Widget {
    public:
        // Parent window
        i64 parent_window_id;
        // Parent widget (index into widgets array in Context)
        i64 parent;
        Vector2 position;
        Vector2 dimensions;
        Style style;
        bool hovered;
        bool clicked;
    };

    class Context {
    public:
        std::unordered_map<u64, Window> windows;
        anton_stl::Vector<Widget> widgets;
        i64 current_window = -1;
        anton_stl::Vector<i64> widget_stack;
        Style default_style;
        anton_stl::Vector<Vertex> vertex_buffer;
        anton_stl::Vector<u32> index_buffer;
        anton_stl::Vector<Draw_Command> draw_commands_buffer;
    };

    // TODO: Better seed.
    static constexpr u32 hash_seed = 123456;

    static Vertex::Color color_to_vertex_color(Color const c) {
        return {static_cast<u8>(255.0f * c.r), static_cast<u8>(255.0f * c.g), static_cast<u8>(255.0f * c.b), static_cast<u8>(255.0f * c.a)};
    }

    Context* create_context() {
        Context* ctx = new Context;
        set_default_style_default_dark(*ctx);
        return ctx;
    }

    void destroy_context(Context* ctx) {
        delete ctx;
    }

    void set_default_style_default_dark(Context& ctx) {
        Style default_theme;
        default_theme.background_color = {0.1f, 0.1f, 0.1f};
        ctx.default_style = default_theme;
    }

    void set_default_style(Context& ctx, Style style) {
        ctx.default_style = style;
    }

    void begin_frame(Context& ctx) {
        ctx.widgets.clear();
        ctx.vertex_buffer.clear();
        ctx.index_buffer.clear();
        ctx.draw_commands_buffer.clear();
    }

    void end_frame(Context& ctx) {
        if (ctx.current_window != -1) {
            throw Exception("A window has not been ended prior to call to end_frame.");
        }

        // generate geometry and indices,
        // and build command list

        // We temporarily ignore the sorting and just render the windows in random order.
        // TODO: Render windows in correct order (active > previously active > ...), aka reverse of latest active.

        auto& verts = ctx.vertex_buffer;
        auto& indices = ctx.index_buffer;
        auto& cmds = ctx.draw_commands_buffer;
        for (auto& entry: ctx.windows) {
            Window& window = entry.second;
            Draw_Command cmd;
            cmd.vertex_offset = verts.size();
            Vertex::Color const color = color_to_vertex_color(window.style.background_color);
            verts.emplace_back(window.position, Vector2{0.0f, 1.0f}, color);
            verts.emplace_back(window.position + Vector2(0, window.dimensions.y), Vector2{0.0f, 0.0f}, color);
            verts.emplace_back(window.position + window.dimensions, Vector2{1.0f, 0.0f}, color);
            verts.emplace_back(window.position + Vector2(window.dimensions.x, 0), Vector2{1.0f, 1.0f}, color);
            cmd.index_offset = indices.size();
            indices.push_back(cmd.vertex_offset);
            indices.push_back(cmd.vertex_offset + 1);
            indices.push_back(cmd.vertex_offset + 2);
            indices.push_back(cmd.vertex_offset);
            indices.push_back(cmd.vertex_offset + 2);
            indices.push_back(cmd.vertex_offset + 3);
            cmd.element_count = 6;
            // TODO: Choose texture.
            cmd.texture = 0;
            cmds.push_back(cmd);
        }
    }

    anton_stl::Slice<Vertex const> get_vertex_data(Context& ctx) {
        return ctx.vertex_buffer;
    }

    anton_stl::Slice<u32 const> get_index_data(Context& ctx) {
        return ctx.index_buffer;
    }

    anton_stl::Slice<Draw_Command const> get_draw_commands(Context& ctx) {
        return ctx.draw_commands_buffer;
    }

    void begin_window(Context& ctx, anton_stl::String_View identifier) {
        if (ctx.current_window != -1) {
            throw Exception("Cannot create window inside another window.");
        }

        u64 const id = murmurhash2(identifier.data(), identifier.size_bytes(), hash_seed);
        if (ctx.windows.find(id) == ctx.windows.end()) {
            Window wnd;
            wnd.style = ctx.default_style;
            ctx.windows.emplace(id, wnd);
        }

        ctx.current_window = id;
    }

    void end_window(Context& ctx) {
        if (ctx.current_window == -1) {
            throw Exception("Trying to end window, but none has been made current.");
        }

        if (ctx.widget_stack.size() != 0) {
            throw Exception("Widget stack not empty. End all child widgets before you attempt to end the window.");
        }

        ctx.current_window = -1;
    }

    void begin_widget(Context& ctx) {
        if (ctx.current_window == -1) {
            throw Exception("Cannot create widget because no window is current.");
        }

        Widget widget;
        widget.style = ctx.default_style;
        widget.parent = (ctx.widget_stack.size() != 0 ? ctx.widget_stack.back() : -1);
        widget.parent_window_id = ctx.current_window;
        ctx.widgets.push_back(widget);
        i64 const widget_id = ctx.widgets.size() - 1;
        ctx.widget_stack.push_back(widget_id);
    }

    void end_widget(Context& ctx) {
        if (ctx.widget_stack.size() == 0) {
            throw Exception("No widgets are active.");
        }

        ctx.widget_stack.pop_back();
    }

    // Get style of current widget or window
    Style get_style(Context& ctx) {
        if (ctx.current_window == -1) {
            throw Exception("No current window.");
        }

        if (ctx.widget_stack.size() != 0) {
            return ctx.widgets[ctx.widget_stack.back()].style;
        } else {
            Window& window = ctx.windows.at(ctx.current_window);
            return window.style;
        }
    }

    // Set style of current widget or window
    void set_style(Context& ctx, Style const style) {
        if (ctx.current_window == -1) {
            throw Exception("No current window.");
        }

        if (ctx.widget_stack.size() != 0) {
            ctx.widgets[ctx.widget_stack.back()].style = style;
        } else {
            Window& window = ctx.windows.at(ctx.current_window);
            window.style = style;
        }
    }

    void set_window_size(Context& ctx, Vector2 const size) {
        if (ctx.current_window == -1) {
            throw Exception("No current window.");
        }

        Window& wnd = ctx.windows.at(ctx.current_window);
        wnd.dimensions = size;
    }

    void set_window_pos(Context& ctx, Vector2 const pos) {
        if (ctx.current_window == -1) {
            throw Exception("No current window.");
        }

        Window& wnd = ctx.windows.at(ctx.current_window);
        wnd.position = pos;
    }

    // bool hovered(Context& ctx) {
    //     if (ctx.current_window == -1) {
    //         throw Exception("No current window.");
    //     }

    //     if (ctx.widget_stack.size() != 0) {
    //         Widget& widget = ctx.widgets[ctx.widget_stack.back()];
    //         return widget.hovered;
    //     } else {
    //         Window& window = get_current_window(ctx);
    //         return window.hovered;
    //     }
    // }

    // bool clicked(Context& ctx) {
    //     if (ctx.current_window == -1) {
    //         throw Exception("No current window.");
    //     }

    //     if (ctx.widget_stack.size() != 0) {
    //         Widget& widget = ctx.widgets[ctx.widget_stack.back()];
    //         return widget.clicked;
    //     } else {
    //         Window& window = get_current_window(ctx);
    //         return window.clicked;
    //     }
    // }
} // namespace anton_engine::imgui
