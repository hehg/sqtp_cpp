#pragma once
#include "sq_define.h"
namespace sq
{
  
    template <typename T>
    size_t vint_encode(T n,char*buff)
    {
        char*p=buff;
        int64_t x=n;
        x=(x<<1)^(x>>63);
        uint64_t xx=x;
        while (xx&~0x7f)
        {
            *p++=(xx&0x7f)|0x80;
            xx>>=7;
        }
        *p++=xx;
        return p-buff;
    }

    template<typename T>
    size_t vint_decode(T&n,const char*buff)
    {
        int64_t ret=0;
        size_t shift=0;
        const char*p=buff;

        while (shift<sizeof(int64_t)*8)
        {
            ret|=static_cast<int64_t>((*p)&0x7f)<<shift;
            if(((*p)&0x80)!=0x80){
                break;
            }
            ++p;
            shift+=7;
        }
        n=static_cast<T> ((static_cast<uint64_t>(ret)>>1)^(-(ret&1)));
        return p-buff+1;
    }

#if 1
    class sq_vint
    {
    public:
       sq_vint(const sq_vint& a){
           val_=a.val_;
       }
       sq_vint(){
           val_=0;
       }
       sq_vint(int32_t a){
           val_=a;
       }
       sq_vint(uint32_t a){
           val_=a;
       }
       sq_vint(int64_t a){
           val_=a;
       }
       sq_vint(uint64_t a){
           val_=a;
       }
        bool operator ==(const sq_vint &t){
            return t.val_==val_;
        }
         uint64_t val_;
    };

    
    #endif
}

namespace std
{
   inline std::ostream& operator <<(std::ostream& out,sq::sq_vint& a)
    {
        out<<a.val_;
        return out;
    }
}