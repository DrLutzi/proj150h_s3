#ifndef WIDGET_PATCHITEM_H
#define WIDGET_PATCHITEM_H

#include <QWidget>
#include "bezierpatch_manager.h"

namespace Ui {
class Widget_PatchItem;
}

class Widget_PatchItem : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_PatchItem(const BezierPatch *patch, QWidget *parent = 0);
    ~Widget_PatchItem();

private:
    Ui::Widget_PatchItem *ui;
};

#endif // WIDGET_PATCHITEM_H
