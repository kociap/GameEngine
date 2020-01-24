#include <imgui.hpp>

#include <anton_assert.hpp>
#include <anton_stl/vector.hpp>
#include <hashing/murmurhash2.hpp>
#include <math/math.hpp>
#include <window.hpp>

#include <unordered_map>

#include <logging.hpp>

namespace anton_engine::imgui {
    class Window {
    public:
        i64 id;

        Vector2 position;
        Vector2 size;
        f32 border_area_width;
        Style style;
        Viewport* viewport;
    };

    class Widget {
    public:
        // Parent window
        i64 parent_window_id;
        // Parent widget (index into widgets array in Context)
        i64 parent;
        Vector2 position;
        Vector2 size;
        Style style;
        bool hovered;
        bool clicked;
    };

    class Viewport {
    public:
        windowing::Window* native_window = nullptr;
        anton_stl::Vector<Draw_Command> draw_commands_buffer;
    };

    class Context {
    public:
        struct {
            std::unordered_map<i64, Window> windows;
        } current, next;
        i64 hot_window = -1;
        i64 active_window = -1;
        // Currently bound window.
        i64 current_window = -1;
        Input_State input = {};
        Input_State prev_input = {};
        anton_stl::Vector<Widget> widgets;
        anton_stl::Vector<i64> widget_stack;
        Style default_style;
        anton_stl::Vector<Vertex> vertex_buffer;
        anton_stl::Vector<u32> index_buffer;
        anton_stl::Vector<Draw_Command> draw_commands_buffer;
        anton_stl::Vector<Viewport*> viewports;
        Viewport* dragged_viewport;
    };

    // TODO: Better seed.
    static constexpr u32 hash_seed = 123456;

    static Vertex::Color color_to_vertex_color(Color const c) {
        return {static_cast<u8>(255.0f * c.r), static_cast<u8>(255.0f * c.g), static_cast<u8>(255.0f * c.b), static_cast<u8>(255.0f * c.a)};
    }

    Context* create_context() {
        Context* ctx = new Context;
        set_default_style_default_dark(*ctx);
        Viewport* main_viewport = new Viewport;
        ctx->viewports.emplace_back(main_viewport);
        ctx->dragged_viewport = nullptr;
        return ctx;
    }

    void destroy_context(Context* ctx) {
        for (i64 i = 1; i < ctx->viewports.size(); ++i) {
            windowing::destroy_window(ctx->viewports[i]->native_window);
            delete ctx->viewports[i];
        }

        delete ctx->viewports[0];
        delete ctx;
    }

    void set_main_viewport_native_window(Context& ctx, windowing::Window* window) {
        ANTON_VERIFY(window, "window is nullptr.");
        ctx.viewports[0]->native_window = window;
    }

    void set_default_style_default_dark(Context& ctx) {
        Style default_theme;
        default_theme.background_color = {0.1f, 0.1f, 0.1f};
        ctx.default_style = default_theme;
    }

    void set_default_style(Context& ctx, Style style) {
        ctx.default_style = style;
    }

    void set_input_state(Context& ctx, Input_State const state) {
        ctx.input = state;
    }

    Input_State get_input_state(Context& ctx) {
        return ctx.input;
    }

    // Assumes the cursor is inside the window.
    // returns -1 if cursor is not in the border area of a window.
    // returns 0 if cursor is in top border area, 1 if right, 2 if bottom, 3 if left.
    // TODO: corner resizing
    static i32 check_cursor_in_border_area(Window const& window, Vector2 const cursor_pos) {
        f32 const dist_top = cursor_pos.y - window.position.y;
        f32 const dist_bottom = window.position.y + window.size.y - cursor_pos.y;
        f32 const dist_left = cursor_pos.x - window.position.x;
        f32 const dist_right = window.position.x + window.size.x - cursor_pos.x;
        f32 const closest_edge_dist = math::min(math::min(dist_top, dist_bottom), math::min(dist_left, dist_right));
        if (closest_edge_dist <= window.border_area_width) {
            if (dist_top == closest_edge_dist) {
                return 0;
            } else if (dist_bottom == closest_edge_dist) {
                return 2;
            } else if (dist_right == closest_edge_dist) {
                return 1;
            } else {
                return 3;
            }
        } else {
            return -1;
        }
    }

    static void drag_window(Context& ctx, Window& window, Vector2 const cursor_delta) {
        if (!ctx.dragged_viewport) {
            Vector2 const native_window_pos = windowing::get_window_pos(window.viewport->native_window);
            Vector2 const imgui_window_pos = window.position + native_window_pos;

            Viewport* viewport = new Viewport;
            ctx.dragged_viewport = viewport;
            ctx.viewports.emplace_back(viewport);
            viewport->native_window = windowing::create_window(window.size.x, window.size.y, nullptr, true, false);
            windowing::focus_window(viewport->native_window);
            windowing::set_window_pos(viewport->native_window, imgui_window_pos);
            window.viewport = viewport;
            window.position = {0, 0};
        }

        Vector2 const native_window_pos = windowing::get_window_pos(ctx.dragged_viewport->native_window);
        windowing::set_window_pos(ctx.dragged_viewport->native_window, native_window_pos + cursor_delta);
    }

    static void process_input(Context& ctx) {
        Vector2 const cursor = ctx.input.cursor_position;
        bool const just_left_clicked = !ctx.prev_input.left_mouse_button && ctx.input.left_mouse_button;
        bool const just_left_released = ctx.prev_input.left_mouse_button && !ctx.input.left_mouse_button;
        if (just_left_clicked) {
            if (ctx.input.left_mouse_button) {
                ctx.active_window = ctx.hot_window;
            }
        } else if (ctx.input.left_mouse_button) {
            if (ctx.active_window != -1) {
                Window& window = ctx.next.windows.at(ctx.active_window);
                Vector2 const cursor_pos_delta = ctx.input.cursor_position - ctx.prev_input.cursor_position;
                ANTON_LOG_INFO("cursor_pos_delta: " + anton_stl::to_string(cursor_pos_delta.x) + " " + anton_stl::to_string(cursor_pos_delta.y));
                if (ctx.dragged_viewport) {
                    drag_window(ctx, window, cursor_pos_delta);
                } else {
                    if (i32 const border = check_cursor_in_border_area(window, ctx.prev_input.cursor_position); border != -1) {
                        switch (border) {
                        case 0: {
                            window.position.y += cursor_pos_delta.y;
                            window.size.y -= cursor_pos_delta.y;
                        } break;

                        case 2: {
                            window.size.y += cursor_pos_delta.y;
                        } break;

                        case 1: {
                            window.size.x += cursor_pos_delta.x;
                        } break;

                        case 3: {
                            window.position.x += cursor_pos_delta.x;
                            window.size.x -= cursor_pos_delta.x;
                        } break;
                        }
                    } else {
                        drag_window(ctx, window, cursor_pos_delta);
                        ANTON_LOG_INFO(anton_stl::to_string(cursor_pos_delta.x) + " " + anton_stl::to_string(cursor_pos_delta.y));
                    }
                }
            }
        } else {
            if (just_left_released) {
                ctx.dragged_viewport = nullptr;
            }

            ctx.hot_window = -1;
            for (auto& entry: ctx.current.windows) {
                Window const& window = entry.second;
                bool const fits_in_x = cursor.x >= window.position.x && cursor.x <= window.position.x + window.size.x;
                bool const fits_in_y = cursor.y >= window.position.y && cursor.y <= window.position.y + window.size.y;
                if (fits_in_x && fits_in_y) {
                    ctx.hot_window = window.id;
                }
            }
        }

        ctx.prev_input = ctx.input;
    }

    void begin_frame(Context& ctx) {
        ctx.widgets.clear();
        ctx.vertex_buffer.clear();
        ctx.index_buffer.clear();
        ctx.draw_commands_buffer.clear();

        for (Viewport* viewport: ctx.viewports) {
            viewport->draw_commands_buffer.clear();
        }

        process_input(ctx);
    }

    void end_frame(Context& ctx) {
        ANTON_VERIFY(ctx.current_window == -1, "A window has not been ended prior to call to end_frame.");

        ctx.current = ctx.next;

        // generate geometry and indices,
        // and build command list

        // We temporarily ignore the sorting and just render the windows in random order.
        // TODO: Render windows in correct order (active > previously active > ...), aka reverse of latest active.

        auto& verts = ctx.vertex_buffer;
        auto& indices = ctx.index_buffer;
        auto& cmds = ctx.draw_commands_buffer;
        for (auto& entry: ctx.current.windows) {
            Window const& window = entry.second;
            Draw_Command cmd;
            cmd.vertex_offset = verts.size();
            Vertex::Color const color = color_to_vertex_color(window.style.background_color);
            verts.emplace_back(window.position, Vector2{0.0f, 1.0f}, color);
            verts.emplace_back(window.position + Vector2(0, window.size.y), Vector2{0.0f, 0.0f}, color);
            verts.emplace_back(window.position + window.size, Vector2{1.0f, 0.0f}, color);
            verts.emplace_back(window.position + Vector2(window.size.x, 0), Vector2{1.0f, 1.0f}, color);
            cmd.index_offset = indices.size();
            indices.push_back(0);
            indices.push_back(1);
            indices.push_back(2);
            indices.push_back(0);
            indices.push_back(2);
            indices.push_back(3);
            cmd.element_count = 6;
            // TODO: Choose texture.
            cmd.texture = 0;
            cmds.push_back(cmd);
            window.viewport->draw_commands_buffer.emplace_back(cmd);
        }
    }

    anton_stl::Slice<Viewport* const> get_viewports(Context& ctx) {
        return ctx.viewports;
    }

    windowing::Window* get_viewport_native_window(Context&, Viewport& viewport) {
        return viewport.native_window;
    }

    anton_stl::Slice<Draw_Command const> get_viewport_draw_commands(Context&, Viewport& viewport) {
        return viewport.draw_commands_buffer;
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

    void begin_window(Context& ctx, anton_stl::String_View identifier, bool new_viewport) {
        ANTON_VERIFY(ctx.current_window == -1, "Cannot create window inside another window.");

        i64 const id = murmurhash2_32(identifier.data(), identifier.size_bytes(), hash_seed);
        auto const iter = ctx.next.windows.find(id);
        if (iter == ctx.next.windows.end()) {
            Window wnd;
            wnd.id = id;
            wnd.style = ctx.default_style;
            wnd.border_area_width = 4.0f;
            wnd.viewport = ctx.viewports[0];
            ctx.next.windows.emplace(id, wnd);
            ctx.current_window = wnd.id;
        } else {
            ctx.current_window = id;
        }
    }

    void end_window(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "Trying to end window, but none has been made current.");
        ANTON_VERIFY(ctx.widget_stack.size() == 0, "Widget stack not empty. End all child widgets before you attempt to end the window.");

        ctx.current_window = -1;
    }

    void begin_widget(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "Cannot create widget because no window is current.");

        Widget widget;
        widget.style = ctx.default_style;
        widget.parent = (ctx.widget_stack.size() != 0 ? ctx.widget_stack[ctx.widget_stack.size() - 1] : -1);
        widget.parent_window_id = ctx.current_window;
        ctx.widgets.push_back(widget);
        i64 const widget_id = ctx.widgets.size() - 1;
        ctx.widget_stack.push_back(widget_id);
    }

    void end_widget(Context& ctx) {
        ANTON_VERIFY(ctx.widget_stack.size() != 0, "No widgets are active.");

        ctx.widget_stack.pop_back();
    }

    // Get style of current widget or window
    Style get_style(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        if (ctx.widget_stack.size() != 0) {
            i64 const index = ctx.widget_stack[ctx.widget_stack.size() - 1];
            return ctx.widgets[index].style;
        } else {
            Window& window = ctx.next.windows.at(ctx.current_window);
            return window.style;
        }
    }

    // Set style of current widget or window
    void set_style(Context& ctx, Style const style) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        if (ctx.widget_stack.size() != 0) {
            i64 const index = ctx.widget_stack[ctx.widget_stack.size() - 1];
            ctx.widgets[index].style = style;
        } else {
            Window& window = ctx.next.windows.at(ctx.current_window);
            window.style = style;
        }
    }

    void set_window_border_area(Context& ctx, f32 const width) {
        ANTON_VERIFY(width >= 1.0f, "Border area may not be thinner than 1 pixel.");
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        Window& window = ctx.next.windows.at(ctx.current_window);
        window.border_area_width = width;
    }

    void set_window_size(Context& ctx, Vector2 const size) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        Window& wnd = ctx.next.windows.at(ctx.current_window);
        wnd.size = size;
    }

    void set_window_pos(Context& ctx, Vector2 const pos) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        Window& wnd = ctx.next.windows.at(ctx.current_window);
        wnd.position = pos;
    }

    bool is_window_hot(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        return ctx.hot_window == ctx.current_window;
    }

    bool is_window_active(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        return ctx.active_window == ctx.current_window;
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
