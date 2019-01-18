#ifndef ENGINE_MODEL_HPP_INCLUDE
#define ENGINE_MODEL_HPP_INCLUDE

#include "mesh.hpp"
#include <vector>
#include <filesystem>

class Model {
public:
    static Model load_from_file(std::filesystem::path const& path);

	void draw(Shader&);
    void draw_instanced(Shader&, uint32_t count);

	template<typename Callable>
	void for_each_mesh(Callable&& callable) {
        for (Mesh& mesh : meshes) {
            callable(mesh);
		}
	}

private:
    std::vector<Mesh> meshes;
};

#endif // !ENGINE_MODEL_HPP_INCLUDE