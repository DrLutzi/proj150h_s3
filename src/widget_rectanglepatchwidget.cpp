#include "widget_rectanglepatchwidget.h"
#include "ui_widget_rectanglepatchwidget.h"
#include "mainwindow.h"
#include "errorsHandler.hpp"

Widget_RectanglePatchWidget::Widget_RectanglePatchWidget(QWidget *parent) :
    Widget_specificPatchWidget(parent),
    ui(new Ui::Widget_RectanglePatchWidget),
    m_parent(NULL)
{
    if((m_parent=qobject_cast<MainWindow*>(parent)) == NULL)
        ERROR("Widget_RectanglePatchWidget: parent must be of type mainwindow");

    ui->setupUi(this);
}

Widget_RectanglePatchWidget::~Widget_RectanglePatchWidget()
{
    delete ui;
}

void Widget_RectanglePatchWidget::on_pushButton_clicked()
{
    m_parent->notifyClickedFromRectanglePatchWidget();
    return;
}

void Widget_RectanglePatchWidget::on_pushButton_update_clicked()
{
    m_parent->notifyClickedUpdateFromRectanglePatchWidget();
    return;
}
