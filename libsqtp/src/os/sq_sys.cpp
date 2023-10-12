#include "os/sq_sys.h"

namespace sq
{

	int get_pid()
	{
#ifdef WINDOWS
		return static_cast<int>(::GetCurrentProcessId());
#else
		return static_cast<int>(::getpid());
#endif
	}
	int get_thread_id()
	{
#ifdef WINDOWS
		 return ::GetCurrentThreadId();
#elif defined(LINUX)
		return ::syscall(SYS_gettid);
#else
		 return 0;
#endif
	}

	 void bind_proc_to_cpu(int cpu_index)
	 {
#if defined(WINDOWS)||defined(Cygwin)||defined(__APPLE__)
#else
		 cpu_set_t mask;
		 CPU_ZERO(&mask);
		 CPU_SET(cpu_index, &mask);
		 if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
			 perror("sched_setaffinity");
		 }
#endif
	 }

	 void bind_thread_to_cpu(int cpu_index)
	 {
#if defined(WINDOWS)||defined(Cygwin)||defined(__APPLE__)
#else
		 cpu_set_t mask;
		 CPU_ZERO(&mask);
		 CPU_SET(cpu_index, &mask);
		 pthread_t thread;

		 thread = pthread_self();
		 if (pthread_setaffinity_np(thread, sizeof(mask), &mask) < 0) {
			 perror("sched_setaffinity");
		 }
#endif
	 }

	 void bind_thread_to_cpu(thread &th, int cpu_index)
	 {

#if defined(WINDOWS) || defined(Cygwin) || defined(__APPLE__)
#else
		 cpu_set_t mask;
		 CPU_ZERO(&mask);
		 CPU_SET(cpu_index, &mask);
		 pthread_t thread;

		 thread = th.native_handle();
		 if (pthread_setaffinity_np(thread, sizeof(mask), &mask) < 0)
		 {
			 perror("sched_setaffinity");
		 }
#endif
	 }
}