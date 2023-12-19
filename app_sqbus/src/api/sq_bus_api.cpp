#include "sq_bus_api.h"
#include <thread>
#include "net/reactor.h"
#include "http/ws_client.h"
#include "pack/sq_package.h"
#include "../base/bus_message.h"
#include "log/sq_logger.h"
using namespace sq;
using namespace bus;
namespace bus
{
    namespace api
    {
        static std::thread *s_api_thread = nullptr;
        static sq::sq_reactor *s_api_reactor = nullptr;
        static sq::ws_client *s_api_ws_client = nullptr;
        static bus_api_rsp *s_api_rsp = nullptr;

        void init_api(bus_api_rsp *rsp)
        {
            s_api_rsp = rsp;
            s_api_reactor = new sq::sq_reactor();
            s_api_thread = new std::thread([&]()
                                           { s_api_reactor->loop(); });

            s_api_ws_client = new sq::ws_client(
                [](ws_channel *channel, void *msg, int size) -> int
                {
                    if(s_api_rsp)
                    {
                        //解析数据
                        sq_package*p=sq_package::create((char*)msg,size);
                        if (p)
                        {
                            sq_pack_header*hdr=p->get_header();
                            if(hdr->topic>0)
                            {
                                field_info * info=p->get_field_info(0);
                                if (info)
                                {
                                    int body_size = info->size;
                                    char *body = info->data;
                                    s_api_rsp->on_topic(hdr->topic, body, body_size);
                                }
                            }
                            //s_api_rsp->on_message((const char *)msg, size);
                            delete p;
                        }
                    }
                    return size;
                },
                [](ws_channel *channel)
                {
                    if (s_api_rsp)
                    {
                        s_api_rsp->on_connected();
                    }
                },
                [](ws_channel *channel)
                {
                    if (s_api_rsp)
                    {
                        s_api_rsp->on_disconnected();
                    }
                },
                s_api_reactor);
        }

        /**
         * @brief 建立连接并握手
         * @note 异步处理
        */
        int connect(const char *address)
        {
            return s_api_ws_client->open(address, "/");
        }
        void disconnect()
        {
            s_api_ws_client->close();
        }

        int send_to_bus(unsigned short tid, unsigned int topic, const char *data, int size)
        {

            sq_package *pack = sq_package::create(tid, topic);
            char *d = pack->alloc_field(0, size);
            if (d)
            {
                memcpy(d, data, size);
            }
            return s_api_ws_client->send_ws_message(pack->get_data(), pack->get_size());
        }
        int publish(unsigned int topic, const char *data, int size)
        {
            sq_package *pack = sq_package::create(bus_tid::bus_tid_pub_topic, topic);
            char *d = pack->alloc_field(0, size);
            if (d)
            {
                memcpy(d, data, size);
            }
            int ret = s_api_ws_client->send_ws_message(pack->get_data(), pack->get_size());
            delete pack;
            return ret;
        }
        int subscribe(unsigned int topic)
        {
            sq_package *pack = sq_package::create(bus_tid::bus_tid_sub_topic, 0);
            bus_subcribe_req *d = pack->alloc_field<bus_subcribe_req>();
            if (d)
            {
                d->topic = topic;
            }
            else{
                
                log_warn("subscribe topic size is too large\n");
                delete pack;
                return -1;
            }
            int ret = s_api_ws_client->send_ws_message(pack->get_data(), pack->get_size());
            delete pack;
            return ret;
        }
    }
}