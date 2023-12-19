#include "apiadapter.h"
#include "datafeed_struct.h"
#include <time.h>
#include "sq_frame.h"
#include <QSettings>
#include "globalsetting.h"
#include "sq_tid_define.h"
#include "sq_struct_def.h"
using namespace sq;
ApiAdapter::ApiAdapter() {}

static void on_quot_callback(int tid, char*data, int size, void*obj)
{
   ApiAdapter*f = (ApiAdapter*)obj;
   // f->on_quot(tid, data, size);

   if (tid == tid_market_data)
   {
      sq_quot *quot = (sq_quot *)data;
      // qDebug()<<"on quot "<<quot->price;
       f->on_best(quot);
      f->on_mbl(quot);
   }
}

void ApiAdapter::start() {
   // QSettings setting=new QSettings("./app.ini",QSettings::IniFormat);
    //setting.value("api/con")

    sq_frame_set_option("callback_param",this);
    sq_frame_set_option("callback_func",(void*)on_quot_callback);

    QString xml_cfg_file_path=GlobalSetting::instance().m_app_dir+"/config.xml";
    sq_frame_open(xml_cfg_file_path.toStdString().c_str());
   // m_thread = new std::thread(&ApiAdapter::run, this);
    //delete setting;
}

void ApiAdapter::run()
{
  while (true)
    {
        //on_best();
        //on_mbl();
        on_best_order();
        on_order_statis();

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
 }


 void ApiAdapter::on_best(sq::sq_quot*quot) {

   // srand(time(0));
    //for (int i = 0; i < 100; i++)
    {
    datafeed_quot_data data;

    data.contract = quot->contract.data();
    data.market = quot->market.data();

    datafeed_best_quot *my_quot = new(data.m_data)datafeed_best_quot;
    my_quot->price = quot->price;
    my_quot->match_qty = quot->match_qty;
    my_quot->turnover = quot->turnover;
    my_quot->open_interest=quot->interest;
    my_quot->bid_price= quot->bid_price();
    my_quot->bid_vol=quot->bid_qty();
    my_quot->ask_price=quot->ask_price();
    my_quot->ask_vol=quot->ask_qty();


    my_quot->open=quot->open;
    my_quot->clear=0;
    my_quot->close=quot->close;
    my_quot->raise_limit=quot->limit_up;
    my_quot->fall_limit=quot->limit_down;



    emit onQuotChanged(1, data);
  }
}
void ApiAdapter::on_mbl(sq::sq_quot*quot)
{
    for (int i = 0; i < 100; i++) {
        datafeed_quot_data data;

        data.contract = quot->contract.data();
        data.market = quot->market.data();

        datafeed_mbl*my_quot=new (data.m_data)datafeed_mbl;
        for(int i=0;i<5;i++){
            my_quot->bid[i]=quot->mbl_buy_price[i];
            my_quot->bid_vol[i]=quot->mbl_buy_qty[i];
            my_quot->ask[i]=quot->mbl_sell_price[i];
            my_quot->ask_vol[i]=quot->mbl_sell_qty[i];

        }

        emit onQuotChanged(2, data);
    }
}
void ApiAdapter::on_best_order()
{
    for (int i = 0; i < 100; i++) {
        datafeed_quot_data data;

        data.contract = QString::asprintf("m%04d", i);
        data.market = "dce";

        datafeed_best_order*quot=new (data.m_data)datafeed_best_order;
        quot->bid=i;
        quot->ask=i;
        for(int i=0;i<10;i++){
            quot->bid_vol[i]=i;
            quot->ask_vol[i]=i;

        }

        emit onQuotChanged(3, data);
    }
}
void ApiAdapter::on_order_statis()
{
   for (int i = 0; i < 100; i++) {
        datafeed_quot_data data;

        data.contract = QString::asprintf("m%04d", i);
        data.market = "dce";

        datafeed_order_statis*quot=new (data.m_data)datafeed_order_statis;
        quot->avg_buy_price=100;
        quot->avg_sell_price=200;
        quot->total_sell_vol=100;
        quot->total_buy_vol=123;

        emit onQuotChanged(4, data);
    }
}
