
#include "SimpleMutex.h"

#include <Alert.h>

SimpleMutex::SimpleMutex (char const *name, BWindow *owner)
	: fMutex(-1),
	fOwner(owner)
{
	fMutex = create_sem(1, name);
	if (fMutex < B_OK) {
		(new BAlert(0, 
			"Unable to create Mutex.  Continuation is impossible", 							"Bummer", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
			
		if (fOwner != NULL) {
			fOwner->QuitRequested();
		}
	}
}


SimpleMutex::~SimpleMutex()
{
	status_t ok = delete_sem(fMutex);
	if (ok == B_BAD_SEM_ID) {
		(new BAlert(0, 
			"Unable to delete Mutex", "Bummer", NULL, NULL, B_WIDTH_AS_USUAL, 				B_STOP_ALERT))->Go();
			
	} else {
		fMutex = -1;
	}	
}


status_t
SimpleMutex::Lock()
{
	return acquire_sem(fMutex);
}


status_t
SimpleMutex::Unlock()
{
	return release_sem(fMutex);
}
