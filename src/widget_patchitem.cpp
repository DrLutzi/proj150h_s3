#include "widget_patchitem.h"
#include "ui_widget_patchitem.h"

Widget_PatchItem::Widget_PatchItem(const BezierPatch *patch, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_PatchItem)
{
    ui->setupUi(this);

    if(patch!=NULL)
    {
        ui->lineEdit_Id->setText(QString::number(patch->id()));
        ui->lineEdit_Name->setText(patch->name());
    }
}

Widget_PatchItem::~Widget_PatchItem()
{
    delete ui;
}
