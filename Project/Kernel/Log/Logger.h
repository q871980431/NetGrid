#ifndef __Logger_h__
#define __Logger_h__
#include "MultiSys.h"
#include "Singleton.h"
#include "ILogger.h"
#include "LogFile.h"
#include "TString.h"
#include "CircularQueue.h"
#include <thread>

#define LOG_FILE_ATT    ".log"

class Logger   : public ILogger, public Singleton<Logger>
{
    struct LogNode 
    {
        tlib::TString<64> _time;
        tlib::TString<LOG_BUFF_SIZE> _contents;
        LogNode() :_time(), _contents(){};
        LogNode(const char *time, const char *contents) :_time(time), _contents(contents){};
    };
    enum 
    {
        LOG_NODE_COUNT = 1024,
        TIME_OUT_FOR_CUT_FILE = 7200 * 1000,
        FLUSH_ASYNC_BATCH = 512,
    };
public:
    //virtual ~Logger(){};

    virtual bool Ready();
    virtual bool Initialize();
    virtual bool Destroy();

    virtual void SyncLog(const char *contents);
    virtual void AsyncLog(const char *contents);
protected:
private:
    const char *GetLogTimeString();

    void ThreadRun();

    bool CreateAsyncFile();

private:

    tlib::TString<MAX_PATH>         _logPath;
    tlib::TString<LOG_PREFIX_LEN>   _asyncPrefix;
    tlib::TString<LOG_PREFIX_LEN>   _syncPrefix;
    LogFile _asyncFile;
    LogFile _syncFile;

    CircluarQueue<LogNode>             *_syncLogs;
    std::thread                         _thread;
    bool                                _terminate;
};

#define LOGGER  (Logger::GetInstancePtr())

#endif