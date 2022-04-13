#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->screenshot_btn, &QPushButton::clicked, [=](){
        if (!s)
        {
            s = new Screen;
            s->setMainWindowQuote(this);
            s->show();
        }
    });

    this->hotkey = new QHotkey(Qt::Key_S, Qt::AltModifier, false, this);
    this->hotkey->setRegistered(true);
    connect(this->hotkey, &QHotkey::activated, [=](){
        // 如果在截图中，忽略截图操作
        if (!s)
        {
            s = new Screen;
            s->setMainWindowQuote(this);
            s->show();
        }
    });
}

void MainWindow::quit()
{
    s = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}
