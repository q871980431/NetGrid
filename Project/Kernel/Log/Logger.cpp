#include "Logger.h"
#include "Tools.h"
#include "Tools_time.h"
#include "../Kernel.h"
#include "Tools_time.h"

template<  > Logger * Singleton<Logger>::_instance = nullptr;
bool Logger::Ready()
{
    _logPath << tools::GetAppPath() << DIR_DELIMITER << "log";
	_syncFile.SetFilePrefixName("sync");
	_asyncFile.SetFilePrefixName("async");
	_asyncThreadFile.SetFilePrefixName("thread");

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
	CreateLogFile(_syncFile);
	_asyncFile.SetAutoFlushNum(FLUSH_ASYNC_BATCH);
	_asyncThreadFile.SetAutoFlushNum(FLUSH_ASYNC_BATCH);
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
	_asyncThreadFile.Close();

    return true;
}

void Logger::SyncLog(const char *contents)
{
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

void Logger::ThreadLog(const char *contents)
{
	LogNode *logNode = (LogNode*)TMALLOC(sizeof(LogNode));
	logNode->_time.Assign(tools::GetCurrentTimeString());
	if (nullptr != contents)
	{
		logNode->_contents.Assign(contents);
	}
	else
	{
		logNode->_contents.Assign("NULL");
	}
	{
		std::lock_guard<std::mutex> guard(_threadLogMutex);
		_threadLog.threadA.push_back(logNode);
	}

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
	{
		std::lock_guard<std::mutex> guard(_threadLog.mutex);
		if (_threadLog.swap.empty())
		{
			std::lock_guard<std::mutex> guad1(_threadLogMutex);
			if (!_threadLog.threadA.empty())
				_threadLog.threadA.swap(_threadLog.swap);
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
	return tools::GetCurrentTimeString("%4d_%02d_%02d_%02d_%02d_%02d");
}

void Logger::ThreadRun()
{
    LogNode *logNode = nullptr;
    s32 delay = 0;
	s32 delay1 = 0;
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
                delay = 0;
            }
		}
		else
		{
			logNode = _write.threadB.front();

			WriteLogNode(_asyncFile, logNode);

			_dels.threadB.push_back(logNode);
			_write.threadB.pop_front();
		}

		if (_threadLog.threadB.empty())
		{
			{
				std::lock_guard<std::mutex> guard(_threadLog.mutex);
				if (!_threadLog.swap.empty())
					_threadLog.threadB.swap(_threadLog.swap);
			}
			delay1 += 1;
			if (delay1 > DELAY_FLUSH_COUNT)
			{
				_asyncThreadFile.Flush();
				delay1 = 0;
			}
		}
		else
		{
			logNode = _threadLog.threadB.front();
			WriteLogNode(_asyncThreadFile, logNode);
			DEL logNode;
			_threadLog.threadB.pop_front();
		}

    }
}

bool Logger::CreateLogFile(LogFile &logFile)
{
	if (logFile.IsOpen())
	{
		ASSERT(false, "log file is open");
		return false;
	}

	tlib::TString<MAX_PATH> logName;
	logName << _procName.c_str() << LOG_CONNECT_SIGN << _procId.c_str() << LOG_CONNECT_SIGN << GetLogTimeString() << LOG_CONNECT_SIGN << logFile.GetFilePrefixName().c_str() << LOG_FILE_ATT;
	if (!logFile.Open(_logPath.GetString(), logName.GetString()))
	{
		ASSERT(false, "Open log file: %s,error", logName.GetString());
		return false;
	}

	return true;
}

bool Logger::WriteLogNode(LogFile &logFile, const LogNode *logNode)
{
	if (!logFile.IsOpen())
	{
		if (!CreateLogFile(logFile))
			return false;
	}

	if (tools::GetTimeMillisecond() - logFile.CreateTick() > TIME_OUT_FOR_CUT_FILE)
	{
		logFile.Close();
		if (!CreateLogFile(logFile))
			return false;
	}

	logFile.Write(logNode->_time.GetString());
	logFile.Write(" | ");
	logFile.Write(logNode->_contents.GetString());
	logFile.Write("\n");
	logFile.Commit();
	return true;
}