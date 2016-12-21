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
    explicit Widget_PatchItem(BezierPatch *patch, QWidget *parent = 0);
    ~Widget_PatchItem();

    unsigned int id() const;

private:
    Ui::Widget_PatchItem *ui;

    BezierPatch *m_patch;
};

#endif // WIDGET_PATCHITEM_H
