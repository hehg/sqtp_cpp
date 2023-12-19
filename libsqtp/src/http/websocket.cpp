#include "http/websocket.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "time/date_time.h"
#include "error/sq_err.h"
#include "sq_macro.h"
#include "log/sq_logger.h"
namespace sq
{
// #define htobe16(v) htons(v)
// #define htobe64(v) htonll(v)
// #define be16toh(v) ntohs(v)
// #define be64toh(v) ntohll(v)
#ifndef __linux__
#define be32toh(v) ntohl(v)
#endif 
    // #define MIN(a, b) (((a) < (b)) ? (a) : (b))
    void ws_header::dump(std::ostream &os)
    {
        os<<"ws_hdr=";
        os << "fin: " << (fin ? "1" : "0") \
        << " opcode: " << opcode << " masked: " \
        << (masked ? "1" : "0") << " payloadlen: " << (int)payloadlen << std::endl;
    }
    inline void hex_print(const char *data, int size)
    {
        for (int i = 0; i < size; i++)
        {
            printf("%02x", data[i]);
        }
        printf("\n");
    }
    /**
     * 基于websocket 协议解析数据
     0                   1                   2                   3
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/63)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+
    */

    
    ws_package parser_websocket_msg(const unsigned char *data, int len)
    {
        ws_package result;
        // const char*p=data;
        int head_size = sizeof(ws_header);
        ws_header *hdr = (ws_header *)data;
        // opecode
        result.op_code = hdr->opcode;
        // fin
        if (hdr->fin == 0)
        {
            result.is_finish = false;
        }
        else
        {
            result.is_finish = true;
        }
        // body size
        if (hdr->payloadlen <= 125)
        {
            result.body_size = hdr->payloadlen;
        }
        if (hdr->payloadlen == 126)
        {
            uint16_t size = *(uint16_t *)(data + sizeof(ws_header));
            result.body_size = ntohs(size);
            head_size += 2;
        }
        else if (hdr->payloadlen == 127)
        {
            uint64_t size = *(uint64_t *)(data + sizeof(ws_header));
            result.body_size = ntohll(size);
            head_size += 8;
        }
        // mask key
        mask_key_t *mask=nullptr;
        if (hdr->masked == 1)
        {
            char *key = (char *)(data + head_size);
            mask= (mask_key_t *)key;
            //memcpy(mask.char_array, key, sizeof(mask_key_t));
            head_size += sizeof(mask_key_t);
        }
        
        //检查是否为完整数据包
        if(len<head_size+result.body_size)
        {
            hdr->dump(std::cout);
            log_info("it isn't a full websocket msg,len={},head_size={},body_size={}\n",len,head_size,result.body_size);
            result.result=err_again;
            return result;
        }
        result.pkg_size=head_size+result.body_size;
        result.body = (char *)(data + head_size);
       
        // unmask
        if (hdr->masked == 1)
        {
            char *body = result.body;
            // hex_print(body,body_size);
            for (uint32_t i = 0; i < result.body_size; i++)
            {
                body[i] = body[i] ^ mask->char_array[i % 4];
            }
        }
        result.result=ok;
        return result;
    }

    static uint32_t rol(uint32_t value, uint32_t bits)
    {
        return (value << bits) | (value >> (32 - bits));
    }
    
    int make_ws_package(const char *data, uint32_t size, char *des_buf, int des_size,int mask)
    {
        //检查一下数据目标数据空间是否够用
        int result_size = sizeof(ws_header)+sizeof(mask_key_t)+size;
        if(size<125){
            //result_size
        }
        else if(size<=0xffff){
            result_size+=2;
        }
        else{
            result_size+=8;
        }
        if(des_size<result_size)
        {
            return -1;
        }
        //assert(result_size< des_size);

        
        ws_header *hdr = (ws_header *)des_buf;
        char *body = des_buf + sizeof(ws_header);

        hdr->fin = 1;
        hdr->rsv1 = 0;
        hdr->rsv2 = 0;
        hdr->rsv3 = 0;
        hdr->opcode = OPCODE_TEXT;

        hdr->masked = mask; // client must set as  1
        if (size <= 125)
        {
            hdr->payloadlen = size;
        }
        else if (size <= 0xFFFF)
        {
            hdr->payloadlen = 126;
            *(uint16_t *)(body) = htons(size);
            body += 2;
        }
        else
        {
            hdr->payloadlen = 127;
            *(uint64_t *)(body) = htonll(size);
            body += 8;
        }
        
        if (hdr->masked)
        {
            // set mask key
            mask_key_t mask;
            mask.integer = (uint32_t)(date_time::get_now_us() & 0xffff);
            *((uint32_t *)body) = mask.integer;
            body += sizeof(mask_key_t);

            for (uint32_t i = 0; i < size; i++)
            {
                *(body++) = data[i] ^ mask.char_array[i % 4];
            }
            // hex_print(body-size,size);
        }
        else
        {
            memcpy(body, data, size);
            body += size;
        }

        return result_size;
    }

    
}