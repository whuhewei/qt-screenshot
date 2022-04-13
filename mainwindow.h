#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "screen.h"
#include <QHotkey>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void quit();

private:
    Ui::MainWindow *ui;

    QHotkey *hotkey;

    Screen *s = NULL;
};

#endif // MAINWINDOW_H
