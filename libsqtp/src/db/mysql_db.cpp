#include "db/mysql_db.h"
#include "text/json.hpp"
#include <sstream>
# if 0
#if  SUPPORT_MYSQL_DB
namespace sq
{
	mysql_db::mysql_db()
	{
		
		
	}
	mysql_db::~mysql_db()
	{
		
	}
	bool mysql_db::connect_db(string instance, string user, string passwd,
		string host_name, int port)
	{
		m_host = host_name;
		m_host_port = port;
		m_user = user;
		m_passwd = passwd;
		try {
			char connect_str[512] = { 0 };
			sprintf(connect_str, "%s/%s@%s", m_user.c_str(), m_passwd.c_str(), 
				instance.c_str());
			m_db.rlogon(connect_str);
			return true;
		}
		catch (otl_exception& p) { // intercept OTL exceptions
			cerr << p.msg << endl; // print out error message
			cerr << p.sqlstate << endl; // print out SQLSTATE 
			cerr << p.stm_text << endl; // print out SQL that caused the error
			cerr << p.var_info << endl; // print out the variable that caused the error
		}
		return false;
	}

	void mysql_db::insert(string&json_str)
	{
#if 0
		using namespace nlohmann;
		json root = json::parse(json_str);
		json fields = root["fields"];
		strstream sql;
		sql << "insert into " <<
			root["table"].get<string>() << "(";

		string str_fields;
		string values;
		int count = fields.size();
		json::iterator it = fields.begin();
		int i = 0;
		for (; it != root.end(); ++it)
		{
			int a = (uint8_t)it->type();
			//cout << it.key() << "  " << a << " " << it->type_name() << endl;
			values += ":" + it->type_name() + "<" + it->type_name() + ">";
			str_fields += it->type_name();
			if (i != count) {
				str_fields += ",";
				values += ",";
			}
		}
		sql << str_fields << ")" << "values(";

		otl_stream select_stream(1, sql.str(), m_db);
		it = fields.begin();
		for (; it != root.end(); ++it)
		{
			if (it->type() == json::value_t::string)
				select_stream << it->get<string>().c_str();
			else if (it->type() == json::value_t::number_integer)
				select_stream << it->get<int>();
			else if (it->type() == json::value_t::number_float)
				select_stream << it->get<double>();
			else
				assert(false);
		}
	}
#endif
#if 0
	void strategy_stock::insert_to_db(stock_posi_info*info)
	{
		try {
			char sql[1024];
			sprintf(sql, "insert into tt_order (signal_id,stock,market,"
				"signal_date,open_price,turnover,open_qty,close_qty,"
				"hold_day,stop_profit_rate,stop_loss_rate,"
				"max_posi_time,fund_quota,"
				"state,is_finish) "
				"values(:signal_id<char[32]>,:stock<char[16]>,:market<char[16]>,"
				":signal_date<char[16]>,:open_price<double>,:turnover<double>,:open_qty<int>,:close_qty<int>,"
				":hold_day<int>,:stop_profit_rate<double>,:stop_loss_rate<double>,"
				":max_posi_time<int>,:fund_quota<double>,"
				":state<char[16]>,:is_finish<int>) ");
			// cout << sql << endl;
			otl_stream select_stream(1, sql, m_db);
			select_stream << info->signal_id;
			select_stream << info->stock;
			select_stream << info->market;
			select_stream << info->signal_date;
			select_stream << info->open_price;
			select_stream << info->turnover;
			select_stream << info->open_qty;
			select_stream << info->close_qty;
			select_stream << info->hold_day;
			select_stream << info->stop_profit_rate;
			select_stream << info->stop_loss_rate;
			select_stream << info->max_posi_time;
			select_stream << info->fund_quota;
			select_stream << info->state;
			select_stream << info->is_finish;

		}
		// 出错打印错误信息
		catch (otl_exception &e)
		{
			cerr << e.msg << endl;
			cerr << e.sqlstate << endl;
			cerr << e.stm_text << endl;
			cerr << e.var_info << endl;
		}
	}
}
#endif
	}
}
#endif
#endif