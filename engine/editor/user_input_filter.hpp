#ifndef EDITOR_USER_INPUT_FILTER_HPP_INCLUDE
#define EDITOR_USER_INPUT_FILTER_HPP_INCLUDE

#include <QObject>

class QEvent;

class User_Input_Filter: public QObject {
    Q_OBJECT

public:
    User_Input_Filter(int32_t initial_cursor_x, int32_t initial_custor_y);

protected:
    bool eventFilter(QObject*, QEvent*) override;

private:
    int32_t last_cursor_pos_x;
    int32_t last_cursor_pos_y;
};

#endif // !EDITOR_USER_INPUT_FILTER_HPP_INCLUDE
