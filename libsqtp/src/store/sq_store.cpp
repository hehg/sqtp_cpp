#include "store/sq_store.h"
#include "store/sq_store_reader.h"
namespace sq
{
    int sq_store::add_reader(sq_store_reader*r)
    {
        m_reader_no++;
        m_readers[m_reader_no] = r;
        return m_reader_no;
    }
    void sq_store::remove_reader(int no)
    {
        m_readers.erase(no);
    }
    void sq_store::notiy_new_data(int32_t id, void*data, int size)
    {
        for (auto r : m_readers) {
            r.second->on_store_data(id, data, size);
        }
    }
}