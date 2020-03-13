#include <core/assert.hpp>

#include <core/diagnostic_macros.hpp>

ANTON_DISABLE_WARNINGS();
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// Windows.h must be included before DbgHelp.h. Otherwise we get undefined identifiers in DbgHelp.h
#include <DbgHelp.h>
#include <debugapi.h>
ANTON_RESTORE_WARNINGS();

#include <core/atl/string.hpp>
#include <core/atl/string_utils.hpp>
#include <core/windows/debugging.hpp>

#include <new>
#include <stdio.h>
#include <string.h> // memset

namespace anton_engine {
    void anton_assert(char const* message, char const* file, unsigned long long line) {
        HANDLE current_process = GetCurrentProcess();
        SymInitialize(current_process, nullptr, true);
        DWORD current_options = SymGetOptions();
        SymSetOptions(current_options | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

        void* stack_trace[1000];
        int captured_frames = CaptureStackBackTrace(1, 1000, stack_trace, nullptr);

        constexpr int max_symbol_length = 512;
        char symbol_info_buffer[sizeof(SYMBOL_INFO) + sizeof(CHAR) * max_symbol_length] = {};
        SYMBOL_INFO& symbol_info = *reinterpret_cast<SYMBOL_INFO*>(symbol_info_buffer);
        symbol_info.SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol_info.MaxNameLen = max_symbol_length + 1;

        atl::String call_stack(atl::reserve, 1024);
        for (int i = 0; i < captured_frames; ++i) {
            if (SymFromAddr(current_process, DWORD64(stack_trace[i]), nullptr, &symbol_info)) {
                CHAR const* name = symbol_info.Name;
                if (atl::compare_equal(name, u8"invoke_main")) {
                    break;
                }

                atl::String function = windows::debugging::get_type_as_string(current_process, symbol_info.ModBase, symbol_info.Index);
                call_stack.append(function);

                IMAGEHLP_LINE64 image_line;
                image_line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                if (DWORD displacement = 0; SymGetLineFromAddr64(current_process, DWORD64(stack_trace[i]), &displacement, &image_line)) {
                    call_stack.append(u8" Line ");
                    call_stack.append(atl::to_string(image_line.LineNumber));
                }
            } else {
                call_stack.append(atl::to_string(stack_trace[i]));
            }

            call_stack.append(u8"\n");
        }

        atl::String dialog_text(atl::reserve, 1024);
        dialog_text.append(u8"Assertion failed:\n");
        dialog_text.append(message);
        dialog_text.append(u8"\nin file ");
        dialog_text.append(file);
        dialog_text.append(u8" line ");
        dialog_text.append(atl::to_string(line));
        dialog_text.append(u8"\n\nStackTrace:\n");
        dialog_text.append(call_stack);
        fprintf(stderr, "%s", dialog_text.data());
        fflush(stderr);
        dialog_text.append(u8"\nPress 'Retry' to break into debug mode.");
        int clicked_button = MessageBoxA(nullptr, dialog_text.data(), "Assertion Failed", MB_ABORTRETRYIGNORE | MB_TASKMODAL);
        if (clicked_button == IDABORT) {
            TerminateProcess(current_process, 900);
        } else if (clicked_button == IDRETRY) {
            DebugBreak();
        }

        SymCleanup(current_process);
    }
} // namespace anton_engine
