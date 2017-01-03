#include "widget_rectanglepatchwidget.h"
#include "ui_widget_rectanglepatchwidget.h"
#include "mainwindow.h"

Widget_RectanglePatchWidget::Widget_RectanglePatchWidget(QWidget *parent) :
    Widget_specificPatchWidget(parent),
    ui(new Ui::Widget_RectanglePatchWidget)
{
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
    m_parent->notifyClickedUpdateDependencyFromSpecificPatchWidget();
    return;
}

void Widget_RectanglePatchWidget::on_pushButton_remove_clicked()
{
    m_parent->notifyClickedRemoveDependencyFromSpecificPatchWidget();
    return;
}
