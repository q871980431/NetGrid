#include "Logger.h"
#include "Tools.h"
template<  > Logger * Singleton<Logger>::_instance = nullptr;
bool Logger::Ready()
{
    _logPath << tools::GetAppPath() << DIR_DELIMITER << "log";
    _asyncPrefix << "async";
    _syncPrefix << "sync";
    tools::Mkdir(_logPath.GetString());
    _syncLogs = new CircluarQueue<LogNode>(LOG_NODE_COUNT);
    _terminate = false;

    return true;
}
bool Logger::Initialize()
{
    tlib::TString<MAX_PATH> syncLogName;
    syncLogName << _syncPrefix.GetString() << GetLogTimeString() << LOG_FILE_ATT;
    if (!_syncFile.Open(_logPath.GetString(), syncLogName.GetString()))
    {
        ASSERT(false, "open log file: %s failed", syncLogName.GetString());
        return false;
    }
    _thread = std::thread(&Logger::ThreadRun, this);

    return true;
}
bool Logger::Destroy()
{
    if ( _thread.joinable())
    {
        _terminate = true;
        _thread.join();
    }
    _asyncFile.Close();
    _syncFile.Close();
    SAFE_DELETE(_syncLogs);

    return true;
}
void Logger::SyncLog(const char *contents)
{
    ECHO("Current Time: %s", tools::GetCurrentTimeString());
    _syncFile.Write(tools::GetCurrentTimeString());
    _syncFile.Write(" | ");
    _syncFile.Write(contents);
    _syncFile.Write("\n");
    _syncFile.Flush();
}
void Logger::AsyncLog(const char *contents)
{
    LogNode *logNode = nullptr;
    while (!(logNode = _syncLogs->Push()))
    {
        MSLEEP(1);
    }
    logNode->_time.Assign(tools::GetCurrentTimeString());
    if ( nullptr != contents )
    {
        logNode->_contents.Assign(contents);
    }else
    {
        logNode->_contents.Assign("NULL");
    }
}

const char * Logger::GetLogTimeString()
{
    static char buffer[64];
    time_t t;

    tm *tm = nullptr;
    t = time(NULL);
    tm = localtime(&t);
    SafeSprintf(buffer, sizeof(buffer), "%4d_%02d_%02d_%02d_%02d_%02d", tm->tm_year + 1900, tm->tm_mon + 1, \
        tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return buffer;
}
void Logger::ThreadRun()
{
    LogNode *logNode = nullptr;
    s32 mark = 0;
    while (true)
    {
        logNode = _syncLogs->Pop();
        if ( nullptr == logNode)
        {
            if (_terminate)
                return;
            MSLEEP(10);
            continue;
        }
        if (!_asyncFile.IsOpen())
        {
            if (!CreateAsyncFile())
                return;
        }
        if (tools::GetTimeMillisecond() - _asyncFile.CreateTick() > TIME_OUT_FOR_CUT_FILE)
        {
            _asyncFile.Close();
            if (!CreateAsyncFile())
                return;
        }
        _asyncFile.Write(logNode->_time.GetString());
        _asyncFile.Write(" | ");
        _asyncFile.Write(logNode->_contents.GetString());
        _asyncFile.Write("\n");
        if (++mark > FLUSH_ASYNC_BATCH)
        {
            _asyncFile.Flush();
            mark = 0;
        }
    }
}

bool Logger::CreateAsyncFile()
{
    if (_asyncFile.IsOpen())
    {
        ASSERT(false, "Async log file is open");
        return false;
    }
    tlib::TString<MAX_PATH> asyncLogName(_asyncPrefix.GetString());
    asyncLogName << GetLogTimeString() << LOG_FILE_ATT;
    if (!_asyncFile.Open( _logPath.GetString(), asyncLogName.GetString()))
    {
        ASSERT(false, "Open asynce log file: %s,error", asyncLogName.GetString());
        return false;
    }
    return true;
}
