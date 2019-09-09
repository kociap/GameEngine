#include <logging.hpp>

#include <time/time_platform.hpp>

#include <build_config.hpp>
#if ANTON_WITH_EDITOR
#    include <editor.hpp>
#    include <editor_window.hpp>
#    include <log_viewer.hpp>
#    include <stdio.h> // TODO: Replace with format
#else
#    include <stdio.h>
#endif

namespace anton_engine {
    void log_message(Log_Message_Severity severity, anton_stl::String_View message) {
        // Higly advanced and complex logging code.
#if ANTON_WITH_EDITOR
        Editor_Window& editor = Editor::get_window();
        System_Time sys_time = get_local_system_time();
        char t[9] = {};
        sprintf(t, "%02d:%02d:%02d", sys_time.hour, sys_time.minutes, sys_time.seconds);
        editor.log_viewer->add_message(severity, t, message);
#else
        System_Time sys_time = get_local_system_time();
        if (severity == Log_Message_Severity::info) {
            fprintf(stdout, "[%02d:%02d:%02d] Info: %s\n", sys_time.hour, sys_time.minutes, sys_time.seconds, message.data());
        } else if (severity == Log_Message_Severity::warning) {
            fprintf(stdout, "[%02d:%02d:%02d] Warning: %s\n", sys_time.hour, sys_time.minutes, sys_time.seconds, message.data());
        } else {
            fprintf(stderr, "[%02d:%02d:%02d] Error: %s\n", sys_time.hour, sys_time.minutes, sys_time.seconds, message.data());
        }
#endif // ANTON_WITH_EDITOR
    }
} // namespace anton_engine
