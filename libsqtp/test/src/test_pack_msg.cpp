#include "unit_test.h"
#include "pack/pack_msg.h"
#include "pack/sq_vint.h"
#include "container/byte_array.h"

#include <sstream>
using namespace sq;

typedef sq::byte_array<9> date_t;

namespace std
{
    std::ostream& operator <<(std::ostream& out,date_t& a)
    {
        out<<a.data();
        return out;
    }
}

#if 0
SQ_TEST(test_vint)
{
    char buf[1024]={0};
    sq_pack pack(buf,1024);
    
    {
        sq_vint a=2;
        pack<<a;
        int len=pack.size();
        printf("len=%d\n",len);
        SQ_EXPECT_EQ(len,1);
    }
    {
        sq_vint a=256;
         pack<<a;
        int len=pack.size();
        printf("len=%d\n",len);
        SQ_EXPECT_EQ(len,3);
    }

    {
        sq_vint a=65536;
       
         pack<<a;
        int len=pack.size();
        printf("len=%d\n",len);
        SQ_EXPECT_EQ(len,6);
    }

    // sq_unpack unpack(buf,1024);
    // {
    //     int a=0;
    //     int len=0;
    //     unpack.unpack_vint(a,len);
    //     SQ_EXPECT_EQ(a,2);
    //     SQ_EXPECT_EQ(len,1);
    // }

    // {
    //     int a=0;
    //     int len=0;
    //     unpack.unpack_vint(a,len);
    //     SQ_EXPECT_EQ(a,256);
    //     SQ_EXPECT_EQ(len,2);
    // }
    // {
    //     int a=0;
    //     int len=0;
    //     unpack.unpack_vint(a,len);
    //     SQ_EXPECT_EQ(a,65536);
    //     SQ_EXPECT_EQ(len,3);
    // }
}
#endif

struct fld_base
{
    uint16_t id;
    uint16_t size;
    int serialize(sq_pack&pack){
        int size = pack.size();
        pack<<id;
        pack<<size;
        return pack.size() - size;
    }
    int unserialize(char*buf,int size){
        sq_unpack unpack(buf,size);
        int s=unpack.size();
        unpack>>id;
        unpack>>size;
        return unpack.size()-s;
    }
    int unserialize(sq_unpack&unpack){
        int size=unpack.size();
        unpack>>id;
        unpack>>size;
        return size-unpack.size();
    }
};
struct fld_contract_ex:public fld_base
{
    date_t date;
    sq_vint contract_no=1;     //合约编号
    sq_vint contract_seq_no=3; //合约行情序号
    uint8_t mdg_no;          //产品组
    uint64_t seq_no;         //最新消息编号
    uint64_t send_time;      //发送时间戳

    
    int serialize(sq_pack&pack){
        int size = pack.size();
        fld_base::serialize(pack);
        pack<<date;
        pack<<contract_no;
        pack<<contract_seq_no;
        pack<<mdg_no<<seq_no<<send_time;
        return pack.size() - size;
    }
    int unserialize(char *buf, int size)
    {
        sq_unpack unpack(buf, size);
        int s = unpack.size();
        fld_base::unserialize(unpack);

        unpack >> date;
        unpack >> contract_no;
        unpack >> contract_seq_no;
        unpack >> mdg_no >> seq_no >> send_time;
        return unpack.size() - s;
    }
    int unserialize(sq_unpack&unpack){
        int size=unpack.size();
        fld_base::unserialize(unpack);
        unpack>>date;
        unpack>>contract_no;
        unpack>>contract_seq_no;
        unpack>>mdg_no>>seq_no>>send_time;
        return unpack.size()-size;
    }
    string to_string()
    {
        stringstream st;
        // st.precision(6);
        st << std::fixed;
        st<<date<<",";
        st<< contract_no<<","
        << contract_seq_no << ","
        << (int)mdg_no << ","
        << seq_no << ","
        << send_time;

        return st.str();
    }
};
SQ_TEST(serialize)
{
    fld_contract_ex ex;
    ex.id=1;
    ex.size=sizeof(fld_contract_ex);
    ex.date="20210908";
    ex.contract_no=12;
    ex.contract_seq_no=23;
    ex.mdg_no=1;
    ex.seq_no=12121212;
    ex.send_time=1234567;
    
    char buf[1024]={0};
    sq_pack pack(buf,1024);
    int size=ex.serialize(pack);
    printf("size=%d\n",size);

    sq_unpack unpack(buf,1024);
    fld_contract_ex ex2;
    //int size2= ex2.unserialize(unpack);
    int size2= ex2.unserialize(buf,1024);
    printf("size2=%d\n",size2);

    assert(size==size2);
    assert(ex2.seq_no==ex.seq_no);
    assert(ex2.date==ex.date);
    assert(ex2.contract_no.val_==ex.contract_no.val_);
    //assert(ex2.contract_seq_no==ex.contract_seq_no);
    assert(ex2.mdg_no==ex.mdg_no);
    assert(ex2.send_time==ex.send_time);

    std::string s=ex.to_string();
    printf("%s\n",s.c_str());

    std::string s1=ex2.to_string();
    printf("%s\n",s1.c_str());
    
}