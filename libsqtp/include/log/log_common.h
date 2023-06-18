#pragma once
#include <chrono>
#include <memory>
#include <string.h>

namespace sq {
    class log_target;
    using log_clock = std::chrono::system_clock;
   
    using log_target_ptr = std::shared_ptr<log_target>;
    typedef enum
    {
        log_off=0,
        log_fata=1,
        log_err=2,
        log_warn=3,
        log_info=4,
        log_debug=5,
    } level_enum;

    static const char* level_names[]{ "","fata", "error", "warn",  "info", "debug" };

    inline const char* to_level_name(level_enum l)
    {
        return level_names[l];
    }
    inline int to_log_level(const char *name)
    {
        if(strcmp(name,"fata")==0){
            return log_fata;
        }
        else if(strcmp(name,"error")==0){
            return log_err;
        }
        else if(strcmp(name,"warn")==0){
            return log_warn;
        }
        else if(strcmp(name,"info")==0){
            return log_info;
        }
        else if(strcmp(name,"debug")==0){
            return log_debug;
        }
        else{
            return 0;
        }
    }
    class null_mutex
    {
    public:
        null_mutex() = default;
        void lock() {}
        void unlock() {}
    };
}