#ifndef APIADAPTER_H
#define APIADAPTER_H
#include <thread>
#include <QObject>
#include "datafeed_struct.h"

namespace sq
{
    struct sq_quot;
}
/**
 * @brief 对服务数据接收的封装
 */
class ApiAdapter:public QObject
{
    Q_OBJECT
public:
    ApiAdapter();
    void start();
    void run();

    void on_best(sq::sq_quot*quot);
    void on_mbl(sq::sq_quot*quot);
    void on_best_order();
    void on_order_statis();

signals:
    void onQuotChanged(int,datafeed_quot_data quot);
private:
    std::thread* m_thread=nullptr;
};

#endif // APIADAPTER_H
