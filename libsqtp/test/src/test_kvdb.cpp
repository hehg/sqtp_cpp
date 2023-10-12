#include "sq_unit_test.h"
#include "db/kv_db.h"
using namespace sq;

struct my_data
{
    int id;
    char name[12];
    double age;
};

SQ_TEST(test_kvdb)
{
    kv_table<int,my_data> tb;
    for(int i=0;i<100;i++){
        my_data data;
        data.id=i;
        tb.put(i,&data);
    }
    my_data*v=tb.begin();
    while (v)
    {
        std::cout<<"id="<<v->id<<std::endl;
        v=tb.next();
    }
    
    for( int i=0;i<100;i++){
       my_data*v= tb.get(i);
       //std::cout<<"id="<<v->id<<std::endl;
    }
}
#if 1
SQ_TEST(test_kvdb2)
{
    kv_table<int,char> tb;
    for(int i=0;i<100;i++){
        my_data data;
        data.id=i;
        tb.put(i,&data,sizeof(my_data));
    }
    for( int i=0;i<100;i++){
       my_data*v= (my_data*)tb.get(i);
       std::cout<<"id="<<v->id<<std::endl;
    }
    std::cout<<tb.size()<<std::endl;
}
#endif