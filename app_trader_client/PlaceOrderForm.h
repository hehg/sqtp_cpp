#ifndef PLACEORDERFORM_H
#define PLACEORDERFORM_H

#include <QWidget>

namespace Ui {
class PlaceOrderForm;
}

class PlaceOrderForm : public QWidget
{
    Q_OBJECT

public:
    explicit PlaceOrderForm(QWidget *parent = nullptr);
    ~PlaceOrderForm();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();


private:
    bool get_value_from_ui();
    Ui::PlaceOrderForm *ui;
    QString m_contract;
    double  m_price;
    int     m_vol;
    int     m_bs;
};

#endif // PLACEORDERFORM_H
