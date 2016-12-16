#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_viewer(),
    m_dialogNewPatch()
{
    ui->setupUi(this);

    m_viewer = new Viewer(this);

    ui->viewer_layout->addWidget(m_viewer);

    m_dialogNewPatch = new Dialog_newPatch(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::notifyNewPatchFromDialogNewPatch(BezierPatch *bp)
{
    /*if(bp!=NULL)
    {
        m_viewer->manager()
    }*/
}

void MainWindow::on_pushbutton_gen_clicked()
{
    m_dialogNewPatch->show();
}
