#include "widget_patchitem.h"
#include "ui_widget_patchitem.h"
#include "errorsHandler.hpp"

Widget_PatchItem::Widget_PatchItem(BezierPatch *patch, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_PatchItem),
    m_patch(patch)
{
    ui->setupUi(this);

    if(patch!=NULL)
    {
        ui->lineEdit_Id->setText(QString::number(patch->id()));
        ui->lineEdit_Name->setText(patch->name());
    }
    else
        WARNING("Widget_PatchItem : class shouldn't be initialized with a NULL patch pointer");
}

Widget_PatchItem::~Widget_PatchItem()
{
    delete ui;
}

unsigned int Widget_PatchItem::id() const
{
    if(m_patch==NULL)
        WARNING("Widget_PatchItem : id has been requested with a NULL patch pointer (returning 0)");
    return (m_patch!=NULL ? m_patch->id() : 0);
}
