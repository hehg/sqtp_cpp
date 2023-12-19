#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#include "../api/sq_ts_api.h"
using namespace sq;
class my_rsp:public sq_ts_api_rsp
{
public:
     void on_qry_contract_list_rsp(sq_rsp_msg*rsp_msg,sq_contract*data)override
    {
       if(data){
        data->to_string(std::cout);
       }
    }
};
int main()
{
    my_rsp rsp;
    sq_ts_api_set_rsp(&rsp);
    int ret = sq_ts_api_connect("ws://127.0.0.1:5401");

    if (ret != 0)
    {
        printf("connect server error\n");
        return -1;
    }
    printf("connect server success\n");

    while (1)
    {
        printf("input command:");
        char cmd[100];
        scanf("%s", cmd);
        if (strcmp(cmd, "q") == 0)
        {
            break;
        }
        sq_ts_api_qry_contract_list("dce","m2305");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    sq_ts_api_disconnect();

    return 0;
}