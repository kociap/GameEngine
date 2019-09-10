#include <log_viewer.hpp>

#include <anton_stl/string.hpp>
#include <anton_stl/utility.hpp>

ANTON_DISABLE_WARNINGS();
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
ANTON_RESTORE_WARNINGS();

namespace anton_engine {
    Log_Message::Log_Message(anton_engine::Log_Message_Severity severity, anton_stl::String_View str, QWidget* parent): QWidget(parent) {
        layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        message = new QLabel(str.data());
        message->setMinimumHeight(32);
        layout->addWidget(message);
    }

    Log_Message::Log_Message(Log_Message&& other) noexcept {
        setParent(static_cast<QWidget*>(other.parent())); // TODO: Hope this doesn't fail
        other.setParent(nullptr);
        layout = other.layout;
        layout->setParent(this);
        other.layout = nullptr;
        // Already belongs to our layout. No need to reparent.
        message = other.message;
        other.message = nullptr;
    }

    Log_Message& Log_Message::operator=(Log_Message&& other) noexcept {
        setParent(static_cast<QWidget*>(other.parent())); // TODO: Hope this doesn't fail
        other.layout->removeWidget(other.message);
        layout->removeWidget(message);
        anton_stl::swap(message, other.message);
        other.layout->addWidget(other.message);
        layout->addWidget(message);
        return *this;
    }

    Log_Message::~Log_Message() {
        delete message;
        delete layout;
    }

    Log_Viewer::Log_Viewer(QWidget* parent): QWidget(parent) {
        layout = new QVBoxLayout(this);
        scroll_area = new QScrollArea();
        scroll_area->setWidgetResizable(true);
        content = new QWidget();
        content_layout = new QVBoxLayout(content);

        message_list = new QWidget;
        message_list_layout = new QVBoxLayout(message_list);
        message_list_layout->setContentsMargins(0, 0, 0, 0);
        message_list_layout->setSpacing(0);

        content_layout->addWidget(message_list);
        content_layout->addStretch(1);

        scroll_area->setWidget(content);
        layout->addWidget(scroll_area);
    }

    // TODO: Implement
    Log_Viewer::Log_Viewer(Log_Viewer&&) noexcept {}

    // TODO: Implement
    Log_Viewer& Log_Viewer::operator=(Log_Viewer&&) noexcept {
        return *this;
    }

    // TODO: Implement
    Log_Viewer::~Log_Viewer() {}

    void Log_Viewer::add_message(anton_engine::Log_Message_Severity severity, anton_stl::String_View date, anton_stl::String_View message) {
        anton_stl::String str(anton_stl::reserve, date.size_bytes() + message.size_bytes() + 12); // 2 brackets, 2 spaces, strlen("warning"), colon
        str.append(u8"[");
        str.append(date);
        str.append(u8"] ");
        switch (severity) {
            case anton_engine::Log_Message_Severity::info:
                str.append(u8"Info: ");
                break;
            case anton_engine::Log_Message_Severity::warning:
                str.append(u8"Warning: ");
                break;
            default:
                str.append(u8"Error: ");
                break;
        }
        str.append(message);
        Log_Message& log_msg = messages.emplace_back(severity, str);
        message_list_layout->addWidget(&log_msg);
        // scroll_area->ensureWidgetVisible(&message);
        // scroll_area->verticalScrollBar()->setValue(scroll_area->verticalScrollBar()->maximum());
    }

    void Log_Viewer::remove_all_messages() {
        for (Log_Message& message: messages) {
            content_layout->removeWidget(&message);
        }

        messages.clear();
    }
} // namespace anton_engine
