#include "os/sq_shm.h"
#include "error/sq_err.h"
#include "os/sq_path.h"
namespace sq
{
    int sq_file_maping::open(string path, uint64_t size)
    {
        m_path = path;
        m_size = size;
        
        if (sq_path::exist(path.c_str()) == false ) {
            m_fd = ::open(path.c_str(), O_RDWR|O_CREAT, 00777);
            if (m_fd == -1)
                return err_fail;
            lseek(m_fd, 0, SEEK_SET);
            char	*bb;
            bb = (char *)malloc(size);
            if (bb == 0) {
                printf("No enough memory for zero share memory\n");
                ::close(m_fd);
                return err_fail;
            }
            memset(bb, 0, size);       // should zero memory
            lseek(m_fd, 0, SEEK_SET);
            write(m_fd, bb, size);
            free(bb);
           // ::close(m_fd);
        }
        else
        {
            m_fd = ::open(path.c_str(), O_RDWR, 00777);
            if (m_fd == -1)
                return err_fail;
        }
#ifdef WINDOWS
		map_addr();
#else
        m_base_addr = mmap(0, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
#endif
		if (m_base_addr == nullptr)
			return err_fail;
        
        return ok;

    }
#ifdef WINDOWS
    void* sq_file_maping::map_addr()
    {
        SECURITY_ATTRIBUTES     attrs;
        memset(&attrs, 0, sizeof(attrs));
        attrs.nLength = sizeof(attrs);
        m_file_hand = CreateFile(m_path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &attrs,
            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_file_hand == NULL) {
            return nullptr;
        }
        LARGE_INTEGER mysize;
        mysize.HighPart = m_size >> 32;
        mysize.LowPart = (DWORD)m_size;

        m_map_hand = CreateFileMapping(m_file_hand, NULL, PAGE_READWRITE,
            0, m_size, m_path.c_str());
        if (m_map_hand == NULL) {
            int lasterr = GetLastError();
            CloseHandle(m_file_hand);
          
            return nullptr;
        }

        m_base_addr = MapViewOfFile(m_map_hand, FILE_MAP_ALL_ACCESS,0,  0, m_size);
        if (m_base_addr == NULL) {
            DWORD e = GetLastError();
            CloseHandle(m_map_hand);
            if (m_file_hand != NULL) CloseHandle(m_file_hand);
            m_file_hand = m_map_hand = NULL;
            return nullptr;
        }
        return m_base_addr;
    }

    void sq_file_maping::close()
    {
        UnmapViewOfFile(m_base_addr);
        CloseHandle(m_file_hand);
        CloseHandle(m_map_hand);
    }
#else
    
    void sq_file_maping::close()
    {
        munmap((caddr_t)m_base_addr, m_size);
        ::close(m_fd);
    }
#endif
	//=================sq_shm_maping============================================
    int sq_shm_maping::open(string name, uint64_t size)
    {
        m_size = size;
        m_path = name;
#ifdef WINDOWS
        m_map_hand = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, name.c_str());
        if (m_map_hand == NULL) {
            DWORD e=GetLastError();
            m_map_hand = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name.c_str());
            if (m_map_hand == NULL)
                return err_fail;
        }
#else
        m_fd = shm_open(name.c_str(), O_RDWR, 0);
        if (m_fd == -1) {
            m_fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 00600);
            if (m_fd == -1) {
                perror("shm_open");
                return err_fail;
            }
        }
#endif
		 map_addr();
		 if (m_base_addr == nullptr)
			 return err_fail;
        return ok;
   }
#ifdef WINDOWS
    void* sq_shm_maping::map_addr()
    {
        m_base_addr = ::MapViewOfFile(m_map_hand, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        return m_base_addr;
    }
    void sq_shm_maping::close()
    {
        UnmapViewOfFile(m_base_addr);
        CloseHandle(m_map_hand);

    }
#else
    void* sq_shm_maping::map_addr()
    {
        m_base_addr = mmap(0, m_size,  PROT_WRITE, MAP_SHARED, m_fd, SEEK_SET);
	return m_base_addr;
    }
    void sq_shm_maping::close()
    {
        munmap((caddr_t)m_base_addr, m_size);
        shm_unlink(m_path.c_str());
    }
#endif
}
