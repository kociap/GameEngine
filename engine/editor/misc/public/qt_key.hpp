#ifndef EDITOR_MISC_QT_KEY_HPP_INCLUDE
#define EDITOR_MISC_QT_KEY_HPP_INCLUDE

#include <diagnostic_macros.hpp>
#include <key.hpp>
ANTON_DISABLE_WARNINGS()
#include <qnamespace.h>
ANTON_RESTORE_WARNINGS()

namespace anton_engine {
    Key key_from_qt_key(Qt::Key);
    Key key_from_qt_mouse_button(Qt::MouseButton);
} // namespace anton_engine

#endif // !EDITOR_MISC_QT_KEY_HPP_INCLUDE
