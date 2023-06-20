#include "gpk_debug.h"

#if defined(GPK_ATMEL)
#	include <stdint.h>
#else
#	include <cstdint>
#endif

#ifndef GPK_TYPEINT_H_29873490827342
#define GPK_TYPEINT_H_29873490827342

namespace gpk
{
	typedef	float			float32_t	;
	typedef	double			float64_t	;
	typedef	char			char_t  ;
	typedef	unsigned char	uchar_t , uchar;

	typedef	const int8_t	cint8_t 	;
	typedef	const int16_t	cint16_t	;
	typedef	const int32_t	cint32_t	;
	typedef	const int64_t	cint64_t	;
	typedef	const uint8_t	cuint8_t	;
	typedef	const uint16_t	cuint16_t	;
	typedef	const uint32_t	cuint32_t	;
	typedef	const uint64_t	cuint64_t	;
	typedef	const float32_t	cfloat32_t	;
	typedef	const float64_t	cfloat64_t	;
	typedef	const char_t	cchar_t , cchar;
	typedef	const uchar_t	cuchar_t, cuchar;

	typedef int8_t		i8_t ;
	typedef int16_t		i16_t;
	typedef int32_t		i32_t;
	typedef int64_t		i64_t;
	typedef uint8_t		u8_t ;
	typedef uint16_t	u16_t;
	typedef uint32_t	u32_t;
	typedef uint64_t	u64_t;
	typedef float		f32_t;
	typedef double		f64_t;

	typedef	const int8_t	ci8_t ;
	typedef	const int16_t	ci16_t;
	typedef	const int32_t	ci32_t;
	typedef	const int64_t	ci64_t;
	typedef	const uint8_t	cu8_t ;
	typedef	const uint16_t	cu16_t;
	typedef	const uint32_t	cu32_t;
	typedef	const uint64_t	cu64_t;
	typedef	const float32_t	cf32_t;
	typedef	const float64_t	cf64_t;

	typedef int8_t		i8 ;
	typedef int16_t		i16;
	typedef int32_t		i32;
	typedef int64_t		i64;
	typedef uint8_t		u8 ;
	typedef uint16_t	u16;
	typedef uint32_t	u32;
	typedef uint64_t	u64;
	typedef float		f32;
	typedef double		f64;

	typedef const int8_t	ci8 ;
	typedef const int16_t	ci16;
	typedef const int32_t	ci32;
	typedef const int64_t	ci64;
	typedef const uint8_t	cu8 ;
	typedef const uint16_t	cu16;
	typedef const uint32_t	cu32;
	typedef const uint64_t	cu64;
	typedef const float		cf32;
	typedef const double	cf64;

	stacxpr	const uint8_t	PLATFORM_BYTE_BIT_COUNT								= 8;
#if defined GPK_WINDOWS
#	if defined(_WIN64) || defined(WIN64)
#		define	GPK_REFCOUNT_T		long long
		typedef volatile GPK_REFCOUNT_T	refcount_t			;
#	else
#		define	GPK_REFCOUNT_T		long
		typedef volatile GPK_REFCOUNT_T	refcount_t			;
#	endif
#else
#	define	GPK_REFCOUNT_T	intptr_t
	typedef GPK_REFCOUNT_T	refcount_t			;
#endif

	template<class _tBase>	struct remove_cv																			{ using type = _tBase; };
	template<class _tBase>	struct remove_cv<const					_tBase>												{ using type = _tBase; };
	template<class _tBase>	struct remove_cv<volatile				_tBase>												{ using type = _tBase; };
	template<class _tBase>	struct remove_cv<const volatile			_tBase>												{ using type = _tBase; };
	template<class _tBase>	using									remove_cv_t											= typename remove_cv<_tBase>::type;

	template<class _tBase>	struct remove_volatile																		{ using type = _tBase; };
	template<class _tBase>	struct remove_volatile<const			_tBase>												{ using type = _tBase; };
	template<class _tBase>	struct remove_volatile<volatile			_tBase>												{ using type = _tBase; };
	template<class _tBase>	struct remove_volatile<const volatile	_tBase>												{ using type = _tBase; };
	template<class _tBase>	using									remove_volatile_t									= typename remove_volatile<_tBase>::type;


}

#define GPK_DEFAULT_OPERATOR(_otherType, ...)	\
	inlcxpr	bool	operator==(const _otherType & other) const noexcept { return __VA_ARGS__; } \
	inlcxpr	bool	operator!=(const _otherType & other) const noexcept { return !operator==(other); }	// I had to add this because Clang coming with vs doesn't support C++20

#endif // GPK_TYPEINT_H_29873490827342
