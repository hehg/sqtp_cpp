#include "taborders.h"
#include <QTreeView>
#include <QStandardItemModel>
#include  <QHBoxLayout>
#include <QSplitter>

TabOrders::TabOrders(QWidget *parent)
    : QWidget{parent}
{
    // 创建订单表格
    {
        m_order_view = new QTreeView(this);
        m_order_view->setRootIsDecorated(false);
        m_order_view->setAlternatingRowColors(true);
        QStringList titles;
        titles<<"本地编号"<<"合约"<<"系统号"<<"市场"<<"买卖方向"<<"开平"<<"委托价格"<<"委托数量"<<"成交数量"<<"订单状态";

        m_order_model = new QStandardItemModel(0, titles.count(), this);

        for(int i=0;i<titles.count();i++)
        {
            m_order_model->setHeaderData(i, Qt::Horizontal, titles[i]);
            m_order_view->setColumnWidth(i,50);
        }

        m_order_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_order_view->setModel(m_order_model);

        for(int i=0;i<titles.count();i++)
        {
            m_order_view->setColumnWidth(i,80);
        }
    }

    {
        m_posi_view = new QTreeView(this);
        m_posi_view->setRootIsDecorated(false);
        m_posi_view->setAlternatingRowColors(true);
        QStringList titles;

        titles<<"合约"<<"买卖"<<"开仓均价"<<"开仓数量"<<"已平量"<<"剩余量"<<"浮动盈亏"<<"平仓盈亏";

        m_posi_model = new QStandardItemModel(0, titles.count(), this);

        for(int i=0;i<titles.count();i++)
        {
            m_posi_model->setHeaderData(i, Qt::Horizontal, titles[i]);
        }

        m_posi_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_posi_view->setModel(m_posi_model);

    }

   // m_PlaceOrderForm=new PlaceOrderForm(this);

    // 进行布局
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QSplitter*splitter=new QSplitter(Qt::Horizontal);


    splitter->addWidget(m_order_view);
    splitter->addWidget(m_posi_view);
    splitter->setStretchFactor(0,4);
    splitter->setStretchFactor(1,3);

    mainLayout->addWidget(splitter);
    setLayout(mainLayout);
}
