
#ifndef _SIMPLE_MUTEX_H_
#define _SIMPLE_MUTEX_H_

class SimpleMutex
{
	public:
	SimpleMutex (char const *debugname);
	virtual ~SimpleMutex();
	
	public:
//	status_t InitCheck (void);
	status_t Lock (void);
	status_t Unlock (void);
//	sem_id Mutex (void) const { return fMutex; }
	
	private:
	sem_id fMutex;
};


#endif // _SIMPLE_MUTEX_H_
