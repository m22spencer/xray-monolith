#pragma once

#include "Lock.hpp"

class Lock;

class ScopeLock : xray::noncopyable
{
	Lock* syncObject;

public:
	ScopeLock(Lock* SyncObject);
	~ScopeLock();
};
