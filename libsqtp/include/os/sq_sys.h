#pragma once
#include "sq_define.h"

namespace sq
{
    extern int get_pid();
    extern int get_thread_id();
    ///将本进程绑定到cpu
    extern void bind_proc_to_cpu(int cpu_index);
    ///将本线程绑定到cpu
    extern void bind_thread_to_cpu(int cpu_index);
    ///将本线程绑定到cpu
    extern void bind_thread_to_cpu(thread &th, int cpu_index);
}