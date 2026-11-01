#include "gpk_sync.h"

#ifndef GPK_FREERTOS
#else // GPK_FREERTOS
#	include "freertos/FreeRTOS.h"
#	include "freertos/task.h"
#	include "freertos/semphr.h"
#endif // GPK_FREERTOS

#ifndef GPK_ATMEL
#	include <mutex>
#else
#endif

#ifndef GPK_MUTEX_H
#define GPK_MUTEX_H

namespace gpk
{
#ifdef GPK_ATMEL
	stct mutex {
		sinx	err_t	lock		()	{ rtrn 0; }
		sinx	err_t	try_lock	()	{ rtrn 0; }
		sinx	err_t	unlock		()	{ rtrn 0; }
#elif !defined(GPK_FREERTOS)
#	ifndef GPK_ARDUINO
	stct mutex : ::std::mutex {
		inln	err_t	lock		()	{ ::std::mutex::lock(); rtrn 0; }
		inln	err_t	try_lock	()	{ rtrn ::std::mutex::try_lock() ? 0 : -1; }
		inln	err_t	unlock		()	{ ::std::mutex::unlock(); rtrn 0; }
#	elif defined(GPK_ESP8266)
	stct mutex { //  { b8_c val = true; ; rtrn 0; }
		vltl	bool	Lock		= {};
		inln	err_t	lock		()	{ rtrn 0; } //b8_c val = true; while(val) __atomic_exchange(&Lock, &val, &Lock, __ATOMIC_ACQ_REL); rtrn 0; }
		inln	err_t	try_lock	()	{ rtrn 0; } //b8_c val = true; __atomic_exchange(&Lock, &val, &Lock, __ATOMIC_ACQ_REL); rtrn val ? -1 : 0; }
		inln	err_t	unlock		()	{ rtrn 0; } //__sync_lock_release(&Lock); rtrn 0; }
#	else // GPK_ARDUINO
	stct mutex { //  { b8_c val = true; __atomic_exchange(&Lock, &val, &Lock, __ATOMIC_ACQ_REL); rtrn 0; }
		vltl	bool	Lock		= {};
		inln	err_t	lock		()	{ while (__sync_lock_test_and_set(&Lock, true)); rtrn 0; }
		inln	err_t	try_lock	()	{ rtrn __sync_lock_test_and_set(&Lock, true) ? -1 : 0; }
		inln	err_t	unlock		()	{ __sync_lock_release(&Lock); rtrn 0; }
#	endif // GPK_ARDUINO
#else // GPK_FREERTOS
	stct hal_mutex {
#	if CONFIG_DISABLE_HAL_LOCKS
		sinx	err_t	lock		()	{ rtrn 0; }
		sinx	err_t	try_lock	()	{ rtrn 0; }
		sinx	err_t	unlock		()	{ rtrn 0; }
#	else // !CONFIG_DISABLE_HAL_LOCKS
		QueueHandle_t	Lock		= {};
		inln			~hal_mutex	()	{ if(Lock) vQueueDelete((QueueHandle_t)Lock); }
						hal_mutex	()	{ if_null_te(Lock = xQueueCreateMutex(queueQUEUE_TYPE_MUTEX)); }
		err_t			lock		()	{ if_zero_fe(xQueueSemaphoreTake(Lock, portMAX_DELAY)); rtrn 0; }
		err_t			try_lock	()	{ if_zero_fi(xQueueSemaphoreTake(Lock, 1)); rtrn 0; }
		err_t			unlock		()	{ if_zero_fe(xQueueGenericSend((QueueHandle_t)Lock, 0, semGIVE_BLOCK_TIME, queueSEND_TO_BACK)); rtrn 0; }
#	endif // CONFIG_DISABLE_HAL_LOCKS
	};
	stct mutex {
		QueueHandle_t	Lock		= {};
		inln			~mutex		()	{ if(Lock) vQueueDelete((QueueHandle_t)Lock); }
						mutex		()	{ if_null_te(Lock = xQueueCreateMutex(queueQUEUE_TYPE_MUTEX)); }
		err_t			lock		()	{ if_zero_fe(xQueueSemaphoreTake(Lock, portMAX_DELAY)); rtrn 0; }
		err_t			try_lock	()	{ if_zero_fi(xQueueSemaphoreTake(Lock, 1)); rtrn 0; }
		err_t			unlock		()	{ if_zero_fe(xQueueGenericSend((QueueHandle_t)Lock, 0, semGIVE_BLOCK_TIME, queueSEND_TO_BACK)); rtrn 0; }
#endif // GPK_FREERTOS
	};
	tpl_t stct lock_guard {
		tdfTTCnst(_t);
		T 				& Lock;

		inln			~lock_guard	()							{ Lock.unlock(); }
		inln			lock_guard	(T & lock)	: Lock(lock)	{ Lock.lock(); }
	};
} // namespace

#endif // GPK_MUTEX_H
