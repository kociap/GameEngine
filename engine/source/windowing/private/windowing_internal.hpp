#ifndef WINDOWING_WINDOWING_INTERNAL_HPP_INCLUDE
#define WINDOWING_WINDOWING_INTERNAL_HPP_INCLUDE

namespace anton_engine {
    // Returns true if windowing has been successfully initialized.
    bool init_windowing();
    void terminate_windowing();
    void poll_window_events();
} // namespace anton_engine

#endif // !WINDOWING_WINDOWING_INTERNAL_HPP_INCLUDE
