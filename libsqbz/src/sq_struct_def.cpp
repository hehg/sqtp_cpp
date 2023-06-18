#include "sq_struct_def.h"
#include "sq_struct_des.h"

#define SQ_DEF_DES_VAR(class_name) static class_name##_desc s_##class_name##_desc 

namespace sq
{
	SQ_DEF_DES_VAR(sq_quot);
	SQ_DEF_DES_VAR(sq_req_order);
	SQ_DEF_DES_VAR(sq_order_record);
	SQ_DEF_DES_VAR(sq_order_match_ntf);
	SQ_DEF_DES_VAR(sq_order_state_ntf);


    void sq_quot::to_string(std::ostream&of)
    {
        s_sq_quot_desc.to_string(this,of);
    }
    void sq_quot::to_json(std::ostream&of)
    {
        s_sq_quot_desc.to_json(this,of);
    }
	void sq_req_order::to_string(std::ostream&of)
    {
        s_sq_req_order_desc.to_string(this,of);
    }
    bool sq_order_record::change_status(order_status_t state)
	{
		if (is_finish)
		{
			return false;
		}
		if ((status == status_all_trade || status == status_part_trade) &&
			state == status_accept)
		{
			return false;
		}
		if (state == status_insert_reject || state == status_cancel ||
			state == status_cancel_reject || state == status_all_trade)
		{
			is_finish = true;
		}
		if (state != status)
		{
			pre_status = status;
			status = state;
			return true;
		}
		return false;
	}

	void sq_order_record::to_string(std::ostream&of)
    {
        s_sq_order_record_desc.to_string(this,of);
    }
	void sq_order_match_ntf::to_string(std::ostream&of)
    {
        s_sq_order_match_ntf_desc.to_string(this,of);
    }
    void sq_order_match_ntf::to_json(std::ostream&of)
    {
        s_sq_order_match_ntf_desc.to_json(this,of);
    }
    void sq_order_state_ntf::to_json(std::ostream&of)
    {
        s_sq_order_state_ntf_desc.to_json(this,of);
    }
	const char* get_state_name(short s)
   {
       static const char* state_name[]={
           "status_unknow",
           "status_insert_submit",
           "status_cancel_submit",
           "status_insert_reject",
           "status_cancel_reject",
           "status_accept",
           "status_part_trade",
           "status_all_trade",
           "status_cancel",
           "status_cancel_reject_by_broker"
       };
       int size=sizeof(state_name)/sizeof(state_name[0]);
       if(s<0 || s>=size){
           assert(false);
       }
       return state_name[s];
   }

	
	int sq_contract_posi::total_posi()
	{
		return m_buy_posi - m_sell_posi +
			m_buy_opening - m_sell_closing -
			m_sell_opening + m_buy_closing;
	}

	int sq_contract_posi::total_pending_posi()
	{
		return m_buy_opening - m_sell_closing -
			m_sell_opening + m_buy_closing;
	}

	int sq_contract_posi::get_buy_posi()
	{
		return m_buy_posi;
	}
	int sq_contract_posi::get_sell_posi()
	{
		return -m_sell_posi;
	}
	bool sq_contract_posi::is_no_pending_order()
	{
		if (m_buy_opening + m_sell_closing +
			m_sell_opening + m_buy_closing == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	int sq_contract_posi::can_sell_close()
	{
		int qty = m_buy_posi - m_sell_closing;
		assert(qty >= 0);
		return qty;
	}
	int sq_contract_posi::can_buy_close()
	{
		int qty = m_sell_posi - m_buy_closing;
		assert(qty >= 0);
		return qty;
	}
}