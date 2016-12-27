#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "errorsHandler.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_viewer(),
    m_dialogNewPatch(),
    m_selectedPatch(NULL),
    m_specificPatchWidget(new Widget_specificPatchWidget(this))
{
    ui->setupUi(this);

    m_viewer = new Viewer(this);

    ui->viewer_layout->addWidget(m_viewer);

    ui->specificPatchWidget_layout->addWidget(m_specificPatchWidget);

    m_dialogNewPatch = new Dialog_newPatch(this);
}

MainWindow::~MainWindow()
{
    ui->specificPatchWidget_layout->removeWidget(m_specificPatchWidget);
    delete m_specificPatchWidget;
    delete ui;
    delete m_viewer;
}

void MainWindow::addBezierPatch2List(BezierPatch *bp)
{
    if(bp!=NULL)
    {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget_Patchs);
        Widget_PatchItem *wpi = new Widget_PatchItem(bp);
        item->setSizeHint(QSize(0, wpi->height()));
        ui->listWidget_Patchs->setItemWidget(item, wpi);
        ui->listWidget_Patchs->addItem(item);
    }
}

//NOTIFICATIONS

void MainWindow::notifyNewPatchFromDialogNewPatch(BezierPatch *bp)
{
    if(bp!=NULL)
    {
        m_viewer->manager()->append(bp, false);
        bp->showCP();
        bp->showPatch();
        bp->setResolution(10);
        m_viewer->manager()->remakeScene();
        m_viewer->updateGL();

        addBezierPatch2List(bp);
    }
}

void MainWindow::notifyClickedFromRectanglePatchWidget()
{
    BezierPatch_Rectangle *rectangle=dynamic_cast<BezierPatch_Rectangle*>(m_selectedPatch);
    if(rectangle==NULL)
    {
        WARNING("notifyClickedFromRectanglePatchWidget: selected patch is not a rectangle");
    }
    else
    {
        unsigned int id=rectangle->id();
        m_viewer->manager()->addDependency(id).createR2TDependency(id);

        //the architecture doesn't allow us to directly append the triangles to the list,
        //so we have to do some trickery
        id=BezierPatch::currentId()-2;

        addBezierPatch2List(m_viewer->manager()->getPatch(id++));
        addBezierPatch2List(m_viewer->manager()->getPatch(id));
    }
}

void MainWindow::notifyClickedUpdateFromRectanglePatchWidget()
{

}

//PRIVATE SLOTS

void MainWindow::on_pushbutton_gen_clicked()
{
    m_dialogNewPatch->show();
}

void MainWindow::on_listWidget_Patchs_itemSelectionChanged()
{
    QListWidgetItem *item=ui->listWidget_Patchs->currentItem();
    Widget_PatchItem *selectedPatchItem=qobject_cast<Widget_PatchItem*>(ui->listWidget_Patchs->itemWidget(item));
    if(selectedPatchItem!=NULL)
    {
        BezierPatch_Manager *mgr=m_viewer->manager();
        m_selectedPatch=mgr->getPatch(selectedPatchItem->id());

        if(m_selectedPatch!=NULL)
        {
            //set UI according to manager

            ui->checkBox_display_CP->setChecked             (m_selectedPatch->isDrawCP());
            ui->checkBox_display_patchLines->setChecked     (m_selectedPatch->isDrawPatch());
            ui->checkBox_display_surface->setChecked        (m_selectedPatch->isDrawSurface());

            QColor color_CP, color_patchLines, color_surface;
            color_CP.setRgbF        (m_selectedPatch->controlPointColor().r, m_selectedPatch->controlPointColor().g, m_selectedPatch->controlPointColor().b);
            color_patchLines.setRgbF(m_selectedPatch->patchColor().r, m_selectedPatch->patchColor().g, m_selectedPatch->patchColor().b);
            color_surface.setRgbF   (m_selectedPatch->surfaceColor().r, m_selectedPatch->surfaceColor().g, m_selectedPatch->surfaceColor().b);

            ui->pushButton_color_CP->setPalette         (QPalette(color_CP));
            ui->pushButton_color_patchLines->setPalette (QPalette(color_patchLines));
            ui->pushButton_color_surface->setPalette    (QPalette(color_surface));

            //set the bezierpatch type-dependant UI

            auto deleteSpecificWidget=[&]()
            {
                ui->specificPatchWidget_layout->removeWidget(m_specificPatchWidget);
                delete m_specificPatchWidget;
            };

            if(dynamic_cast<BezierPatch_Rectangle*>(m_selectedPatch))
            {
                deleteSpecificWidget();
                m_specificPatchWidget=new Widget_RectanglePatchWidget(this);
                ui->specificPatchWidget_layout->addWidget(m_specificPatchWidget);
                m_specificPatchWidget->show();
            }
        }
        else
            WARNING("on_listWidget_Patchs_itemSelectionChanged : patch should have been found inside manager");
    }
}

void MainWindow::on_checkBox_display_CP_toggled(bool checked)
{
    if(m_selectedPatch!=NULL)
    {
        m_selectedPatch->setDrawCP(checked);
        m_viewer->manager()->remakeScene();
        m_viewer->updateGL();
    }
}


void MainWindow::on_checkBox_display_patchLines_toggled(bool checked)
{
    if(m_selectedPatch!=NULL)
    {
        m_selectedPatch->setDrawPatch(checked);
        m_viewer->manager()->remakeScene();
        m_viewer->updateGL();
    }
}


void MainWindow::on_checkBox_display_surface_toggled(bool checked)
{
    if(m_selectedPatch!=NULL)
    {
        m_selectedPatch->setDrawSurface(checked);
        m_viewer->manager()->remakeScene();
        m_viewer->updateGL();
    }
}

void MainWindow::on_pushButton_color_CP_clicked()
{
    if(m_selectedPatch!=NULL)
    {
        QColor color_CP = QColorDialog::getColor(ui->pushButton_color_CP->palette().color(QPalette::Base), this, "Pick a control point color");
        if(color_CP.isValid())
        {
            ui->pushButton_color_CP->setPalette(QPalette(color_CP));
            m_selectedPatch->setControlPointColor(glm::vec4(color_CP.redF(), color_CP.greenF(), color_CP.blueF(), 1.0f));
            m_viewer->updateGL();
        }
    }
}

void MainWindow::on_pushButton_color_patchLines_clicked()
{
    if(m_selectedPatch!=NULL)
    {
        QColor color_patchLines = QColorDialog::getColor(ui->pushButton_color_patchLines->palette().color(QPalette::Base), this, "Pick a patch color");
        if(color_patchLines.isValid())
        {
            ui->pushButton_color_patchLines->setPalette(QPalette(color_patchLines));
            m_selectedPatch->setPatchColor(glm::vec4(color_patchLines.redF(), color_patchLines.greenF(), color_patchLines.blueF(), 1.0f));
            m_viewer->updateGL();
        }
    }
}

void MainWindow::on_pushButton_color_surface_clicked()
{
    if(m_selectedPatch!=NULL)
    {
        QColor color_surface = QColorDialog::getColor(ui->pushButton_color_surface->palette().color(QPalette::Base), this, "Pick a surface color");
        if(color_surface.isValid())
        {
            ui->pushButton_color_surface->setPalette(QPalette(color_surface));
            m_selectedPatch->setSurfaceColor(glm::vec4(color_surface.redF(), color_surface.greenF(), color_surface.blueF(), 1.0f));
            m_viewer->updateGL();
        }
    }
}

void MainWindow::on_spinBox_resolution_valueChanged(int arg1)
{
    if(m_selectedPatch!=NULL)
    {
        m_selectedPatch->setResolution(arg1);
        m_viewer->manager()->remakeScene();
        m_viewer->updateGL();
    }
}

void MainWindow::on_pushButton_remove_clicked()
{
    if(m_selectedPatch!=NULL)
    {
        m_viewer->manager()->remove(m_selectedPatch->id());
        delete m_selectedPatch;
        m_selectedPatch=NULL;

        QListWidgetItem *selectedItem=ui->listWidget_Patchs->currentItem();
        if(selectedItem!=NULL)
        {
            ui->listWidget_Patchs->removeItemWidget(selectedItem);
            delete selectedItem;
        }
        else
            WARNING("on_pushButton_remove_clicked : current Item of listWidget expected to not be NULL");
        m_viewer->updateGL();
    }
}
