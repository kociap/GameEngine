#include <imgui/imgui.hpp>

#include <core/assert.hpp>
#include <core/hashing/murmurhash2.hpp>
#include <core/intrinsics.hpp>
#include <core/math/math.hpp>
#include <core/atl/string.hpp>
#include <core/atl/utility.hpp>
#include <core/atl/vector.hpp>
#include <windowing/window.hpp>
#include <rendering/fonts.hpp>
#include <core/atl/flat_hash_map.hpp>

#include <core/logging.hpp>

namespace anton_engine::imgui {
    enum class Layout_Tile_Type {
        root,
        vertical_layout,
        horizontal_layout,
        dockspace,
    };

    class Layout_Tile {
    public:
        Layout_Tile* layout_parent = nullptr;
        Layout_Tile_Type tile_type;
        Vector2 size;
        Vector2 position;
    };

    class Layout_Root: public Layout_Tile {
    public:
        Layout_Tile* child = nullptr;
    };

    class Layout: public Layout_Tile {
    public:
        atl::Vector<Layout_Tile*> tiles;
    };

    class Dockspace: public Layout_Tile {
    public:
        atl::Vector<i64> windows;
        i64 active_window = -1;
        Viewport* viewport = nullptr;
        // Vector2 position;
        // Size of content area.
        Vector2 content_size;
        // TODO: Hardcoded. Compute from font size.
        f32 tab_bar_height = 20;
    };

    class Draw_Context {
    public:
        Vector2 draw_pos;
        atl::Vector<Draw_Command> draw_commands;
        atl::Vector<Vertex> vertex_buffer;
        atl::Vector<u32> index_buffer;
    };

    class Window {
    public:
        atl::String _debug_name;
        Style style;
        i64 id;
        Dockspace* dockspace;
        f32 border_area_width;
        bool enabled;
        // TODO: Rename to make multi-purpose.
        atl::Flat_Hash_Map<i64, Button_State> button_state;
        Draw_Context draw_context;
    };

    class Widget {
    public:
        // -1 if window, otherwise index into widgets in Window
        i64 parent;
        // Used by text widget
        atl::String text;
        Widget_Style style;
        // Used for layout calculations
        Vector2 size;
    };

    class Viewport {
    public:
        windowing::Window* native_window = nullptr;
        atl::Vector<Draw_Command> draw_commands_buffer;
        atl::Vector<Dockspace*> dockspaces;
        Layout_Root layout_root;
    };

    class Context {
    public:
        atl::Flat_Hash_Map<i64, Window> windows;
        i64 hot_window = -1;
        i64 active_window = -1;
        // Currently bound window.
        i64 current_window = -1;
        i64 current_widget = -1;
        Input_State input = {};
        Input_State prev_input = {};
        atl::Vector<Widget> widgets;
        atl::Vector<i64> widget_stack;
        Style default_style;
        Font_Style default_font_style;
        Settings settings;
        atl::Vector<Vertex> vertex_buffer;
        atl::Vector<u32> index_buffer;
        atl::Vector<Viewport*> viewports;
        atl::Vector<Dockspace*> dockspaces;
        Viewport* dragged_viewport = nullptr;
        bool dragging = false;
        bool clicked_tab = false;

        struct Drag_Info {
            // Dockspace which is not owned by a window (one that has multiple windows).
            // Is only set when the user repositions a tab within a dockspace.
            Dockspace* alien_dockspace = nullptr;
            Dockspace* hot_dockspace = nullptr;
            Vector2 original_size;
            Vector2 tab_click_offset;
        } drag;
    };

    static Vertex::Color color_to_vertex_color(Color const c) {
        return {(u8)math::clamp(255.0f * c.r, 0.0f, 255.0f), (u8)math::clamp(255.0f * c.g, 0.0f, 255.0f), (u8)math::clamp(255.0f * c.b, 0.0f, 255.0f),
                (u8)math::clamp(255.0f * c.a, 0.0f, 255.0f)};
    }

    static bool test_cursor_in_box(Vector2 const cursor, Vector2 const position, Vector2 const size) {
        bool const in_box_x = cursor.x >= position.x && cursor.x < position.x + size.x;
        bool const in_box_y = cursor.y >= position.y && cursor.y < position.y + size.y;
        return in_box_x && in_box_y;
    }

    static bool test_point_in_box(Vector2 const point, Vector2 const position, Vector2 const size) {
        bool const in_box_x = point.x >= position.x && point.x < position.x + size.x;
        bool const in_box_y = point.y >= position.y && point.y < position.y + size.y;
        return in_box_x && in_box_y;
    }

    static i64 hash_string(atl::String_View string) {
        // TODO: Choose better seed. Currently random prime.
        return murmurhash2_32(string.data(), string.size_bytes(), 758943349);
    }

    Context* create_context(Font_Style font_style) {
        Context* ctx = new Context;
        ctx->default_font_style = font_style;
        set_default_style_default_dark(*ctx);
        ctx->settings.window_drop_area_width = 0.5f;
        Viewport* main_viewport = new Viewport;
        main_viewport->layout_root.tile_type = Layout_Tile_Type::root;
        main_viewport->layout_root.child = nullptr;
        main_viewport->layout_root.layout_parent = nullptr;
        ctx->viewports.emplace_back(main_viewport);
        return ctx;
    }

    void destroy_context(Context* ctx) {
        for (Dockspace* dockspace: ctx->dockspaces) {
            delete dockspace;
        }

        delete ctx->viewports[0];
        for (i64 i = 1; i < ctx->viewports.size(); ++i) {
            windowing::destroy_window(ctx->viewports[i]->native_window);
            delete ctx->viewports[i];
        }

        delete ctx;
    }

    Viewport* get_main_viewport(Context& ctx) {
        return ctx.viewports[0];
    }

    static Viewport* create_viewport(Context& ctx, Vector2 const size, bool const decorated) {
        Viewport* viewport = new Viewport;
        viewport->native_window = windowing::create_window(size.x, size.y, decorated);
        viewport->layout_root.tile_type = Layout_Tile_Type::root;
        viewport->layout_root.layout_parent = nullptr;
        viewport->layout_root.child = nullptr;
        viewport->layout_root.size = size;
        ctx.viewports.emplace_back(viewport);
        return viewport;
    }

    static void destroy_viewport(Context& ctx, Viewport* const viewport) {
        for (i64 i = 0; i < ctx.viewports.size(); i += 1) {
            if (ctx.viewports[i] == viewport) {
                ctx.viewports.erase_unsorted_unchecked(i);
                break;
            }
        }

        windowing::destroy_window(viewport->native_window);
        delete viewport;
    }

    static Vector2 get_viewport_position(Viewport* const viewport) {
        return windowing::get_window_pos(viewport->native_window);
    }

    static Vector2 get_viewport_screen_pos(Viewport* const viewport) {
        return windowing::get_window_pos(viewport->native_window);
    }

    static Vector2 get_viewport_size(Viewport* const viewport) {
        return windowing::get_window_size(viewport->native_window);
    }

    static void set_viewport_screen_pos(Viewport* const viewport, Vector2 const pos) {
        windowing::set_window_pos(viewport->native_window, pos);
    }

    static void set_viewport_size(Viewport* const viewport, Vector2 const size) {
        windowing::set_size(viewport->native_window, size);
        viewport->layout_root.size = size;
    }

    // Doesn't verify whether the added dockspace is not already present.
    static void add_dockspace(Viewport* const viewport, Dockspace* const dockspace) {
        viewport->dockspaces.emplace_back(dockspace);
        dockspace->viewport = viewport;
    }

    static void remove_dockspace(Viewport* const viewport, Dockspace* const dockspace) {
        for (i64 i = 0; i < viewport->dockspaces.size(); i += 1) {
            if (viewport->dockspaces[i] == dockspace) {
                dockspace->viewport = nullptr;
                viewport->dockspaces.erase_unsorted_unchecked(i);
                break;
            }
        }
    }

    static Dockspace* create_dockspace(Context& ctx) {
        Dockspace* const dockspace = new Dockspace;
        dockspace->layout_parent = nullptr;
        dockspace->tile_type = Layout_Tile_Type::dockspace;
        ctx.dockspaces.emplace_back(dockspace);
        return dockspace;
    }

    static void destroy_dockspace(Context& ctx, Dockspace* const dockspace) {
        for (i64 i = 0; i < ctx.dockspaces.size(); ++i) {
            if (dockspace == ctx.dockspaces[i]) {
                ctx.dockspaces.erase_unsorted_unchecked(i);
                break;
            }
        }
        delete dockspace;
    }

    static void add_window(Dockspace* const dockspace, Window& window, i64 const index) {
        i64 const window_id = window.id;
        dockspace->windows.insert(index, window_id);
        dockspace->active_window = window_id;
        window.dockspace = dockspace;
    }

    static void remove_window(Dockspace* const dockspace, Window& window) {
        i64 const window_id = window.id;
        i64 const windows_count = dockspace->windows.size();
        for (i64 i = 0; i < windows_count; i += 1) {
            if (dockspace->windows[i] == window_id) {
                if (window_id == dockspace->active_window) {
                    // Prefer going left.
                    if (i > 0) {
                        dockspace->active_window = dockspace->windows[i - 1];
                    } else if (i + 1 < windows_count) {
                        dockspace->active_window = dockspace->windows[i + 1];
                    } else {
                        dockspace->active_window = -1;
                    }
                }

                dockspace->windows.erase(dockspace->windows.begin() + i, dockspace->windows.begin() + i + 1);
                window.dockspace = nullptr;
                break;
            }
        }
    }

    static Vector2 get_dockspace_screen_pos(Dockspace const* const dockspace) {
        return dockspace->position + get_viewport_position(dockspace->viewport);
    }

    static Vector2 get_dockspace_content_screen_pos(Dockspace const* const dockspace) {
        return dockspace->position + get_viewport_position(dockspace->viewport) + Vector2{0.0f, dockspace->tab_bar_height};
    }

    static Vector2 get_dockspace_size(Dockspace const* const dockspace) {
        return dockspace->content_size + Vector2{0.0f, dockspace->tab_bar_height};
    }

    static Vector2 get_dockspace_content_size(Dockspace const* const dockspace) {
        return dockspace->content_size;
    }

    static Vector2 get_dockspace_tab_bar_screen_pos(Dockspace const* const dockspace) {
        return Vector2{0.0f, 0.0f};
    }

    static Vector2 get_dockspace_tab_bar_size(Dockspace const* const dockspace) {
        return {dockspace->size.x, dockspace->tab_bar_height};
    }

    // Tab index of given window or -1 if window with given id doesn't exist in dockspace.
    static i64 get_tab_index(Dockspace* const dockspace, i64 const window) {
        for (i64 i = 0; i < dockspace->windows.size(); i += 1) {
            if (dockspace->windows[i] == window) {
                return i;
            }
        }

        return -1;
    }

    static i64 get_tab_count(Dockspace* const dockspace) {
        return dockspace->windows.size();
    }

    static f32 compute_tab_width(Dockspace* const dockspace) {
        return dockspace->content_size.x / dockspace->windows.size();
    }

    static f32 compute_tab_width(Dockspace* const dockspace, i64 const tab_count) {
        return dockspace->content_size.x / tab_count;
    }

    static Layout* create_layout_horizontal() {
        Layout* layout = new Layout;
        layout->layout_parent = nullptr;
        layout->tile_type = Layout_Tile_Type::horizontal_layout;
        return layout;
    }

    static Layout* create_layout_vertical() {
        Layout* layout = new Layout;
        layout->layout_parent = nullptr;
        layout->tile_type = Layout_Tile_Type::vertical_layout;
        return layout;
    }

    static void destroy_layout(Layout* layout) {
        delete layout;
    }

    static Layout_Tile* get_layout_parent(Dockspace* tile) {
        return tile->layout_parent;
    }

    static void recalculate_sublayout_size(Layout_Tile* tile) {
        switch (tile->tile_type) {
        case Layout_Tile_Type::root: {
            Layout_Root* const root = (Layout_Root*)tile;
            if (root->child) {
                root->child->size = root->size;
                root->child->position = {0.0f, 0.0f};
                recalculate_sublayout_size(root->child);
            }
        } break;

        case Layout_Tile_Type::horizontal_layout: {
            Layout* layout = (Layout*)tile;
            Vector2 const available_space = layout->size;
            f32 prev_width = 0.0f;
            for (Layout_Tile* child: layout->tiles) {
                prev_width += child->size.x;
            }

            if (prev_width == 0.0f) {
                f32 pos_offset = 0.0f;
                Vector2 const parent_pos = layout->position;
                Vector2 const child_size = {available_space.x / layout->tiles.size(), available_space.y};
                for (Layout_Tile* child: layout->tiles) {
                    child->size = child_size;
                    child->position = parent_pos + Vector2{pos_offset, 0.0f};
                    pos_offset += child_size.x;
                    recalculate_sublayout_size(child);
                }
            } else {
                f32 pos_offset = 0.0f;
                Vector2 const parent_pos = layout->position;
                f32 const size_factor = available_space.x / prev_width;
                for (Layout_Tile* child: layout->tiles) {
                    Vector2 const new_size = {child->size.x * size_factor, available_space.y};
                    child->size = new_size;
                    child->position = parent_pos + Vector2{pos_offset, 0.0f};
                    pos_offset += new_size.x;
                    recalculate_sublayout_size(child);
                }
            }
        } break;

        case Layout_Tile_Type::vertical_layout: {
            Layout* layout = (Layout*)tile;
            Vector2 const available_space = layout->size;
            f32 prev_height = 0.0f;
            for (Layout_Tile* child: layout->tiles) {
                prev_height += child->size.y;
            }

            if (prev_height == 0.0f) {
                f32 pos_offset = 0.0f;
                Vector2 const parent_pos = layout->position;
                Vector2 const child_size = {available_space.x, available_space.y / layout->tiles.size()};
                for (Layout_Tile* child: layout->tiles) {
                    child->size = child_size;
                    child->position = parent_pos + Vector2{0.0f, pos_offset};
                    pos_offset += child_size.x;
                    recalculate_sublayout_size(child);
                }
            } else {
                f32 pos_offset = 0.0f;
                Vector2 const parent_pos = layout->position;
                f32 const size_factor = available_space.y / prev_height;
                for (Layout_Tile* child: layout->tiles) {
                    Vector2 const new_size = {available_space.x, child->size.y * size_factor};
                    child->size = new_size;
                    child->position = parent_pos + Vector2{0.0f, pos_offset};
                    pos_offset += new_size.y;
                    recalculate_sublayout_size(child);
                }
            }
        } break;

        case Layout_Tile_Type::dockspace: {
            Dockspace* const dockspace = (Dockspace*)tile;
            Vector2 const tab_bar_size = get_dockspace_tab_bar_size(dockspace);
            dockspace->content_size = dockspace->size - Vector2{0.0f, tab_bar_size.y};
            // Nothing to do
        } break;
        }
    }

    static void _unparent_internal(Layout* layout) {
        Layout_Tile* parent = layout->layout_parent;
        if (layout->tiles.size() == 1) {
            switch (parent->tile_type) {
            case Layout_Tile_Type::root: {
                Layout_Root* const root = (Layout_Root*)parent;
                Layout_Tile* const tile = layout->tiles[0];
                root->child = tile;
                tile->layout_parent = root;
                tile->size = root->size;
                tile->position = {0.0f, 0.0f};
                recalculate_sublayout_size(tile);
            } break;

            case Layout_Tile_Type::horizontal_layout:
            case Layout_Tile_Type::vertical_layout: {
                Layout* const parent_layout = (Layout*)parent;
                for (i64 i = 0, tiles_count = parent_layout->tiles.size(); i < tiles_count; ++i) {
                    if (parent_layout->tiles[i] == layout) {
                        Layout_Tile* const tile = layout->tiles[0];
                        parent_layout->tiles[i] = tile;
                        tile->layout_parent = parent_layout;
                        tile->size = layout->size;
                        tile->position = layout->position;
                        recalculate_sublayout_size(tile);
                        break;
                    }
                }
            } break;

            default:
                ANTON_UNREACHABLE();
            }
            destroy_layout(layout);
        } else {
            recalculate_sublayout_size(layout);
        }
    }

    static void unparent(Dockspace* dockspace) {
        Layout_Tile* parent = dockspace->layout_parent;
        switch (parent->tile_type) {
        case Layout_Tile_Type::root: {
            Layout_Root* const root = (Layout_Root*)parent;
            root->child = nullptr;
            dockspace->layout_parent = nullptr;
        } break;

        case Layout_Tile_Type::vertical_layout:
        case Layout_Tile_Type::horizontal_layout: {
            Layout* const layout = (Layout*)parent;
            for (i64 i = 0, tiles_count = layout->tiles.size(); i < tiles_count; ++i) {
                if (layout->tiles[i] == dockspace) {
                    layout->tiles.erase(layout->tiles.begin() + i, layout->tiles.begin() + i + 1);
                    dockspace->layout_parent = nullptr;
                    break;
                }
            }
            _unparent_internal(layout);
        } break;

        default:
            ANTON_UNREACHABLE();
        }

        remove_dockspace(dockspace->viewport, dockspace);
    }

    // Parents dockspace to viewport's root.
    // This function is intended to be used only to handle the case when a viewport has no dockspaces.
    // If the viewport contains any dockspaces, this function doesn't remove them.
    // Assumes dockspace has no parent.
    //
    static void parent_to_root(Dockspace* dockspace, Viewport* viewport) {
        viewport->layout_root.child = dockspace;
        dockspace->layout_parent = &viewport->layout_root;
        dockspace->size = viewport->layout_root.size;
        recalculate_sublayout_size(&viewport->layout_root);
        add_dockspace(viewport, dockspace);
    }

    // Assumes dockspace has no parent.
    //
    static void parent_vertical(Dockspace* dockspace, Dockspace* relative_to, bool const parent_before) {
        Layout_Tile* const parent = get_layout_parent(relative_to);
        switch (parent->tile_type) {
        case Layout_Tile_Type::root: {
            Layout_Root* root = (Layout_Root*)parent;
            Layout* layout = create_layout_vertical();
            if (parent_before) {
                layout->tiles.emplace_back(dockspace);
                layout->tiles.emplace_back(relative_to);
            } else {
                layout->tiles.emplace_back(relative_to);
                layout->tiles.emplace_back(dockspace);
            }
            root->child = layout;
            layout->layout_parent = root;
            relative_to->layout_parent = layout;
            dockspace->layout_parent = layout;

            layout->size = root->size;
            dockspace->size = {layout->size.x, layout->size.y * 0.5f};
            relative_to->size = {layout->size.x, layout->size.y * 0.5f};
            // TODO: Recalculate size to account for resize bars.
            recalculate_sublayout_size(layout);
        } break;

        case Layout_Tile_Type::horizontal_layout: {
            Layout* parent_layout = (Layout*)parent;
            Layout* layout = create_layout_vertical();
            for (i64 i = 0, parent_tile_count = parent_layout->tiles.size(); i < parent_tile_count; ++i) {
                if (parent_layout->tiles[i] == relative_to) {
                    parent_layout->tiles[i] = layout;
                    break;
                }
            }
            layout->layout_parent = parent_layout;
            if (parent_before) {
                layout->tiles.emplace_back(dockspace);
                layout->tiles.emplace_back(relative_to);
            } else {
                layout->tiles.emplace_back(relative_to);
                layout->tiles.emplace_back(dockspace);
            }
            relative_to->layout_parent = layout;
            dockspace->layout_parent = layout;

            layout->size = relative_to->size;
            layout->position = relative_to->position;
            dockspace->size = {layout->size.x * 0.5f, layout->size.y};
            relative_to->size = {layout->size.x * 0.5f, layout->size.y};
            // TODO: Recalculate size to account for resize bars.
            recalculate_sublayout_size(layout);
        } break;

        case Layout_Tile_Type::vertical_layout: {
            Layout* parent_layout = (Layout*)parent;
            for (i64 i = 0, parent_tile_count = parent_layout->tiles.size(); i < parent_tile_count; ++i) {
                if (parent_layout->tiles[i] == relative_to) {
                    if (parent_before) {
                        parent_layout->tiles.insert(i, dockspace);
                    } else {
                        parent_layout->tiles.insert(i + 1, dockspace);
                    }
                    break;
                }
            }
            dockspace->layout_parent = parent_layout;

            // TODO: recalculate size to account for resize bars.
            dockspace->size = relative_to->size = {relative_to->size.x, relative_to->size.y * 0.5f};
            recalculate_sublayout_size(parent_layout);
        } break;

        default:
            ANTON_UNREACHABLE();
        }

        add_dockspace(relative_to->viewport, dockspace);
    }

    // Assumes dockspace has no parent.
    //
    static void parent_horizontal(Dockspace* dockspace, Dockspace* relative_to, bool const parent_before) {
        Layout_Tile* const parent = get_layout_parent(relative_to);
        switch (parent->tile_type) {
        case Layout_Tile_Type::root: {
            Layout_Root* root = (Layout_Root*)parent;
            Layout* layout = create_layout_horizontal();
            if (parent_before) {
                layout->tiles.emplace_back(dockspace);
                layout->tiles.emplace_back(relative_to);
            } else {
                layout->tiles.emplace_back(relative_to);
                layout->tiles.emplace_back(dockspace);
            }
            root->child = layout;
            layout->layout_parent = root;
            relative_to->layout_parent = layout;
            dockspace->layout_parent = layout;

            // TODO: recalculate size to account for resize bars.
            layout->size = root->size;
            dockspace->size = {layout->size.x * 0.5f, layout->size.y};
            relative_to->size = {layout->size.x * 0.5f, layout->size.y};
            recalculate_sublayout_size(layout);
        } break;

        case Layout_Tile_Type::vertical_layout: {
            Layout* parent_layout = (Layout*)parent;
            Layout* layout = create_layout_horizontal();
            for (i64 i = 0, parent_tile_count = parent_layout->tiles.size(); i < parent_tile_count; ++i) {
                if (parent_layout->tiles[i] == relative_to) {
                    parent_layout->tiles[i] = layout;
                    break;
                }
            }
            layout->layout_parent = parent_layout;
            if (parent_before) {
                layout->tiles.emplace_back(dockspace);
                layout->tiles.emplace_back(relative_to);
            } else {
                layout->tiles.emplace_back(relative_to);
                layout->tiles.emplace_back(dockspace);
            }
            relative_to->layout_parent = layout;
            dockspace->layout_parent = layout;

            // TODO: recalculate size to account for resize bars.
            layout->size = relative_to->size;
            layout->position = relative_to->position;
            dockspace->size = {layout->size.x, layout->size.y * 0.5f};
            relative_to->size = {layout->size.x, layout->size.y * 0.5f};
            recalculate_sublayout_size(layout);
        } break;

        case Layout_Tile_Type::horizontal_layout: {
            Layout* parent_layout = (Layout*)parent;
            for (i64 i = 0, parent_tile_count = parent_layout->tiles.size(); i < parent_tile_count; ++i) {
                if (parent_layout->tiles[i] == relative_to) {
                    if (parent_before) {
                        parent_layout->tiles.insert(i, dockspace);
                    } else {
                        parent_layout->tiles.insert(i + 1, dockspace);
                    }
                    break;
                }
            }
            dockspace->layout_parent = parent_layout;

            // TODO: recalculate size to account for resize bars.
            dockspace->size = relative_to->size = {relative_to->size.x * 0.5f, relative_to->size.y};
            recalculate_sublayout_size(parent_layout);
        } break;

        default:
            ANTON_UNREACHABLE();
        }

        add_dockspace(relative_to->viewport, dockspace);
    }

    void set_main_viewport_native_window(Context& ctx, windowing::Window* window) {
        ANTON_VERIFY(window, "window is nullptr.");
        Viewport* const main_viewport = ctx.viewports[0];
        main_viewport->native_window = window;
        main_viewport->layout_root.size = get_viewport_size(main_viewport);
        recalculate_sublayout_size(&main_viewport->layout_root);
    }

    void set_default_font(Context& ctx, Font_Style style) {
        // TODO: Should this function change default style?
        ctx.default_font_style = style;
    }

    void set_default_style_default_dark(Context& ctx) {
        Style default_theme;
        default_theme.background_color = {0.1f, 0.1f, 0.1f};
        default_theme.preview_guides_color = {0.5f, 0.5f, 0.5f};
        default_theme.preview_color = {0.0f, 111.0f / 255.0f, 1.0f, 0.5f};
        default_theme.widgets.background_color = {0.1f, 0.1f, 0.1f};
        default_theme.button.background_color = {0.1f, 0.1f, 0.1f};
        default_theme.button.border_color = {0.1f, 0.1f, 0.1f};
        default_theme.button.border = {0.0f, 0.0f, 0.0f, 0.0f};
        default_theme.button.padding = {0.0f, 0.0f, 0.0f, 0.0f};
        default_theme.button.font = ctx.default_font_style;
        default_theme.hot_button = default_theme.button;
        default_theme.hot_button.background_color = {0.25f, 0.25f, 0.25f};
        default_theme.active_button = default_theme.hot_button;
        default_theme.active_button.background_color = {0.4f, 0.4f, 0.4f};
        ctx.default_style = default_theme;
    }

    void set_default_style(Context& ctx, Style style) {
        ctx.default_style = style;
    }

    Style get_default_style(Context& ctx) {
        return ctx.default_style;
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
    static i32 check_cursor_in_border_area(Vector2 const cursor_pos, Vector2 const pos, Vector2 const size, Vector2 const border_area_width) {
        Vector2 const aspect = {size.y / size.x, 1.0f};
        Vector2 const size_a = {size.y, size.y};
        Vector2 const cursor_a = math::multiply_componentwise(cursor_pos - pos, aspect);
        Vector2 const border_a = math::multiply_componentwise(border_area_width, aspect);
        if (!test_point_in_box(cursor_a, border_a, size_a - 2 * border_a)) {
            f32 const dist_top = cursor_a.y;
            f32 const dist_bottom = size_a.y - cursor_a.y;
            f32 const dist_left = cursor_a.x;
            f32 const dist_right = size_a.x - cursor_a.x;
            f32 const closest_edge_dist = math::min(math::min(dist_top, dist_bottom), math::min(dist_left, dist_right));
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

    static void process_input(Context& ctx) {
        Vector2 const cursor = ctx.input.cursor_position;
        bool const just_left_clicked = !ctx.prev_input.left_mouse_button && ctx.input.left_mouse_button;
        bool const just_left_released = ctx.prev_input.left_mouse_button && !ctx.input.left_mouse_button;
        // TODO: Hoist hot window search.
        if (just_left_clicked) {
            if (ctx.input.left_mouse_button) {
                if (ctx.hot_window != -1) {
                    ctx.active_window = ctx.hot_window;
                    Window& window = ctx.windows.find(ctx.active_window)->value;
                    Dockspace* const dockspace = window.dockspace;
                    dockspace->active_window = window.id;

                    Vector2 const dockspace_pos = get_dockspace_screen_pos(dockspace);
                    Vector2 const tab_size = {compute_tab_width(dockspace), dockspace->tab_bar_height};
                    i64 const tab_count = dockspace->windows.size();
                    for (i64 i = 0; i < tab_count; i += 1) {
                        Vector2 const tab_pos = dockspace_pos + Vector2{tab_size.x * i, 0.0f};
                        if (test_cursor_in_box(cursor, tab_pos, tab_size)) {
                            ctx.clicked_tab = true;
                            ctx.drag.tab_click_offset = cursor - tab_pos;
                            break;
                        }
                    }
                }
            }
        } else if (ctx.input.left_mouse_button) {
            if (ctx.active_window != -1) {
                Window& window = ctx.windows.find(ctx.active_window)->value;
                Vector2 const cursor_pos_delta = ctx.input.cursor_position - ctx.prev_input.cursor_position;
                // ANTON_LOG_INFO("cursor_pos_delta: " + atl::to_string(cursor_pos_delta.x) + " " + atl::to_string(cursor_pos_delta.y));
                if (ctx.dragging) {
                    ctx.drag.hot_dockspace = nullptr;
                    for (Dockspace* const dockspace: ctx.dockspaces) {
                        Vector2 const dockspace_pos = get_dockspace_screen_pos(dockspace);
                        Vector2 const dockspace_size = get_dockspace_size(dockspace);
                        bool const in_dockspace = test_point_in_box(cursor, dockspace_pos, dockspace_size);
                        if (dockspace != window.dockspace && in_dockspace) {
                            ctx.drag.hot_dockspace = dockspace;
                        }
                    }


                    if(ctx.drag.alien_dockspace) {
                        // We were in a dockspace's tab bar last frame.
                        Dockspace* const alien_dockspace = ctx.drag.alien_dockspace;
                        if (test_cursor_in_box(cursor, get_dockspace_screen_pos(alien_dockspace), get_dockspace_tab_bar_size(alien_dockspace))) {
                            // We are still in tab bar. Update tab position.
                            f32 const tab_width = compute_tab_width(alien_dockspace);
                            Vector2 const dockspace_pos = get_dockspace_screen_pos(alien_dockspace);
                            f32 const offset = cursor.x - dockspace_pos.x;
                            i64 const index = math::floor(offset / tab_width);
                            i64 const current_tab_index = get_tab_index(alien_dockspace, window.id);
                            if (current_tab_index != index) {
                                // Readd window at new index.
                                remove_window(alien_dockspace, window);
                                add_window(alien_dockspace, window, index);
                            }
                        } else {
                            // We left the tab bar. Create new undecorated viewport.
                            remove_window(ctx.drag.alien_dockspace, window);
                            Dockspace* const dockspace = create_dockspace(ctx);
                            add_window(dockspace, window, 0);

                            Viewport* const viewport = create_viewport(ctx, ctx.drag.original_size, false);
                            parent_to_root(dockspace, viewport);
                            f32 const tab_offset = compute_tab_width(ctx.drag.alien_dockspace);
                            i64 const tab_index = get_tab_index(ctx.drag.alien_dockspace, window.id);
                            Vector2 const new_pos = cursor - ctx.drag.tab_click_offset;
                            set_viewport_screen_pos(viewport, new_pos);

                            ctx.drag.alien_dockspace = nullptr;
                        }
                    } else {
                        set_viewport_screen_pos(window.dockspace->viewport, cursor - ctx.drag.tab_click_offset);
                    }
                }

                if (ctx.clicked_tab && !ctx.dragging && cursor_pos_delta != Vector2{0.0f, 0.0f}) {
                    ctx.dragging = true;
                    ctx.drag.original_size = window.dockspace->size;
                    if (window.dockspace->windows.size() == 1) {
                        Dockspace* const dockspace = window.dockspace;
                        Viewport* const current_viewport = dockspace->viewport;
                        // Prevent main viewport from being moved by the dockspaces.
                        if (current_viewport->dockspaces.size() == 1 && current_viewport != ctx.viewports[0]) {
                            Vector2 const current_pos = get_viewport_screen_pos(current_viewport);
                            set_viewport_screen_pos(current_viewport, current_pos + cursor_pos_delta);
                        } else {
                            Vector2 const current_pos = get_dockspace_screen_pos(dockspace);
                            Viewport* const viewport = create_viewport(ctx, get_dockspace_size(dockspace), false);
                            set_viewport_screen_pos(viewport, current_pos + cursor_pos_delta);
                            unparent(dockspace);
                            parent_to_root(dockspace, viewport);
                        }
                    } else {
                        // TODO: Only tests for containment in tab bar.
                        if (test_point_in_box(cursor, get_dockspace_screen_pos(window.dockspace), get_dockspace_tab_bar_size(window.dockspace))) {
                            ctx.drag.alien_dockspace = window.dockspace;
                        } else {
                            Dockspace* const prev_dockspace = window.dockspace;
                            remove_window(prev_dockspace, window);
                            Dockspace* const dockspace = create_dockspace(ctx);
                            add_window(dockspace, window, 0);

                            Viewport* viewport = create_viewport(ctx, get_dockspace_size(prev_dockspace), false);
                            parent_to_root(dockspace, viewport);
                            i64 const tab_index = get_tab_index(prev_dockspace, prev_dockspace->active_window);
                            f32 const tab_width = compute_tab_width(prev_dockspace);
                            set_viewport_screen_pos(viewport, get_dockspace_screen_pos(prev_dockspace) + Vector2{tab_index * tab_width, 0} + cursor_pos_delta);
                        }
                    }
                }
            }
        } else {
            if (just_left_released) {
                if (ctx.dragging && !ctx.drag.alien_dockspace) {
                    Window& window = ctx.windows.find(ctx.active_window)->value;
                    Viewport* main_viewport = get_main_viewport(ctx);
                    bool const cursor_in_main_viewport = test_point_in_box(cursor, get_viewport_screen_pos(main_viewport), get_viewport_size(main_viewport));
                    if (main_viewport->dockspaces.size() == 0 && cursor_in_main_viewport) {
                        // Special case for main_viewport when it has 0 dockspaces.
                        // We have to parent to root in this case.
                        Dockspace* const dockspace = window.dockspace;
                        Viewport* prev_viewport = dockspace->viewport;
                        unparent(dockspace);
                        destroy_viewport(ctx, prev_viewport);
                        parent_to_root(dockspace, main_viewport);
                    } else {
                        for (Dockspace* const dockspace: ctx.dockspaces) {
                            Vector2 const dockspace_content_pos = get_dockspace_content_screen_pos(dockspace);
                            Vector2 const dockspace_content_size = get_dockspace_content_size(dockspace);
                            Vector2 const dockspace_tab_bar_screen_pos = get_dockspace_tab_bar_screen_pos(dockspace);
                            Vector2 const dockspace_tab_bar_size = get_dockspace_tab_bar_size(dockspace);
                            if (dockspace != window.dockspace && test_point_in_box(cursor, dockspace_content_pos, dockspace_content_size)) {
                                Viewport* const prev_viewport = window.dockspace->viewport;
                                Vector2 const border_area_width = dockspace_content_size * 0.5f * ctx.settings.window_drop_area_width;
                                i32 const border_section = check_cursor_in_border_area(cursor, dockspace_content_pos, dockspace_content_size, border_area_width);
                                switch (border_section) {
                                case 0:
                                case 2: {
                                    unparent(window.dockspace);
                                    bool const parent_before = border_section == 0;
                                    parent_vertical(window.dockspace, dockspace, parent_before);
                                    destroy_viewport(ctx, prev_viewport);
                                } break;

                                case 1:
                                case 3: {
                                    unparent(window.dockspace);
                                    bool const parent_before = border_section == 3;
                                    parent_horizontal(window.dockspace, dockspace, parent_before);
                                    destroy_viewport(ctx, prev_viewport);
                                } break;
                                }

                                // Can only be over 1 dockspace (enforced by layout and z-order)
                                break;
                            } else if(dockspace != window.dockspace && test_point_in_box(cursor, dockspace_tab_bar_screen_pos, dockspace_tab_bar_size)) {
                                i64 const tab_count = get_tab_count(dockspace);
                                f32 const tab_width = compute_tab_width(dockspace, tab_count + 1);
                                Vector2 const dockspace_pos = get_dockspace_screen_pos(dockspace);
                                f32 const offset = cursor.x - dockspace_pos.x;
                                i64 const index = math::floor(offset / tab_width);
                                destroy_viewport(ctx, window.dockspace->viewport);
                                destroy_dockspace(ctx, window.dockspace);
                                add_window(dockspace, window, index);
                                
                                // Can only be over 1 dockspace (enforced by layout and z-order)
                                break;
                            }
                        }
                    }
                }

                ctx.dragging = false;
                ctx.clicked_tab = false;
                ctx.drag.alien_dockspace = nullptr;
                ctx.drag.hot_dockspace = nullptr;
            }

            ctx.hot_window = -1;
            Dockspace* hot_dockspace = nullptr;
            for (Dockspace* const dockspace: ctx.dockspaces) {
                Vector2 const dockspace_pos = get_dockspace_screen_pos(dockspace);
                Vector2 const dockspace_size = get_dockspace_size(dockspace);
                if (test_cursor_in_box(cursor, dockspace_pos, dockspace_size)) {
                    hot_dockspace = dockspace;
                }
            }

            if (hot_dockspace) {
                Vector2 const hot_dockspace_pos = get_dockspace_screen_pos(hot_dockspace);
                Vector2 const content_pos = get_dockspace_content_screen_pos(hot_dockspace);
                Vector2 const hot_dockspace_size = get_dockspace_size(hot_dockspace);
                if (test_cursor_in_box(cursor, content_pos, hot_dockspace_size)) {
                    ctx.hot_window = hot_dockspace->active_window;
                } else {
                    Vector2 const tab_size = {hot_dockspace_size.x / hot_dockspace->windows.size(), hot_dockspace->tab_bar_height};
                    f32 tab_offset = 0;
                    for (i64 i = 0; i < hot_dockspace->windows.size(); i += 1) {
                        Vector2 const tab_pos = hot_dockspace_pos + Vector2{tab_offset, 0.0f};
                        if (test_cursor_in_box(cursor, tab_pos, tab_size)) {
                            ctx.hot_window = hot_dockspace->windows[i];
                            break;
                        }
                        tab_offset += tab_size.x;
                    }
                }
            }
        }

        ctx.prev_input = ctx.input;
    }

    void begin_frame(Context& ctx) {
        ctx.widgets.clear();
        ctx.vertex_buffer.clear();
        ctx.index_buffer.clear();

        for (Viewport* viewport: ctx.viewports) {
            viewport->draw_commands_buffer.clear();
        }

        for (auto [_, window]: ctx.windows) {
            window.enabled = false;
            window.draw_context.draw_pos = Vector2{0.0f, 0.0f};
            window.draw_context.index_buffer.clear();
            window.draw_context.vertex_buffer.clear();
            window.draw_context.draw_commands.clear();
        }

        process_input(ctx);
    }

    // From top-left counterclockwise order of vertices.
    static void add_quad(atl::Vector<Vertex>& verts, atl::Vector<u32>& indices, Vertex v1, Vertex v2, Vertex v3, Vertex v4,
                         u32 inx1, u32 inx2, u32 inx3, u32 inx4, u32 inx5, u32 inx6) {
        verts.emplace_back(v1);
        verts.emplace_back(v2);
        verts.emplace_back(v3);
        verts.emplace_back(v4);
        indices.emplace_back(inx1);
        indices.emplace_back(inx2);
        indices.emplace_back(inx3);
        indices.emplace_back(inx4);
        indices.emplace_back(inx5);
        indices.emplace_back(inx6);
    }

    void end_frame(Context& ctx) {
        ANTON_VERIFY(ctx.current_window == -1, "A window has not been ended prior to call to end_frame.");

        // TODO: handle disabled windows. we have to recalculate layout if we remove dockspaces

        // Remove disabled windows from dockspaces and destroy empty dockspaces.
        // for (i64 i = 0; i < ctx.dockspaces.size(); i += 1) {
        //     Dockspace* const dockspace = ctx.dockspaces[i];
        //     for (i64 j = 0; j < dockspace->windows.size(); j += 1) {
        //         Window const& window = ctx.windows.at(dockspace->windows[j]);
        //         if (!window.enabled) {
        //             if (window.id == dockspace->active_window) {
        //                 // Prefer going left.
        //                 if (j > 0) {
        //                     dockspace->active_window = dockspace->windows[j - 1];
        //                 } else if (j + 1 < dockspace->windows.size()) {
        //                     dockspace->active_window = dockspace->windows[j + 1];
        //                 }
        //                 // Else dockspace has no windows and will be removed, so we don't care.
        //             }

        //             dockspace->windows.erase(dockspace->windows.begin() + j, dockspace->windows.end() + j + 1);
        //             j -= 1;
        //         }
        //     }

        //     if (dockspace->windows.size() == 0) {
        //         destroy_dockspace(ctx, dockspace);
        //         i -= 1;
        //     }
        // }

        // generate geometry and indices,
        // and build command list

        // TODO: Windows z-order

        auto& verts = ctx.vertex_buffer;
        auto& indices = ctx.index_buffer;
        for (Dockspace const* const dockspace: ctx.dockspaces) {
            atl::Vector<Draw_Command>& draw_cmd_buffer = dockspace->viewport->draw_commands_buffer;
            Vector2 const dockspace_pos = get_dockspace_screen_pos(dockspace);
            Vector2 const dockspace_size = get_dockspace_size(dockspace);
            Vector2 const dockspace_content_pos = get_dockspace_content_screen_pos(dockspace);
            Vector2 const dockspace_content_size = get_dockspace_content_size(dockspace);

            // Tab bar background
            Draw_Command tab_bar_cmd;
            tab_bar_cmd.vertex_offset = verts.size();
            // TODO: Make color customizable.
            Vertex::Color const tab_bar_color = color_to_vertex_color(ctx.default_style.background_color);
            verts.emplace_back(dockspace_pos, Vector2{0.0f, 1.0f}, tab_bar_color);
            verts.emplace_back(dockspace_pos + Vector2{0, dockspace->tab_bar_height}, Vector2{0.0f, 0.0f}, tab_bar_color);
            verts.emplace_back(dockspace_pos + Vector2{dockspace_size.x, dockspace->tab_bar_height}, Vector2{1.0f, 0.0f}, tab_bar_color);
            verts.emplace_back(dockspace_pos + Vector2{dockspace_size.x, 0}, Vector2{1.0f, 1.0f}, tab_bar_color);
            tab_bar_cmd.index_offset = indices.size();
            indices.push_back(0);
            indices.push_back(1);
            indices.push_back(2);
            indices.push_back(0);
            indices.push_back(2);
            indices.push_back(3);
            tab_bar_cmd.element_count = 6;
            // No texture
            tab_bar_cmd.texture = 0;
            draw_cmd_buffer.emplace_back(tab_bar_cmd);

            // Render tabs
            f32 const tab_width = dockspace_size.x / dockspace->windows.size();
            f32 tab_offset = 0.0f;
            f32 constexpr separator_width = 5;
            for (i64 const id: dockspace->windows) {
                Window& window = ctx.windows.find(id)->value;
                Vertex::Color const tab_color = id == ctx.hot_window || id == ctx.active_window
                                                    ? color_to_vertex_color(window.style.background_color * Color(1.5f, 1.5f, 1.5f, 1.0f))
                                                    : color_to_vertex_color(window.style.background_color * Color(1.2f, 1.2f, 1.2f, 1.0f));

                // Vertex::Color const tab_color =
                //     (id == ctx.hot_window || id == ctx.active_window ? Vertex::Color{255, 187, 61, 255}
                //                                                      : Vertex::Color{224, 138, 0, 255}); // Vertex::Color{255, 157, 0, 255}
                Vector2 const tab_pos = dockspace_pos + Vector2{tab_offset, 0};
                Draw_Command cmd;
                cmd.vertex_offset = verts.size();
                verts.emplace_back(tab_pos, Vector2{0.0f, 1.0f}, tab_color);
                verts.emplace_back(tab_pos + Vector2{0.0f, dockspace->tab_bar_height}, Vector2{0.0f, 0.0f}, tab_color);
                verts.emplace_back(tab_pos + Vector2{tab_width - separator_width, dockspace->tab_bar_height}, Vector2{1.0f, 0.0f}, tab_color);
                verts.emplace_back(tab_pos + Vector2{tab_width - separator_width, 0.0f}, Vector2{1.0f, 1.0f}, tab_color);
                cmd.index_offset = indices.size();
                indices.push_back(0);
                indices.push_back(1);
                indices.push_back(2);
                indices.push_back(0);
                indices.push_back(2);
                indices.push_back(3);
                cmd.element_count = 6;
                // No texture
                cmd.texture = 0;
                draw_cmd_buffer.emplace_back(cmd);

                // TODO: Temporarily added tab separators.
                Vertex::Color const separator_color = {50, 50, 50, 255};
                Vector2 const separator_pos = dockspace_pos + Vector2{tab_offset + tab_width - separator_width, 0};
                Draw_Command separator_cmd;
                separator_cmd.vertex_offset = verts.size();
                verts.emplace_back(separator_pos, Vector2{0.0f, 1.0f}, separator_color);
                verts.emplace_back(separator_pos + Vector2{0.0f, dockspace->tab_bar_height}, Vector2{0.0f, 0.0f}, separator_color);
                verts.emplace_back(separator_pos + Vector2{separator_width, dockspace->tab_bar_height}, Vector2{1.0f, 0.0f}, separator_color);
                verts.emplace_back(separator_pos + Vector2{separator_width, 0.0f}, Vector2{1.0f, 1.0f}, separator_color);
                separator_cmd.index_offset = indices.size();
                indices.push_back(0);
                indices.push_back(1);
                indices.push_back(2);
                indices.push_back(0);
                indices.push_back(2);
                indices.push_back(3);
                separator_cmd.element_count = 6;
                // No texture
                separator_cmd.texture = 0;
                draw_cmd_buffer.emplace_back(separator_cmd);

                tab_offset += tab_width;
            }

            // Render window background
            Window& window = ctx.windows.find(dockspace->active_window)->value;
            Draw_Command cmd;
            cmd.vertex_offset = verts.size();
            Vertex::Color const color = color_to_vertex_color(window.style.background_color);
            Vector2 const window_pos = dockspace_content_pos;
            verts.emplace_back(window_pos, Vector2{0.0f, 1.0f}, color);
            verts.emplace_back(window_pos + Vector2{0, dockspace_content_size.y}, Vector2{0.0f, 0.0f}, color);
            verts.emplace_back(window_pos + dockspace_content_size, Vector2{1.0f, 0.0f}, color);
            verts.emplace_back(window_pos + Vector2{dockspace_content_size.x, 0}, Vector2{1.0f, 1.0f}, color);
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
            draw_cmd_buffer.emplace_back(cmd);

            // Copy window's draw list
            u32 const index_offset = indices.size();
            u32 const vertex_offset = verts.size();
            u32 const cmd_count = draw_cmd_buffer.size();
            Draw_Context& dc = window.draw_context;
            verts.insert(verts.size(), dc.vertex_buffer.begin(), dc.vertex_buffer.end());
            for(i64 i = vertex_offset; i < verts.size(); ++i) {
                verts[i].position += dockspace_content_pos;
            }
            indices.insert(indices.size(), dc.index_buffer.begin(), dc.index_buffer.end());
            draw_cmd_buffer.insert(draw_cmd_buffer.size(), dc.draw_commands.begin(), dc.draw_commands.end());
            for(i64 i = cmd_count; i < draw_cmd_buffer.size(); ++i) {
                draw_cmd_buffer[i].vertex_offset += vertex_offset;
                draw_cmd_buffer[i].index_offset += index_offset;
            }

            for (i64 const id: dockspace->windows) {
                Window& window = ctx.windows.find(id)->value;
                Draw_Context& dc = window.draw_context;
                dc.vertex_buffer.clear();
                dc.index_buffer.clear();
                dc.draw_commands.clear();
                dc.draw_pos = Vector2{0.0f, 0.0f};
            }
        }

        // Render dockspce drag preview guides
        if (ctx.drag.hot_dockspace) {
            Viewport* const viewport = ctx.drag.hot_dockspace->viewport;
            Vector2 const dockspace_content_pos = get_dockspace_content_screen_pos(ctx.drag.hot_dockspace);
            Vector2 const dockspace_content_size = get_dockspace_content_size(ctx.drag.hot_dockspace);
            Vector2 const dockspace_tab_bar_pos = get_dockspace_tab_bar_screen_pos(ctx.drag.hot_dockspace);
            Vector2 const dockspace_tab_bar_size = get_dockspace_tab_bar_size(ctx.drag.hot_dockspace);
            Vector2 const cursor = ctx.input.cursor_position;
            if(test_point_in_box(cursor, dockspace_content_pos, dockspace_content_size)) {
                // In dockspace's content area.
                Vertex::Color const color = color_to_vertex_color(ctx.default_style.preview_guides_color);
                Vector2 const border_area_width = dockspace_content_size * 0.5f * ctx.settings.window_drop_area_width;

                Vector2 const _verts[] = {
                    // outside top-left
                    dockspace_content_pos + Vector2{1.0f, 0.0f},
                    dockspace_content_pos + Vector2{0.0f, 1.0f},
                    // outside top-right
                    dockspace_content_pos + Vector2{dockspace_content_size.x - 1.0f, 0.0f},
                    dockspace_content_pos + Vector2{dockspace_content_size.x, 1.0f},
                    // outside bottom-left
                    dockspace_content_pos + Vector2{0.0, dockspace_content_size.y - 1.0f},
                    dockspace_content_pos + Vector2{1.0f, dockspace_content_size.y},
                    // outside top-right
                    dockspace_content_pos + dockspace_content_size + Vector2{0.0f, -1.0f},
                    dockspace_content_pos + dockspace_content_size + Vector2{-1.0f, 0.0f},

                    // inside top-left
                    dockspace_content_pos + border_area_width + Vector2{0.0f, -1.0f},
                    dockspace_content_pos + border_area_width + Vector2{-1.0f, 0.0f},
                    // inside top-right
                    dockspace_content_pos + Vector2{dockspace_content_size.x - border_area_width.x, border_area_width.y - 1.0f},
                    dockspace_content_pos + Vector2{dockspace_content_size.x - border_area_width.x + 1.0f, border_area_width.y},
                    // inside bottom-left
                    dockspace_content_pos + Vector2{border_area_width.x - 1.0f, dockspace_content_size.y - border_area_width.y},
                    dockspace_content_pos + Vector2{border_area_width.x, dockspace_content_size.y - border_area_width.y + 1.0f},
                    // inside bottom-right
                    dockspace_content_pos + Vector2{dockspace_content_size.x - border_area_width.x + 1.0f, dockspace_content_size.y - border_area_width.y},
                    dockspace_content_pos + Vector2{dockspace_content_size.x - border_area_width.x, dockspace_content_size.y - border_area_width.y + 1.0f},
                };

                u32 const _indices[] = {
                    // inside rectangle
                    8, 9, 11, 8, 11, 10,    // top
                    10, 15, 14, 10, 14, 11, // right
                    12, 13, 15, 13, 15, 14, // bottom
                    8, 9, 12, 8, 12, 13,    // left
                    // outside lines
                    0, 1, 9, 0, 9, 8,    // top-left
                    2, 10, 11, 2, 11, 3, // top-right
                    4, 13, 12, 4, 5, 13, // bottom-left
                    7, 14, 15, 7, 6, 14, // bottom-right
                };

                Draw_Command guides_cmd;
                guides_cmd.vertex_offset = verts.size();
                guides_cmd.index_offset = indices.size();
                guides_cmd.element_count = atl::size(_indices);
                guides_cmd.texture = 0;
                viewport->draw_commands_buffer.emplace_back(guides_cmd);

                for (i64 i = 0; i < (i64)atl::size(_verts); ++i) {
                    verts.emplace_back(_verts[i], Vector2{0, 0}, color);
                }

                for (i64 i = 0; i < (i64)atl::size(_indices); ++i) {
                    indices.emplace_back(_indices[i]);
                }

                i32 const border_section = check_cursor_in_border_area(cursor, dockspace_content_pos, dockspace_content_size, border_area_width);
                if (border_section != -1) {
                    Draw_Command preview_cmd;
                    preview_cmd.vertex_offset = verts.size();
                    preview_cmd.index_offset = indices.size();
                    preview_cmd.element_count = 6;
                    preview_cmd.texture = 0;
                    viewport->draw_commands_buffer.emplace_back(preview_cmd);

                    Vertex::Color const preview_color = color_to_vertex_color(ctx.default_style.preview_color);
                    Dockspace* const dockspace = ctx.drag.hot_dockspace;
                    Vector2 const dockspace_pos = get_dockspace_screen_pos(dockspace);
                    Vector2 const dockspace_size = get_dockspace_size(dockspace);
                    Vector2 preview_pos;
                    Vector2 preview_size;
                    switch (border_section) {
                    case 0: {
                        preview_pos = dockspace_pos;
                        preview_size = {dockspace_size.x, dockspace_size.y * 0.5f};
                    } break;

                    case 1: {
                        preview_pos = dockspace_pos + Vector2{dockspace_size.x * 0.5f, 0.0f};
                        preview_size = {dockspace_size.x * 0.5f, dockspace_size.y};
                    } break;

                    case 2: {
                        preview_pos = dockspace_pos + Vector2{0.0f, dockspace_size.y * 0.5f};
                        preview_size = {dockspace_size.x, dockspace_size.y * 0.5f};
                    } break;

                    case 3: {
                        preview_pos = dockspace_pos;
                        preview_size = {dockspace_size.x * 0.5f, dockspace_size.y};
                    } break;
                    }
                    verts.emplace_back(preview_pos, Vector2{0, 1.0}, preview_color);
                    verts.emplace_back(preview_pos + Vector2{0.0f, preview_size.y}, Vector2{0.0f, 0.0f}, preview_color);
                    verts.emplace_back(preview_pos + preview_size, Vector2{1.0f, 0.0f}, preview_color);
                    verts.emplace_back(preview_pos + Vector2{preview_size.x, 0.0f}, Vector2{1.0f, 1.0f}, preview_color);

                    indices.push_back(0);
                    indices.push_back(1);
                    indices.push_back(2);
                    indices.push_back(0);
                    indices.push_back(2);
                    indices.push_back(3);
                }
            } else if(test_point_in_box(cursor, dockspace_tab_bar_pos, dockspace_tab_bar_size)) {
                Draw_Command cmd;
                cmd.texture = 0;
                cmd.element_count = 6;
                cmd.vertex_offset = verts.size();
                cmd.index_offset = indices.size();
                viewport->draw_commands_buffer.emplace_back(cmd);

                Vertex::Color const preview_color = color_to_vertex_color(ctx.default_style.preview_color);
                verts.emplace_back(dockspace_content_pos, Vector2{0.0f, 1.0f}, preview_color);
                verts.emplace_back(dockspace_content_pos + Vector2{0.0f, dockspace_content_size.y}, Vector2{0.0f, 0.0f}, preview_color);
                verts.emplace_back(dockspace_content_pos + dockspace_content_size, Vector2{1.0f, 0.0f}, preview_color);
                verts.emplace_back(dockspace_content_pos + Vector2{dockspace_content_size.x, 0.0f}, Vector2{1.0f, 1.0f}, preview_color);
                indices.emplace_back(0);
                indices.emplace_back(1);
                indices.emplace_back(2);
                indices.emplace_back(0);
                indices.emplace_back(2);
                indices.emplace_back(3);
            }
        }
    }

    atl::Slice<Viewport* const> get_viewports(Context& ctx) {
        return ctx.viewports;
    }

    windowing::Window* get_viewport_native_window(Context&, Viewport& viewport) {
        return viewport.native_window;
    }

    atl::Slice<Draw_Command const> get_viewport_draw_commands(Context&, Viewport& viewport) {
        return viewport.draw_commands_buffer;
    }

    atl::Slice<Vertex const> get_vertex_data(Context& ctx) {
        return ctx.vertex_buffer;
    }

    atl::Slice<u32 const> get_index_data(Context& ctx) {
        return ctx.index_buffer;
    }

    // TODO: remove new_viewport.
    void begin_window(Context& ctx, atl::String_View identifier, bool new_viewport) {
        ANTON_VERIFY(ctx.current_window == -1, "Cannot create window inside another window.");

        i64 const id = hash_string(identifier);
        auto const iter = ctx.windows.find(id);
        if (iter == ctx.windows.end()) {
            Window window;
            window._debug_name = identifier;
            window.id = id;
            window.style = ctx.default_style;
            window.border_area_width = 4.0f;
            window.enabled = true;
            ctx.current_window = window.id;
            auto new_iter = ctx.windows.emplace(id, window);

            Dockspace* const dockspace = create_dockspace(ctx);
            add_window(dockspace, new_iter->value, 0);
            // TODO: Hardcoded size.
            Viewport* const viewport = create_viewport(ctx, {200.0f, 200.0f}, false);
            parent_to_root(dockspace, viewport);
        } else {
            iter->value.enabled = true;
            ctx.current_window = id;
        }
    }

    void end_window(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "Trying to end window, but none has been made current.");
        // ANTON_VERIFY(ctx.widget_stack.size() == 0, "Widget stack not empty. End all child widgets before you attempt to end the window.");
        ctx.current_window = -1;
    }

    void begin_widget(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "Cannot create widget because no window is current.");

        // Window& current_window = ctx.windows.at(ctx.current_window);
        // current_window.widgets.emplace_back(ctx.current_widget, atl::String{}, ctx.default_style.widgets, Vector2{});
        // ctx.current_widget = current_window.widgets.size() - 1;
    }

    void end_widget(Context& ctx) {
        ANTON_VERIFY(ctx.current_widget != -1, "No widgets are active.");

        // Window& current_window = ctx.windows.at(ctx.current_window);
        // ctx.current_widget = current_window.widgets[ctx.current_widget].parent;
    }

    void text(Context& ctx, atl::String_View text, Font_Style font) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");
    }

    Button_State button(Context& ctx, atl::String_View text) {
        return button(ctx, text, ctx.default_style.button, ctx.default_style.hot_button, ctx.default_style.active_button);
    }

    Button_State button(Context& ctx, atl::String_View text, Button_Style const inactive_style, Button_Style const hot_style, Button_Style const active_style) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");
        Window& window = ctx.windows.find(ctx.current_window)->value;
        Dockspace* const dockspace = window.dockspace;
        if(dockspace->active_window != window.id) {
            i64 const hash = hash_string(text);
            Button_State& state = window.button_state.find_or_emplace(hash)->value;
            state = Button_State::inactive;
            return Button_State::inactive;
        }

        // TODO: Check window z-order to determine whether this window is top-level for correct button behaviour.
        i64 const hash = hash_string(text);
        Button_State& state = window.button_state.find_or_emplace(hash)->value;
        Button_Style style;
        switch(state) {
            case Button_State::inactive: {
                style = inactive_style;
            } break;

            case Button_State::hot: {
                style = hot_style;
            } break;

            case Button_State::clicked: {
                style = active_style;
            } break;
        }

        // TODO: Text wrap

        rendering::Text_Metrics metrics = rendering::compute_text_dimensions(style.font.face, {style.font.size, style.font.h_dpi, style.font.v_dpi}, text);
        f32 button_height = math::max(0.0f, style.padding[0]) + math::max(0.0f, style.padding[2]) + metrics.height;
        f32 button_width = math::max(0.0f, style.padding[1]) + math::max(0.0f, style.padding[3]) + metrics.width;
        f32 border_height = button_height + style.border[0] + style.border[2];
        f32 border_width = button_width + style.border[1] + style.border[3];

        Draw_Context& dc = window.draw_context;
        Vector2 const border_draw_pos = dc.draw_pos;
        Vector2 const dockspace_pos = get_dockspace_content_screen_pos(dockspace);
        Vector2 const cursor = ctx.input.cursor_position;
        bool const lmb = ctx.input.left_mouse_button;
        if(test_point_in_box(cursor, dockspace_pos + border_draw_pos, Vector2{border_width, border_height})) {
            // TODO: Will report click even when the cursor hovers the button with lmb already pressed.
            if(lmb) {
                state = Button_State::clicked;
                style = active_style;
            } else {
                state = Button_State::hot;
                style = hot_style;
            }
        } else {
            state = Button_State::inactive;
            style = inactive_style;
        }

        metrics = rendering::compute_text_dimensions(style.font.face, {style.font.size, style.font.h_dpi, style.font.v_dpi}, text);
        button_height = math::max(0.0f, style.padding[0]) + math::max(0.0f, style.padding[2]) + metrics.height;
        button_width = math::max(0.0f, style.padding[1]) + math::max(0.0f, style.padding[3]) + metrics.width;
        border_height = button_height + style.border[0] + style.border[2];
        border_width = button_width + style.border[1] + style.border[3];

        Vector2 const button_draw_pos = border_draw_pos + Vector2{style.border[3], style.border[0]};
        Vertex::Color const bg_color = color_to_vertex_color(style.background_color);
        Vertex::Color const border_color = color_to_vertex_color(style.border_color);
        {
            Draw_Command cmd;
            cmd.texture = 0;
            cmd.element_count = 12;
            cmd.vertex_offset = dc.vertex_buffer.size();
            cmd.index_offset = dc.index_buffer.size();
            dc.vertex_buffer.emplace_back(border_draw_pos, Vector2{0.0f, 1.0f}, border_color);
            dc.vertex_buffer.emplace_back(border_draw_pos + Vector2{0.0f, border_height}, Vector2{0.0f, 0.0f}, border_color);
            dc.vertex_buffer.emplace_back(border_draw_pos + Vector2{border_width, border_height}, Vector2{1.0f, 0.0f}, border_color);
            dc.vertex_buffer.emplace_back(border_draw_pos + Vector2{border_width, 0.0f}, Vector2{1.0f, 1.0f}, border_color);
            dc.vertex_buffer.emplace_back(button_draw_pos, Vector2{0.0f, 1.0f}, bg_color);
            dc.vertex_buffer.emplace_back(button_draw_pos + Vector2{0.0f, button_height}, Vector2{0.0f, 0.0f}, bg_color);
            dc.vertex_buffer.emplace_back(button_draw_pos + Vector2{button_width, button_height}, Vector2{1.0f, 0.0f}, bg_color);
            dc.vertex_buffer.emplace_back(button_draw_pos + Vector2{button_width, 0.0f}, Vector2{1.0f, 1.0f}, bg_color);
            dc.index_buffer.emplace_back(0);
            dc.index_buffer.emplace_back(1);
            dc.index_buffer.emplace_back(2);
            dc.index_buffer.emplace_back(0);
            dc.index_buffer.emplace_back(2);
            dc.index_buffer.emplace_back(3);
            dc.index_buffer.emplace_back(0 + 4);
            dc.index_buffer.emplace_back(1 + 4);
            dc.index_buffer.emplace_back(2 + 4);
            dc.index_buffer.emplace_back(0 + 4);
            dc.index_buffer.emplace_back(2 + 4);
            dc.index_buffer.emplace_back(3 + 4);
            dc.draw_commands.emplace_back(cmd);
        }
        dc.draw_pos += Vector2{0.0f, border_height};

        Vector2 const text_draw_pos = button_draw_pos + Vector2{style.padding[3], style.padding[0]};
        {
            rendering::Font_Render_Info fri = {style.font.size, style.font.h_dpi, style.font.v_dpi};
            rendering::Text_Image text_img = rendering::render_text(style.font.face, fri, text);
            Draw_Command text_cmd;
            text_cmd.texture = text_img.texture;
            text_cmd.element_count = 6;
            text_cmd.vertex_offset = dc.vertex_buffer.size();
            text_cmd.index_offset = dc.index_buffer.size();
            dc.vertex_buffer.emplace_back(text_draw_pos, Vector2{0.0f, 1.0f}, Vertex::Color{0, 0, 0, 0});
            dc.vertex_buffer.emplace_back(text_draw_pos + Vector2{0.0f, (f32)text_img.height}, Vector2{0.0f, 0.0f}, Vertex::Color{0, 0, 0, 0});
            dc.vertex_buffer.emplace_back(text_draw_pos + Vector2{(f32)text_img.width, (f32)text_img.height}, Vector2{1.0f, 0.0f}, Vertex::Color{0, 0, 0, 0});
            dc.vertex_buffer.emplace_back(text_draw_pos + Vector2{(f32)text_img.width, 0.0f}, Vector2{1.0f, 1.0f}, Vertex::Color{0, 0, 0, 0});
            dc.index_buffer.emplace_back(0);
            dc.index_buffer.emplace_back(1);
            dc.index_buffer.emplace_back(2);
            dc.index_buffer.emplace_back(0);
            dc.index_buffer.emplace_back(2);
            dc.index_buffer.emplace_back(3);
            dc.draw_commands.emplace_back(text_cmd);
        }
        return state;
    }

    void image(Context& ctx, u64 const texture, Vector2 const size, Vector2 const uv_top_left, Vector2 const uv_bottom_right) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");
        Window& window = ctx.windows.find(ctx.current_window)->value;
        Dockspace* const dockspace = window.dockspace;
        if(dockspace->active_window != window.id) {
            return;
        }
        
        Vector2 const window_size = get_dockspace_content_size(window.dockspace);
        Draw_Context& dc = window.draw_context;
        Vector2 const draw_pos = dc.draw_pos;
        Vector2 const window_space = window_size - draw_pos;
        Vector2 const clipped_size = {math::min(window_space.x, size.x), math::min(window_space.y, size.y)};
        Vector2 const uv_diff = uv_bottom_right - uv_top_left;
        Vector2 const scale_fac = {clipped_size.x / size.x, clipped_size.y / size.y};
        Vector2 const uv_br = uv_top_left + math::multiply_componentwise(uv_diff, scale_fac);
        Vector2 const uv_tl = uv_top_left;

        Draw_Command cmd;
        cmd.texture = texture;
        cmd.element_count = 6;
        cmd.vertex_offset = dc.vertex_buffer.size();
        cmd.index_offset = dc.index_buffer.size();
        dc.vertex_buffer.emplace_back(draw_pos, uv_tl, Vertex::Color{0, 0, 0, 0});
        dc.vertex_buffer.emplace_back(draw_pos + Vector2{0.0f, clipped_size.y}, Vector2{uv_tl.x, uv_br.y}, Vertex::Color{0, 0, 0, 0});
        dc.vertex_buffer.emplace_back(draw_pos + clipped_size, uv_br, Vertex::Color{0, 0, 0, 0});
        dc.vertex_buffer.emplace_back(draw_pos + Vector2{clipped_size.x, 0.0f}, Vector2{uv_br.x, uv_tl.y}, Vertex::Color{0, 0, 0, 0});
        dc.index_buffer.emplace_back(0);
        dc.index_buffer.emplace_back(1);
        dc.index_buffer.emplace_back(2);
        dc.index_buffer.emplace_back(0);
        dc.index_buffer.emplace_back(2);
        dc.index_buffer.emplace_back(3);
        dc.draw_commands.emplace_back(cmd);
        
        dc.draw_pos += Vector2{0.0f, clipped_size.y};
    }

    // Get style of current widget or window
    Style get_style(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        Window& window = ctx.windows.find(ctx.current_window)->value;
        return window.style;
    }

    // Set style of current widget or window
    void set_style(Context& ctx, Style const style) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        Window& window = ctx.windows.find(ctx.current_window)->value;
        window.style = style;
    }

    void set_window_border_area(Context& ctx, f32 const width) {
        ANTON_VERIFY(width >= 1.0f, "Border area may not be thinner than 1 pixel.");
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        Window& window = ctx.windows.find(ctx.current_window)->value;
        window.border_area_width = width;
    }

    void set_window_size(Context& ctx, Vector2 const size) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        Window& window = ctx.windows.find(ctx.current_window)->value;
        Dockspace* dockspace = window.dockspace;
        if(dockspace->layout_parent->tile_type == Layout_Tile_Type::root && dockspace->windows.size() == 1) {
            Vector2 const tab_size = get_dockspace_tab_bar_size(dockspace);
            Vector2 const new_size = size + Vector2{0.0f, tab_size.y};
            set_viewport_size(dockspace->viewport, new_size);
            recalculate_sublayout_size(window.dockspace->layout_parent);
        }
    }

    void set_window_pos(Context& ctx, Vector2 const pos) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        Window& window = ctx.windows.find(ctx.current_window)->value;
        Dockspace* const dockspace = window.dockspace;
        if(dockspace->layout_parent->tile_type == Layout_Tile_Type::root && dockspace->windows.size() == 1) {
            set_viewport_screen_pos(dockspace->viewport, pos);
        }
    }

    bool is_window_hot(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        return ctx.hot_window == ctx.current_window;
    }

    bool is_window_active(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");

        return ctx.active_window == ctx.current_window;
    }

    Vector2 get_window_dimensions(Context& ctx) {
        ANTON_VERIFY(ctx.current_window != -1, "No current window.");
        Window& window = ctx.windows.find(ctx.current_window)->value;
        return get_dockspace_content_size(window.dockspace);
    }

    Vector2 get_cursor_position(Context& ctx) {
        return ctx.input.cursor_position;
    }
} // namespace anton_engine::imgui
