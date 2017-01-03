#include "widget_specificpatchwidget.h"
#include "mainwindow.h"

Widget_specificPatchWidget::Widget_specificPatchWidget(QWidget *parent) :
    QWidget(parent),
    m_parent(NULL)
{
    if((m_parent=qobject_cast<MainWindow*>(parent)) == NULL)
        ERROR("Widget_SpecificPatchWidget: parent must be of type mainwindow");
}
