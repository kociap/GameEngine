#include <gizmo_context.hpp>

namespace anton_engine {
    static Gizmo_Context construct_default() {
        Gizmo_Context gizmo_ctx;
        gizmo_ctx.type = Gizmo_Transform_Type::translate;
        gizmo_ctx.space = Gizmo_Transform_Space::world;
        gizmo_ctx.grab.grabbed = false;
        return gizmo_ctx;
    }

    static Gizmo_Context ctx = construct_default();

    Gizmo_Context& get_gizmo_context() {
        return ctx;
    }
} // namespace anton_engine
