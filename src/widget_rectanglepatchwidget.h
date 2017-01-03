#ifndef WIDGET_RECTANGLEPATCHWIDGET_H
#define WIDGET_RECTANGLEPATCHWIDGET_H

#include <QWidget>
#include "widget_specificpatchwidget.h"

namespace Ui {
class Widget_RectanglePatchWidget;
}

class Widget_RectanglePatchWidget : public Widget_specificPatchWidget
{
    Q_OBJECT

public:
    explicit Widget_RectanglePatchWidget(QWidget *parent);
    ~Widget_RectanglePatchWidget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_update_clicked();

    void on_pushButton_remove_clicked();

private:
    Ui::Widget_RectanglePatchWidget *ui;
};

#endif // WIDGET_RECTANGLEPATCHWIDGET_H
