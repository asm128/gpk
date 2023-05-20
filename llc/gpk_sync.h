#include "gpk_typeint.h"
#include "gpk_error.h"

#ifndef GPK_SYNC_H_023749082374
#define GPK_SYNC_H_023749082374

#if defined(GPK_ANDROID) || defined(GPK_LINUX)
#	include <thread>
#elif defined(GPK_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif
namespace gpk
{
#if defined(GPK_ANDROID) || defined(GPK_LINUX)
/// <Atomic Builtins> http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Atomic-Builtins.html#Atomic-Builtins
#	define gpk__sync_increment(nCount)							(__sync_add_and_fetch(&nCount, 1))
#	define gpk__sync_decrement(nCount)							(__sync_sub_and_fetch(&nCount, 1))
#	define gpk__sync_exchange(target, value)					(__sync_lock_test_and_set(&(target), (value)))
#	define gpk__sync_compare_exchange(nCount, value, comparand)	(__sync_val_compare_and_swap(&(nCount), (comparand), (value)))
#	define DECLARE_SHARED_SECTION(Name)							::std::mutex Name //(true)
#	define INIT_SHARED_SECTION(Name)							(1)
#	define ENTER_SHARED_SECTION(Name)							(Name).lock()
#	define LEAVE_SHARED_SECTION(Name)							(Name).unlock()
#	define DELETE_SHARED_SECTION(Name)							(0)
	stainli ::gpk::error_t								sleep							(uint32_t milliseconds)		noexcept	{ std::this_thread::sleep_for (std::chrono::milliseconds(milliseconds)); return 0; }
#elif defined(GPK_WINDOWS)
#	if (defined( _WIN64 ) || defined( WIN64 ))
#		define gpk__sync_increment(nCount)							( InterlockedIncrement64		( &(nCount) ) )
#		define gpk__sync_decrement(nCount)							( InterlockedDecrement64		( &(nCount) ) )
#		define gpk__sync_exchange(target, value)					( InterlockedExchange64			( &(target), (value) ) )
#		define gpk__sync_compare_exchange(target, value, comparand)	( InterlockedCompareExchange64	( &(target), (value), (comparand) ) )
#	elif (defined( _WIN32 ) || defined( WIN32 ))
#		define gpk__sync_increment(nCount)							( InterlockedIncrement			( &(nCount) ) )
#		define gpk__sync_decrement(nCount)							( InterlockedDecrement			( &(nCount) ) )
#		define gpk__sync_exchange(target, value)					( InterlockedExchange			( &(target), (value) ) )
#		define gpk__sync_compare_exchange(target, value, comparand)	( InterlockedCompareExchange	( &(target), (value), (comparand) ) )
#	endif
#	define DECLARE_SHARED_SECTION(Name)							CRITICAL_SECTION Name
#	define INIT_SHARED_SECTION(Name)							((int32_t)InitializeCriticalSectionAndSpinCount(&Name, 400))
#	define ENTER_SHARED_SECTION(Name)							EnterCriticalSection (&Name)
#	define LEAVE_SHARED_SECTION(Name)							LeaveCriticalSection (&Name)
#	define DELETE_SHARED_SECTION(Name)							DeleteCriticalSection(&Name)
	stainli ::gpk::error_t								sleep							(uint32_t milliseconds)		noexcept	{ Sleep(milliseconds); return 0; }
#else
#	define gpk__sync_increment(nCount)								(++(nCount))
#	define gpk__sync_decrement(nCount)								(--(nCount))
#	define gpk__sync_exchange(Target, Value)							((Target) = (Value))
#	define DECLARE_SHARED_SECTION(Name)							::std::mutex Name //(true)
#	define INIT_SHARED_SECTION(Name)							(1)
#	define ENTER_SHARED_SECTION(Name)							(Name).lock()
#	define LEAVE_SHARED_SECTION(Name)							(Name).unlock()
#	define DELETE_SHARED_SECTION(Name)							(0)
	stainli ::gpk::error_t								sleep							(uint32_t milliseconds)		noexcept	{ std::this_thread::sleep_for (std::chrono::milliseconds(milliseconds)); return 0; }
#endif

#if defined(GPK_MTSUPPORT)
#	define DECLARE_CRITICAL_SECTION								DECLARE_SHARED_SECTION
#	define INIT_CRITICAL_SECTION								INIT_SHARED_SECTION
#	define ENTER_CRITICAL_SECTION								ENTER_SHARED_SECTION
#	define LEAVE_CRITICAL_SECTION								LEAVE_SHARED_SECTION
#	define DELETE_CRITICAL_SECTION								DELETE_SHARED_SECTION
#	if defined(GPK_ANDROID) || defined(GPK_LINUX)
#		define gpk_sync_increment									gpk__sync_increment
#		define gpk_sync_decrement									gpk__sync_decrement
#		define gpk_sync_exchange									gpk__sync_exchange
#		define gpk_sync_compare_exchange							gpk__sync_compare_exchange
#	elif defined(GPK_WINDOWS)
#		if (defined( _WIN64 ) || defined( WIN64 ))
#			define gpk_sync_increment									gpk__sync_increment
#			define gpk_sync_decrement									gpk__sync_decrement
#			define gpk_sync_exchange									gpk__sync_exchange
#			define gpk_sync_compare_exchange							gpk__sync_compare_exchange
#		elif (defined( _WIN32 ) || defined( WIN32 ))
#			define gpk_sync_increment									gpk__sync_increment
#			define gpk_sync_decrement									gpk__sync_decrement
#			define gpk_sync_exchange									gpk__sync_exchange
#			define gpk_sync_compare_exchange							gpk__sync_compare_exchange
#		endif
#	else
#		define gpk_sync_increment									gpk__sync_increment
#		define gpk_sync_decrement									gpk__sync_decrement
#		define gpk_sync_exchange									gpk__sync_exchange
#		define gpk_sync_compare_exchange							gpk__sync_compare_exchange
#	endif
#else
#	define gpk_sync_increment(nCount)								(++(nCount))
#	define gpk_sync_decrement(nCount)								(--(nCount))
#	define gpk_sync_exchange(target, value)							((target) = (value))
#	define gpk_sync_compare_exchange(target, value, comparand)	((target) = ((target) == (comparand)) ? (value) : (target))

#	define DECLARE_CRITICAL_SECTION(...)
#	define INIT_CRITICAL_SECTION(...)								(1)
#	define ENTER_CRITICAL_SECTION(...)								(0)
#	define LEAVE_CRITICAL_SECTION(...)								(0)
#	define DELETE_CRITICAL_SECTION(...)								do {} while(false)
#endif
}

#endif // GPK_SYNC_H_023749082374
