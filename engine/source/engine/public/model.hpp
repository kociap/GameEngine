#ifndef ENGINE_MODEL_HPP_INCLUDE
#define ENGINE_MODEL_HPP_INCLUDE

#include "mesh.hpp"
#include <vector>
#include <filesystem>

class Model {
public:
    static Model load_from_file(std::filesystem::path const& path);

	void draw(Shader&);

private:
    std::vector<Mesh> meshes;
};



#endif // !ENGINE_MODEL_HPP_INCLUDE