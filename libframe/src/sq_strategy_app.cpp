/**
 * @brief
 */

#include "sq_strategy_app.h"
#include <assert.h>
#include "sq_frame.h"
#include "sq_mdb.h"
#include "error/sq_err.h"
#include "sq_bz_utils.h"
using namespace sq;

static void _sq_strategy_callback_(int tid, char *data, int size, void *param)
{
    sq_frame_caller *app = (sq_frame_caller *)param;
    assert(app);
    app->callback(tid, data, size);
}
void sq_frame_caller::callback(int tid,char*msg,int msg_size)
{
    //回调
   if(m_callback_)
   {
      if (tid == tid_market_data)
      {
         sq_quot *quot = (sq_quot *)msg;
         m_callback_->on_market_data(quot->market.data(),
                                     quot->market.data(),
                                     quot->price, quot->match_qty,
                                     quot->turnover, quot->interest,
                                     quot->mbl_buy_price, quot->mbl_buy_qty,
                                     quot->mbl_sell_price, quot->mbl_sell_qty);
      }
      else if(tid==tid_order_state){
         sq_order_state_ntf*ntf=(sq_order_state_ntf*)msg;
         if(ntf->status==order_status_t::status_accept){
            m_callback_->on_order_accepted(ntf->sq_local_id,ntf->sys_id.data());    
         }
         else if(is_reject_statues(ntf->status))
         {
            m_callback_->on_order_rejected(ntf->sq_local_id,ntf->err_code);
         }
         else if(ntf->status==order_status_t::status_cancel)
         {
            m_callback_->on_order_canceled(ntf->sq_local_id);
         }
         else{
            sq_panic("not support order status=%d\n",ntf->status);
         }
         
      }
      else if (tid == tid_order_match)
      {
         sq_order_match_ntf *ntf = (sq_order_match_ntf *)msg;
         m_callback_->on_order_matched(ntf->sq_local_id,
                                       ntf->contract.data(),
                                       ntf->match_price,
                                       ntf->match_qty);
      }
      else{
        sq_panic("not support tid=%d\n",tid);
      }
   }
}
int sq_frame_caller::open(sq_strategy_rsp *rsp, const char *cfg_path)
{
    int ret = ok;
    m_callback_ = rsp;
    ret = sq_frame_open(cfg_path);
    if (ret!=ok)
    {
        log_error("open frame config path failed\n");
        return ret;
    }

    // 为框架设置参数
    sq_frame_set_option("callback_func", (void*)_sq_strategy_callback_);
    sq_frame_set_option("callback_param", this);

    return ok;
}
void sq_frame_caller::run()
{

}
int sq_frame_caller::buy_open(int local_id, const char *contract, double price, int qty)
{
    return sq_frame_buy_open(local_id,contract,price,qty);
}
int sq_frame_caller::sell_open(int local_id, const char *contract, double price, int qty)
{
    return sq_frame_sell_open(local_id,contract,price,qty);
}
int sq_frame_caller::buy_close(int local_id, const char *contract, double price, int qty)
{
    return sq_frame_buy_close(local_id,contract,price,qty);
}
int sq_frame_caller::sell_close(int local_id, const char *contract, double price, int qty)
{
    return sq_frame_sell_close(local_id,contract,price,qty);
}
 int sq_frame_caller::cancel_order(int local_id)
 {
    return sq_frame_cancel_order(local_id);
 }