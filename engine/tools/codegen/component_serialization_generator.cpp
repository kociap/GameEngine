#include <core/atl/vector.hpp>
#include <component_header_parser.hpp>
#include <file.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

using namespace std::string_view_literals;

namespace anton_engine {
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

    static void serach_for_components(std::string_view directory, atl::Vector<Component>& out) {
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

    struct Indent {
        uint32_t indent_level;
    };

    std::ostream& operator<<(std::ostream& out, Indent i) {
        while (i.indent_level > 0) {
            out << u8"    ";
            i.indent_level -= 1;
        }
        return out;
    }

    Indent indent(uint32_t indent_level) {
        return {indent_level};
    }
} // namespace anton_engine

// argv[1] is the path to the output file
// argv[2], argv[3]... are the search locations
int main(int argc, char** argv) {
    using namespace anton_engine;

    if (argc < 3) {
        throw std::runtime_error("Too few parameters");
    }

    std::ios_base::sync_with_stdio(false);

    std::string_view output_file(argv[1]);

    atl::Vector<Component> components;
    for (int32_t i = 2; i < argc; ++i) {
        std::string_view components_search_directory(argv[i]);
        serach_for_components(components_search_directory, components);
    }

    std::ofstream generated_file(output_file);
    generated_file << "#include <component_serialization_funcs.hpp>\n#include <engine/ecs/component_container.hpp>\n\n";
    for (auto& [include_directory, name]: components) {
        generated_file << "#include <" << include_directory << ">\n";
    }
    generated_file << '\n'
                   << "namespace anton_engine {\n"
                   << indent(1) << "atl::Vector<Component_Serialization_Funcs>& get_component_serialization_functions() {\n"
                   << indent(2) << "static atl::Vector<Component_Serialization_Funcs> serialization_funcs{atl::variadic_construct,\n";
    int32_t component_index = 1;
    for (auto& [include_directory, name]: components) {
        generated_file << indent(3) << "Component_Serialization_Funcs{Type_Family::family_id<" << name << ">(), &Component_Container<" << name
                       << ">::serialize, &Component_Container<" << name << ">::deserialize}";
        if (component_index < components.size()) {
            generated_file << ",\n";
        } else {
            generated_file << "};\n";
        }
        ++component_index;
    }
    generated_file << indent(2) << "return serialization_funcs;\n"
                   << indent(1) << "}\n"
                   << "} // namespace anton_engine\n";

    return 0;
}
