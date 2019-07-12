#ifndef __Logger_h__
#define __Logger_h__
#include "MultiSys.h"
#include "Singleton.h"
#include "ILogger.h"
#include "LogFile.h"
#include "TString.h"
#include "CircularQueue.h"
#include <thread>
#include <mutex>
#include <list>

#define LOG_FILE_ATT    ".log"
#define LOG_CONNECT_SIGN "_"

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
		COMPUT_TIME_BATCH_COUNT = 200,
		DELAY_FLUSH_COUNT = 2,
		SLEEP_TIME = 100,
    };

	typedef std::list<LogNode*> LogList;
	struct LogListThreadData
	{
		LogList threadA;
		LogList threadB;
		LogList	swap;
		std::mutex	mutex;
	};

public:
    //virtual ~Logger(){};

    virtual bool Ready();
    virtual bool Initialize();
    virtual bool Destroy();

    virtual void SyncLog(const char *contents);
    virtual void AsyncLog(const char *contents);
	virtual void ThreadLog(const char *contents);
	virtual void Process(s32 tick);

	s32	LogLevel() { return _logLevel; };
private:
    void ThreadRun();

private:
	const char *GetLogTimeString();
	bool CreateLogFile(LogFile &logFile);
	bool WriteLogNode(LogFile &logFile, const LogNode *logNode);

private:

    tlib::TString<MAX_PATH>         _logPath;
    tlib::TString<LOG_PREFIX_LEN>   _asyncPrefix;
    tlib::TString<LOG_PREFIX_LEN>   _syncPrefix;
    LogFile _asyncFile;
	LogFile _asyncThreadFile;		
    LogFile _syncFile;

    std::thread                         _thread;
    bool                                _terminate;
	std::string							_procName;
	std::string							_procId;

	LogListThreadData					_write;
	LogListThreadData					_dels;
	LogListThreadData					_threadLog;
	std::mutex							_threadLogMutex;
	s32	_logLevel;
};

#define LOGGER  (Logger::GetInstancePtr())

#endif