#include "RowObject.h"

bool RowObject::Init()
{
	_memeroy.addr = _des->CreateMemeory(_memeroy.size);
	if (_memeroy.addr)
		return true;

	return false;
}
