
#ifndef _SIMPLE_MUTEX_H_
#define _SIMPLE_MUTEX_H_

#include <OS.h>
#include <Window.h>

class SimpleMutex
{
	public:
	SimpleMutex (char const *name, BWindow *owner); // owner can be NULL
	virtual ~SimpleMutex();
	
	public:
	status_t Lock();
	status_t Unlock();
	sem_id Mutex (void) { return fMutex; }
	
	private:
	sem_id fMutex;
	BWindow *fOwner;
};


#endif // _SIMPLE_MUTEX_H_
