#include "plug_base.h"
#include <thread>
#include <set>
#include <atomic>
/**
 * @brief
*/
namespace sq_plug
{
    /**
     * @brief 风控插件
     * @details
     *   1 最大撤单量监控
     *   2 自成交监控
    */
    class plug_bz_risk_imp : public plug_base
    {
    public:
        plug_bz_risk_imp();
        ~plug_bz_risk_imp();
        int open();
        int close();
       
        int put(uint16_t tid, char* data, uint16_t size);
        
private:
        int m_cfg_cancel_max_count=1000;    //最大允许取消次数，默认1000
        int m_cancel_order_count=0;         //撤单次数
    };
}