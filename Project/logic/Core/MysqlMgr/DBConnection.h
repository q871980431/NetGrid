#ifndef DBCONNECTION_H
#define DBCONNECTION_H
#ifdef WIN32
#include <winsock2.h>
#endif
#include "mysql.h"
#include "errmsg.h"
#include <string>
#include "IKernel.h"
#include "IMysqlMgr.h"

struct ConnectionInfo
{
	std::string  hostDes;
	unsigned int port;
	std::string  unixSocket;
	unsigned int clientFlag;
	std::string  charset;
	std::string  username;		
	std::string  password;		
	std::string  databaseName;	
	ConnectionInfo() :port(0), clientFlag(0)
	{}
	inline const char* User(){return username.c_str();}
	inline const char* Password(){return password.c_str();}
	inline const char* DBName(){return databaseName.c_str();}
	inline const char* Host() {return hostDes.empty() ? nullptr : hostDes.c_str();}
	inline const char* UnixHanderName() { return unixSocket.empty() ? nullptr : unixSocket.c_str();}
	inline unsigned int Port(){return port;}
	inline unsigned int ClientFlag(){return clientFlag;}
	inline const char* Charset(){ return charset.c_str();}
};

const static char *MYSQL_ERROR_LABL = "MysqlError";
const static s32   RECONNECT_TIME_OUT = 30;	//30 minutes
const static s32   RECONNECT_SLEEP_TIME = 300;	//300 ms
struct ConnectionTraceInfo 
{
	s64	_execCount;
	s64 _failCount;
	s32 _maxExecTime;
	s32 _minExecTime;
};
class DBConnection : public IEscapeStringHandler
{
public:
	DBConnection(core::IKernel *kernel,  const char* hostIp, int port, const char* databaseName, const char* username, const char* password, unsigned int connectionflag, const char * charset);
	~DBConnection();

	virtual s32   EscapeString(const char* pszSrc, int nSize, char* pszDest) { return (s32)EscapeStringEx(pszSrc, nSize, pszDest); };
	bool open();
	bool execute(const char* sql);
	unsigned int GetLastErrNo();
	const char* GetLastErrInfo();
	unsigned long  EscapeStringEx(const char* pszSrc, int nSize, char* pszDest );
	unsigned long GetAffectedRow();
	inline MYSQL * GetMYSQL() { return &_mysqlHandler; };
	inline ConnectionTraceInfo * GetTraceInfo() { return &_traceInfo; };
private:
	bool reopen();
	bool isActive();
	bool InnerExecute(const char *sql);

private:
	DBConnection( const DBConnection& );
	DBConnection& operator=( const DBConnection& );

private:
	core::IKernel *_kernel;
	MYSQL _mysqlHandler;
	ConnectionInfo _connectionInfo;

	ConnectionTraceInfo _traceInfo;
};

#endif //DBCONNECTION_H
