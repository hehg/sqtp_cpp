#pragma once
#include "http/websocket.h"

namespace sq
{
    using on_ws_message_fun_t = std::function<int(const char*package, int size)>;
    class ws_client : public tcp_client
    {
    public:
        ws_client();
        int open(const char *server_ip, const char *url);
        void add_head(const char *key, const char *val);
        /** 发送ws格式的normal 数据
         * @return 0 成功 其它 失败
        */
        int send_ws_message(const char *msg, int size);
        ws_client& reg_on_ws_msg(on_ws_message_fun_t f){
            m_on_ws_msg=f;
            return *this;
        }
    protected:
        int send_ws_package(uint8_t opcode, const char *data, uint32_t size, bool fin = true);
        /** 收到消息处理函数
         * @return 处理的消息大小
        */
        int on_message(void *package, int size, void *from);

        /** 握手结束后使用此函数处理ws message
         * @return 0--成功， 其它 失败
        */
        int on_ws_message(const char *data, int size);


        std::map<string, string> m_heads;
        int         m_state = 0;
        string      shake_line;
        char        tmp_buf[4086];
        on_ws_message_fun_t m_on_ws_msg=nullptr;
    };
}