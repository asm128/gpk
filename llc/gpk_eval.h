#include "gpk_platform.h"

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
	template<typename _tValue>	static inline constexpr		const _tValue&					min				(const _tValue& a, const _tValue& b)												noexcept	{ return (a < b) ? a : b; }
	template<typename _tValue>	static inline constexpr		const _tValue&					max				(const _tValue& a, const _tValue& b)												noexcept	{ return (a > b) ? a : b; }
	template<typename _tValue>	static inline constexpr		bool							in_range		(const _tValue& valueToTest, const _tValue& rangeStart, const _tValue& rangeStop)	noexcept	{ return (valueToTest >= rangeStart) && (valueToTest < rangeStop);	}

	template<typename _tValue>	
	struct SMinMax	{
					_tValue					Min;
					_tValue					Max;

		inline		bool					operator==(const SMinMax & other)		const	noexcept	{ return Min == other.Min && Max == other.Max; }
					bool					operator!=(const SMinMax & other)		const	noexcept	{ return !operator==(other); }
	};
#pragma pack(pop)
#define true_if(expression)					((expression) ? true : false)	// returns true	 if the parameter evaluates to true
#define false_if(expression)				((expression) ? false : true)	// returns false if the parameter evaluates to true
#define one_if(expression)					((expression) ? 1 : 0)			// returns one   if the parameter evaluates to true
#define zero_if(expression)					((expression) ? 0 : 1)			// returns zero  if the parameter evaluates to true
//
#define is_one(your_int)					( 1 == (your_int))				// returns true if the parameter is equal to one
#define is_zero(your_int)					( 0 == (your_int))				// returns true if the parameter is equal to zero
} // namespace

#endif // GPK_EVAL_H_2983742983047239874
