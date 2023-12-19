#include "sq_memory_db.h"
#include "os/sq_path.h"
namespace sq
{
    sq_memory_db::sq_memory_db()
    {
        m_contract_table = new sq_contract_table();
    }
    sq_memory_db::~sq_memory_db()
    {
        safe_delete(m_contract_table);
    }
    
}