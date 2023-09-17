#include "gpk_typeint.h"

#ifndef GPK_PACKED_INT_H
#define GPK_PACKED_INT_H

namespace gpk 
{
	tplt<tpnm _tInt>
	nodstxp	uint8_t		tail_width		(_tInt value) 	{
		//return 
		return uint8_t
			( (1 == sizeof(_tInt)) ? ((uint8_t (value) > 0x7FU) ? 1 : 0)
			: (2 == sizeof(_tInt)) ? ((uint16_t(value) > 0x7FU) ? 1 : 0)
			: (4 == sizeof(_tInt)) ? 
				( (uint32_t(value) > (0x3FFFFFUL >> (8 * 0))) ? 3
				: (uint32_t(value) > (0x3FFFFFUL >> (8 * 1))) ? 2
				: (uint32_t(value) > (0x3FFFFFUL >> (8 * 2))) ? 1
				: 0
				)
			: (8 == sizeof(_tInt)) ? 
				( (uint64_t(value) > (0x1FFFFFFFFFFFFFULL >> (8 * 0))) ? 7
				: (uint64_t(value) > (0x1FFFFFFFFFFFFFULL >> (8 * 1))) ? 6
				: (uint64_t(value) > (0x1FFFFFFFFFFFFFULL >> (8 * 2))) ? 5
				: (uint64_t(value) > (0x1FFFFFFFFFFFFFULL >> (8 * 3))) ? 4
				: (uint64_t(value) > (0x1FFFFFFFFFFFFFULL >> (8 * 4))) ? 3
				: (uint64_t(value) > (0x1FFFFFFFFFFFFFULL >> (8 * 5))) ? 2
				: (uint64_t(value) > (0x1FFFFFFFFFFFFFULL >> (8 * 6))) ? 1
				: 0												  
				)
			: sizeof(_tInt)
			);
	}
	tplt<tpnm _tInt>
	ndstinx	uint8_t		value_width		(_tInt count)	{ return tail_width(count) + 1; }

	tplt<tpnm _tInt>
	nodstxp	uint8_t		tail_multiplier	(_tInt count)	{
		return uint8_t
			( (1 == sizeof(_tInt)) ? ((uint8_t (count) > 0x7F) ? byte_at(count, 0) : count)
			: (2 == sizeof(_tInt)) ? ((uint16_t(count) > 0x7F) ? byte_at(count, 1) : count)
			: (4 == sizeof(_tInt)) ? 
				( (uint32_t(count) > (0x3FFFFFU >> (0 * 8))) ? byte_at(count, 3)
				: (uint32_t(count) > (0x3FFFFFU >> (1 * 8))) ? byte_at(count, 2)
				: (uint32_t(count) > (0x3FFFFFU >> (2 * 8))) ? byte_at(count, 1)
				: count																				
				)
			: (8 == sizeof(_tInt)) ? 
				( (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (1 * 8))) ? byte_at(count, 7)
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (2 * 8))) ? byte_at(count, 6)
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (3 * 8))) ? byte_at(count, 5)
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (4 * 8))) ? byte_at(count, 4)
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (5 * 8))) ? byte_at(count, 3)
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (6 * 8))) ? byte_at(count, 2)
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (7 * 8))) ? byte_at(count, 1)
				: count																				
				)
			: count
			);
	}

	tplt<tpnm _tInt>
	nodstxp	_tInt		tail_base		(_tInt count)	{
		return _tInt
			( (1 == sizeof(_tInt)) ? ((uint8_t (count) > 0x7FU) ? count & 0xFFU : count)
			: (2 == sizeof(_tInt)) ? ((uint16_t(count) > 0x7FU) ? count & 0xFFU : count)
			: (4 == sizeof(_tInt)) ? 
				( (uint32_t(count) > (0x3FFFFFU >> (0 * 8))) ? count & (0xFFFFFFFFU >> (1 * 8))
				: (uint32_t(count) > (0x3FFFFFU >> (1 * 8))) ? count & (0xFFFFFFFFU >> (2 * 8))
				: (uint32_t(count) > (0x3FFFFFU >> (2 * 8))) ? count & (0xFFFFFFFFU >> (3 * 8))
				: count
				)
			: (8 == sizeof(_tInt)) ? 
				( (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (1 * 8))) ? count & (0xFFFFFFFFFFFFFFFFU >> (1 * 8))
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (2 * 8))) ? count & (0xFFFFFFFFFFFFFFFFU >> (2 * 8))
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (3 * 8))) ? count & (0xFFFFFFFFFFFFFFFFU >> (3 * 8))
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (4 * 8))) ? count & (0xFFFFFFFFFFFFFFFFU >> (4 * 8))
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (5 * 8))) ? count & (0xFFFFFFFFFFFFFFFFU >> (5 * 8))
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (6 * 8))) ? count & (0xFFFFFFFFFFFFFFFFU >> (6 * 8))
				: (uint64_t(count) > ((0x1FFFFFFFFFFFFFFF) >> (7 * 8))) ? count & (0xFFFFFFFFFFFFFFFFU >> (7 * 8))
				: count
				)
			: count
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
		const	T		Tail			: max((uint8_t)1U, uint8_t((sizeof(T) - 1) * 8));

		inlcxpr			packed_int		()												: TailWidth{}, Multiplier{}, Tail{} {}
		inlcxpr			packed_int		(const T & value)								: TailWidth{(T)tail_width(value)}, Multiplier{(T)tail_multiplier(value)}, Tail{(T)tail_base(value)} {}
		inlcxpr			packed_int		(uint8_t tailWidth, uint8_t multiplier, T tail)	: TailWidth(tailWidth), Multiplier(multiplier), Tail(tail) {}

		ndincxp	uint8_t	ValueWidth		() 	const	noexcept	{ return uint8_t(1 + TailWidth); }
		nodscrd	T		Value			()	const	noexcept	{
			if(0 == TailWidth)
				return Multiplier;

			uint64_t tail = 0; 
			memcpy(&tail, ((const char*)this) + 1, TailWidth); 
			return T((uint64_t(Multiplier) << (TailWidth * 8)) + tail); 
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
