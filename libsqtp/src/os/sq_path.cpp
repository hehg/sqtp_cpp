#include "os/sq_path.h"
#include "sq_define.h"
#include "text/sq_string.h"
namespace sq
{
	sq_path::sq_path(const char* path)
	{
		strcpy(m_path, path);
#ifdef WINDOWS
		_splitpath(m_path, m_driver, m_dir, m_name, m_ext);
#else
		char *dirc = strdup(m_path);
		char *basec = strdup(m_path);

		strcpy(m_dir, dirname(dirc));
		strcpy(m_name, basename(basec));
		free(dirc);
		free(basec);
#endif

	}

	sq_path::~sq_path()
	{
	}
	

	bool sq_path::mkdir(const char*dir)
	{
		if (access(dir, 0) == -1)
		{
			int ret = -1;
#ifdef WINDOWS
			ret = _mkdir(dir);
#else
			ret = ::mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			return true;
#endif
			return ret == 0;

		}
		else
			return true;
	}
#ifndef WINDOWS
	static int unlink_func(const char *path, const struct stat *sb, int type_flag, struct FTW *ftw)
	{
		if (remove(path) != 0)
		{
			int errcode = errno;
			printf("could not remove %s: %s", path, strerror(errcode));
		}

		return 0;
	}
#endif
	bool sq_path::rmdir(const char*dir)
	{
#ifdef WINDOWS
		SHFILEOPSTRUCT FileOp;
		FileOp.fFlags = FOF_NOCONFIRMATION;
		FileOp.hNameMappings = NULL;
		FileOp.hwnd = NULL;
		FileOp.lpszProgressTitle = NULL;
		FileOp.pFrom = dir;
		FileOp.pTo = NULL;
		FileOp.wFunc = FO_DELETE;
		int ret = SHFileOperation(&FileOp);
		return ret == 0;
#else
		nftw(dir, unlink_func, 64, FTW_DEPTH | FTW_PHYS);
		return true;
#endif
	}

	bool  sq_path::exist(const char*path)
	{
		if (access(path, 0) == -1)
		{
			return false;
		}
		else
			return true;
	}

	vector<string> sq_path::get_files(const char* dir, const string &postfix)
	{
		vector<string> files;
		string mydir(dir);

#ifdef WINDOWS
		string mypath;
		if (mydir.at(mydir.length() - 1) != '\\' && mydir.at(mydir.length() - 1) != '/')
		{
			mydir += '/';
		}
		mypath = mydir;
		mypath += string("*.*");


		_finddata_t file;
		intptr_t lf = -1;
		if ((lf = _findfirst(mypath.c_str(), &file)) == -1l)
		{
			return files;
		}
		else
		{
			do {
				string tmppath;

				if (!((file.attrib&_A_HIDDEN) || (file.attrib&_A_SUBDIR)))
				{
					if (ends_with(file.name, postfix))
					{
						 make_path(tmppath,mydir.c_str(), file.name);
						//printf("tmppath=%s\n", tmppath.c_str());

						files.push_back(tmppath);
					}
				}


			} while (_findnext(lf, &file) == 0);

			_findclose(lf);

		}
#else
		struct dirent* ent = NULL;
		DIR* pDir;
		pDir = opendir(dir);
		while (NULL != (ent = readdir(pDir)))
		{
			string fullpath;
			make_path(fullpath,dir, ent->d_name);
			if (is_file(fullpath.c_str()))
			{
				if (postfix == "" || strstr(ent->d_name, postfix.c_str()) != NULL)
				{
					files.push_back(ent->d_name);
				}
			}
		}
		closedir(pDir);

#endif
		return files;
	}
	bool  sq_path::is_folder(const char*path)
	{
#ifdef WINDOWS
		_finddata_t file;
		long lf;
		if ((lf = _findfirst(path, &file)) == -1l)
		{
			return false;
		}
		bool ret = false;
		if (file.attrib &_A_SUBDIR)
		{
			ret = true;
		}
		_findclose(lf);
		return ret;
#else
		struct stat st;
		int ret = stat(path, &st);
		if (ret == 0) {
			return S_ISDIR(st.st_mode);
		}
		else {
			return false;
		}
#endif
	}
	bool  sq_path::is_file(const char*path)
	{
#ifdef WINDOWS
		_finddata_t file;
		long lf;
		if ((lf = _findfirst(path, &file)) == -1l)
		{
			return false;
		}
		bool ret = false;
		if (!(file.attrib &_A_SUBDIR))
		{
			ret = true;
		}
		_findclose(lf);
		return ret;
#else
		struct stat st;
		int ret = stat(path, &st);
		if (ret == 0) {
			return S_ISREG(st.st_mode);
		}
		else {
			return false;
		}
#endif
	}
	bool  sq_path::is_hide(const char*path)
	{
#ifdef WINDOWS
		_finddata_t file;
		long lf;
		if ((lf = _findfirst(path, &file)) == -1l)
		{
			return false;
		}
		bool ret = false;
		if (file.attrib &_A_HIDDEN)
		{
			ret = true;
		}
		_findclose(lf);
		return ret;
#else
		return false;
#endif
	}

	/**
	 * 遍历目录下的所有文件
	 */
	void sq_path::walk_dir(const char *dir_path, std::vector<string> &files, bool revers)
	{
#ifdef WINDOWS
#else
		DIR *dirp;
		struct dirent *direntp;
		int dir_path_len = strlen(dir_path);
		bool append_ = false;
		if (dir_path[dir_path_len - 1] != '/')
		{
			append_ = true;
		}
		dirp = opendir(dir_path);
		if (dirp)
		{
			while ((direntp = readdir(dirp)) != NULL)
			{
				if (strcmp(".", direntp->d_name) == 0 ||
					strcmp("..", direntp->d_name) == 0)
				{
					continue;
				}
				if (direntp->d_type == DT_DIR && revers)
				{
					char buf[512];
					if (append_)
					{
						snprintf(buf, 511, "%s/%s", dir_path, direntp->d_name);
					}
					else
					{
						snprintf(buf, 512, "%s%s", dir_path, direntp->d_name);
					}

					walk_dir(buf, files, revers);
				}
				else if (direntp->d_type == DT_REG)
				{
					char buf[512] = {0};
					if (append_)
						snprintf(buf, 511, "%s/%s", dir_path, direntp->d_name);
					else
						snprintf(buf, 511, "%s%s", dir_path, direntp->d_name);
					files.push_back(buf);
				}
			}
			closedir(dirp);

		}
#endif
	}
}
