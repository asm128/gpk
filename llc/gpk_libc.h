#include "gpk_typeint.h"

#ifndef GPK_LIBC_H_23627
#define GPK_LIBC_H_23627

nmsp gpk
{
//
	void	free		(void* memory);
	void*	memalign	(uP_t alignment, uP_t size);
	stin err_t memcmp	(cnst void* left, cnst void* right, uP_t size) {
		u0_c* const bytesLeft	= (u0_c*)left;
		u0_c* const bytesRight	= (u0_c*)right;
		for(uP_t iByte = 0; iByte < size; ++iByte)
			if(bytesLeft[iByte] != bytesRight[iByte])
				return (err_t)bytesLeft[iByte] - (err_t)bytesRight[iByte];
		return 0;
	}
	stin void* memcpy	(void* destination, cnst void* source, uP_t size) {
		u0_t* const bytesDestination	= (u0_t*)destination;
		u0_c* const bytesSource		= (u0_c*)source;
		for(uP_t iByte = 0; iByte < size; ++iByte)
			bytesDestination[iByte] = bytesSource[iByte];
		return destination;
	}
	void*	memmove		(void* destination, cnst void* source, uP_t size);
	stin void* memset	(void* destination, int value, uP_t size) {
		u0_t* const bytesDestination	= (u0_t*)destination;
		for(uP_t iByte = 0; iByte < size; ++iByte)
			bytesDestination[iByte] = (u0_t)value;
		return destination;
	}
//
	sc_t*	strerror	(err_t errorCode);
	uP_t	strlen		(sc_c* text);
	err_t	strncmp		(sc_c* left, sc_c* right, uP_t count);
	err_t	tolower		(err_t character);
	//
	f3_t	pow			(f3_t base, f3_t exponent);
	err_t	rand		();
	f3_t	sin			(f3_t value);
	f3_t	acos		(f3_t value);
	f3_t	cos			(f3_t value);
	void	sincos		(f3_t value, f3_t* sine, f3_t* cosine);
	f2_t	sinf		(f2_t value);
	f3_t	sqrt		(f3_t value);
	void	srand		(u2_t seed);
	//
	struct time_t;
	::time_t time		(::time_t* output);
	sc_t*	setlocale	(int category, sc_c* locale);
	::va_list;
	err_t	vsnprintf	(sc_t* buffer, uP_t bufferSize, sc_c* format, ::va_list arguments);
//
	struct FILE;
	::FILE*	fopen		(sc_c* filename, sc_c* mode);
	err_t	fclose		(::FILE* stream);
	uP_t	fread		(void* buffer, uP_t elementSize, uP_t elementCount, ::FILE* stream);
	uP_t	fwrite		(cnst void* buffer, uP_t elementSize, uP_t elementCount, ::FILE* stream);
	err_t	fseek		(::FILE* stream, long offset, int origin);
	long	ftell		(::FILE* stream);
}

#endif // GPK_LIBC_H_23627
