
#include <OS.h>
#include <iostream>
#include <string.h>

#include "SimpleMutex.h"


SimpleMutex::SimpleMutex (char const *debugname, status_t &error)
{
	error = create_sem(1, debugname);
	
	std::cout << "SimpleMutex::SimpleMutex()" << std::endl;
}


SimpleMutex::~SimpleMutex()
{
	std::cout << "SimpleMutex::~SimpleMutex()" << std::endl;
	delete_sem (fMutex);
}


status_t
SimpleMutex::Lock (void)
{
	return acquire_sem(fMutex);
}


status_t
SimpleMutex::Unlock (void)
{
	return release_sem(fMutex);
}
