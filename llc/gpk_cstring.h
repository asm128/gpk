/// Copyright 2016-2017 - asm128
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

#ifndef GPK_CSTRING_H
#define GPK_CSTRING_H

namespace gpk
{
#if defined(GPK_WINDOWS)
#	pragma warning(disable : 4996)
#endif
	stainli	int	strcat_s		(sc_t *dst, size_t bufferSize, const sc_t *src)										{
		if((uint32_t)strlen(dst)+(uint32_t)strlen(src)+1U > (uint32_t)bufferSize)
			return -1;
		strcat(dst, src);
		return 0;
	}

	stainli	int	strcpy_s		(sc_t *dst, size_t bufferSize, const sc_t *src)										{
		if((uint32_t)strlen(src)+1U > (uint32_t)bufferSize)
			return -1;
		strcpy(dst, src);
		return 0;
	}

	stainli	int	strncpy_s		(sc_t *dst, const sc_t *src, size_t bufferSize)										{
		//if((uint32_t)strlen(src)+1U > (uint32_t)bufferSize)
		//	return -1;
		strncpy(dst, src, bufferSize);
		return 0;
	}

	stainli	int	_snprintf_s		(sc_t* buffer, size_t bufferSize, size_t count, const sc_t* format, ...)			{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= vsnprintf( buffer, max(count, bufferSize - 1), format, args );
		va_end(args);
		return result;
	}

	tplt<size_t _bufferSize>
	stainli	int	_snprintf_s		(sc_t (&buffer)[_bufferSize], size_t count, const sc_t* format, ...)				{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= _snprintf_s( buffer, _bufferSize, count, format, args );
		va_end(args);
		return result;
	}

	//stainli	int	sprintf_s		(sc_t *buffer, size_t bufferSize, const sc_t *format, ...)							{
	//	va_list			args			= {};
	//	va_start(args, format);
	//	const int		result			= vsprintf(buffer, format, args);
	//	va_end(args);
	//	return result;
	//}

#ifdef GPK_ATMEL
	stainli	int	vsprintf_s		(sc_t *buffer, size_t bufferSize, const sc_t *format, ...)							{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= ::vsnprintf(buffer, bufferSize - 1, format, args);
		va_end(args);
		return result;
	}

	tplt<size_t _bufferSize>
	stainli	int	sprintf_s		(sc_t (&buffer)[_bufferSize], const sc_t* format, ...)								{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= ::vsnprintf(buffer, _bufferSize - 1, format, args);
		va_end(args);
		return result;
	}

	stainli	int	sprintf_s		(sc_t *buffer , uint32_t bufferSize, const sc_t* format, ...)								{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= ::vsnprintf(buffer, bufferSize - 1, format, args);
		va_end(args);
		return result;
	}
#else
	stainli	int	vsprintf_s		(sc_t *buffer, size_t bufferSize, const sc_t *format, ...)							{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= std::vsnprintf(buffer, bufferSize - 1, format, args);
		va_end(args);
		return result;
	}

	tplt<size_t _bufferSize>
	stainli	int	sprintf_s		(sc_t (&buffer)[_bufferSize], const sc_t* format, ...)								{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= std::vsnprintf(buffer, _bufferSize - 1, format, args);
		va_end(args);
		return result;
	}

	stainli	int	sprintf_s		(sc_t *buffer , uint32_t bufferSize, const sc_t* format, ...)								{
		va_list			args			= {};
		va_start(args, format);
		const int		result			= std::vsnprintf(buffer, bufferSize - 1, format, args);
		va_end(args);
		return result;
	}
#endif
	tplt<size_t _Size> stainli	int	strcat_s		( sc_t (&dst)[_Size], const sc_t *src )						{ return strcat_s	(dst, _Size, src);				}
	tplt<size_t _Size> stainli	int	strcpy_s		( sc_t (&dst)[_Size], const sc_t *src )						{ return strcpy_s	(dst, _Size, src);				}
	tplt<size_t _Size> stainli	int	strncpy_s		( sc_t (&dst)[_Size], const sc_t *src )						{ return strncpy_s	(dst, src, _Size);				}
	stainli	int	_vsnprintf_s	( sc_t* buffer, size_t bufferSize, size_t count, const sc_t* format, va_list args )	{ return vsnprintf	(buffer, max(count, bufferSize - 1), format, args);	}
	stainli	int	vsprintf_s		( sc_t *buffer, size_t bufferSize, const sc_t *format, va_list args )				{ return vsnprintf	(buffer, bufferSize - 1, format, args);			}
#if defined(GPK_WINDOWS)
#	pragma warning(default: 4996)
#endif
} // namespace

#endif // GPK_CSTRING_H
