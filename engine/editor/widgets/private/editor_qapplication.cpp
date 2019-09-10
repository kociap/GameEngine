#define _CRT_SECURE_NO_WARNINGS 1

#include <editor_qapplication.hpp>
#include <paths.hpp>
#include <utils/filesystem.hpp>

#include <string.h> // strcpy
#include <string>

namespace anton_engine {
    [[nodiscard]] static char** make_argv() {
        char** argv = new char*[2];
        std::string executable_path = utils::concat_paths(paths::engine_executable_directory(), paths::engine_executable_name()).generic_string();
        char* path = new char[executable_path.size() + 1];
        strcpy(path, executable_path.data());
        argv[0] = path;
        argv[1] = nullptr;
        return argv;
    }

    Editor_Indirect_QApplication::Editor_Indirect_QApplication(int* ac, char** av): QApplication(*ac, av), argc(ac), argv(av) {}

    Editor_Indirect_QApplication::~Editor_Indirect_QApplication() {
        delete argc;
        delete argv[0];
        delete argv;
    }

    Editor_QApplication::Editor_QApplication(): Editor_Indirect_QApplication(new int(1), make_argv()) {}
} // namespace anton_engine
