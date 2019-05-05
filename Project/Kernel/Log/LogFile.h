#ifndef __LogFile_h__
#define __LogFile_h__
#include "MultiSys.h"
class LogFile
{
public:
    LogFile() :_file(nullptr),_createTick(0), _flushNum(256){};
    ~LogFile(){ Close(); };

    bool Open(const char *path, const char *name);
    void Close();
    void Write(const char *contents);
	void Commit();
    void Flush();

    inline bool IsOpen(){ return nullptr != _file; };
    inline s64  CreateTick(){ return _createTick; };
	inline void SetAutoFlushNum(s32 autoFlushNum) { _flushNum = autoFlushNum; };
	inline void SetFilePrefixName(const std::string &prefix) { _filePrefix = prefix; };
	inline std::string & GetFilePrefixName() { return _filePrefix; };
private:
    FILE    *_file;
    s64     _createTick;
	s32		_mark;
	s32		_flushNum;
	std::string _filePrefix;
};
#endif