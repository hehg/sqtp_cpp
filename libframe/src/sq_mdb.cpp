#include "sq_mdb.h"
#include <fstream>
#include <unordered_map>
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "sq_order_table.h"
#include "sq_match_table.h"
#include "sq_quot_table.h"
#include "sq_posi_table.h"
#include "time/date_time.h"
#include "sq_frame.h"
using namespace std;
using namespace sq;

sq_order_table * s_order_table=nullptr;//订单表
sq_match_table * s_match_table = nullptr;//成交表
sq_quot_table  * s_quot_table=nullptr; //行情表
sq_posi_table  * s_posi_table=nullptr;//持仓表

//extern const char* sq_frame_get_cur_time();
//成交表
//持仓表

int sq_mdb_open()
{
    s_order_table=new sq_order_table();
	s_match_table = new sq_match_table();
    s_quot_table=new sq_quot_table();
    s_posi_table=new sq_posi_table();
    return ok;
}
int sq_mdb_close()
{
	safe_delete(s_order_table);
	safe_delete(s_match_table);
    safe_delete(s_quot_table);
    safe_delete(s_posi_table);
    return ok;
}
void *get_table(const char *name)
{
    if (strcmp(name, "order_table") == 0)
    {
        return s_order_table;
    }
    else if (strcmp(name, "match_table") == 0)
    {
        return s_match_table;
    }
    else if (strcmp(name, "quot_table") == 0)
    {
        return s_quot_table;
    }
    else if (strcmp(name, "posi_table") == 0)
    {
        return s_posi_table;
    }
    return nullptr;
}
int sq_mdb_put(int tid, char *data, int size)
{
   // log_debug("sq_mdb_put,tid={},size={}\n",tid,size)
    if(tid==tid_place_order){
        sq_req_order*req=(sq_req_order*)data;
        sq_order_record*order=s_order_table->find(req->sq_local_id);
        if(order!=nullptr){
            log_error("have exitst order,sq_local_id={}\n",req->sq_local_id);
            return err_repeat_order_id;
        }
        
        sq_order_record*rec = new sq_order_record();
        memset(rec, 0, sizeof(sq_order_record));
        rec->user_id= req->user_id;
        rec->sq_local_id = req->sq_local_id;
        rec->market = req->market;
        rec->contract = req->contract;
        rec->direction = req->direction;
        rec->offset = req->offset;
        rec->price = req->price;
        rec->qty = req->qty;
        rec->type = req->type;
		rec->product_type = req->product_type;
        rec->status = status_insert_submit;
        rec->insert_time = date_time::get_now_us();
		rec->quot_time = sq_frame_get_cur_time();
        
        int ret= s_order_table->add(req->sq_local_id,rec);
        if(ret!=ok){
            log_error("add to order_table error,ret={},sq_local_id={}\n",ret,req->sq_local_id);
        }

        s_posi_table->on_place_order(req->contract.data(),
        rec->price,
        rec->qty,
        rec->direction,
        rec->offset);

        //log_info("add order to mdb,sq_local_id={}\n",rec->sq_local_id);
        
    }
    else if(tid==tid_order_state){
        sq_order_state_ntf*ntf=(sq_order_state_ntf*)data;
        sq_order_record*order=s_order_table->find(ntf->sq_local_id);
        if(order==nullptr){
            log_error("not exit order,sq_local_id={}\n",ntf->sq_local_id);
            return err_not_exist;
        }
        log_info("update order to mdb by order_state_ntf,sq_local_id={},state={}\n",ntf->sq_local_id,get_state_name(ntf->status));
        //更新订单表
        s_order_table->on_order_state(ntf->sq_local_id,
        ntf->sys_id,
        ntf->status,
        ntf->cancel_qty);

        s_posi_table->on_order_state(order->contract.data(),
        order->status,
        order->qty,
        order->cancel_qty,
        order->direction,
        order->offset);

    }
    else if(tid==tid_order_match){
        sq_order_match_ntf*ntf=(sq_order_match_ntf*)data;
        sq_order_record*order=s_order_table->on_order_match(ntf->sq_local_id,
        ntf->match_price,ntf->match_qty);

		s_match_table->add(ntf->match_id, *ntf);
        s_posi_table->on_order_match(order->contract.data(),ntf->match_price,
        ntf->match_qty,ntf->direction,ntf->offset);

        log_info("update order to mdb by order_match_ntf,sq_local_id={}\n",ntf->sq_local_id);

    }
    else if(tid==tid_market_data){
        sq_quot*quot=(sq_quot*)data;
        s_quot_table->update(quot->contract.to_string(),*quot);
    }
    else{
        log_warn("not support tid={}\n",tid);
    }
    return ok;
}

int sq_mdb_set_option(const char *key, void *val)
{
    return ok;
}
int sq_mdb_get_option(const char *key, void *val)
{
    return ok;
}
sq::sq_order_record* sq_mdb_get_order_by_id(int local_id)
{
	if (s_order_table == nullptr) {
		return nullptr;
	}
    return s_order_table->find(local_id);
}
sq::sq_order_record *sq_mdb_get_order_by_index(int index)
{
    if (s_order_table)
    {
        return s_order_table->at(index);
    }
    return nullptr;
}
int sq_mdb_order_tb_size()
{
    if(s_order_table){
        return s_order_table->size();
    }
    return 0;
}

int sq_mdb_get_buy_posi(const char *contract)
{
    if (s_posi_table)
    {
        sq_contract_posi_table *t = s_posi_table->find(contract);
        if (t)
        {
			return t->m_sq_contract_posi.get_buy_posi();
        }
    }
    return 0;
}
int sq_mdb_get_sell_posi(const char *contract)
{
    if (s_posi_table)
    {
        sq_contract_posi_table *t = s_posi_table->find(contract);
        if (t)
        {
            return t->m_sq_contract_posi.get_sell_posi();
        }
    }
    return 0;
}
double sq_mdb_get_profit()
{
    double profit = 0;
    if (s_posi_table)
    {
		profit = s_posi_table->get_profit();
    }
    return profit;
}

int sq_mdb_get_posi_table_size() {
	if (s_posi_table) {
		return s_posi_table->size();
	}
	return 0;
	
}
sq_contract_posi* sq_mdb_get_posi_record(int index) {
	if (s_posi_table) {
		return &s_posi_table->at(index)->m_sq_contract_posi;
	}
	return nullptr;
}
void sq_mdb_get_posi_detail_by_index(int index) {

}

sq::sq_quot* sq_mdb_get_quot(const char*contract)
{
     return s_quot_table->find(contract);
}
void sq_mdb_dump()
{
    using namespace std;
    std::fstream f;
    f.open("./mdb.log",ios::out|ios::trunc);
    f<<"##order_table\n";
    s_order_table->dump(f);
    f<<"##match_table\n";
    s_match_table->dump(f);
    f.close();
}