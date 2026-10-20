/// Copyright 2016-2024 - ogarnd
#include "gpk_eval.h"

#if defined GPK_ATMEL
#	include <string.h>
#	include <stdarg.h>
#	include <stdio.h>
#else
#	include <cstring>
#	include <cstdarg>
#	include <cstdio>
#endif

#ifndef GPK_CSTRING_H_23627
#define GPK_CSTRING_H_23627

namespace gpk
{
#if defined(GPK_WINDOWS)
#	pragma warning(disable : 4996)
#endif
	stin	int	strcat_s		(sc_t * dst, u2_t bufferSize, sc_c * src)										{
		if((::gpk::u2_t)strlen(dst)+(::gpk::u2_t)strlen(src)+1U > (::gpk::u2_t)bufferSize)
			return -1;
		strcat(dst, src);
		return 0;
	}

	stin	int	strcpy_s		(sc_t * dst, u2_t bufferSize, sc_c * src)										{
		if((::gpk::u2_t)strlen(src)+1U > (::gpk::u2_t)bufferSize)
			return -1;
		strcpy(dst, src);
		return 0;
	}

	stin	int	strncpy_s		(sc_t * dst, sc_c * src, u2_t bufferSize)										{
		//if((::gpk::u2_t)strlen(src)+1U > (::gpk::u2_t)bufferSize)
		//	return -1;
		strncpy(dst, src, bufferSize);
		return 0;
	}

	stin	int	_snprintf_s		(sc_t * buffer, u2_t bufferSize, u2_t count, sc_c * format, ...)			{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= vsnprintf(buffer, count < bufferSize ? count + 1 : bufferSize, format, args);
		va_end(args);
		return result;
	}

	tplt<u2_t _bufferSize>
	stin	int	_snprintf_s		(char (&buffer)[_bufferSize], u2_t count, sc_c * format, ...)				{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= vsnprintf(buffer, count < _bufferSize ? count + 1 : _bufferSize, format, args);
		va_end(args);
		return result;
	}

	//stin	int	sprintf_s		(sc_t * buffer, u2_t bufferSize, sc_c * format, ...)							{
	//	va_list			args			= {};
	//	va_start(args, format);
	//	const int		result			= vsprintf(buffer, format, args);
	//	va_end(args);
	//	return result;
	//}

#ifdef GPK_ATMEL
	stin	int	vsprintf_s		(sc_t * buffer, u2_t bufferSize, sc_c * format, ...)							{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= ::vsnprintf(buffer, bufferSize, format, args);
		va_end(args);
		return result;
	}

	tplt<u2_t _bufferSize>
	stin	int	sprintf_s		(char (&buffer)[_bufferSize], sc_c * format, ...)								{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= ::vsnprintf(buffer, _bufferSize, format, args);
		va_end(args);
		return result;
	}

	stin	int	sprintf_s		(sc_t * buffer , ::gpk::u2_t bufferSize, sc_c * format, ...)								{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= ::vsnprintf(buffer, bufferSize, format, args);
		va_end(args);
		return result;
	}
#else
	stin	int	vsprintf_s		(sc_t * buffer, u2_t bufferSize, sc_c * format, ...)							{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= std::vsnprintf(buffer, bufferSize, format, args);
		va_end(args);
		return result;
	}

	tplt<u2_t _bufferSize>
	stin	int	sprintf_s		(char (&buffer)[_bufferSize], sc_c * format, ...)								{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= std::vsnprintf(buffer, _bufferSize, format, args);
		va_end(args);
		return result;
	}

	stin	int	sprintf_s		(sc_t * buffer , u2_t bufferSize, sc_c * format, ...)								{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= std::vsnprintf(buffer, bufferSize, format, args);
		va_end(args);
		return result;
	}
#endif
	tplt<u2_t _Size> stin	int	strcat_s		( char (&dst)[_Size], sc_c * src )						{ return strcat_s	(dst, _Size, src);				}
	tplt<u2_t _Size> stin	int	strcpy_s		( char (&dst)[_Size], sc_c * src )						{ return strcpy_s	(dst, _Size, src);				}
	tplt<u2_t _Size> stin	int	strncpy_s		( char (&dst)[_Size], sc_c * src )						{ return strncpy_s	(dst, src, _Size);				}
	stin	int	_vsnprintf_s	( sc_t * buffer, u2_t bufferSize, u2_t count, sc_c * format, va_list args )	{ return vsnprintf	(buffer, count < bufferSize ? count + 1 : bufferSize, format, args);	}
	stin	int	vsprintf_s		( sc_t * buffer, u2_t bufferSize, sc_c * format, va_list args )				{ return vsnprintf	(buffer, bufferSize, format, args);			}
#if defined(GPK_WINDOWS)
#	pragma warning(default: 4996)
#endif
} // namespace

#endif // GPK_CSTRING_H_23627


