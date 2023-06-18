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
#define be32toh(v) ntohl(v)
    // #define MIN(a, b) (((a) < (b)) ? (a) : (b))

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
            //log_info("it isn't a full websocket msg,len={},head_size={},body_size={}\n",len,head_size,result.body_size);
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
    // Be cautious that *in* will be modified and up to 64 bytes will be appended, so make sure in buffer is long enough
    uint32_t sha1base64(uint8_t *in, uint64_t in_len, char *out)
    {
        uint32_t h0[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
        uint64_t total_len = in_len;
        in[total_len++] = 0x80;
        int padding_size = (64 - (total_len + 8) % 64) % 64;
        while (padding_size--)
            in[total_len++] = 0;
        for (uint64_t i = 0; i < total_len; i += 4)
        {
            uint32_t &w = *(uint32_t *)(in + i);
            w = be32toh(w);
        }
        *(uint32_t *)(in + total_len) = (uint32_t)(in_len >> 29);
        *(uint32_t *)(in + total_len + 4) = (uint32_t)(in_len << 3);
        for (uint8_t *in_end = in + total_len + 8; in < in_end; in += 64)
        {
            uint32_t *w = (uint32_t *)in;
            uint32_t h[5];
            memcpy(h, h0, sizeof(h));
            for (uint32_t i = 0, j = 0; i < 80; i++, j += 4)
            {
                uint32_t &a = h[j % 5], &b = h[(j + 1) % 5], &c = h[(j + 2) % 5], &d = h[(j + 3) % 5], &e = h[(j + 4) % 5];
                if (i >= 16)
                    w[i & 15] = rol(w[(i + 13) & 15] ^ w[(i + 8) & 15] ^ w[(i + 2) & 15] ^ w[i & 15], 1);
                if (i < 40)
                {
                    if (i < 20)
                        e += ((b & (c ^ d)) ^ d) + 0x5A827999;
                    else
                        e += (b ^ c ^ d) + 0x6ED9EBA1;
                }
                else
                {
                    if (i < 60)
                        e += (((b | c) & d) | (b & c)) + 0x8F1BBCDC;
                    else
                        e += (b ^ c ^ d) + 0xCA62C1D6;
                }
                e += w[i & 15] + rol(a, 5);
                b = rol(b, 30);
            }
            for (int i = 0; i < 5; i++)
                h0[i] += h[i];
        }
        const char *base64tb = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        uint32_t triples[7] = {h0[0] >> 8,
                               (h0[0] << 16) | (h0[1] >> 16),
                               (h0[1] << 8) | (h0[2] >> 24),
                               h0[2],
                               h0[3] >> 8,
                               (h0[3] << 16) | (h0[4] >> 16),
                               h0[4] << 8};
        for (uint32_t i = 0; i < 7; i++)
        {
            out[i * 4] = base64tb[(triples[i] >> 18) & 63];
            out[i * 4 + 1] = base64tb[(triples[i] >> 12) & 63];
            out[i * 4 + 2] = base64tb[(triples[i] >> 6) & 63];
            out[i * 4 + 3] = base64tb[triples[i] & 63];
        }
        out[27] = '=';
        return 28;
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