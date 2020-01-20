#ifndef WINDOWING_WINDOWING_INTERNAL_HPP_INCLUDE
#define WINDOWING_WINDOWING_INTERNAL_HPP_INCLUDE

namespace anton_engine::windowing {
    // Returns true if windowing has been successfully initialized.
    bool init();
    void terminate();
    void poll_events();
} // namespace anton_engine::windowing

#endif // !WINDOWING_WINDOWING_INTERNAL_HPP_INCLUDE
