#include "gpk_platform.h"

#if defined(GPK_ATMEL)
#	include <stdint.h>
#else
#	include <cstdint>
#endif

#ifndef GPK_TYPEINT_H_29873490827342
#define GPK_TYPEINT_H_29873490827342

//typedef	int8_t			int8_t	;
//typedef	uint8_t			uint8_t	;

namespace gpk
{
	//typedef	char			char_t	;
	typedef	unsigned char	uchar_t	;
	typedef	double			float64_t	;
	typedef	float			float32_t	;

	typedef	const int8_t	cint8_t 	;
	typedef	const int16_t	cint16_t	;
	typedef	const int32_t	cint32_t	;
	typedef	const int64_t	cint64_t	;
	typedef	const uint8_t	cuint8_t	;
	typedef	const uint16_t	cuint16_t	;
	typedef	const uint32_t	cuint32_t	;
	typedef	const uint64_t	cuint64_t	;
	typedef	const float64_t	cfloat64_t	;
	typedef	const float32_t	cfloat32_t	;
	typedef	const char		cchar_t		;
	typedef	const uchar_t	cuchar_t	;	

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


	typedef uint8_t			u8	;
	typedef uint16_t		u16	;
	typedef uint32_t		u32	;
	typedef uint64_t		u64	;

	typedef int8_t			i8	;
	typedef int16_t			i16	;
	typedef int32_t			i32	;
	typedef int64_t			i64	;
	typedef float			f32	;
	typedef double			f64	;
}

#define GPK_DEFAULT_OPERATOR_NE(_otherType, ...)	\
	inlcxpr	bool	operator!=	(const _otherType & other) const noexcept { return !operator==(other);	} \
	inlcxpr	bool	operator==	(const _otherType & other) const noexcept { return __VA_ARGS__;			}

#endif // GPK_TYPEINT_H_29873490827342
