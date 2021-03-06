#include "gpk_log.h"
#include "gpk_string.h"


#if defined(GPK_ANDROID)
#	include <android/log.h>
#endif
#if defined(GPK_WINDOWS)
#	include <Windows.h>
#else
#	include <errno.h>
#endif
#include <cstdio>


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO , "gpk_app", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "gpk_app", __VA_ARGS__))

// For debug builds, always enable the debug traces in this library
#ifndef NDEBUG
#	define LOGV(...)  ((void)__android_log_print(ANDROID_LOG_VERBOSE, "gpk_app", __VA_ARGS__))
#else
#	define LOGV(...)  ((void)0)
#endif

		::gpk::debug_print_t	gpk_debug_printf				= {};

		void					gpk::_base_debug_print			(const char* text, uint32_t textLen)									{
	if(textLen) {
		if(gpk_debug_printf)
			::gpk_debug_printf(text, textLen);
		else {
#if defined(GPK_WINDOWS)
			OutputDebugStringA(text);
#elif defined(GPK_ANDROID)
			LOGI("%s", text);
#elif defined(GPK_ST)
#else
			printf("%s", text);
#endif
		}
	}
}

static	::gpk::error_t			getSystemErrorAsString			(const uint64_t lastError, char* buffer, uint32_t bufferSize)			{	// Get the error message, if any.
#if defined(GPK_WINDOWS)
	if(nullptr == buffer)
		return -1;
	if(0 == lastError)
		return 0;
	const DWORD							size							= ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)(lastError & 0xFFFFFFFF), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, bufferSize, NULL);
	return (::gpk::error_t)size;
#else
	(void) bufferSize;
	sprintf(buffer, "%u.", (uint32_t)lastError);
	return 0;
#endif
}

		void					gpk::_gpk_print_system_errors	(const char* prefix, uint32_t prefixLen)								{
	char								bufferError[4096]				= {};
#if defined(GPK_WINDOWS)
	int64_t								lastSystemError					= ::GetLastError() & 0xFFFFFFFFFFFFFFFFLL;
#else
	int64_t								lastSystemError					= -1;
#endif
	if(lastSystemError) {
		::gpk::error_t						stringLength					= ::getSystemErrorAsString((uint64_t)lastSystemError, bufferError, ::gpk::size(bufferError));
		base_debug_print(prefix, prefixLen);
		base_debug_print(bufferError, (uint32_t)stringLength);
		base_debug_print("\n", 1);
	}
	lastSystemError					= errno;
	if(lastSystemError) {
#if defined(GPK_WINDOWS)
		::strerror_s(bufferError, (int)lastSystemError);
#else
		strcpy_s(bufferError, ::strerror((int)lastSystemError));
#endif
		char								bufferError2[4096]				= {};
#if defined(GPK_WINDOWS)
		size_t								stringLength					= ::snprintf(bufferError2, ::gpk::size(bufferError2) - 2, "Last system error: 0x%llX '%s'.", lastSystemError, bufferError);
#else
		size_t								stringLength					= ::snprintf(bufferError2, ::gpk::size(bufferError2) - 2, "Last system error: 0x%llX '%s'.", (unsigned long long)lastSystemError, bufferError);
#endif
		base_debug_print(prefix, prefixLen);
		base_debug_print(bufferError2, (uint32_t)stringLength);
		base_debug_print("\n", 1);
	}
}
