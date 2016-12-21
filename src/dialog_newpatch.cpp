#include "dialog_newpatch.h"
#include "ui_dialog_newpatch.h"
#include "mainwindow.h"

Dialog_newPatch::Dialog_newPatch(QWidget *parent) :
    QDialog(parent),
    m_createdPatch(NULL),
    ui(new Ui::Dialog_newPatch)
{
    ui->setupUi(this);
}

Dialog_newPatch::~Dialog_newPatch()
{
    delete ui;
}

void Dialog_newPatch::on_comboBox_currentIndexChanged(int index)
{

}

void Dialog_newPatch::on_buttonBox_accepted()
{
    comboBox2BezierPatch_t index=static_cast<comboBox2BezierPatch_t>(ui->comboBox->currentIndex());
    MainWindow *cast_parent=static_cast<MainWindow*>(parent());

    //assuming the indexes for types are correct
    switch(index)
    {
        case RECTANGLE:
            m_createdPatch = BezierPatch_Rectangle::generate(ui->spinBox_size1->value(), ui->spinBox_size2->value(),
                                                             ui->doubleSpinBox_xSpace->value(), ui->doubleSpinBox_ySpace->value(),
                                                             ui->doubleSpinBox_maxNoise->value());
            break;

        case TRIANGLE:
            m_createdPatch = BezierPatch_Triangle::generate(ui->spinBox_size1->value(),
                                                            ui->doubleSpinBox_xSpace->value(), ui->doubleSpinBox_ySpace->value(),
                                                            ui->doubleSpinBox_maxNoise->value());
            break;

        case HEXAEDRON:
            m_createdPatch = BezierPatch_Hexaedron::generate(ui->spinBox_size1->value(), ui->spinBox_size2->value(), ui->spinBox_size3->value(),
                                                             ui->doubleSpinBox_xSpace->value(), ui->doubleSpinBox_ySpace->value(), ui->doubleSpinBox_zSpace->value(),
                                                             ui->doubleSpinBox_maxNoise->value());
            break;

        case TETRAHEDRON:
            m_createdPatch = BezierPatch_Tetrahedron::generate(ui->spinBox_size1->value(),
                                                               ui->doubleSpinBox_xSpace->value(), ui->doubleSpinBox_ySpace->value(), ui->doubleSpinBox_zSpace->value(),
                                                               ui->doubleSpinBox_maxNoise->value());
            break;

        default:
            m_createdPatch = NULL;
            break;
    }
    cast_parent->notifyNewPatchFromDialogNewPatch(m_createdPatch);
}
