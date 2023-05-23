#include "gpk_typeint.h"
#if !defined(GPK_ATMEL)
#include <memory.h>
#endif

#ifndef GPK_EVAL_H_2983742983047239874
#define GPK_EVAL_H_2983742983047239874

#if defined(GPK_WINDOWS)
#	if defined(min)
#		undef min
#	endif
#	if defined(max)
#		undef max
#	endif
#	if !defined(NOMINMAX)
#		define NOMINMAX
#	endif
#endif

namespace gpk
{
#pragma pack(push, 1)
	template<typename _t>	inlcxpr	const _t&	min			(const _t & a, const _t & b)	noexcept	{ return (a < b) ? a : b; }
	template<typename _t>	inlcxpr	const _t&	max			(const _t & a, const _t & b)	noexcept	{ return (a > b) ? a : b; }

	template<typename _t>	stincxp	_t			clamp		(const _t _value, const _t _min, const _t _max)	noexcept	{ return ::gpk::min(_max, ::gpk::max(_min, _value)); }

	// excluding the stop value
	template<typename _t>	inlcxpr	bool		in_range	(const _t & valueToTest, const _t & rangeStart, const _t & rangeStop)	noexcept	{ return (valueToTest >= rangeStart) && (valueToTest < rangeStop);	}


	template<typename _t>	bool				equal		(const _t * other, const _t * local, uint32_t count)		{
		for(uint32_t iElement = 0; iElement < count; ++iElement)
			if(other[iElement] != local[iElement])
				return false;
		return true;
	}
#if !defined(GPK_ATMEL)
	template<>	inline	bool		equal		(const double	* other, const double	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(double	)*count); }
	template<>	inline	bool		equal		(const float	* other, const float	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(float		)*count); }
	template<>	inline	bool		equal		(const int32_t	* other, const int32_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(int32_t	)*count); }
	template<>	inline	bool		equal		(const uint32_t	* other, const uint32_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(uint32_t	)*count); }
	template<>	inline	bool		equal		(const int16_t	* other, const int16_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(int16_t	)*count); }
	template<>	inline	bool		equal		(const uint16_t	* other, const uint16_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(uint16_t	)*count); }
	template<>	inline	bool		equal		(const int8_t	* other, const int8_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(int8_t	)*count); }
	template<>	inline	bool		equal		(const uint8_t	* other, const uint8_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(uint8_t	)*count); }
#endif

	template<typename _tValue>
	struct SMinMax	{
				_tValue					Min;
				_tValue					Max;

		inline	bool					operator==	(const SMinMax & other)		const	noexcept	{ return Min == other.Min && Max == other.Max; }
				bool					operator!=	(const SMinMax & other)		const	noexcept	{ return !operator==(other); }
	};

	typedef SMinMax<float> SNearFar;

	template<typename _tUnit> struct SRange { _tUnit Offset, Count	; GPK_DEFAULT_OPERATOR_NE(SRange<_tUnit>, Offset == other.Offset && Count == other.Count); };
	template<typename _tUnit> struct SSlice { _tUnit Begin, End		; GPK_DEFAULT_OPERATOR_NE(SSlice<_tUnit>, Begin  == other.Begin  && End   == other.End  ); };

#pragma pack(pop)
#ifndef true_if
#	define true_if(expression)		((expression) ? true : false)	// returns true  if the parameter evaluates to true
#endif
#ifndef false_if
#	define false_if(expression)		((expression) ? false : true)	// returns false if the parameter evaluates to true
#endif
#ifndef one_if
#	define one_if(expression)		((expression) ? 1 : 0)			// returns one   if the parameter evaluates to true
#endif
#ifndef zero_if
#	define zero_if(expression)		((expression) ? 0 : 1)			// returns zero  if the parameter evaluates to true
#endif
//
#ifndef is_one
#	define is_one(your_int)			(1 == (your_int))				// returns true if the parameter is equal to one
#endif
#ifndef is_zero
#	define is_zero(your_int)		(0 == (your_int))				// returns true if the parameter is equal to zero
#endif

} // namespace

#endif // GPK_EVAL_H_2983742983047239874



