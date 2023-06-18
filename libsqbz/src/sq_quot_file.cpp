#include "sq_quot_file.h"
#include <assert.h>
#include <string>
#include <float.h>
#include "os/sq_path.h"
#include "sq_struct_def.h"
#include "error/sq_err.h"
#include "sq_struct_des.h"
#include "text/sq_string.h"
#include "log/sq_logger.h"
using namespace std;
/** 支持的表头如下
tid--报文类型
type--类型
pkg_no--编号
market--市场
variety--品种
date--日期 格式如:20170101
time--时间 格式如:09:00:01.234
contract-合约
price -最新价
match_qty-成交量
interest--持仓量
bid_price --买1价
bid_qty --买1量
ask_price --买1价
ask_qty-- 卖1量
BP1(2,3,4,5)--
BV1(2,3,4,5)--
SP1(2,3,4,5)--
SV1(2,3,4,5)--
recv_time-- 接收时间
limit_up-- 涨板
limit_down--跌板
turnover-- 成交额
market-- 市场
open-- 开盘价
high-- 最高价
low--- 最低价
mstime-- 时间戳 毫秒
p--概率
close--收盘
open--开盘
multi--价格因子
pre_close--昨收盘
*/
namespace sq
{
  string format_price(double price)
  {
    if (price == DBL_MAX)
    {
      return "null";
    }
    else
    {
      stringstream st;
      st.precision(6);
      st << std::fixed << price;
      return st.str();
    }
  }
  static const char *s_hdrs[] = {"tid", "type", "pkg_no", "date", "time", "market", "variety", "contract",
                                 "price", "match_qty", "turnover", "interest",
                                 "BP5", "BV5", "BP4", "BV4", "BP3", "BV3", "BP2", "BV2", "BP1", "BV1",
                                 "SP1", "SV1", "SP2", "SV2", "SP3", "SV3", "SP4", "SV4", "SP5", "SV5",
                                 "open", "close", "high", "low", "pre_close", "limit_up", "limit_down",
                                 "mstime", "recv_time", "multi"};

// 列id 与名称的对应关系
  static map<int, string> m_dict = {
      std::map<int, string>::value_type(Id_date, "date"),
      std::map<int, string>::value_type(Id_time, "time"),
      std::map<int, string>::value_type(Id_contract, "contract"),
      std::map<int, string>::value_type(Id_price, "price"),
      std::map<int, string>::value_type(Id_match_qty, "match_qty"),
      std::map<int, string>::value_type(Id_interest, "interest"),
      std::map<int, string>::value_type(Id_BP1, "BP1"),
      std::map<int, string>::value_type(Id_BP2, "BP2"),
      std::map<int, string>::value_type(Id_BP3, "BP3"),
      std::map<int, string>::value_type(Id_BP4, "BP4"),
      std::map<int, string>::value_type(Id_BP5, "BP5"),
      std::map<int, string>::value_type(Id_BV1, "BV1"),
      std::map<int, string>::value_type(Id_BV2, "BV2"),
      std::map<int, string>::value_type(Id_BV3, "BV3"),
      std::map<int, string>::value_type(Id_BV4, "BV4"),
      std::map<int, string>::value_type(Id_BV5, "BV5"),

      std::map<int, string>::value_type(Id_SP1, "SP1"),
      std::map<int, string>::value_type(Id_SP2, "SP2"),
      std::map<int, string>::value_type(Id_SP3, "SP3"),
      std::map<int, string>::value_type(Id_SP4, "SP4"),
      std::map<int, string>::value_type(Id_SP5, "SP5"),
      std::map<int, string>::value_type(Id_SV1, "SV1"),
      std::map<int, string>::value_type(Id_SV2, "SV2"),
      std::map<int, string>::value_type(Id_SV3, "SV3"),
      std::map<int, string>::value_type(Id_SV4, "SV4"),
      std::map<int, string>::value_type(Id_SV5, "SV5"),
      std::map<int, string>::value_type(Id_Recvtime, "recv_time"),
      std::map<int, string>::value_type(Id_limit_up, "limit_up"),
      std::map<int, string>::value_type(Id_limit_down, "limit_down"),
      std::map<int, string>::value_type(Id_turnover, "turnover"),
      std::map<int, string>::value_type(Id_market, "market"),
      std::map<int, string>::value_type(Id_open, "open"),
      std::map<int, string>::value_type(Id_high, "high"),
      std::map<int, string>::value_type(Id_low, "low"),
      std::map<int, string>::value_type(Id_timems, "mstime"),
      std::map<int, string>::value_type(Id_variety, "variety"),
      std::map<int, string>::value_type(Id_tid, "tid"),
      std::map<int, string>::value_type(Id_type, "type"),
      std::map<int, string>::value_type(Id_pkg_no, "pkg_no"),
      std::map<int, string>::value_type(Id_close, "close"),
      std::map<int, string>::value_type(Id_pre_close, "pre_close"),
      std::map<int, string>::value_type(Id_multi, "multi"),
  };

  
  sq_quot_file::sq_quot_file()
  {
    auto it = m_dict.begin();
    for (; it != m_dict.end(); ++it)
    {
      m_dict_name[it->second] = it->first;
    }
  }
  sq_quot_file::~sq_quot_file()
  {
    close();
  }
  int sq_quot_file::close()
  {
    if (m_file.is_open())
    {
      m_file.close();
    }
    return ok;
  }
  void sq_quot_file::set_header(const char *hdr)
  {
    m_cols.clear();
    std::vector<std::string> hdrs;
    sq_split(hdr, hdrs);
    for (int i = 0; i < hdrs.size(); i++)
    {
      auto it = m_dict_name.find(hdrs[i]);
      if (it == m_dict_name.end())
      {
        log_fata("not support col name={}\n", hdrs[i].c_str());
      }
      column_info info;
      info.col = i;
      info.col_name = hdrs[i];
      info.id = m_dict_name[info.col_name];
      m_cols.emplace_back(info);
    }
  }
  sq_quot_file_writer::sq_quot_file_writer()
  {
    for(int i=0;i<sizeof(s_hdrs)/sizeof(s_hdrs[0]);i++){
      column_info info;
      info.col=i;
      info.col_name=s_hdrs[i];
      info.id=m_dict_name[info.col_name];
      m_cols.emplace_back(info);
    }
  }
  int sq_quot_file_writer::open(const char *path, const char *mode, bool have_head)
  {
    bool is_exist = sq_path::exist(path);
    if (strcmp(mode, "w+") == 0)
    {
      is_exist = false;
      m_file.open(path, std::ios::out | std::ios::trunc);
    }
    else if (strcmp(mode, "a+") == 0)
    {
      m_file.open(path, std::ios::out | std::ios::app);
    }
    else
    {
      assert(false);
      return 1;
    }
    // 写头信息
    if (!is_exist && have_head)
    {
      for (int i = 0; i < m_cols.size(); i++)
      {
        m_file << m_cols[i].col_name;
        if (i != m_cols.size() - 1)
        {
          m_file << ",";
        }
      }
      m_file << "\n";
    }
    m_file << std::fixed;
    m_file.precision(6);
    return ok;
  }

  int sq_quot_file_writer::write(int tid, char *data, int size)
  {
    if (tid == tid_market_data)
    {
      sq_quot *quot = (sq_quot *)data;
      m_file << tid << ","
             << (int)quot->type << ","                   // 1
             << quot->pkg_no << ","                 // 2
             << quot->date.data() << ","            // 3
             << quot->time.data() << ","            // 4
             << quot->market.data() << ","          // 5
             << quot->variety.data() << ","         // 6
             << quot->contract.data() << ","        // 7
             << format_price(quot->price) << ","    // 8
             << quot->match_qty << ","              // 9
             << format_price(quot->turnover) << "," // 10
             << quot->interest << ","               // 11
             // 12-13
             << format_price(quot->mbl_buy_price[4]) << "," << quot->mbl_buy_qty[4] << ","
             // 14-15
             << format_price(quot->mbl_buy_price[3]) << "," << quot->mbl_buy_qty[3] << ","
             // 16-17
             << format_price(quot->mbl_buy_price[2]) << "," << quot->mbl_buy_qty[2] << ","
             // 18-19
             << format_price(quot->mbl_buy_price[1]) << "," << quot->mbl_buy_qty[1] << ","
             // 20-21
             << format_price(quot->mbl_buy_price[0]) << "," << quot->mbl_buy_qty[0] << ","

             // 22-23
             << format_price(quot->mbl_sell_price[0]) << "," << quot->mbl_sell_qty[0] << ","
             // 24-25
             << format_price(quot->mbl_sell_price[1]) << "," << quot->mbl_sell_qty[1] << ","
             // 26-27
             << format_price(quot->mbl_sell_price[2]) << "," << quot->mbl_sell_qty[2] << ","
             // 28-29
             << format_price(quot->mbl_sell_price[3]) << "," << quot->mbl_sell_qty[3] << ","
             // 30-31
             << format_price(quot->mbl_sell_price[4]) << "," << quot->mbl_sell_qty[4] << ","
             // 32

             << format_price(quot->open) << ","
             << format_price(quot->close) << ","
             << format_price(quot->high) << ","
             << format_price(quot->low) << ","
             << quot->pre_close << ","
             << quot->limit_up << ","
             << quot->limit_down << ","

             << quot->mstime << ","
             << quot->recv_time << ","
             << quot->multi << "\n";
    }
    else
    {
      log_fata("not support tid={}\n", tid);
    }
    return ok;
  }

  
  //======================================================
  sq_quot_file_reader::sq_quot_file_reader()
  {
    
  };
  sq_quot_file_reader::~sq_quot_file_reader()
  {
  }
  bool sq_quot_file_reader::open(const char *path, bool hasHead)
  {
    m_file.open(path, std::ios::in);
    if (!m_file.is_open())
    {
      return false;
    }
    if (hasHead)
    {
      m_cols.clear();
      string header;
      std::getline(m_file, header);
      m_tmp_out.clear();
      sq_split(header.c_str(), m_tmp_out);
      // 解析头,构建列信息
      for (short i = 0; i < m_tmp_out.size(); i++)
      {
        string name = m_tmp_out[i];
        auto it = m_dict_name.find(name);
        if (it == m_dict_name.end())
        {
          log_fata("not support col name {}\n", name.c_str());
          return false;
        }
        column_info info = column_info{i, name, (short)it->second};
        m_cols.emplace_back(info);
      }
    }

    return true;
  }

  bool sq_quot_file_reader::read_line(sq_quot &quot)
  {
    string line;
    std::getline(m_file, line);
    if(m_file.eof()){
      return false;
    }
    m_tmp_out.clear();
    int count = sq_split(line.c_str(), m_tmp_out);

    for (int i = 0; i < count; i++)
    {
      short id=m_cols[i].id;
      switch (id)
      {
      case Id_date:
      {
        quot.date = m_tmp_out[i];
        break;
      }
      case Id_time:
      {
        quot.time = m_tmp_out[i];

        break;
      }

      case Id_tid:
      {
        quot.tid = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_pkg_no:
      {
        quot.pkg_no=atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_type:
      {
        quot.type = (product_type_t)atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_contract:
      {
        quot.contract = m_tmp_out[i];
        break;
      }
      case Id_price:
      {
        quot.price = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_match_qty:
      {
        quot.match_qty = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_interest:
      {
        quot.interest = atof(m_tmp_out[i].c_str());
        break;
      }

      case Id_BP1:
      {
        quot.mbl_buy_price[0] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_BP2:
      {
        quot.mbl_buy_price[1] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_BP3:
      {
        quot.mbl_buy_price[2] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_BP4:
      {
        quot.mbl_buy_price[3] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_BP5:
      {
        quot.mbl_buy_price[4] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_BV1:
      {
        quot.mbl_buy_qty[0] = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_BV2:
      {
        quot.mbl_buy_qty[1] = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_BV3:
      {
        quot.mbl_buy_qty[2] = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_BV4:
      {
        quot.mbl_buy_qty[3] = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_BV5:
      {
        quot.mbl_buy_qty[4] = atoi(m_tmp_out[i].c_str());
        break;
      }

      case Id_SP1:
      {
        quot.mbl_sell_price[0] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_SP2:
      {
        quot.mbl_sell_price[1] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_SP3:
      {
        quot.mbl_sell_price[2] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_SP4:
      {
        quot.mbl_sell_price[3] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_SP5:
      {
        quot.mbl_sell_price[4] = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_SV1:
      {
        quot.mbl_sell_qty[0] = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_SV2:
      {
        quot.mbl_sell_qty[1] = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_SV3:
      {
        quot.mbl_sell_qty[2] = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_SV4:
      {
        quot.mbl_sell_qty[3] = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_SV5:
      {
        quot.mbl_sell_qty[4] = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_Recvtime:
      {
        quot.recv_time = atoll(m_tmp_out[i].c_str());
        break;
      }
      case Id_limit_up:
      {
        quot.limit_up = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_limit_down:
      {
        quot.limit_down = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_turnover:
      {
        quot.turnover = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_open:
      {
        quot.open = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_close:
      {
        quot.close = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_pre_close:
      {
        quot.pre_close = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_market:
      {
        quot.market = m_tmp_out[i];
        break;
      }

      case Id_high:
      {
        quot.high = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_low:
      {
        quot.low = atof(m_tmp_out[i].c_str());
        break;
      }
      case Id_timems:
      {
        quot.mstime = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_multi:
      {
        quot.multi = atoi(m_tmp_out[i].c_str());
        break;
      }
      case Id_variety:
      {
        quot.variety = m_tmp_out[i];
        break;
      }

      default:
      {
        log_fata("quot_file::read_line not support id={}\n",id);
        // printf(" quot_file::read_line not support quot file\n");
        break;
      }
      }
      
    }
    return true;
  }
}
