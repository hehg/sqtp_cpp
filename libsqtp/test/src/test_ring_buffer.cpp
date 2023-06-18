#include "unit_test.h"
#include "container/ring_buffer.h"
#include <thread>
#pragma pack(push, 1)
struct my_message1
{
    int tid;
    char name[16];
    double size;
};

struct my_message2
{
    int tid;
    char err_msg[60];
    int err_code;
    char name[16];
    double size;
};
#pragma pack(pop)
#if 1
SQ_TEST(test_ring_buf_var)
{
    std::cout << sizeof(my_message2) << std::endl;
    sq::ring_buffer_var<1024> rb;
    for (int i = 0; i < 50; )
    {
        sq::MsgHeader*hdr=(sq::MsgHeader*)rb.alloc(sizeof(my_message2));
        if (hdr)
        {
            // assert(hdr);
             my_message2 *mg = (my_message2 *)rb.alloc(sizeof(my_message2));
            //  my_message2*mg=( my_message2*)((char*)hdr)+sizeof( sq::MsgHeader);
            mg->err_code = i;
            sprintf(mg->err_msg, "err%d", i);
            mg->size = i;
            sprintf(mg->name, "name%d", i);

            //std::cout << "idx=" << i << "size=" << hdr->size;
            //std::cout<<"write_idx="<<rb.write_idx<<std::endl;
            rb.push();
            i++;

        }
        else{
            //sq::MsgHeader *hdr2=rb.front();
            my_message2 *mg2 = (my_message2 *)rb.front();
            printf("name=%s\n",mg2->name);
            // SQ_EXPECT_EQ(mg->err_code, mg2->err_code);
            //  SQ_EXPECT_EQ(mg->size, mg2->size);
            //  SQ_EXPECT_STREQ(mg->err_msg, mg2->err_msg);
            //  SQ_EXPECT_STREQ(mg->name, mg2->name);
            rb.pop();
        }


        

    }
}
#endif
#if 1
SQ_TEST(test_ring_buf_var2)
{
    std::cout << sizeof(my_message2) << std::endl;
    sq::ring_buffer_var<1024> rb;
    thread t1([&]() {
        for (int i = 0; i <10000; i++)
        {
            my_message2 *mg = (my_message2 *)rb.alloc(sizeof(my_message2));
            //my_message2*mg=( my_message2*)((char*)hdr)+sizeof( sq::MsgHeader);
            mg->err_code = i;
            sprintf(mg->err_msg, "err%d", i);
            mg->size = i;
            sprintf(mg->name, "name%d", i);

            std::cout << "idx=" << i << std::endl;
            std::cout<<"write_idx="<<rb.write_idx<<std::endl;
            rb.push();
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    });
   // t1.join();
    #if 1
    thread t2([&]() {
        for (int i = 0; i < 10000;)
        {
            my_message2 *mg2 = (my_message2 *)rb.front();
            if (mg2)
            {
                SQ_EXPECT_EQ(i, mg2->err_code);
                SQ_EXPECT_EQ(i, mg2->size);
                std::cout<<"name="<<mg2->name<<std::endl;
                rb.pop();
                i++;
            }
        }
    });
    t1.join();
    t2.join();
    #endif
}
#endif