/// Copyright 2010-2017 - asm128
#include "gpk_typeint.h"

#ifndef GPK_BIT_H_034792701397
#define GPK_BIT_H_034792701397

namespace gpk
{
	template<typename _tField>		constexpr					_tField		bit_make	(uint8_t bitIndex)											noexcept	{
//#if defined(GPK_WINDOWS) && defined(_DEBUG)
//		if(bitIndex >= (sizeof(_tField) * PLATFORM_BYTE_BIT_COUNT), "")
//			OutputDebugString("Out of range bit.");
//#endif
		return (_tField)(((_tField)1) << bitIndex);
	}
	template<typename _tBitField> stainli	_tBitField	bit_set		(_tBitField& inout_FlagVariable, _tBitField in_BitsToSet  )	noexcept	{ return ((inout_FlagVariable & in_BitsToSet) == in_BitsToSet)	? inout_FlagVariable					: inout_FlagVariable |= in_BitsToSet	;}
	template<typename _tBitField> stainli	_tBitField	bit_clear	(_tBitField& inout_FlagVariable, _tBitField in_BitsToClear)	noexcept	{ return (inout_FlagVariable & in_BitsToClear)					? inout_FlagVariable &= ~in_BitsToClear : inout_FlagVariable					;}
	template<typename _tBitField> stincxp		bool		bit_true	(const _tBitField in_FlagVariable, const _tBitField in_Bit)	noexcept	{ return ((in_FlagVariable & in_Bit) == in_Bit) ? true : false; }
	template<typename _tBitField> stincxp		bool		bit_false	(const _tBitField in_FlagVariable, const _tBitField in_Bit)	noexcept	{ return ((in_FlagVariable & in_Bit) != in_Bit) ? true : false; }

#define	gbit_make(bitIndex)								( 1 << (bitIndex) )
#define	gbit_set(inout_FlagVariable, in_BitsToSet  )	( (((inout_FlagVariable) & (in_BitsToSet)) == (in_BitsToSet))	? (inout_FlagVariable)						: (inout_FlagVariable) |= (in_BitsToSet)	)
#define	gbit_clear(inout_FlagVariable, in_BitsToClear)	( ((inout_FlagVariable) & (in_BitsToClear))						? (inout_FlagVariable) &= ~(in_BitsToClear)	: (inout_FlagVariable)						)
#define	gbit_true(in_FlagVariable, in_BitToTest)				( ((in_FlagVariable) & (in_BitToTest)) == (in_BitToTest) ) 
#define	gbit_false(in_FlagVariable, in_BitToTest)				( ((in_FlagVariable) & (in_BitToTest)) != (in_BitToTest) ) 
	template<typename _tField>
	static	_tField															reverse_bitfield									(_tField input, const int32_t bitDepth)	{
		const uint32_t																sizeType											= (uint32_t)(sizeof(_tField) * 8);
		const _tField																mask												= ((_tField)(-1)) >> (sizeType - bitDepth);
		_tField																		result												= 0;
		for(uint32_t iBit = 0; iBit < sizeType; iBit += bitDepth) {
			result																	|= (input & mask) << (sizeType - bitDepth - iBit);
			input																	>>= bitDepth;
		}
		return result;
	}


}

#endif // GPK_BIT_H_034792701397
//[{"params": {"sessionId": "424995b9-118f-4142-b62d-9cc0eb97a8151"}, "method":"openPlay"}, {"params": {"linesPlayed": 5, "betPerLine": 1, "sessionId": "424995b9-118f-4142-b62d-9cc0eb97a815", "type": "spin", "bet": 25}, "method": "placeBet"}, {"params": {"sessionId":"424995b9-118f-4142-b62d-9cc0eb97a815"}, "method": "getBetResult"}]