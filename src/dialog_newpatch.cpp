#include "dialog_newpatch.h"
#include "ui_dialog_newpatch.h"

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

    //assuming the indexes for types are correct
   /* switch(index)
    {
    //TODO
        case RECTANGLE:
            m_createdPatch = new BezierPatch_Rectangle(ui->)
    }
*/
}
