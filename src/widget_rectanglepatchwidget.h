#ifndef WIDGET_RECTANGLEPATCHWIDGET_H
#define WIDGET_RECTANGLEPATCHWIDGET_H

#include <QWidget>

#include "widget_specificpatchwidget.h"

class MainWindow;

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

private:
    Ui::Widget_RectanglePatchWidget *ui;

    MainWindow *m_parent;
};

#endif // WIDGET_RECTANGLEPATCHWIDGET_H
