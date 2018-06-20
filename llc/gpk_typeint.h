#include "gpk_platform.h"

#include <cstdint>

#ifndef GPK_TYPEINT_H_29873490827342
#define GPK_TYPEINT_H_29873490827342

typedef					signed char						schar_t	, sbyte_t	;
typedef					char							char_t	, byte_t	;
typedef					unsigned char					uchar_t	, ubyte_t	;

namespace gpk
{
	static constexpr		const uint8_t					PLATFORM_BYTE_BIT_COUNT								= 8;
#if defined GPK_WINDOWS
#	if defined(_WIN64) || defined(WIN64)		
#		define	REFCOUNT_T								long long
		typedef volatile REFCOUNT_T						refcount_t			;
#	else
#		define	REFCOUNT_T								long
		typedef volatile REFCOUNT_T						refcount_t			;
#	endif
#else
#	define	REFCOUNT_T								intptr_t
	typedef REFCOUNT_T								refcount_t			;
#endif
}

#endif // GPK_TYPEINT_H_29873490827342
