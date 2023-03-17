/// Copyright 2016-2017 - asm128
#include "gpk_platform.h"

#if defined GPK_ATMEL
#	include <string.h>
#else
#	include <cstring>
#endif

#ifndef GPK_STRING_H_56847984984
#define GPK_STRING_H_56847984984

#include <cstddef>
#include <cstdarg>
#include <cstdio>

namespace gpk
{
#if defined(GPK_WINDOWS)
#	pragma warning(disable : 4996)
#endif
	stainli	int	strcat_s		(char *dst, size_t bufferSize, const char *src)										{
		if((uint32_t)strlen(dst)+(uint32_t)strlen(src)+1U > (uint32_t)bufferSize)
			return -1;
		strcat(dst, src);
		return 0;
	}

	stainli	int	strcpy_s		(char *dst, size_t bufferSize, const char *src)										{
		if((uint32_t)strlen(src)+1U > (uint32_t)bufferSize)
			return -1;
		strcpy(dst, src);
		return 0;
	}

	stainli	int	strncpy_s		(char *dst, const char *src, size_t bufferSize)										{
		//if((uint32_t)strlen(src)+1U > (uint32_t)bufferSize)
		//	return -1;
		strncpy(dst, src, bufferSize);
		return 0;
	}

	stainli	int	_snprintf_s		(char* buffer, size_t bufferSize, size_t count, const char* format, ...)			{
		va_list args;
		va_start(args, format);
		const int									result			= vsnprintf( buffer, ::gpk::max(count, bufferSize - 1), format, args );
		va_end(args);
		return result;
	}

	template<size_t _bufferSize>
	stainli	int	_snprintf_s		(char (&buffer)[_bufferSize], size_t count, const char* format, ...)				{
		va_list args;
		va_start(args, format);
		const int									result			= _snprintf_s( buffer, _bufferSize, count, format, args );
		va_end(args);
		return result;
	}

	//stainli	int	sprintf_s		(char *buffer, size_t bufferSize, const char *format, ...)							{
	//	va_list args;
	//	va_start(args, format);
	//	const int									result			= vsprintf(buffer, format, args);
	//	va_end(args);
	//	return result;
	//}

	stainli	int	vsprintf_s		(char *buffer, size_t bufferSize, const char *format, ...)							{
		va_list args;
		va_start(args, format);
		const int									result			= std::vsnprintf(buffer, bufferSize - 1, format, args);
		va_end(args);
		return result;
	}

	template<size_t _bufferSize>
	stainli	int	sprintf_s		(char (&buffer)[_bufferSize], const char* format, ...)								{
		va_list args;
		va_start(args, format);
		const int									result			= vsprintf_s(buffer, _bufferSize, format, args);
		va_end(args);
		return result;
	}

	stainli	int	sprintf_s		(char *buffer , uint32_t bufferSize, const char* format, ...)								{
		va_list args;
		va_start(args, format);
		const int									result			= vsprintf_s(buffer, bufferSize, format, args);
		va_end(args);
		return result;
	}

	template<size_t _Size> stainli	int	strcat_s		( char (&dst)[_Size], const char *src )												{ return strcat_s	(dst, _Size, src);				}
	template<size_t _Size> stainli	int	strcpy_s		( char (&dst)[_Size], const char *src )												{ return strcpy_s	(dst, _Size, src);				}
	template<size_t _Size> stainli	int	strncpy_s		( char (&dst)[_Size], const char *src )												{ return strncpy_s	(dst, src, _Size);				}
	stainli	int	_vsnprintf_s	( char* buffer, size_t bufferSize, size_t count, const char* format, va_list args )	{ return vsnprintf	(buffer, ::gpk::max(count, bufferSize - 1), format, args);	}
	stainli	int	vsprintf_s		( char *buffer, size_t bufferSize, const char *format, va_list args )				{ return vsnprintf	(buffer, bufferSize - 1, format, args);			}
#if defined(GPK_WINDOWS)
#	pragma warning(default: 4996)
#endif
} // namespace

#endif // GPK_STRING_H_56847984984
