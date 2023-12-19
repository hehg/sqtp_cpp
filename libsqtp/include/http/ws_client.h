#pragma once
#include "http/websocket.h"
#include "http/ws_channel.h"
namespace sq
{
    /**
     * @brief websocket 客户端
     *  目前只维护了一个 ws channel
    */
    class ws_client 
    {
    public:
        ws_client(on_ws_channel_msg_fun_t on_msg,
			on_ws_channel_connected_fun_t on_connected = nullptr,
			on_ws_channel_disconnected_fun_t on_disconnected = nullptr,
			event_reactor *r = nullptr);

        int open(const char *server_ip, const char *url);

        void close();
        
        void add_head(const char *key, const char *val);
        /** 发送ws格式的normal 数据
         * @return 0 成功 其它 失败
        */
        int send_ws_message(const char *msg, int size);
    protected:
        ws_channel* add_ws_channel(const char* server_address);
        /** 收到消息处理函数
         * @return 处理的消息大小
        */
        int on_message(void *from,void *package, int size);

        /** 握手结束后使用此函数处理ws message
         * @return 0--成功， 其它 失败
        */
        int on_ws_message(ws_channel*ch,const char *data, int size);


        std::map<string, string> m_heads;
       
        on_ws_channel_msg_fun_t             m_on_ws_channel_msg=nullptr;
        on_ws_channel_connected_fun_t       m_on_ws_channel_connected=nullptr;
        on_ws_channel_disconnected_fun_t    m_on_ws_channel_disconnected=nullptr;
        event_reactor *                     m_reactor = nullptr;
        ws_channel*                         m_ws_channel = nullptr; 
        //string                              m_server_ip;
    };
}