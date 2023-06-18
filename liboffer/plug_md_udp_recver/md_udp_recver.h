#include "plug_base.h"
#include <thread>
#include <set>
#include <atomic>
#include "net/net_socket.h"
namespace sq_plug
{

    class md_udp_recver : public plug_base
    {
    public:
        md_udp_recver();
        ~md_udp_recver();
        int open();
        int close();
        void run();
        int put(uint16_t tid, char* data, uint16_t size);

private:
        sq::udp_socket* m_udp_socket=nullptr;
        bool m_run_flag=false;
        std::thread*m_engine=nullptr;
    };
}