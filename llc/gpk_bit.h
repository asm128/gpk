/// Copyright 2010-2023 - asm128
#include "gpk_typeint.h"

#ifndef GPK_BIT_H_034792701397
#define GPK_BIT_H_034792701397

namespace gpk
{
	template<typename _tBitField> stainli	_tBitField	bit_clear		(_tBitField & state, _tBitField bitsToClear)							noexcept	{ return (state & bitsToClear)				? state &= ~bitsToClear	: state; }
	template<typename _tBitField> stainli	_tBitField	bit_set			(_tBitField & state, _tBitField bitsToSet  )							noexcept	{ return ((state & bitsToSet) != bitsToSet)	? state |= bitsToSet	: state; }
	template<typename _tBitField> stainli	_tBitField	bit_set			(_tBitField & state, _tBitField bitsToSet, bool value)					noexcept	{ return value ? bit_set(state, bitsToSet) : bit_clear(state, bitsToSet); }
	template<typename _tBitField> stincxp	_tBitField	bit_test		(_tBitField state, _tBitField bitsToTest)								noexcept	{ return ((state & bitsToTest) == bitsToTest) ? bitsToTest : (_tBitField)0; }
	template<typename _tBitField> stacxpr	_tBitField	bit_set_masked	(_tBitField & state, _tBitField mask, _tBitField bitsToSet, bool value)	noexcept	{ return bit_test(mask, bitsToSet) ? bit_set(state, bitsToSet, value) : (_tBitField)0; }
	template<typename _tBitField> stacxpr	_tBitField	bit_test_masked	(_tBitField state, _tBitField mask, _tBitField value)					noexcept	{ return bit_test(mask, value) ? bit_test(state, value) : (_tBitField)0; }
	template<typename _tBitField> stincxp	_tBitField	bit_true		(_tBitField state, _tBitField bitsToTest)								noexcept	{ return bit_test(state, bitsToTest); }
	template<typename _tBitField> stacxpr	_tBitField	bit_false		(_tBitField state, _tBitField bitsToTest)								noexcept	{
		const _tBitField	bitsThatMatch		= (state & bitsToTest);
		return _tBitField((bitsThatMatch == bitsToTest) ? 0 : bitsToTest & ~bitsThatMatch);
	}

	template<typename _tBitField> cnstxpr	_tBitField	bit_make		(uint8_t bitIndex)	noexcept	{
#if defined(GPK_DEBUG_ENABLED)
		rve_if((_tBitField)-1LL, bitIndex >= (sizeof(_tBitField) * PLATFORM_BYTE_BIT_COUNT), "Invalid bit index: %i", bitIndex);
#endif
		return (_tBitField)(((_tBitField)1) << bitIndex);
	}

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
//[{"parms": {"sessiId": "424995b9-118f-4142-b62d-9cc0eb97a8151"}, "method":"opnPly"}, {"params": {"lnsPlyd": 5, "btPrLn": 1, "sessnId": "424995b9-118f-4142-b62d-9cc0eb97a815", "tpe": "spi", "bt": 25}, "mthod": "plceBet"}, {"paras": {"sessionId":"424995b9-118f-4142-b62d-9cc0eb97a815"}, "method": "getBtRslt"}]