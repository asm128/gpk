/// Copyright 2010-2023 - asm128
#include "gpk_typeint.h"

#ifndef GPK_BIT_H_23627
#define GPK_BIT_H_23627

namespace gpk
{
	tplt<tpnm _tBit> nodstxp	_tBit	bit_clear		(const _tBit state, const _tBit bitsToClear)					noexcept	{ return (state & bitsToClear)				? state & ~bitsToClear	: state; }
	tplt<tpnm _tBit> nodstxp	_tBit	bit_set			(const _tBit state, const _tBit bitsToSet  )					noexcept	{ return ((state & bitsToSet) != bitsToSet)	? state | bitsToSet	: state; }
	tplt<tpnm _tBit> nodstxp	_tBit	bit_set			(const _tBit state, const _tBit bitsToSet, const bool value)	noexcept	{ return value ? bit_set(state, bitsToSet) : bit_clear(state, bitsToSet); }
	tplt<tpnm _tBit> stacxpr	_tBit	bit_test		(const _tBit state, const _tBit bitsToTest)						noexcept	{ return ((state & bitsToTest) == bitsToTest) ? state : (_tBit)0; }

	tplt<tpnm _tBit> nodstxp	_tBit	bit_set_masked	(const _tBit state, const _tBit mask, const _tBit bitsToSet, const bool value)	noexcept	{ return (mask & bitsToSet) ? bit_set(state, mask & bitsToSet, value) : state; }
	tplt<tpnm _tBit> stincxp	_tBit	bit_test_masked	(const _tBit state, const _tBit mask, const _tBit value)						noexcept	{ return (mask & value) ? bit_test(state, mask & value) : (_tBit)0; }
	tplt<tpnm _tBit> stincxp	_tBit	bit_true		(const _tBit state, const _tBit bitsToTest)										noexcept	{ return bit_test(state, bitsToTest); }
	tplt<tpnm _tBit> stacxpr	_tBit	bit_false		(const _tBit state, const _tBit bitsToTest)										noexcept	{
		const _tBit		bitsThatMatch		= (state & bitsToTest);
		return _tBit((bitsThatMatch == bitsToTest) ? 0 : (state | bitsToTest) ? state | bitsToTest : -1);
	}

#ifdef GPK_DEBUG_ENABLED
	tplt<tpnm _tBit> 			_tBit	bit_make		(uint8_t bitIndex)	noexcept	{
		rve_if((_tBit)-1LL, bitIndex >= (sizeof(_tBit) * PLATFORM_BYTE_BIT_COUNT), "Invalid bit index: %i", bitIndex);
#else
	tplt<tpnm _tBit> cnstxpr	_tBit	bit_make		(uint8_t bitIndex)	noexcept	{
#endif
		return (_tBit)(((_tBit)1) << bitIndex);
	}

//#ifdef GPK_BIGENDIAN
//	stincxp	uint8_t		le_byte_at		(uint32_t bytes, uint8_t byteIndex)	noexcept	{ return ((0xFF000000 >> (byteIndex * 8)) & bytes) >> ((sizeof(bytes) - byteIndex) * 8); }
//#else
//	stincxp	uint8_t		le_byte_at		(uint32_t bytes, uint8_t byteIndex)	noexcept	{ return ((0xFF << (byteIndex * 8)) & bytes) >> (byteIndex * 8); }
//#endif

	tplt<tpnm _tField>
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

#endif // GPK_BIT_H_23627
