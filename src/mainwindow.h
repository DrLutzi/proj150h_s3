#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include <QPalette>
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

    void on_listWidget_Patchs_itemSelectionChanged();

    void on_checkBox_display_CP_toggled(bool checked);

    void on_checkBox_display_patchLines_toggled(bool checked);

    void on_checkBox_display_surface_toggled(bool checked);

    void on_pushButton_color_CP_clicked();

    void on_pushButton_color_patchLines_clicked();

    void on_pushButton_color_surface_clicked();

    void on_spinBox_resolution_valueChanged(int arg1);

    void on_pushButton_remove_clicked();

private:
    Ui::MainWindow *ui;
    Viewer *m_viewer;
    Dialog_newPatch *m_dialogNewPatch;

    BezierPatch *m_selectedPatch;
};

#endif // MAINWINDOW_H
