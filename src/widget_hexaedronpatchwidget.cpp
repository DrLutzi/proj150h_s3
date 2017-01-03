#include "widget_hexaedronpatchwidget.h"
#include "ui_widget_hexaedronpatchwidget.h"
#include "mainwindow.h"

Widget_HexaedronPatchWidget::Widget_HexaedronPatchWidget(QWidget *parent) :
    Widget_specificPatchWidget(parent),
    ui(new Ui::Widget_HexaedronPatchWidget)
{
    ui->setupUi(this);
}

Widget_HexaedronPatchWidget::~Widget_HexaedronPatchWidget()
{
    delete ui;
}

void Widget_HexaedronPatchWidget::on_pushButton_clicked()
{
    int index=ui->comboBox->currentIndex();
    switch(index)
    {
    case 0:
        break;

    case 1: //must be front
        m_parent->notifyClickedFromHexaedronPatchWidget(BezierPatch_Hexaedron::FRONT);
        break;

    case 2: //must be back
        m_parent->notifyClickedFromHexaedronPatchWidget(BezierPatch_Hexaedron::BACK);
        break;

    case 3: //must be left
        m_parent->notifyClickedFromHexaedronPatchWidget(BezierPatch_Hexaedron::LEFT);
        break;

    case 4: //must be right
        m_parent->notifyClickedFromHexaedronPatchWidget(BezierPatch_Hexaedron::RIGHT);
        break;

    case 5: //must be top
        m_parent->notifyClickedFromHexaedronPatchWidget(BezierPatch_Hexaedron::TOP);
        break;

    case 6: //must be bottom
        m_parent->notifyClickedFromHexaedronPatchWidget(BezierPatch_Hexaedron::BOTTOM);
        break;
    }
    return;
}

void Widget_HexaedronPatchWidget::on_pushButton_update_clicked()
{
    m_parent->notifyClickedUpdateDependencyFromSpecificPatchWidget();
    return;
}

void Widget_HexaedronPatchWidget::on_pushButton_remove_clicked()
{
    m_parent->notifyClickedRemoveDependencyFromSpecificPatchWidget();
    return;
}
