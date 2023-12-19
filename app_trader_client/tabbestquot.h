#ifndef TABBESTQUOT_H
#define TABBESTQUOT_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTreeView>
#include "datafeed_struct.h"
#include <QMap>
#include "PlaceOrderForm.h"
/**
 * @brief 最优行情展示 tab
 */

class TabBestQuot : public QWidget
{
    Q_OBJECT
public:
    explicit TabBestQuot(QWidget *parent = nullptr);

    void on_best_quot( datafeed_quot_snap*quot,bool is_new=false);


private:
    void init_mdbl_view();
    void init_best_order_view();

private slots:
    void currentRowChanged(const QModelIndex cur, const QModelIndex &previous);
signals:
public:
    //最优行情
    QStandardItemModel* m_model;
    QTreeView*          m_view;
    //深度行情
    QTreeView*          m_mbl_view;
    QStandardItemModel* m_mbl_model;
    //最优订单
    PlaceOrderForm*     m_PlaceOrderForm;
    QString             m_cur_contract;




   // QMap<QString,int>   m_support_contract;
};

#endif // TABBESTQUOT_H
