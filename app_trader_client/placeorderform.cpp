#include "placeorderform.h"
#include "ui_placeorderform.h"

PlaceOrderForm::PlaceOrderForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaceOrderForm)
{
    ui->setupUi(this);
}

PlaceOrderForm::~PlaceOrderForm()
{
    delete ui;
}

void PlaceOrderForm::on_pushButton_clicked()
{
    bool ret=get_value_from_ui();
    if(ret){

    }
}


bool PlaceOrderForm::get_value_from_ui()
{
    m_contract=ui->plainTextEdit_contract->toPlainText();
    m_bs=0;
    if(ui->radioButton_sell->isChecked())
    {
        m_bs=1;
    }
    m_vol=ui->spinBox_vol->value();
    m_price=ui->doubleSpinBox_price->value();

    if(0==ui->comboBox_price_type->currentIndex())
    {

    }
    else{
        m_price=0;
    }

     qDebug()<<"contract="<<m_contract<<","<<"bs_flag="<<m_bs<<","<<"vol="<<m_vol<<","<<"price="<<m_price<<"\n";

    return true;
}
void PlaceOrderForm::on_pushButton_2_clicked()
{

    bool ret=get_value_from_ui();
    if(ret){

    }

    //平仓


}

