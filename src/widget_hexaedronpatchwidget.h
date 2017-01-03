#ifndef WIDGET_HEXAEDRONPATCHWIDGET_H
#define WIDGET_HEXAEDRONPATCHWIDGET_H

#include <QWidget>
#include "widget_specificpatchwidget.h"
#include "bezierpatch_hexaedron.h"

namespace Ui {
class Widget_HexaedronPatchWidget;
}

class Widget_HexaedronPatchWidget : public Widget_specificPatchWidget
{
    Q_OBJECT

public:
    explicit Widget_HexaedronPatchWidget(QWidget *parent = 0);
    ~Widget_HexaedronPatchWidget();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_update_clicked();

    void on_pushButton_remove_clicked();

private:
    Ui::Widget_HexaedronPatchWidget *ui;

    BezierPatch_Hexaedron::Face_t m_face;
};

#endif // WIDGET_HEXAEDRONPATCHWIDGET_H
