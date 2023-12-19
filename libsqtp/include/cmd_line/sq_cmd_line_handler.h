#pragma once
#include <stdio.h>
#ifdef SUPPORT_READLINE



namespace sq
{
    /**
     * @brief 控制台命令处理器 使用了 readline 库，需要提前安装
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
#endif