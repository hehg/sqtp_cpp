#include "crypto/sq_crypto.h"

#if SUPPORT_OPENSSL
namespace sq
{
    int sq_idea::idea_en(const unsigned char *in, int in_len, unsigned char *out, int out_len,idea_key_st &key)
	{
		int count = 0;
		if (in_len % 8 == 0)
		{
			count = in_len / 8;
		}
		else
		{
			count = in_len / 8 + 1;
		}
		int i = 0;
		for (; i < count; i++)
		{
			idea_ecb_encrypt(in + i * 8, out + i * 8, &key);
		}
		int ret = i * 8;
		return ret;
	}

	//===========
	int sq_md5::encrypt(const unsigned char *input, int in_len, unsigned char *out, int out_len)
	{
		MD5_CTX ctx;
		assert(out_len >= 33);
		out[32] = 0;
		unsigned char md[16];
		char buf[32] = {0};
		char tmp[3] = {0};
		int i;

		MD5_Init(&ctx);
		MD5_Update(&ctx, input, in_len);
		MD5_Final(md, &ctx);

		for (i = 0; i < 16; i++)
		{
			sprintf(tmp, "%02X", md[i]);
			strcat(buf, tmp);
		}
		memcpy(out, buf, 32);
		return 32;
	}
} // namespace sq

#endif

namespace sq
{
	static uint32_t rol(uint32_t value, uint32_t bits) { return (value << bits) | (value >> (32 - bits)); }
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
        w = ntohl(w);
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

}