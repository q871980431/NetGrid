#include "Logger.h"
#include "Tools.h"
#include "Tools_time.h"
#include "../Kernel.h"
#include "Tools_time.h"

template<  > Logger * Singleton<Logger>::_instance = nullptr;
bool Logger::Ready()
{
	
    _logPath << tools::GetAppPath() << DIR_DELIMITER << "log";
    _asyncPrefix << "async";
    _syncPrefix << "sync";
    tools::Mkdir(_logPath.GetString());
    _terminate = false;

    return true;
}
bool Logger::Initialize()
{
	_procName = Kernel::GetInstance().GetCmdArg("name");
	_procId = Kernel::GetInstance().GetCmdArg("id");

    tlib::TString<MAX_PATH> syncLogName;
    syncLogName << _procName.c_str() << LOG_CONNECT_SIGN  << _procId.c_str() << LOG_CONNECT_SIGN << GetLogTimeString() << LOG_CONNECT_SIGN << _syncPrefix.GetString() << LOG_FILE_ATT;
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
    LogNode *logNode = (LogNode*)TMALLOC(sizeof(LogNode));
    logNode->_time.Assign(tools::GetCurrentTimeString());
    if ( nullptr != contents )
    {
        logNode->_contents.Assign(contents);
    }else
    {
        logNode->_contents.Assign("NULL");
    }
	_write.threadA.push_back(logNode);
}

void Logger::Process(s32 tick)
{
	s64 start = tools::GetTimeMillisecond();
	{
		std::lock_guard<std::mutex> guard(_write.mutex);
		if (_write.swap.empty())
			_write.threadA.swap(_write.swap);
	}
	{
		std::lock_guard<std::mutex> guard(_dels.mutex);
		if (!_dels.swap.empty())
		{
			if (_dels.threadA.empty())
				_dels.threadA.swap(_dels.swap);
		}
	}

	s32 count = 0;
	while (!_dels.threadA.empty())
	{
		LogNode *logNode = _dels.threadA.front();
		TFREE(logNode);
		_dels.threadA.pop_front();
		count++;
		if (count > COMPUT_TIME_BATCH_COUNT)
		{
			s64 now = tools::GetTimeMillisecond();
			count = 0;
			if (now - start > tick)
				break;
		}
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
    s32 delay = 0;
    while (true)
    {
        if ( _write.threadB.empty())
        {
			{
				std::lock_guard<std::mutex> guard(_write.mutex);
				if (!_write.swap.empty())
					_write.threadB.swap(_write.swap);
			}
			{
				std::lock_guard<std::mutex> guard(_dels.mutex);
				if (_dels.swap.empty())
					_dels.threadB.swap(_dels.swap);
			}

            if (_terminate)
                return;

            delay += 1;
            MSLEEP(SLEEP_TIME);
            if (delay > DELAY_FLUSH_COUNT)
            {
                _asyncFile.Flush();
                mark = 0;
                delay = 0;
            }
            continue;
        }

		logNode = _write.threadB.front();

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

		_dels.threadB.push_back(logNode);
		_write.threadB.pop_front();
    }
}

bool Logger::CreateAsyncFile()
{
    if (_asyncFile.IsOpen())
    {
        ASSERT(false, "Async log file is open");
        return false;
    }

    tlib::TString<MAX_PATH> asyncLogName;
    asyncLogName << _procName.c_str() << LOG_CONNECT_SIGN << _procId.c_str() << LOG_CONNECT_SIGN << GetLogTimeString() << LOG_CONNECT_SIGN << _asyncPrefix.GetString() << LOG_FILE_ATT;
    if (!_asyncFile.Open( _logPath.GetString(), asyncLogName.GetString()))
    {
        ASSERT(false, "Open asynce log file: %s,error", asyncLogName.GetString());
        return false;
    }

    return true;
}
