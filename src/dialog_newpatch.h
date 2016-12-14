#ifndef DIALOG_NEWPATCH_H
#define DIALOG_NEWPATCH_H

#include <QDialog>
#include "bezierpatch_manager.h"

namespace Ui {
class Dialog_newPatch;
}

class Dialog_newPatch : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_newPatch(QWidget *parent = 0);
    ~Dialog_newPatch();

    BezierPatch *m_createdPatch;

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_buttonBox_accepted();

private:
    typedef enum{NOTHING=0, RECTANGLE=1, TRIANGLE=2, HEXAEDRON=3, TETRAHEDRON=4} comboBox2BezierPatch_t;

    Ui::Dialog_newPatch *ui;
};

#endif // DIALOG_NEWPATCH_H
