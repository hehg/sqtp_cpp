#ifndef TABORDERS_H
#define TABORDERS_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTreeView;
class QStandardItemModel;
QT_END_NAMESPACE
//class PlaceOrderForm;

/**
 * @brief 下单界面
 */
class TabOrders : public QWidget
{
    Q_OBJECT
public:
    explicit TabOrders(QWidget *parent = nullptr);

signals:

public:
    QTreeView*          m_order_view;
    QStandardItemModel* m_order_model;

    QTreeView*          m_posi_view;
    QStandardItemModel* m_posi_model;

    //PlaceOrderForm*     m_PlaceOrderForm;
};

#endif // TABORDERS_H
