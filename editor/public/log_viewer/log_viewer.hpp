#pragma once

#include <anton/array.hpp>
#include <anton/string_view.hpp>
#include <core/logging.hpp>
#include <list_widget.hpp>

#include <core/diagnostic_macros.hpp>
ANTON_DISABLE_WARNINGS();
#include <QWidget>
ANTON_RESTORE_WARNINGS();

class QLabel;
class QVBoxLayout;
class QScrollArea;
class QResizeEvent;

namespace anton_engine {
  class Log_Message: public QWidget {
  public:
    Log_Message(anton_engine::Log_Message_Severity, anton::String_View,
                QWidget* parent = nullptr);
    Log_Message(Log_Message&&) noexcept;
    Log_Message& operator=(Log_Message&&) noexcept;
    ~Log_Message() override;

  protected:
    void resizeEvent(QResizeEvent*) override;

  private:
    QLabel* message = nullptr;
  };

  class Log_Viewer: public QWidget {
  public:
    Log_Viewer(QWidget* parent = nullptr);
    Log_Viewer(Log_Viewer&&) noexcept;
    Log_Viewer& operator=(Log_Viewer&&) noexcept;
    ~Log_Viewer() override;

    void add_message(anton_engine::Log_Message_Severity,
                     anton::String_View time, anton::String_View message);
    void remove_all_messages();

  private:
    QVBoxLayout* layout = nullptr;
    QScrollArea* scroll_area = nullptr;
    QWidget* content = nullptr;
    QVBoxLayout* content_layout = nullptr;
    List_Widget<Log_Message>* message_list = nullptr;
  };
} // namespace anton_engine
