#include <assert.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// Windows.h must be included before DbgHelp.h. Otherwise we get undefined identifiers in DbgHelp.h
#include <DbgHelp.h>
#include <debugapi.h>

#include <anton_stl/string.hpp>
#include <anton_stl/string_utils.hpp>
#include <windows/debugging.hpp>

#include <new>
#include <stdio.h>
#include <string.h> // memset

namespace anton_engine {

    void anton_assert(char const* message) {
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

        anton_stl::String call_stack(anton_stl::reserve, 1024);
        for (int i = 0; i < captured_frames; ++i) {
            if (SymFromAddr(current_process, DWORD64(stack_trace[i]), nullptr, &symbol_info)) {
                CHAR const* name = symbol_info.Name;
                if (anton_stl::compare_equal(name, u8"invoke_main")) {
                    break;
                }

                anton_stl::String function = windows::debugging::get_type_as_string(current_process, symbol_info.ModBase, symbol_info.Index);
                call_stack.append(function);

                IMAGEHLP_LINE64 image_line;
                image_line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                if (DWORD displacement = 0; SymGetLineFromAddr64(current_process, DWORD64(stack_trace[i]), &displacement, &image_line)) {
                    call_stack.append(u8" Line ");
                    call_stack.append(anton_stl::to_string(image_line.LineNumber));
                }
            } else {
                call_stack.append(anton_stl::to_string(stack_trace[i]));
                // anton_stl::String error_message = get_last_error_message(); // TODO: Maybe log?
            }

            call_stack.append(u8"\n");
        }

        anton_stl::String dialog_text(anton_stl::reserve, 1024);
        dialog_text.append(u8"Assertion failed:\n");
        dialog_text.append(message);
        dialog_text.append(u8"\n\nStack Trace:\n");
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
