#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sq_define.h"
#if SUPPORT_OPENSSL
#include <openssl/md5.h>
#include <openssl/idea.h>
//#include <crypto/sm4.h>
#endif
#include <assert.h>
#include <iostream>

namespace sq
{
#if SUPPORT_OPENSSL
    /**
     * 基于openssl 实现的对idea 的封装
    */
    class sq_idea
    {
    public:
        void set_key(const char *key)
        {
            idea_set_encrypt_key((const unsigned char *)key, &en_key);
            idea_set_decrypt_key(&en_key, &de_key);
        }
        int encrypt(const unsigned char *input, int in_len, unsigned char *out, int out_len)
        {
            int ret = idea_en((const unsigned char *)input, in_len, out, out_len, en_key);
            assert(ret > 0);
            return ret;
        }
        int decrypt(const unsigned char *input, int in_len, unsigned char *out, int out_len)
        {
            int ret = idea_en((const unsigned char *)input, in_len, out, out_len, de_key);
            assert(ret > 0);
            return ret;
        }

    private:
        int idea_en(const unsigned char *in, int in_len, unsigned char *out, int out_len, idea_key_st &key);

        idea_key_st en_key;
        idea_key_st de_key;
    };
    /**
     * 基于openssl 实现的对md5 的封装
    */
    class sq_md5
    {
        public:
         int encrypt(const unsigned char *input, int in_len, unsigned char *out, int out_len);

    };
    #endif

    uint32_t sha1base64(uint8_t *in, uint64_t in_len, char *out);

} // namespace sq
