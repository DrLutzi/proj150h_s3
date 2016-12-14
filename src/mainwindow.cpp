#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_viewer()
{
    ui->setupUi(this);

    m_viewer=new Viewer(this);

    ui->viewer_layout->addWidget(m_viewer);
}

MainWindow::~MainWindow()
{
    delete ui;
}
