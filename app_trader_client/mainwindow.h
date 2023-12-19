#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include "datafeed_struct.h"

QT_BEGIN_NAMESPACE
class QTabWidget;

QT_END_NAMESPACE

class ApiAdapter;
class TabOrders;
class TabBestQuot;
class TabOptParam;
class TabOrders;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void CreateMarketTables();
    void CreateOrderTables();

 public  slots:
    void on_quot(int id,datafeed_quot_data quot);
private:
    QTabWidget* m_TopTabWidget=nullptr;
    QTabWidget* m_BottomTabWidget=nullptr;

    TabBestQuot* m_TabBestQuot=nullptr;
    TabOptParam* m_TabOptParam=nullptr;

    TabOrders*   m_TabOrders=nullptr;

    ApiAdapter*  m_ApiAdapter=nullptr;
    QHash<QString,datafeed_quot_snap*> m_quot_table;

};
#endif // MAINWINDOW_H
