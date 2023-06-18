#include "plug_base.h"
#include <thread>
#include <set>
#include <atomic>
#include "net/net_socket.h"
namespace sq_plug
{

    class bz_md_post : public plug_base
    {
    public:
        bz_md_post();
        ~bz_md_post();
        int open();
        int close();
        void run();
        int put(uint16_t tid, char* data, uint16_t size);

private:
        sq::udp_socket* m_udp_socket=nullptr;

       // std::thread *m_engine = nullptr;
        //std::atomic<bool> run_flag_ = false;

        std::set<string> m_support_symbol;
        std::set<string> m_support_variety;
        int m_support_trade_type;
        std::set<int> m_support_tid;

    };
}