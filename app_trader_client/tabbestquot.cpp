#include "tabbestquot.h"
#include <QHBoxLayout>
#include <QModelIndex>
#include <QSplitter>

extern QHash<QString,datafeed_quot_snap*> glob_quot_table;

TabBestQuot::TabBestQuot(QWidget *parent) : QWidget{parent} {
  // 创建最优行情的表格
  {
    m_view = new QTreeView(this);
    m_view->setRootIsDecorated(false);
    m_view->setAlternatingRowColors(true);
    QStringList titles;
    titles<<"合约"<<"价格"<<"成交量"<<"成交额"<<"持仓量"<<"买1价"<<"买1量"<<"卖1价"<<"卖1量"<<"开盘"<<"收盘"<<"结算";
    titles<<"涨板价"<<"跌板价";
    m_model = new QStandardItemModel(0, titles.count(), this);

    for(int i=0;i<titles.count();i++)
    {
         m_model->setHeaderData(i, Qt::Horizontal, titles[i]);
    }
   // m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    //m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setModel(m_model);
  }

  // 创建深度行情的表格
  {
    m_mbl_view = new QTreeView(this);
    m_mbl_view->setRootIsDecorated(false);
    m_mbl_view->setAlternatingRowColors(true);

    m_mbl_model = new QStandardItemModel(0, 3, this);
    m_mbl_model->setHeaderData(0, Qt::Horizontal, QObject::tr("档位"));
    m_mbl_model->setHeaderData(1, Qt::Horizontal, QObject::tr("价格"));
    m_mbl_model->setHeaderData(2, Qt::Horizontal, QObject::tr("数量"));
    m_mbl_model->setHeaderData(3, Qt::Horizontal, QObject::tr("推导"));
    m_mbl_view->setModel(m_mbl_model);


   // m_mbl_view->setSelectionModel()

    init_mdbl_view();
  }

  // 创建最优价前 10笔的表格
  {
      m_PlaceOrderForm=new PlaceOrderForm();
  }

  // 进行布局
  QHBoxLayout *mainLayout = new QHBoxLayout;
  QSplitter*splitter=new QSplitter(Qt::Horizontal);
  QSplitter*splitter_right=new QSplitter(Qt::Vertical);
  QSplitter*splitter_left=new QSplitter(Qt::Vertical);

  //左侧部分
  {
      splitter_left->addWidget(m_view);
      //splitter_left->addWidget(m_statis_view);
  }
  //右侧部分
  {
      splitter_right->addWidget(m_mbl_view);
      splitter_right->addWidget(m_PlaceOrderForm);
      splitter_right->setStretchFactor(0,3);
      splitter_right->setStretchFactor(1,3);
  }

  splitter->addWidget(splitter_left);
  splitter->addWidget(splitter_right);

  splitter->setStretchFactor(0,3);
  splitter->setStretchFactor(1,1);

  mainLayout->addWidget(splitter);

  setLayout(mainLayout);

  //建立信号
  connect(m_view->selectionModel(),&QItemSelectionModel::currentRowChanged, this,&TabBestQuot::currentRowChanged);
}

void TabBestQuot::on_best_quot( datafeed_quot_snap *snap, bool is_new) {

    int index=snap->index;
  if (is_new) {
    // add new
    QStandardItem *item = new QStandardItem(snap->contract);
    m_model->appendRow(item);

  }

  /*
    titles<<"合约"<<"价格"<<"成交量"<<"成交额"<<"持仓量"<<"买1价"<<"买1量"<<"卖1价"<<"卖1量"<<"开盘"<<"收盘"<<"结算";
    titles<<"涨板价"<<"跌板价";
*/
  {
      const datafeed_best_quot &quot = snap->m_best_quot;
      m_model->setData(m_model->index(index, 1), QString::asprintf("%.2f",quot.price));
    m_model->setData(m_model->index(index, 2), quot.match_qty);
    m_model->setData(m_model->index(index, 3), QString::asprintf("%.2f",quot.turnover));
    m_model->setData(m_model->index(index, 4), quot.open_interest);
    m_model->setData(m_model->index(index, 5), QString::asprintf("%.2f",quot.bid_price));
    m_model->setData(m_model->index(index, 6), quot.bid_vol);
    m_model->setData(m_model->index(index, 7), QString::asprintf("%.2f",quot.ask_price));
    m_model->setData(m_model->index(index, 8), quot.ask_vol);
    m_model->setData(m_model->index(index, 9), QString::asprintf("%.2f",quot.open));
    m_model->setData(m_model->index(index, 10), QString::asprintf("%.2f",quot.close));
    m_model->setData(m_model->index(index, 11), QString::asprintf("%.2f",quot.clear));
    m_model->setData(m_model->index(index, 12), QString::asprintf("%.2f",quot.raise_limit));
    m_model->setData(m_model->index(index, 13), QString::asprintf("%.2f",quot.fall_limit));
  }
  if(snap->contract==m_cur_contract)
  {
      m_mbl_model->setData(m_mbl_model->index(0,1),QString::asprintf("%.2f",snap->m_mbl.ask[4]));
      m_mbl_model->setData(m_mbl_model->index(0,2),QString::asprintf("%d",snap->m_mbl.ask_vol[4]));
      m_mbl_model->setData(m_mbl_model->index(1,1),QString::asprintf("%.2f",snap->m_mbl.ask[3]));
      m_mbl_model->setData(m_mbl_model->index(1,2),QString::asprintf("%d",snap->m_mbl.ask_vol[3]));
      m_mbl_model->setData(m_mbl_model->index(2,1),QString::asprintf("%.2f",snap->m_mbl.ask[2]));
      m_mbl_model->setData(m_mbl_model->index(2,2),QString::asprintf("%d",snap->m_mbl.ask_vol[2]));
      m_mbl_model->setData(m_mbl_model->index(3,1),QString::asprintf("%.2f",snap->m_mbl.ask[1]));
      m_mbl_model->setData(m_mbl_model->index(3,2),QString::asprintf("%d",snap->m_mbl.ask_vol[1]));
      m_mbl_model->setData(m_mbl_model->index(4,1),QString::asprintf("%.2f",snap->m_mbl.ask[0]));
      m_mbl_model->setData(m_mbl_model->index(4,2),QString::asprintf("%d",snap->m_mbl.ask_vol[0]));

      m_mbl_model->setData(m_mbl_model->index(5,1),QString::asprintf("%.2f",snap->m_mbl.bid[0]));
      m_mbl_model->setData(m_mbl_model->index(5,2),QString::asprintf("%d",snap->m_mbl.bid_vol[0]));
      m_mbl_model->setData(m_mbl_model->index(6,1),QString::asprintf("%.2f",snap->m_mbl.bid[1]));
      m_mbl_model->setData(m_mbl_model->index(6,2),QString::asprintf("%d",snap->m_mbl.bid_vol[1]));
      m_mbl_model->setData(m_mbl_model->index(7,1),QString::asprintf("%.2f",snap->m_mbl.bid[2]));
      m_mbl_model->setData(m_mbl_model->index(7,2),QString::asprintf("%d",snap->m_mbl.bid_vol[2]));
      m_mbl_model->setData(m_mbl_model->index(8,1),QString::asprintf("%.2f",snap->m_mbl.bid[3]));
      m_mbl_model->setData(m_mbl_model->index(8,2),QString::asprintf("%d",snap->m_mbl.bid_vol[3]));
      m_mbl_model->setData(m_mbl_model->index(9,1),QString::asprintf("%.2f",snap->m_mbl.bid[4]));
      m_mbl_model->setData(m_mbl_model->index(9,2),QString::asprintf("%d",snap->m_mbl.bid_vol[4]));
  }


}



void TabBestQuot::init_mdbl_view()
{
    const QString titles[10]={"卖5","卖4","卖3","卖2","卖1","买1","买2","买3","买4","买5"};
    for(int i=0;i<10;i++)
    {
        QStandardItem *item = new QStandardItem(titles[i]);
        m_mbl_model->appendRow(item);
    }
}




void TabBestQuot::currentRowChanged(const QModelIndex cur, const QModelIndex &)
{
    static const int ask_row[]={4,3,2,1,0};
    static const int bid_row[]={5,6,7,8,9};

    qDebug()<<"currentRowChanged"<<cur.row();
    QModelIndex index=cur.sibling(cur.row(),0);
    QStandardItem*item=m_model->itemFromIndex(index);
    if(item){
        QString contract= item->text();
        //set mbl
        auto it= glob_quot_table.find(contract);
        if(it!=glob_quot_table.end())
        {
            m_cur_contract=contract;
            for(int i=0;i<5;i++)
            {
                m_mbl_model->setData(m_mbl_model->index(ask_row[i],1),it.value()->m_mbl.ask[i]);
                m_mbl_model->setData(m_mbl_model->index(ask_row[i],2),it.value()->m_mbl.ask_vol[i]);
            }

            for(int i=0;i<5;i++)
            {
                m_mbl_model->setData(m_mbl_model->index(bid_row[i],1),it.value()->m_mbl.bid[i]);
                m_mbl_model->setData(m_mbl_model->index(bid_row[i],2),it.value()->m_mbl.bid_vol[i]);
            }



        }

    }
}
