#include <log_viewer.hpp>

#include <anton_stl/string.hpp>
#include <anton_stl/utility.hpp>
#include <iostream>
ANTON_DISABLE_WARNINGS();
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
ANTON_RESTORE_WARNINGS();

namespace anton_engine {
    Log_Message::Log_Message(anton_engine::Log_Message_Severity severity, anton_stl::String_View str, QWidget* parent): QWidget(parent) {
        setMinimumHeight(32);
        message = new QLabel(str.data(), this);
        message->show();
    }

    Log_Message::Log_Message(Log_Message&& other) noexcept: message(other.message) {
        other.message = nullptr;

        message->setParent(this);

        setParent(other.parentWidget());
        other.setParent(nullptr);

        show();
        other.hide();
    }

    Log_Message& Log_Message::operator=(Log_Message&& other) noexcept {
        using anton_stl::swap;
        swap(message, other.message);
        message->setParent(this);
        other.message->setParent(&other);
        setParent(other.parentWidget());
        other.setParent(nullptr);
        return *this;
    }

    Log_Message::~Log_Message() {}

    void Log_Message::resizeEvent(QResizeEvent*) {
        message->setGeometry(0, 0, width(), height());
    }

    Log_Viewer::Log_Viewer(QWidget* parent): QWidget(parent) {
        layout = new QVBoxLayout(this);
        scroll_area = new QScrollArea();
        scroll_area->setWidgetResizable(true);
        content = new QWidget();
        content_layout = new QVBoxLayout(content);

        message_list = new List_Widget<Log_Message>;
        message_list->set_spacing(2);

        content_layout->addWidget(message_list);
        // content_layout->addStretch(1);

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
        Log_Message& log_msg = message_list->emplace_back(severity, str);
        std::cout << str.data() << "\n";
        log_msg.show();
        // scroll_area->ensureWidgetVisible(&message);
        // scroll_area->verticalScrollBar()->setValue(scroll_area->verticalScrollBar()->maximum());
    }

    void Log_Viewer::remove_all_messages() {
        message_list->erase(message_list->begin(), message_list->end());
    }
} // namespace anton_engine
