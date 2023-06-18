#pragma once
#include "sq_struct_def.h"
#include "sq_biz_table_base.h"
namespace sq
{
	
	class sq_match_table:public sq_biz_table_base<match_id_t,sq_order_match_ntf>
	{
	public:
		sq_match_table() {};
		~sq_match_table(){};
	};

}