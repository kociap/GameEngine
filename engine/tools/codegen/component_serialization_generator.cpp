#include <anton_stl/vector.hpp>
#include <component_header_parser.hpp>
#include <file.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

using namespace std::string_view_literals;

struct Component {
    std::string include_directory;
    std::string name;
};

static std::string_view extract_include_directory(std::string_view path) {
    // TODO extend to allow user-defined components in project directory
    if (auto pos = path.find("public/"); pos != std::string_view::npos) {
        path.remove_prefix(pos + "public/"sv.size());
    } else if (auto pos = path.find("private/"); pos != std::string_view::npos) {
        path.remove_prefix(pos + "private/"sv.size());
    } else if (auto pos = path.find("scripts/"); pos != std::string_view::npos) { // TODO remove this branch, temporary script location
        path.remove_prefix(pos + "scripts/"sv.size());
    }
    return path;
}

static void serach_for_components(std::string_view directory, anton_stl::Vector<Component>& out) {
    std::filesystem::recursive_directory_iterator dir_iterator(directory);
    std::filesystem::path header_extension(".hpp");
    for (auto entry: dir_iterator) {
        if (auto current_path = entry.path(); entry.is_regular_file() && current_path.extension() == header_extension) {
            File header(current_path);
            auto classes_found = parse_component_header(header);
            for (auto& name: classes_found) {
                std::string str = current_path.generic_string(); // Because windows uses retarded wstring and \ as the directory separator
                std::string_view include_dir = extract_include_directory(str);
                out.push_back(Component{std::string(include_dir.data(), include_dir.size()), std::move(name)});
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        throw std::runtime_error("Too few parameters");
    }

    std::ios_base::sync_with_stdio(false);

    std::string_view output_file(argv[1]);

    anton_stl::Vector<Component> components;
    for (int32_t i = 2; i < argc; ++i) {
        std::string_view components_search_directory(argv[i]);
        serach_for_components(components_search_directory, components);
    }

    std::ofstream generated_file(output_file);
    generated_file << "#include <component_serialization_funcs.hpp>\n#include <ecs/component_container.hpp>\n\n";
    for (auto& [include_directory, name]: components) {
        generated_file << "#include <" << include_directory << ">\n";
    }
    generated_file << '\n'
                   << "anton_stl::Vector<Component_Serialization_Funcs>& get_component_serialization_functions() {\n"
                   << "    static anton_stl::Vector<Component_Serialization_Funcs> serialization_funcs{\n";
    int32_t component_index = 1;
    for (auto& [include_directory, name]: components) {
        generated_file << "        {Type_Family::family_id<" << name << ">(), &Component_Container<" << name << ">::serialize, &Component_Container<" << name
                       << ">::deserialize}";
        if (component_index < components.size()) {
            generated_file << ",\n";
        } else {
            generated_file << "};\n";
        }
        ++component_index;
    }
    generated_file << "    return serialization_funcs;\n"
                   << "}\n";

    return 0;
}