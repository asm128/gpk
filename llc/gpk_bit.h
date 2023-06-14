/// Copyright 2010-2017 - asm128
#include "gpk_typeint.h"

#ifndef GPK_BIT_H_034792701397
#define GPK_BIT_H_034792701397

namespace gpk
{
	template<typename _tField>	cnstxpr		_tField		bit_make	(uint8_t bitIndex)	noexcept	{
#if defined(GPK_DEBUG_ENABLED)
		rve_if((_tField)-1LL, bitIndex >= (sizeof(_tField) * PLATFORM_BYTE_BIT_COUNT), "%i", bitIndex);
#endif
		return (_tField)(((_tField)1) << bitIndex);
	}
	template<typename _tBitField> stainli	_tBitField	bit_clear		(_tBitField& inout_FlagVariable, _tBitField in_BitsToClear)				noexcept	{ return (inout_FlagVariable & in_BitsToClear)					? inout_FlagVariable &= ~in_BitsToClear : inout_FlagVariable					;}
	template<typename _tBitField> stainli	_tBitField	bit_set			(_tBitField& inout_FlagVariable, _tBitField in_BitsToSet  )				noexcept	{ return ((inout_FlagVariable & in_BitsToSet) == in_BitsToSet)	? inout_FlagVariable					: inout_FlagVariable |= in_BitsToSet	;}
	template<typename _tBitField> stainli	_tBitField	bit_set			(_tBitField& inout_FlagVariable, _tBitField in_BitsToSet, bool value)	noexcept	{ return value ? bit_set(inout_FlagVariable, in_BitsToSet) : bit_clear(inout_FlagVariable, in_BitsToSet); }
	template<typename _tBitField> stincxp	_tBitField	bit_test		(const _tBitField in_FlagVariable, const _tBitField bitsToTest)			noexcept	{ return ((in_FlagVariable & bitsToTest) == bitsToTest) ? bitsToTest : (_tBitField)0; }
	template<typename _tBitField> cnstxpr	_tBitField	bit_test_masked	(const _tBitField state, const _tBitField mask, const _tBitField value)	noexcept	{ return bit_test(mask, value) ? bit_test(state, value) : (_tBitField)0; }
	template<typename _tBitField> stincxp	_tBitField	bit_true		(const _tBitField in_FlagVariable, const _tBitField bitsToTest)			noexcept	{ return bit_test(in_FlagVariable, bitsToTest); }
	template<typename _tBitField> stincxp	_tBitField	bit_false		(const _tBitField in_FlagVariable, const _tBitField bitsToTest)			noexcept	{
		const _tBitField	bitsThatMatch		= (in_FlagVariable & bitsToTest);
		return _tBitField((bitsThatMatch == bitsToTest) ? 0 : bitsToTest & ~bitsThatMatch);
	}

#define	gbit_make(bitIndex)								( 1 << (bitIndex) )
#define	gbit_set(inout_FlagVariable, in_BitsToSet  )	( (((inout_FlagVariable) & (in_BitsToSet)) == (in_BitsToSet))	? (inout_FlagVariable)						: (inout_FlagVariable) |= (in_BitsToSet)	)
#define	gbit_clear(inout_FlagVariable, in_BitsToClear)	( ((inout_FlagVariable) & (in_BitsToClear))						? (inout_FlagVariable) &= ~(in_BitsToClear)	: (inout_FlagVariable)						)
#define	gbit_true(in_FlagVariable, in_BitToTest)		( ((in_FlagVariable) & (in_BitToTest)) == (in_BitToTest) ) 
#define	gbit_false(in_FlagVariable, in_BitToTest)		( ((in_FlagVariable) & (in_BitToTest)) != (in_BitToTest) ) 
	template<typename _tField>
	static	_tField			reverse_bitfield	(_tField input, const int32_t bitDepth)	{
		const uint32_t				sizeType			= (uint32_t)(sizeof(_tField) * 8);
		const _tField				mask				= ((_tField)(-1)) >> (sizeType - bitDepth);
		_tField						result				= 0;
		for(uint32_t iBit = 0; iBit < sizeType; iBit += bitDepth) {
			result					|= (input & mask) << (sizeType - bitDepth - iBit);
			input					>>= bitDepth;
		}
		return result;
	}
}

#endif // GPK_BIT_H_034792701397
//[{"params": {"sessiId": "424995b9-118f-4142-b62d-9cc0eb97a8151"}, "method":"opnPly"}, {"params": {"lnsPlyd": 5, "btPrLn": 1, "sessnId": "424995b9-118f-4142-b62d-9cc0eb97a815", "tpe": "spi", "bt": 25}, "mthod": "plceBet"}, {"paras": {"sessionId":"424995b9-118f-4142-b62d-9cc0eb97a815"}, "method": "getBetResult"}]