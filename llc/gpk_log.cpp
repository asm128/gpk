#include "gpk_log.h"
#include "gpk_chrono.h"

GPK_USING_TYPEINT();

#ifdef GPK_ATMEL
#	include <stdio.h>
#else
#	include <cstdio>
#endif

#ifdef GPK_WINDOWS
#	include <Windows.h>
#elif defined(GPK_ARDUINO)
#	include <Arduino.h>
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

#ifdef GPK_ATMEL
stxp		int		LOG_PREFIX_BUFFER_SIZE	= 64;
#else
stxp		int		LOG_PREFIX_BUFFER_SIZE	= 256;
#endif

gpk::err_t 		gpk::debug_print_prefix				(s0_t severity, const char * path, ::gpk::u2_t line, const char * function) {
	stxp sc_c	STR_DEBUG_PREFIX[]					= "%i|%llu|%s(%" GPK_FMT_U2 "){%s}:";
	char			formatted[::LOG_PREFIX_BUFFER_SIZE]	= {};
	::gpk::sprintf_s(formatted, STR_DEBUG_PREFIX, severity, ::gpk::timeCurrentInMs(), path, line, function);
	return log_print(formatted);
}

#if defined(GPK_WINDOWS)
static	gpk::err_t default_log_write		(const char * text, ::gpk::u2_t textLen) 	{  u2_t iChar = 0; for(; iChar < textLen; ++iChar) { sc_c buf[2] = {text[iChar], 0}; OutputDebugStringA(buf); } return (::gpk::err_t)fprintf(stderr, "%.*s", (int)textLen, text); }
static	gpk::err_t default_log_print		(const char * text)						{ OutputDebugStringA(text); return (::gpk::err_t)fprintf(stderr, "%s", text); }
#elif defined(GPK_ANDROID)
static	gpk::err_t default_log_write		(const char * text, ::gpk::u2_t textLen)	{ LOGI("%.*s", (int)textLen, text); return textLen; }
static	gpk::err_t default_log_print		(const char * text)						{ LOGI("%s", text); return (::gpk::err_t)strlen(text); }
#elif defined(GPK_ARDUINO)
static	gpk::err_t default_log_write		(const char * text, ::gpk::u2_t textLen)	{ return Serial ? Serial.write(text, textLen) : textLen; }
static	gpk::err_t default_log_print		(const char * text)						{ return Serial ? Serial.print(text) : (::gpk::err_t)strlen(text); }
#else
static	gpk::err_t default_log_write		(const char * text, ::gpk::u2_t textLen)	{ return (::gpk::err_t)fprintf(stderr, "%.*s", (int)textLen, text); }
static	gpk::err_t default_log_print		(const char * text)						{ return (::gpk::err_t)fprintf(stderr, "%s", text); }
#endif

::gpk::log_write_t		gpk_log_write					= default_log_write;
::gpk::log_print_t		gpk_log_print					= default_log_print;

::gpk::err_t gpk::getLogCallbacks(log_print_t & funcLogPrint, log_write_t & funcLogWrite) {
	funcLogPrint = ::gpk_log_print;
	funcLogWrite = ::gpk_log_write;
	return 0;
}

gpk::err_t 		gpk::log_print					(const char * text)						{ return (gpk_log_print && text) ? ::gpk_log_print(text) : 0; }
gpk::err_t 		gpk::log_write					(const char * text, ::gpk::u2_t textLen)	{ return (gpk_log_write && text && textLen) ? ::gpk_log_write(text, textLen) : 0; }
#ifdef GPK_LOG_ARDUINO_FLASHSTRINGHELPER
gpk::err_t 		gpk::log_print_P				(const __FlashStringHelper * text)		{ return (Serial && text) ? (::gpk::err_t)Serial.print(text) : 0; }
#endif

gpk::err_t 		gpk::setupDefaultLogCallbacks	()	{ return setupLogCallbacks(default_log_print, default_log_write); }

gpk::err_t 		gpk::setupLogCallbacks
	( gpk::log_print_t	funcLogPrint
	, gpk::log_write_t	funcLogWrite
	) {
	::gpk_log_print	= funcLogPrint;
	::gpk_log_write	= funcLogWrite;

	struct gpk_debug_checker_struct {
		gpk_debug_checker_struct() {
			info_printf("DYNAMIC_LIBRARY_EXTENSION:\"" GPK_DYNAMIC_LIBRARY_EXTENSION "\".");
#	ifdef GPK_ARDUINO
			info_printf("ARDUINO"       );
#	endif // GPK_ARDUINO
#	ifdef GPK_DEBUG_ENABLED
			info_printf("DEBUG_ENABLED" );
#	endif // GPK_DEBUG_ENABLED
#	ifdef GPK_ESP32
			info_printf("ESP32"         );
#	endif // GPK_ESP32
#	ifdef GPK_ESP8266
			info_printf("ESP8266"       );
#	endif // GPK_ESP8266
#	ifdef GPK_ATMEL
			info_printf("ATMEL"         );
#	endif // GPK_ATMEL
#	ifdef GPK_ESPIDF
			info_printf("ESPIDF"        );
#	endif // GPK_ESPIDF
#	ifdef GPK_WINDOWS
			info_printf("WINDOWS"       );
#	endif // GPK_WINDOWS
#	ifdef GPK_LINUX
			info_printf("LINUX"         );
#	endif // GPK_LINUX
#	ifdef GPK_ANDROID
			info_printf("ANDROID"       );
#	endif // GPK_ANDROID
#	ifdef USE_DEBUG_BREAK_ON_ERROR_LOG
			info_printf("USE_DEBUG_BREAK_ON_ERROR_LOG");
#	endif // USE_DEBUG_BREAK_ON_ERROR_LOG
#	ifdef GPK_MTSUPPORT
			info_printf("MTSUPPORT"     );
#	endif // GPK_MTSUPPORT
#	ifdef GPK_DISABLE_CPP_EXCEPTIONS
			info_printf("DISABLE_CPP_EXCEPTIONS");
#	endif // GPK_DISABLE_CPP_EXCEPTIONS
#	ifdef NOMINMAX
			info_printf("NOMINMAX");
#	endif // NOMINMAX
#	ifdef WIN32_LEAN_AND_MEAN
			info_printf("WIN32_LEAN_AND_MEAN");
#	endif // WIN32_LEAN_AND_MEAN
#	ifdef __GNUC__
			info_printf("__GNUC__");
#	endif // __GNUC__
		}
	} gpk_debug_checker_instance = {};
	(void)gpk_debug_checker_instance;
	return 0;
}

#ifndef GPK_ARDUINO

static	gpk::err_t getSystemErrorAsString			(const u3_t lastError, char* buffer, ::gpk::u2_t bufferSize)			{	// Get the error message, if any.
#ifdef GPK_WINDOWS
	if_true_fe(0 == buffer);
	return lastError
		? ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)(lastError & 0xFFFFFFFF), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, bufferSize, NULL)
		: 0
		;
#else
	return ::gpk::sprintf_s(buffer, bufferSize, "%" GPK_FMT_U2 ".", (::gpk::u2_t)lastError);
#endif
}

void					gpk::_gpk_print_system_errors	(const char* prefix, ::gpk::u2_t prefixLen)								{
	char								bufferError[256]				= {};
#ifdef GPK_WINDOWS
	s3_t								lastSystemError					= ::GetLastError() & 0xFFFFFFFFFFFFFFFFLL;
#else
	s3_t								lastSystemError					= -1;
#endif
	if(lastSystemError) {
		log_write("\n", 1);
		gpk::err_t 					stringLength					= ::getSystemErrorAsString((u3_t)lastSystemError, bufferError, ::gpk::size(bufferError));
		log_write(prefix, prefixLen);
		log_write(bufferError, (::gpk::u2_t)stringLength);
		log_write("\n", 1);
	}
	lastSystemError					= errno;
	if(lastSystemError) {
		log_write("\n", 1);
#ifdef GPK_WINDOWS
		::strerror_s(bufferError, (s2_t)lastSystemError);
		{
#else
		const char * serr = ::strerror((s2_t)lastSystemError);
		if(serr) {
			strcpy_s(bufferError, serr);
#endif
			char		bufferError2[256]	= {};
#ifdef GPK_WINDOWS
			cnst ::gpk::err_t formattedLength	= ::gpk::sprintf_s(bufferError2, "Last system error: 0x%llX '%s'.", lastSystemError, bufferError);
#else
			cnst ::gpk::err_t formattedLength	= ::gpk::sprintf_s(bufferError2, "Last system error: 0x%llX '%s'.", (unsigned long long)lastSystemError, bufferError);
#endif
			cnst u2_t	stringLength		= 0 < formattedLength ? ::gpk::min((u2_t)formattedLength, ::gpk::size(bufferError2) - 1) : 0;
			log_write(prefix, prefixLen);
			log_write(bufferError2, stringLength);
			log_write("\n", 1);
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
