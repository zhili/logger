/*
 *  mutex.h
 *  Logging
 *
 *  Created by zhili hu on 2/15/10.
 *  Copyright 2010 scut. All rights reserved.
 *
 */
#ifndef MUTEX_H_94ZJYTYP
#define MUTEX_H_94ZJYTYP

#if defined(WIN32)
#include <windows.h>
typedef CRITICAL_SECTION MutexType;
#else
#include <pthread.h>
typedef pthread_mutex_t MutexType;
#endif
#include <stdlib.h>      // for abort()

#define DISALLOW_EVIL_CONSTRUCTORS(TypeName)    \
	TypeName(const TypeName&);                  \
	void operator=(const TypeName&)

#define SAFE_PTHREAD(fncall)  do {   /* run fncall if is_safe_ is true */  \
		if (_is_safe && fncall(&_mutex) != 0) abort();                     \
	} while (0)

class Mutex {
public:
	inline Mutex();
	inline ~Mutex();
	inline void Lock();
	inline void Unlock();
	inline bool TryLock();
	
private:
	MutexType _mutex;
	volatile bool _is_safe;
	bool _destroy;
	inline void SetIsSafe() { _is_safe = true; }
	DISALLOW_EVIL_CONSTRUCTORS(Mutex);
};

#if defined(WIN32) 
Mutex::Mutex() : _destroy(true) {
	InitializeCriticalSection(&_mutex);
	SetIsSafe();
}
Mutex::~Mutex() {
	if (_destroy)
		DeleteCriticalSection(&_mutex);
}
void Mutex::Lock() {
	if (_is_safe)
		EnterCriticalSection(&_mutex);
}
void Mutex::Unlock() {
	if (_is_safe)
		LeaveCriticalSection(&_mutex);
}
bool Mutex::TryLock() {
	return _is_safe ? TryEnterCriticalSection(&_mutex) != 0 : true;
}
#else
Mutex::Mutex() : _destroy(true) {
	SetIsSafe();
	if (_is_safe && pthread_mutex_init(&_mutex, NULL) != 0)
		abort();
}
Mutex::~Mutex() {
	if (_destroy)
		SAFE_PTHREAD(pthread_mutex_destroy);
}
void Mutex::Lock() {
	SAFE_PTHREAD(pthread_mutex_lock);
}
void Mutex::Unlock() {
	SAFE_PTHREAD(pthread_mutex_unlock);
}
bool Mutex::TryLock() {
	return _is_safe ? pthread_mutex_trylock(&_mutex) == 0 : true;
}
#endif

#endif /* end of include guard: MUTEX_H_94ZJYTYP */

