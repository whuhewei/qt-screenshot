#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->screenshot_btn, &QPushButton::clicked, [=](){
        Screen * s = new Screen;
        s->show();
    });

    this->hotkey = new QHotkey(Qt::Key_S, Qt::AltModifier, false, this);
    this->hotkey->setRegistered(true);
    connect(this->hotkey, &QHotkey::activated, [=](){
        Screen * s = new Screen;
        s->show();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
