#ifndef __Client_h__
#define __Client_h__
#include "IKernel.h"
#include "ClientSession.h"
class Client
{
public:
	Client(ClientSession *session);

protected:
private:
	ClientSession *_gateSession;
};
#endif
