/// Copyright 2010-2023 - asm128
#ifndef GPK_PLATFORM_GLOBALS_H_23627
#define GPK_PLATFORM_GLOBALS_H_23627

#if (defined(DEBUG) || defined(_DEBUG)) && (!defined(GPK_DISABLE_DEBUG)) && !defined(GPK_DEBUG_ENABLED)
#	define GPK_DEBUG_ENABLED
#	if !defined(USE_DEBUG_BREAK_ON_ERROR_LOG)
#		define USE_DEBUG_BREAK_ON_ERROR_LOG
#	endif
#endif	// _DEBUG

#if (!defined(GPK_MTSUPPORT)) && !defined(GPK_DISABLE_MTSUPPORT)
#	define GPK_MTSUPPORT
#endif

#if (!defined(GPK_ANDROID)) && (defined(ANDROID) || defined(__ANDROID__))
#	define GPK_ANDROID
#elif (!defined(GPK_LINUX)) && defined(__linux__)
#	define GPK_LINUX
#elif (!defined(GPK_WINDOWS)) && (defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64))
#	define GPK_WINDOWS
#elif !defined(GPK_ST) && !defined(GPK_CMSIS) && !defined(GPK_ESP32) && !defined(GPK_ATMEL)
#	error "Unsupported platform."
#	define GPK_UNKNOWN
#endif

#ifdef __GNUC__
#	define UNUSED __attribute__((__unused__))
#elif defined(GPK_WINDOWS)
#	ifdef _DEBUG
#		define UNUSED
#	else
#		define UNUSED
#	endif
#else
#	define UNUSED
#endif

#if defined(GPK_WINDOWS)
//#	pragma warning(disable : 4592)		// We needed this because of a bug in the compiler causing our static const variables to throw a warning as if its constructor was defined as constexpr.
//#	pragma warning(disable : 6326)		// Bogus potential comparison of a constant with another constant for static tplt functionality.
#	pragma warning(disable : 4706)		// Enable assignment within conditional expression. We do this constantly inside our macros in a completely valid way.
#endif

#if defined(GPK_WINDOWS)
#	if !defined(NOMINMAX)
#		define NOMINMAX
#	endif
#	if !defined(WIN32_LEAN_AND_MEAN)
#		define WIN32_LEAN_AND_MEAN
#	endif
#	define GPK_DYNAMIC_LIBRARY_EXTENSION "dll"
#else
#	define GPK_DYNAMIC_LIBRARY_EXTENSION "os"
#endif

#if defined GPK_ANDROID
#	define GPK_DISABLE_CPP_EXCEPTIONS
#endif

#if !defined(GPK_WINDOWS)
	#if defined(GPK_ATMEL)
	#	include <stddef.h>
	#else
	#	include <cstddef>
	#endif
#endif

#if defined(GPK_ATMEL)
#	include "initializer_list.h"
#else
#	include <initializer_list>
#	include <new>
#endif

// These aliases make keyword width to be consistent and makes the code less verbose.
#define cnstxpr	constexpr
#define inlcxpr	inline cnstxpr
#define stacxpr	static cnstxpr
#define stainli	static inline
#define stincxp	stainli	cnstxpr

#define nodscrd [[nodiscard]]
#define nodinli nodscrd inline
#define ndincxp nodscrd inlcxpr
#define nodcxpr nodscrd cnstxpr
#define nodstin nodscrd stainli
#define nodstxp nodscrd stacxpr
#define ndstinx nodstin cnstxpr

#define tpnm	typename
#define tplt	template

namespace gpk
{
	// -- Returns 0 on little-endian machines
	nodstin int		test_endianness		()	noexcept	{ const unsigned short test = 0xFF00; return (((const unsigned char*)&test)[0] == 0xFFU) ? 1 : 0; }
}

#endif // GPK_PLATFORM_GLOBALS_H_23627
