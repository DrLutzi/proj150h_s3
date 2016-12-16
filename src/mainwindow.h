#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "viewer.h"
#include "widget_patchitem.h"
#include "dialog_newpatch.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void notifyNewPatchFromDialogNewPatch(BezierPatch *bp);

private slots:
    void on_pushbutton_gen_clicked();

private:
    Ui::MainWindow *ui;
    Viewer *m_viewer;
    Dialog_newPatch *m_dialogNewPatch;
};

#endif // MAINWINDOW_H
