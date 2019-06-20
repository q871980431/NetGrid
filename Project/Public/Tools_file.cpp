#include "Tools_file.h"
#include "Tools.h"
#ifdef LINUX
#include <sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#endif

#ifdef __cplusplus
extern "C"  {
#endif
//===================================START===============================
void GetFileNameNotExtension(const char *name, char out[MAX_PATH])
{
    tools::SafeMemset(out, sizeof(out), 0, sizeof(out));
    const char *pos = strchr(name, '.');
    if (nullptr != pos)
        tools::SafeMemcpy(out, sizeof(out), name, (pos - name));
    else
        tools::SafeStrcpy(out, sizeof(out), name, strlen(name));
}

bool CheckFileExtension(const char *filename, const char *extension)
{
    const char *pos = strchr(filename, '.');
    if (nullptr != pos)
        return strcmp(pos+1, extension) == 0 ? true : false;
    return false;
}

void GetDirName(const char *dirpath, char name[MAX_PATH])
{
    s32 len = strlen(dirpath);
    tools::SafeMemset(name, sizeof(name), 0, sizeof(name));

    s32 start = len - 1;
    while (start >= 0 && dirpath[start] == '/')
    {
        start--;
    }

    const char *pos = nullptr;
    for (s32 i = start; i >= 0; i--)
    {
        if (dirpath[i] == '/')
        {
            pos = &dirpath[i];
            break;
        }
    }

    tools::SafeMemcpy(name, sizeof(name), pos + 1, &dirpath[start] - pos);
}

#ifdef WIN32
    void FindDirectoryFiles(const char *dirpath, const char *extension, tools::DirInfo &dir)
    {
        WIN32_FIND_DATA  FindFileData;
        HANDLE           hFind;
        tools::FileName file(dirpath);
        file << "/*";

        hFind = FindFirstFile(file.GetString(), &FindFileData); //该函数会获取符合条件的第一个文件名，并获得一个句柄给FindNextFile或FindClose使用.  
        if (hFind == INVALID_HANDLE_VALUE){
            return;
        }
        else{
            do 
            {
                tools::FileAttr attr;
                if (strcmp(FindFileData.cFileName, "..") == 0)
                    continue;
                ECHO("%s", FindFileData.cFileName);             //这行语句会输出所有符合条件的文件名 
                attr.attrib = 0;
                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                    attr.attrib |= _A_RDONLY;
                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
                    attr.attrib |= _A_HIDDEN;
                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
                    attr.attrib |= _A_SYSTEM;
                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    attr.attrib |= _A_SUBDIR;
                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
                    attr.attrib |= _A_ARCH;

                tools::KEYINT64 tmp;
                tmp.lVal = FindFileData.ftCreationTime.dwLowDateTime;
                tmp.hVal = FindFileData.ftCreationTime.dwHighDateTime;
                attr.time_create = tmp.val;
                tmp.lVal = FindFileData.ftLastAccessTime.dwLowDateTime;
                tmp.hVal = FindFileData.ftLastAccessTime.dwHighDateTime;
                attr.time_access = tmp.val;
                tmp.lVal = FindFileData.ftLastWriteTime.dwLowDateTime;
                tmp.hVal = FindFileData.ftLastWriteTime.dwHighDateTime;
                attr.time_write = tmp.val;
                tmp.lVal = FindFileData.nFileSizeLow;
                tmp.hVal = FindFileData.nFileSizeHigh;
                attr.size = tmp.val;
                if (attr.attrib & _A_SUBDIR)
                {
                    if (strcmp(FindFileData.cFileName, ".") == 0)
                    {
                        char name[MAX_PATH];
                        GetDirName(dirpath, name);
                        dir.attr = attr;
                        dir.attr.name = name;
                    }
                    else
                    {
                        tools::DirInfo subdir;
                        dir.dirs.push_back(subdir);
                        dir.dirs.back();
                        char subpath[MAX_PATH];
                        SafeSprintf(subpath, sizeof(subpath), "%s/%s", dirpath, FindFileData.cFileName);
                        tools::FindDirectoryFiles(subpath, extension, dir.dirs.back());
                    }
                }
                else
                {
                    if (CheckFileExtension(FindFileData.cFileName, extension))
                    {
                        attr.name = FindFileData.cFileName;
                        dir.files.insert(std::make_pair(attr.name, attr));
                    }
                }
            } while (FindNextFile(hFind, &FindFileData));
        }

        FindClose(hFind);
    }
#endif

#ifdef LINUX
    void FindDirectoryFiles(const char *dirpath, const char *extension, tools::DirInfo &dir)
    {
        struct stat s;
        struct stat statbuf;
        lstat(dirpath, &s);
        if (!S_ISDIR(s.st_mode))
        {
            return;
        }

        struct dirent * filename;    // return value for readdir()  
		DIR * dirPtr = opendir(dirpath);  // return value for opendir()
        if (NULL == dirPtr)
        {
            ASSERT(false, "Can not open dir %s", dirpath);
            return;
        }

        /* read all the files in the dir ~ */
        while ((filename = readdir(dirPtr)) != NULL)
        {
			char fullPath[MAX_PATH];
			SafeSprintf(fullPath, sizeof(fullPath), "%s/%s", dirpath, filename->d_name);
			//ECHO("FullPath:%s", fullPath);
            lstat(fullPath, &statbuf);

            tools::FileAttr attr;
            if (strcmp(filename->d_name, "..") == 0)
                continue;
            ECHO("file %s, type:%d, mode:%d", filename->d_name, filename->d_type, statbuf.st_mode);                           //这行语句会输出所有符合条件的文件名 
            attr.attrib = 0;
            if (S_ISDIR(statbuf.st_mode))
                attr.attrib |= _A_SUBDIR;

            attr.time_create = 0;
            attr.time_access = statbuf.st_atime;
            attr.time_write = statbuf. st_mtime;
            attr.size = statbuf.st_size;

            if (attr.attrib & _A_SUBDIR)
            {
                if (strcmp(filename->d_name, ".") == 0)
                {
                    char name[MAX_PATH];
                    GetDirName(dirpath, name);
                    dir.attr = attr;
                    dir.attr.name = name;
                }
                else
                {
                    tools::DirInfo subdir;
                    dir.dirs.push_back(subdir);
                    dir.dirs.back();
                    char subpath[MAX_PATH];
                    SafeSprintf(subpath, sizeof(subpath), "%s/%s", dirpath, filename->d_name);
                    tools::FindDirectoryFiles(subpath, extension, dir.dirs.back());
                }
            }
            else
            {
                if (CheckFileExtension(filename->d_name, extension))
                {
                    attr.name = filename->d_name;
                    dir.files.insert(std::make_pair(attr.name, attr));
                }
            }
        }
        closedir(dirPtr);
    }
#endif
//===================================END======================================
#ifdef __cplusplus
}
#endif

