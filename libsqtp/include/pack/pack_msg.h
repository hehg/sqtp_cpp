#pragma once
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include "pack/sq_vint.h"
#include "container/byte_array.h"
namespace sq
{
   /** 
    * @brief 序列化 二进制,
    * 将各种信息已二进制的形式序列化到一个 buffer 中
   */

   class sq_pack
   {
   public:
      sq_pack(char *buf, int size)
      {
         m_buf = buf;
         m_w = m_buf;
         m_capacity = size;
      }
      void reset(){
         m_w = m_buf;
      }
      //固定大小
      template <typename T>
      sq_pack &operator<<(const T val)
      {
         assert(sizeof(T) <= remain());
         T *p = (T *)m_w;
         *p = val;
         m_w += sizeof(T);
         return *this;
      }
      //动态大小
      sq_pack &operator<<(const char *val)
      {
         int len = strlen(val);
         assert(len+sizeof(int) <= remain());
         *this << len;
         strncpy(m_w, val, len);
         m_w += len;
         return *this;
      }
      //动态大小
      sq_pack &operator<<(const std::string &val)
      {
         int len = val.length();
         assert(len+sizeof(int) <= remain());
         *this << len;
         strncpy(m_w, val.c_str(), len);
         m_w += len;
         return *this;
      }
      //动态大小
      template <typename T>
      sq_pack &operator<<(const std::vector<T> &val)
      {
         int len = val.length();
         assert(len+sizeof(int) <= remain());
         *this << len;
         for (const auto a : val)
         {
            *this << a;
         }
         return *this;
      }
      //固定大小
      sq_pack &pack_bin(const char *val, int len)
      {
         assert(len <= remain());
         strncpy(m_w, val, len);
         m_w += len;
         return *this;
      }
      //===固定大小==byte_array=====
      sq_pack &operator<<(byte_array<5> &val)
      {
         int len = val.size();
         assert(len <= remain());
         memcpy(m_w, val.data(), len);
         m_w += len;
         return *this;
      }
      sq_pack &operator<<(byte_array<7> &val)
      {
         int len = val.size();
         assert(len <= remain());
         memcpy(m_w, val.data(), len);
         m_w += len;
         return *this;
      }
      sq_pack &operator<<(byte_array<9> &val)
      {
         int len = val.size();
         assert(len <= remain());
         memcpy(m_w, val.data(), len);
         m_w += len;
         return *this;
      }
      sq_pack &operator<<(byte_array<13> &val)
      {
         int len = val.size();
         assert(len <= remain());
         memcpy(m_w, val.data(), len);
         m_w += len;
         return *this;
      }
      sq_pack &operator<<(byte_array<17> &val)
      {
         int len = val.size();
         assert(len <= remain());
         memcpy(m_w, val.data(), len);
         m_w += len;
         return *this;
      }
      sq_pack &operator<<(byte_array<16> &val)
      {
         int len = val.size();
         assert(len <= remain());
         memcpy(m_w, val.data(), len);
         m_w += len;
         return *this;
      }
      sq_pack &operator<<(byte_array<61> &val)
      {
         int len = val.size();
         assert(len <= remain());
         memcpy(m_w, val.data(), len);
         m_w += len;
         return *this;
      }
      sq_pack &operator<<(byte_array<65> &val)
      {
         int len = val.size();
         assert(len <= remain());
         memcpy(m_w, val.data(), len);
         m_w += len;
         return *this;
      }
      sq_pack &operator<<(byte_array<129> &val)
      {
         int len = val.size();
         assert(len <= remain());
         memcpy(m_w, val.data(), len);
         m_w += val.size();
         return *this;
      }
      //===固定大小==byte_array end =====
      sq_pack &operator<<(const sq_vint &val)
      {
         int len = vint_encode(val.val_, m_w);
         assert(len <= remain());
         m_w += len;
         return *this;
      }
      inline int remain() { return m_capacity - (m_w - m_buf); }
      inline int size() { return m_w - m_buf; }

      char *m_w = nullptr;
      char *m_buf = nullptr;
      int m_capacity = 0;
   };

   /** 
    * @brief 反序列化 二进制
   */
   class sq_unpack
   {
   public:
      sq_unpack(char *buf, int size)
      {
         m_r = buf;
         m_buf = buf;
      }
      template <typename T>
      sq_unpack &operator>>(T &val)
      {
         T *p = (T *)m_r;
         val = *p;
         m_r += sizeof(T);
         return *this;
      }
      //动态大小
      sq_unpack &operator>>(char *val)
      {
         int len = 0;
         *this >> len;
         strncpy(val, m_r, len);
         m_r += len;
         return *this;
      }
      sq_unpack &operator>>(std::string &val)
      {
         int len = 0;
         *this >> len;
         val.append(m_r, len);
         m_r += len;
         return *this;
      }
      //固定大小
      sq_unpack &unpack_bin(char *val, int len)
      {
         strncpy(val, m_r, len);
         m_r += len;
         return *this;
      }
      template <typename T>
      sq_unpack &operator>>(std::vector<T> &val)
      {
         int len = 0;
         *this >> len;
         for (int i = 0; i < len; i++)
         {
            T a;
            *this >> a;
            val.push_back(a);
         }
         return *this;
      }

      sq_unpack &operator>>(sq_vint &val)
      {
         int len = vint_decode(val.val_, m_r);
         m_r += len;
         return *this;
      }
      int size() { return m_r - m_buf; }

      char *m_buf = nullptr;
      char *m_r = nullptr;
   };

}