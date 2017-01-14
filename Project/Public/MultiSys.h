#ifndef __Muliti_Sys_h__
#define __Muliti_Sys_h__
#include "WinSys.h"
#include "LinSys.h"


#define SAFE_DELETE(p) if((p)){delete p; p = nullptr;}
#define SAFE_DELETE_ARRAY(p) if((p)){delete[] p; p = nullptr;}
#define MAKE_BOOL(p)    ((0 == (p)) ? false : true)
#define DIR_DELIMITER   "/"

enum DATA_TYPE
{
    DATA_TYPE_NULL      = 0,
    DATA_TYPE_S8        = 1,
    DATA_TYPE_S16       = 2,
    DATA_TYPE_S32       = 3,
    DATA_TYPE_S64       = 4,
    DATA_TYPE_STR       = 5,
    DATA_TYPE_BINARY    = 6
};

#endif
