#pragma once
#include <stdint.h>
#include <map>
#include <string>
#include "net/tcp_client.h"

using namespace std;
namespace sq
{
#pragma pack(push, 1)
    struct ws_header
    {
        uint8_t opcode : 4; // 操作码
        uint8_t rsv3 : 1;   // 保留字段
        uint8_t rsv2 : 1;
        uint8_t rsv1 : 1;
        uint8_t fin : 1; // 结束标识
        /*报文长度
        payloadlen<126 :代表报文长度
        payloadlen==126:接下来的2个字节，代表长度
        payloadlen==124:接下来的4个字节，代表长度
        */
        uint8_t payloadlen : 7;
        // 是否加密
        uint8_t masked : 1;  // 0--不使用掩码 1--使用掩码
    };

#pragma pack(pop)
    static const uint8_t OPCODE_CONT = 0;  //continue frame
    static const uint8_t OPCODE_TEXT = 1;  //utf-8 数据
    static const uint8_t OPCODE_BINARY = 2;
    //3~7  保留暂未使用  可以 扩展使用
    static const uint8_t OPCODE_CLOSE = 8;
    static const uint8_t OPCODE_PING = 9;
    static const uint8_t OPCODE_PONG = 0xa;
    //0xb~0xf  保留暂未使用  可以 扩展使用

    //4个字节的mask key,如果ws_header::masked==1 ,说明数据需要用mask 
    //key 来进行xor操作  才能得到真正的数据
    union mask_key_t
    {
        char char_array[4];
        uint32_t integer;
    };
    #define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
    
    struct ws_package{
        int     pkg_size    =0;
        int     body_size   =0;
        uint8_t op_code     =0;
        bool    is_finish   =false;
        char*   body        =nullptr;   //解析后的数据
        int     result      =0;         //解析结果
    };

    /** 
     * 构造ws 数据包
     * @return 数据包大小
    */
    int make_ws_package(const char *data, uint32_t size,char*des_buf,int des_size,
    int mask=1);
    /**
     * 解析websocket数据包 
     * @return ws_package
     */ 
    ws_package parser_websocket_msg(const unsigned char*data,int len);
    uint32_t sha1base64(uint8_t *in, uint64_t in_len, char *out);

    void hex_print(const char*data,int size);
}