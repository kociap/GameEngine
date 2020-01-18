#include <imgui_rendering.hpp>

#include <anton_stl/vector.hpp>
#include <glad.hpp>
#include <opengl.hpp>

namespace anton_engine::imgui {
    struct GPU_Buffer {
        u32 handle;
        void* mapped;
        i64 size;
    };

    template <typename T>
    struct Buffer {
        T* buffer;
        T* head;
        i64 size;
    };

    static GPU_Buffer gpu_vertex_buffer;
    static Buffer<Vertex> vertex_buffer;
    static GPU_Buffer gpu_element_buffer;
    static Buffer<u32> element_buffer;
    static GPU_Buffer gpu_draw_cmd_buffer;
    static Buffer<Draw_Elements_Command> draw_cmd_buffer;
    static u32 vao;
    static anton_stl::Vector<Draw_Elements_Command> draw_commands;

    void setup_rendering() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 2, GL_FLOAT, false, offsetof(Vertex, position));
        glVertexAttribBinding(0, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribFormat(1, 2, GL_FLOAT, false, offsetof(Vertex, uv));
        glVertexAttribBinding(1, 0);
        glEnableVertexAttribArray(2);
        glVertexAttribFormat(2, 4, GL_UNSIGNED_BYTE, true, offsetof(Vertex, color));
        glVertexAttribBinding(2, 0);

        constexpr u32 buffer_flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT;

        gpu_vertex_buffer.size = 65536 * sizeof(Vertex);
        glGenBuffers(1, &gpu_vertex_buffer.handle);
        glBindBuffer(GL_ARRAY_BUFFER, gpu_vertex_buffer.handle);
        glBufferStorage(GL_ARRAY_BUFFER, gpu_vertex_buffer.size, 0, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        gpu_vertex_buffer.mapped = glMapBufferRange(GL_ARRAY_BUFFER, 0, gpu_vertex_buffer.size, buffer_flags);
        vertex_buffer.buffer = vertex_buffer.head = reinterpret_cast<Vertex*>(gpu_vertex_buffer.mapped);
        vertex_buffer.size = gpu_vertex_buffer.size / sizeof(Vertex);

        gpu_element_buffer.size = 65536 * sizeof(u32);
        glGenBuffers(1, &gpu_element_buffer.handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_element_buffer.handle);
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, gpu_element_buffer.size, 0, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        gpu_element_buffer.mapped = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, gpu_element_buffer.size, buffer_flags);
        element_buffer.buffer = element_buffer.head = reinterpret_cast<u32*>(gpu_element_buffer.mapped);
        element_buffer.size = gpu_element_buffer.size / sizeof(u32);

        gpu_draw_cmd_buffer.size = 16384 * sizeof(Draw_Elements_Command);
        glGenBuffers(1, &gpu_draw_cmd_buffer.handle);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gpu_draw_cmd_buffer.handle);
        glBufferStorage(GL_DRAW_INDIRECT_BUFFER, gpu_draw_cmd_buffer.size, 0, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        gpu_draw_cmd_buffer.mapped = glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, gpu_draw_cmd_buffer.size, buffer_flags);
        draw_cmd_buffer.buffer = draw_cmd_buffer.head = reinterpret_cast<Draw_Elements_Command*>(gpu_draw_cmd_buffer.mapped);
        draw_cmd_buffer.size = gpu_draw_cmd_buffer.size / sizeof(Draw_Elements_Command);
    }

    void bind_buffers() {
        glBindVertexArray(vao);
        glBindVertexBuffer(0, gpu_vertex_buffer.handle, 0, sizeof(Vertex));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_element_buffer.handle);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gpu_draw_cmd_buffer.handle);
    }

    Draw_Elements_Command write_geometry(anton_stl::Slice<Vertex const> const vertices, anton_stl::Slice<u32 const> const indices) {
        Draw_Elements_Command cmd = {};

        i64 vert_head_offset = vertex_buffer.head - vertex_buffer.buffer;
        if (vertex_buffer.size - vert_head_offset < vertices.size()) {
            vertex_buffer.head = vertex_buffer.buffer;
            vert_head_offset = 0;
        }
        memcpy(vertex_buffer.head, vertices.data(), vertices.size() * sizeof(Vertex));
        vertex_buffer.head += vertices.size();
        cmd.base_vertex = vert_head_offset;

        i64 elem_head_offset = element_buffer.head - element_buffer.buffer;
        if (element_buffer.size - elem_head_offset < indices.size()) {
            element_buffer.head = element_buffer.buffer;
            elem_head_offset = 0;
        }
        memcpy(element_buffer.head, indices.data(), indices.size() * sizeof(u32));
        element_buffer.head += indices.size();
        cmd.count = indices.size();
        cmd.first_index = elem_head_offset;

        return cmd;
    }

    void add_draw_command(Draw_Elements_Command const cmd) {
        draw_commands.push_back(cmd);
    }

    void commit_draw() {
        if (draw_commands.size() > 0) {
            if (draw_cmd_buffer.head - draw_cmd_buffer.buffer + draw_commands.size() > draw_cmd_buffer.size) {
                draw_cmd_buffer.head = draw_cmd_buffer.buffer;
            }

            memcpy(draw_cmd_buffer.head, draw_commands.data(), draw_commands.size() * sizeof(Draw_Elements_Command));
            i64 const offset = (draw_cmd_buffer.head - draw_cmd_buffer.buffer) * sizeof(Draw_Elements_Command);
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)offset, draw_commands.size(), sizeof(Draw_Elements_Command));
            draw_cmd_buffer.head += draw_commands.size();
            draw_commands.clear();
        }
    }
} // namespace anton_engine::imgui
