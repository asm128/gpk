#include "gpk_typeint.h"

#ifndef GPK_PACKED_INT_H
#define GPK_PACKED_INT_H

namespace gpk 
{
	nodstxp	uint8_t		tail_width		(uint64_t value) 	{
		//return 
		return uint8_t
			( (value > (0x1FFFFFFFFFFFFFFFULL >> (8 * 1))) ? 7
			: (value > (0x1FFFFFFFFFFFFFFFULL >> (8 * 2))) ? 6
			: (value > (0x1FFFFFFFFFFFFFFFULL >> (8 * 3))) ? 5
			: (value > (0x1FFFFFFFFFFFFFFFULL >> (8 * 4))) ? 4
			: (value > (0x3FFFFFFFFFFFFFFFULL >> (8 * 5))) ? 3
			: (value > (0x3FFFFFFFFFFFFFFFULL >> (8 * 6))) ? 2
			: (value > (0x3FFFFFFFFFFFFFFFULL >> (8 * 7))) ? 1
			: 0
			);
	}
	ndstinx	uint8_t		value_width		(uint64_t count)	{ return tail_width(count) + 1; }

	nodstxp	uint8_t		tail_multiplier	(uint64_t count)	{
		return uint8_t
			( (count > 0x003FFFFF) ? ((count & 0x3F000000) >> 24)
			: (count > 0x00003FFF) ? ((count & 0x003F0000) >> 16)
			: (count > 0x0000003F) ? ((count & 0x00003F00) >> 8)
			//( (count > (0x1FFFFFFFFFFFFFFFULL >> (8 * 1)))	? count & ((0x1FULL << (8 * 7)) >> (8 * 7))
			//: (count > (0x1FFFFFFFFFFFFFFFULL >> (8 * 2)))	? count & ((0x1FULL << (8 * 6)) >> (8 * 6))
			//: (count > (0x1FFFFFFFFFFFFFFFULL >> (8 * 3)))	? count & ((0x1FULL << (8 * 5)) >> (8 * 5))
			//: (count > (0x1FFFFFFFFFFFFFFFULL >> (8 * 4)))	? count & ((0x1FULL << (8 * 4)) >> (8 * 4))
			//: (count > (0x3FFFFFFFFFFFFFFFULL >> (8 * 5)))	? count & ((0x3FULL << (8 * 3)) >> (8 * 3))
			//: (count > (0x3FFFFFFFFFFFFFFFULL >> (8 * 6)))	? count & ((0x3FULL << (8 * 2)) >> (8 * 2))
			//: (count > (0x3FFFFFFFFFFFFFFFULL >> (8 * 7)))	? count & ((0x3FULL << (8 * 1)) >> (8 * 1))
			: count																				
			);
	}

	tplt<tpnm _tInt>
	nodstxp	_tInt		tail_base		(_tInt count)	{
		return _tInt
			( (uint64_t(count) > 0x003FFFFF) ? count & 0x00FFFFFFU
			: (uint64_t(count) > 0x00003FFF) ? count & 0x0000FFFFU
			: (uint64_t(count) > 0x0000003F) ? count & 0x000000FFU
			//( (uint64_t(count) > (0x1FFFFFFFFFFFFFFFULL >> (8 * 1))) ? count & 0x00FFFFFFFFFFFFFFULL
			//: (uint64_t(count) > (0x1FFFFFFFFFFFFFFFULL >> (8 * 2))) ? count & 0x0000FFFFFFFFFFFFULL
			//: (uint64_t(count) > (0x1FFFFFFFFFFFFFFFULL >> (8 * 3))) ? count & 0x000000FFFFFFFFFFULL
			//: (uint64_t(count) > (0x1FFFFFFFFFFFFFFFULL >> (8 * 4))) ? count & 0x00000000FFFFFFFFULL
			//: (uint64_t(count) > (0x3FFFFFFFFFFFFFFFULL >> (8 * 5))) ? count & 0x0000000000FFFFFFULL
			//: (uint64_t(count) > (0x3FFFFFFFFFFFFFFFULL >> (8 * 6))) ? count & 0x000000000000FFFFULL
			//: (uint64_t(count) > (0x3FFFFFFFFFFFFFFFULL >> (8 * 7))) ? count & 0x00000000000000FFULL
			: 0
			);
	}

	tplt<tpnm _tInt>
	nodstxp	uint8_t			width_field_size	()		{ 
		return (sizeof(_tInt) > 4) ? 3
			: (sizeof(_tInt) > 2) ? 2
			: (sizeof(_tInt) > 1) ? 1 
			: 1; 
	};
#pragma pack(push, 1)
	tplt<tpnm _tInt = int32_t, uint8_t widthField = width_field_size<_tInt>()>
	struct packed_int { 
		typedef _tInt	T;

		const	T		TailWidth		: widthField;
		const	T		Multiplier		: 8 - widthField;
		const	T		Tail			: (sizeof(T) - 1) * 8;

		inlcxpr			packed_int		()												: TailWidth{}, Multiplier{}, Tail{} {}
		inlcxpr			packed_int		(const T & value)								: TailWidth{tail_width(value)}, Multiplier{tail_multiplier(value)}, Tail{tail_base(value)} {}
		inlcxpr			packed_int		(uint8_t tailWidth, uint8_t multiplier, T tail)	: TailWidth(tailWidth), Multiplier(multiplier), Tail(tail) {}

		ndincxp	uint8_t	ValueWidth		() 	const	noexcept	{ return uint8_t(1 + TailWidth); }
		nodscrd	T		Value			()	const	noexcept	{
			if(0 == TailWidth)
				return Multiplier;

			T tail = 0; 
			memcpy(&tail, ((const char*)this) + 1, TailWidth); 
			return T((T(Multiplier) << (TailWidth * 8)) + tail); 
		}

	};
#pragma pack(pop)

	typedef packed_int<uint16_t>	packedu16;
	typedef packed_int<uint32_t>	packedu32;
	typedef packed_int<uint64_t>	packedu64;
	typedef packed_int<int16_t>		packedi16;
	typedef packed_int<int32_t>		packedi32;
	typedef packed_int<int64_t>		packedi64;

	nodstin uint16_t			int_unpack	(const ::gpk::packedu16 value) { return value.Value(); } 
	nodstin uint32_t			int_unpack	(const ::gpk::packedu32 value) { return value.Value(); } 
	nodstin uint64_t			int_unpack	(const ::gpk::packedu64 value) { return value.Value(); } 
	nodstin int16_t				int_unpack	(const ::gpk::packedi16 value) { return value.Value(); } 
	nodstin int32_t				int_unpack	(const ::gpk::packedi32 value) { return value.Value(); } 
	nodstin int64_t				int_unpack	(const ::gpk::packedi64 value) { return value.Value(); } 

	ndstinx ::gpk::packedu16	int_pack	(const uint16_t value) { return {tail_width(value), tail_multiplier(value), tail_base(value)}; } 
	ndstinx ::gpk::packedu32	int_pack	(const uint32_t value) { return {tail_width(value), tail_multiplier(value), tail_base(value)}; } 
	ndstinx ::gpk::packedu64	int_pack	(const uint64_t value) { return {tail_width(value), tail_multiplier(value), tail_base(value)}; } 
	ndstinx ::gpk::packedi16	int_pack	(const int16_t  value) { return {tail_width(value), tail_multiplier(value), tail_base(value)}; } 
	ndstinx ::gpk::packedi32	int_pack	(const int32_t  value) { return {tail_width(value), tail_multiplier(value), tail_base(value)}; } 
	ndstinx ::gpk::packedi64	int_pack	(const int64_t  value) { return {tail_width(value), tail_multiplier(value), tail_base(value)}; } 
}

#endif // GPK_PACKED_INT_H
