#include "gpk_error.h"

#ifndef GPK_SYNC_H_23627
#define GPK_SYNC_H_23627

#ifdef GPK_WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	include <thread>
#	include <chrono>
#elif defined(GPK_ARDUINO)
#	include <Arduino.h>
//#elif defined(GPK_ANDROID) || defined(GPK_LINUX) || defined(GPK_ESP32)
#else
#	include <thread>
#	include <chrono>
#endif
namespace gpk
{
#ifdef GPK_WINDOWS
	stin err_t	sleep	(u2_t milliseconds)		noexcept	{ Sleep(milliseconds); return (err_t)milliseconds; }
	stin u3_t	sleepUs	(u3_t microseconds)		noexcept	{ std::this_thread::sleep_for(std::chrono::microseconds(microseconds)); return (s3_t)microseconds; }
#elif defined(GPK_ARDUINO)
	stin err_t	sleep	(u2_t milliseconds)		noexcept	{ ::delay(milliseconds); return (err_t)milliseconds; }
	stin u3_t	sleepUs	(u3_t microseconds)		noexcept	{ ::delayMicroseconds(microseconds); return microseconds; }
#else
	stin err_t	sleep	(u2_t milliseconds)		noexcept	{ std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds)); return (err_t)milliseconds; }
	stin u3_t	sleepUs	(u3_t microseconds)		noexcept	{ std::this_thread::sleep_for(std::chrono::microseconds(microseconds)); return microseconds; }
#endif

#ifndef GPK_MTSUPPORT
#	define gpk_sync_increment(nCount)							(++(nCount))
#	define gpk_sync_decrement(nCount)							(--(nCount))
#	define gpk_sync_exchange(Target, Value)						((Target) = (Value))
#	define gpk_sync_compare_exchange(target, value, comparand)	((target) = ((target) == (comparand)) ? (value) : (target))
#else
#	if defined(GPK_ANDROID) || defined(GPK_LINUX)
/// <Atomic Builtins> http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Atomic-Builtins.html#Atomic-Builtins
#		define gpk_sync_increment(nCount)							(__sync_add_and_fetch(&(nCount), 1))
#		define gpk_sync_decrement(nCount)							(__sync_sub_and_fetch(&(nCount), 1))
#		define gpk_sync_exchange(target, value)						(__sync_lock_test_and_set(&(target), (value)))
#		define gpk_sync_compare_exchange(nCount, value, comparand)	(__sync_val_compare_and_swap(&(nCount), (comparand), (value)))
#	elif !defined(GPK_WINDOWS)
#		define gpk_sync_increment(nCount)							(++(nCount))													//	include <atomic>
#		define gpk_sync_decrement(nCount)							(--(nCount))													//	define gpk__sync_increment(nCount)			::std::atomic_fetch_add(&nCount, 1)
#		define gpk_sync_exchange(Target, Value)						((Target) = (Value))											//	define gpk__sync_decrement(nCount)			::std::atomic_fetch_sub(&nCount, 1)
#		define gpk_sync_compare_exchange(target, value, comparand)	((target) = ((target) == (comparand)) ? (value) : (target))		//	define gpk__sync_exchange(Target, Value)	((Target) = (Value))
#	elif defined(_WIN64) || defined(WIN64)
#		define gpk_sync_increment(nCount)							InterlockedIncrement64			(&(nCount))
#		define gpk_sync_decrement(nCount)							InterlockedDecrement64			(&(nCount))
#		define gpk_sync_exchange(target, value)						InterlockedExchange64			(&(target), (value))
#		define gpk_sync_compare_exchange(target, value, comparand)	InterlockedCompareExchange64	(&(target), (value),(comparand))
#	elif defined(_WIN32) || defined(WIN32)
#		define gpk_sync_increment(nCount)							InterlockedIncrement		(&(nCount))
#		define gpk_sync_decrement(nCount)							InterlockedDecrement		(&(nCount))
#		define gpk_sync_exchange(target, value)						InterlockedExchange			(&(target), (value))
#		define gpk_sync_compare_exchange(target, value, comparand)	InterlockedCompareExchange	(&(target), (value),(comparand))
#	endif
#endif
}

#endif // GPK_SYNC_H_23627


