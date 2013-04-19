
#ifndef _SIMPLE_MUTEX_H_
#define _SIMPLE_MUTEX_H_

class SimpleMutex
{
	public:
	SimpleMutex (char const *debugname, status_t &error);
	virtual ~SimpleMutex();
	
	public:
	status_t InitCheck (void);
	status_t Lock (void);
	status_t Unlock (void);
	sem_id Mutex (void) { return fMutex; }
	
	private:
	sem_id fMutex;
};


#endif // _SIMPLE_MUTEX_H_
