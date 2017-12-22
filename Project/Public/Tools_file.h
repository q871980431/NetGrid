#ifndef __Tools_file_h__
#define __Tools_file_h__
#include "MultiSys.h"
#include "TString.h"
#include <map>
#include <vector>
namespace tools
{
    #define _A_NORMAL       0x00    /* Normal file - No read/write restrictions */
    #define _A_RDONLY       0x01    /* Read only file */
    #define _A_HIDDEN       0x02    /* Hidden file */
    #define _A_SYSTEM       0x04    /* System file */
    #define _A_SUBDIR       0x10    /* Subdirectory */
    #define _A_ARCH         0x20    /* Archive file */

    typedef tlib::TString<MAX_PATH> FileName;
    struct FileAttr
    {
        s32         attrib;
        s64         time_create;    /* -1 for FAT file systems */
        s64         time_access;    /* -1 for FAT file systems */
        s64         time_write;
        s64         size;
        FileName    name;
    };
    typedef std::map<FileName, FileAttr> FileMap;
    class DirInfo;
    typedef std::vector<DirInfo>         DirLists;
    struct DirInfo 
    {
        FileAttr attr;
        DirLists dirs;
        FileMap  files;
    };
#ifdef __cplusplus
    extern "C"{
#endif
        //============================START==================================
        void GetFileNameNotExtension(const char *name, char out[MAX_PATH]);
        void FindDirectoryFiles(const char *dirpath, const char *extension, tools::DirInfo &dir);
        //=============================END===================================
#ifdef __cplusplus
    }
#endif
}

#endif
