#include "LogFile.h"
#include "Tools_time.h"


bool LogFile::Open(const char *path, const char *name)
{
    char filePath[MAX_PATH];
    s32 len = 0;
    ASSERT(nullptr == _file, "file exist");
    SafeSprintf(filePath, sizeof(filePath), "%s/%s", path, name);
    _file = fopen(filePath, "ab+");
    if ( nullptr == _file )
    {
        ASSERT(false, "file open failed, file name = %s", filePath);
        return false;
    }
    _createTick = tools::GetTimeMillisecond();
    return true;
}
void LogFile::Close()
{
    if (_file)
    {
        fflush(_file);
        fclose(_file);
        _file = nullptr;
        _createTick = 0;
    }
}
void LogFile::Write(const char *contents)
{
    if (_file)
    {
        if (nullptr != contents )
        {
            fwrite(contents, (s32)strlen(contents), 1, _file);
        }else
        {
            fwrite("NULL", sizeof("NULL") - 1, 1, _file);
        }

    }
}

void LogFile::Commit()
{

}

void LogFile::Flush()
{
    if (_file)
    {
        fflush(_file);
    }
}