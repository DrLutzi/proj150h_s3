#ifndef WIDGET_SPECIFICPATCHWIDGET_H
#define WIDGET_SPECIFICPATCHWIDGET_H

#include <QWidget>

class MainWindow;

class Widget_specificPatchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_specificPatchWidget(QWidget *parent = 0);

signals:

public slots:

protected:

    MainWindow *m_parent;

};

#endif // WIDGET_SPECIFICPATCHWIDGET_H
