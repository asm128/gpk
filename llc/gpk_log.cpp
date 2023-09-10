#include "gpk_log.h"
#include "gpk_string.h"
#include "gpk_size.h"

#ifdef GPK_ATMEL
#	include <stdio.h>
#else
#	include <cstdio>
#endif

#ifdef GPK_WINDOWS
#	include <Windows.h>
#else
#	include <errno.h>
#	ifdef GPK_ANDROID
#		include <android/log.h>
#		define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO , "gpk_app", __VA_ARGS__))
#		define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "gpk_app", __VA_ARGS__))
#		ifndef NDEBUG
#			define LOGV(...)  ((void)__android_log_print(ANDROID_LOG_VERBOSE, "gpk_app", __VA_ARGS__))
#		else
#			define LOGV(...)  ((void)0)
#		endif
#	endif
#endif

#ifdef GPK_LOG_ARDUINO_FLASHSTRINGHELPER
#	include <Stream.h>
extern Stream & GPKLogStream;
#endif

static void default_base_log_write(const char * text, uint32_t textLen) {
#if defined(GPK_WINDOWS)
	OutputDebugStringA(text); (void)textLen;
#elif defined(GPK_ANDROID)
	LOGI("%s", text); (void)textLen;
#elif defined(GPK_ST)
#elif defined(GPK_ARDUINO)
	GPKLogStream.write(text, textLen);
#elif defined(GPK_ATMEL)
#elif defined(GPK_ESP32)
#else
	printf("%s", text); (void)textLen;
#endif
}

static void default_base_log_print(const char * text) {
#if defined(GPK_WINDOWS)
	OutputDebugStringA(text);
#elif defined(GPK_ANDROID)
	LOGI("%s", text);
#elif defined(GPK_ST)
#elif defined(GPK_ARDUINO)
	GPKLogStream.print(text);
#elif defined(GPK_ATMEL)
#elif defined(GPK_ESP32)
#else
	printf("%s", text);
#endif
}

::gpk::log_write_t		gpk_log_write					= default_base_log_write;
::gpk::log_print_t		gpk_log_print					= default_base_log_print;
void					gpk::_base_log_print			(const char* text)						{ if(gpk_log_print && text) ::gpk_log_print(text); }
void					gpk::_base_log_write			(const char* text, uint32_t textLen)	{ if(gpk_log_write && text && textLen) ::gpk_log_write(text, textLen); }

#ifdef GPK_LOG_ARDUINO_FLASHSTRINGHELPER
static	void			default_base_log_print_P		(const __FlashStringHelper * text)		{ GPKLogStream.print(text); }
::gpk::log_print_P_t	gpk_log_print_P					= default_base_log_print_P;
void					gpk::_base_log_print_P			(const __FlashStringHelper* text)		{ if(gpk_log_print_P && text) ::gpk_log_print_P(text); }
#endif

#ifndef GPK_ARDUINO

static	::gpk::error_t	getSystemErrorAsString			(const uint64_t lastError, char* buffer, uint32_t bufferSize)			{	// Get the error message, if any.
#ifdef GPK_WINDOWS
	rees_if(0 == buffer);
	return lastError 
		? ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)(lastError & 0xFFFFFFFF), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, bufferSize, NULL)
		: 0
		;
#else
	(void) bufferSize;
	sprintf(buffer, "%u.", (uint32_t)lastError);
	return 0;
#endif
}

		void					gpk::_gpk_print_system_errors	(const char* prefix, uint32_t prefixLen)								{
	char								bufferError[256]				= {};
#if defined(GPK_WINDOWS)
	int64_t								lastSystemError					= ::GetLastError() & 0xFFFFFFFFFFFFFFFFLL;
#else
	int64_t								lastSystemError					= -1;
#endif
	if(lastSystemError) {
		base_log_write("\n", 1);
		::gpk::error_t						stringLength					= ::getSystemErrorAsString((uint64_t)lastSystemError, bufferError, ::gpk::size(bufferError));
		base_log_write(prefix, prefixLen);
		base_log_write(bufferError, (uint32_t)stringLength);
		base_log_write("\n", 1);
	}
	lastSystemError					= errno;
	if(lastSystemError) {
		base_log_write("\n", 1);
#ifdef GPK_WINDOWS
		::strerror_s(bufferError, (int)lastSystemError);
		{
#else
		const char * serr = ::strerror((int)lastSystemError);
		if(serr) {
			strcpy_s(bufferError, serr);
#endif
			char		bufferError2[256]	= {};
#ifdef GPK_WINDOWS
			size_t		stringLength		= ::snprintf(bufferError2, ::gpk::size(bufferError2) - 2, "Last system error: 0x%llX '%s'.", lastSystemError, bufferError);
#else
			size_t		stringLength		= ::snprintf(bufferError2, ::gpk::size(bufferError2) - 2, "Last system error: 0x%llX '%s'.", (unsigned long long)lastSystemError, bufferError);
#endif
			base_log_write(prefix, prefixLen);
			base_log_write(bufferError2, (uint32_t)stringLength);
			base_log_write("\n", 1);
		}
	}
#ifdef GPK_CLEAR_SYSTEM_ERROR_ON_ERROR_LOG
#	ifdef GPK_WINDOWS
#		define gpkClearSystemError() SetLastError(0)
#	else
#		define gpkClearSystemError() do{} while(0)
#	endif
#else
#	define gpkClearSystemError() do{} while(0)
#endif // GPK_CLEAR_SYSTEM_ERROR_ON_ERROR_LOG

	gpkClearSystemError();
}

#endif // GPK_ARDUINO
