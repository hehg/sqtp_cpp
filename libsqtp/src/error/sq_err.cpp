#include "error/sq_err.h"
#if defined(WINDOWS) 
#include <windows.h>
#include <strsafe.h>
#endif

namespace sq
{
	void sq_abort(const char *errmsg_)
	{
#if  defined(WINDOWS)

		//  Raise STATUS_FATAL_APP_EXIT.
		ULONG_PTR extra_info[1];
		extra_info[0] = (ULONG_PTR)errmsg_;
		RaiseException(0x40000015, EXCEPTION_NONCONTINUABLE, 1, extra_info);
#else
		(void)errmsg_;
		abort();
#endif
	}



    const char* get_last_error_info()
    {
#if 0
        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError();

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);

        // Display the error message and exit the process

        lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
            (lstrlen((LPCTSTR)lpMsgBuf)  + 40) * sizeof(TCHAR));
        StringCchPrintf((LPTSTR)lpDisplayBuf,
            LocalSize(lpDisplayBuf) / sizeof(TCHAR),
            TEXT("failed with error %d: %s"), dw, lpMsgBuf);
        //MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
        printf("%s\n", lpDisplayBuf);
        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
        //ExitProcess(dw);
#endif
        return std::strerror(errno);

    }

#define def_error_code(code) {code,#code}

	static std::map<int, const char *> s_err_map = {
		def_error_code(ok),
		def_error_code(err_fail),
		def_error_code(err_time_out),
		def_error_code(err_not_exist),
		def_error_code(err_repeat),
        def_error_code(err_invalid_package),
        def_error_code(err_invalid_param),
        def_error_code(err_again),
        def_error_code(err_sql_exec),
        def_error_code(err_db_connect),
        def_error_code(err_net_connect),
        def_error_code(err_invalid_net_address),
        def_error_code(err_load_data),
        def_error_code(err_no_data),
		//offer
		def_error_code(err_no_exit_order),
		def_error_code(err_no_login),
		def_error_code(err_no_enough_posi),
		def_error_code(err_repeat_order_id),
    };
    const char *get_err_name(int code)
    {
        auto it = s_err_map.find(code);
        if (it != s_err_map.end())
        {
            return it->second;
        }
        const static char *unkonw = "unknow";
        return unkonw;
    }
    void add_error(int code, const char *msg)
    {
        auto it = s_err_map.find(code);
        if (it == s_err_map.end())
        {
            s_err_map[code] = msg;
        }
        else{
            printf("have exits err code %d\n",code);
        }
    }
    void add_errors(std::map<int, const char *> errs)
    {
        for (auto const &a : errs)
        {
            add_error(a.first, a.second);
        }
    }

}