#include "mainwindow.h"
#include <QtWidgets>
#include <string.h>
#include <QSplitter>
#include "apiadapter.h"
#include "tabbestquot.h"
#include "taboptparam.h"
#include "taborders.h"

QHash<QString,datafeed_quot_snap*> glob_quot_table;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(QSize(800,600));
    QSplitter*mainSplitter=new QSplitter(Qt::Vertical);
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout;

    setCentralWidget(mainWidget);

    CreateMarketTables();
    CreateOrderTables();

    //进行布局
    mainSplitter->addWidget(m_TopTabWidget);
    mainSplitter->addWidget(m_BottomTabWidget);
    mainSplitter->setStretchFactor(0,3);
    mainSplitter->setStretchFactor(0,2);
    mainLayout->addWidget(mainSplitter);
    mainWidget->setLayout(mainLayout);

    setWindowTitle("交易客户端");


    //启动 api
    m_ApiAdapter=new ApiAdapter();
    //与 api 建立链接
    connect(m_ApiAdapter,&ApiAdapter::onQuotChanged,this,&MainWindow::on_quot);
    m_ApiAdapter->start();


}

MainWindow::~MainWindow() {}

void MainWindow::CreateMarketTables()
{

    m_TopTabWidget= new QTabWidget();

    //add 深度行情 page
    m_TabBestQuot=new TabBestQuot();
    m_TopTabWidget->addTab(m_TabBestQuot,"最优行情");

    //add 期权参数 page
    m_TabOptParam=new TabOptParam();
    m_TopTabWidget->addTab(m_TabOptParam,"期权参数");
}
void MainWindow::CreateOrderTables()
{
    m_BottomTabWidget=new QTabWidget();
    m_TabOrders=new TabOrders();
    m_BottomTabWidget->addTab(m_TabOrders,"订单");
}
void MainWindow::on_quot(int id, datafeed_quot_data quot)
{
    auto it= glob_quot_table.find(quot.contract);
    //qDebug()<<"on_quot,"<<"id="<<id;
    bool is_new=false;
    if(it==glob_quot_table.end()){
        datafeed_quot_snap* new_data=new datafeed_quot_snap;
        new_data->index=glob_quot_table.count();
        it=glob_quot_table.insert(quot.contract,new_data);
        it.value()->contract=quot.contract;
        it.value()->market=quot.market;
        is_new=true;
    }
    //复制数据
    if(id==1){
        memcpy(&it.value()->m_best_quot,(datafeed_best_quot*)quot.m_data,sizeof(datafeed_best_quot));
    }
    else if(id==2){
        it.value()->m_mbl=*(datafeed_mbl*)quot.m_data;
    }
    else if(id==3){
        it.value()->m_best_order=*(datafeed_best_order*)quot.m_data;
    }
    else if(id==4){
        it.value()->m_order_statis=*(datafeed_order_statis*)quot.m_data;
    }
    //传给 UI
    m_TabBestQuot->on_best_quot(it.value(),is_new);
}
