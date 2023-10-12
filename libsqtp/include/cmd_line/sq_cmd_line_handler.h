#pragma once
#include <stdio.h>
#if SUPPORT_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

namespace sq
{
    /**
     * @brief 控制台命令处理器
    */
    class sq_cmd_line_handler
    {

    public:
        sq_cmd_line_handler(/* args */);
        ~sq_cmd_line_handler();
       
         void run();
         virtual  void handle_cmd(const char* cmd){
            printf("cmd=%s\n",cmd);
         };
        /* data */
    };
    
    
    
}