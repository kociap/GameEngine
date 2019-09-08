#ifndef EDITOR_EDITOR_QAPPLICATION_HPP_INCLUDE
#define EDITOR_EDITOR_QAPPLICATION_HPP_INCLUDE

#include <diagnostic_macros.hpp>
ANTON_DISABLE_WARNINGS();
#include <QApplication>
ANTON_RESTORE_WARNINGS();

class Editor_Indirect_QApplication: public QApplication {
public:
    Editor_Indirect_QApplication(int* argc, char** argv);
    ~Editor_Indirect_QApplication() override;

private:
    int* argc;
    char** argv;
};

class Editor_QApplication: public Editor_Indirect_QApplication {
public:
    Editor_QApplication();
};

#endif // !EDITOR_EDITOR_QAPPLICATION_HPP_INCLUDE
