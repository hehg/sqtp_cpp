
#include "plug_bz_risk_imp.h"
#include "log/sq_logger.h"
#include "sq_struct_def.h"
#include "sq_macro.h"
#include "sq_tid_define.h"
using namespace sq;
namespace sq_plug
{
    plug_bz_risk_imp::plug_bz_risk_imp()
    {
        m_name="plug_bz_risk_imp";
        
    }
    plug_bz_risk_imp::~plug_bz_risk_imp()
    {
        
    }
    int plug_bz_risk_imp::open()
    {
        plug_base::open();
        
          
        log_info("open plug plug_bz_risk_imp\n");
        SQ_LOG_FLUSH();
        return 0;
    }
    int plug_bz_risk_imp::close()
    {
        
        return 0;
    }
    int plug_bz_risk_imp::put(uint16_t tid, char *data, uint16_t size)
    {
        if(tid==tid_cancel_order){
            m_cancel_order_count++;
            if(m_cancel_order_count>=m_cfg_cancel_max_count){
                log_error("m_cancel_order_count={} reached\n",m_cancel_order_count);
                return -1;
            }
        }
        return ok;
    }
    
}