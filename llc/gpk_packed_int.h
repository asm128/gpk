#include "gpk_typeint.h"

#ifndef GPK_PACKED_INT_H
#define GPK_PACKED_INT_H

namespace gpk 
{
	tplt<tpnm _tInt>	ndsx	u0_t		uint_width_field_size	()		{ 
		return 
			( (szof(_tInt) > 4) ? 3
			: (szof(_tInt) > 2) ? 2
			: (szof(_tInt) > 1) ? 1 
			: 1
			);
	}
	tplt<tpnm _tInt>	ndsx	u3_t	uint_tail_mask	()	{
		return
			( (1 == szof(_tInt)) ? 0x7FU
			: (2 == szof(_tInt)) ? 0x7FU
			: (4 == szof(_tInt)) ? 0x3FFFFFU
			: (8 == szof(_tInt)) ? 0x1FFFFFFFFFFFFFULL
			: 0xFFFFFFFFFFFFFFULL // 0xFFFFFFFFFFFFFFULL
			);
	}
	tplt<tpnm _tInt>	nsix	u3_t	uint_tail_mask	(_tInt)			{ return uint_tail_mask<_tInt>(); }
	tplt<tpnm _tInt>	ndsx	u0_t	uint_tail_width	(_tInt value)	{
		return u0_t
			( (1 == szof(_tInt)) ? ((u0_t(value) > uint_tail_mask<_tInt>()) ? 1 : 0)
			: (2 == szof(_tInt)) ? ((u1_t(value) > uint_tail_mask<_tInt>()) ? 1 : 0)
			: (4 == szof(_tInt)) ? 
				( (u2_t(value) > (uint_tail_mask<_tInt>() >> (8 * 0))) ? 3
				: (u2_t(value) > (uint_tail_mask<_tInt>() >> (8 * 1))) ? 2
				: (u2_t(value) > (uint_tail_mask<_tInt>() >> (8 * 2))) ? 1
				: 0
				)
			: (8 == szof(_tInt)) ? 
				( (u3_t(value) > (uint_tail_mask<_tInt>() >> (8 * 0))) ? 7
				: (u3_t(value) > (uint_tail_mask<_tInt>() >> (8 * 1))) ? 6
				: (u3_t(value) > (uint_tail_mask<_tInt>() >> (8 * 2))) ? 5
				: (u3_t(value) > (uint_tail_mask<_tInt>() >> (8 * 3))) ? 4
				: (u3_t(value) > (uint_tail_mask<_tInt>() >> (8 * 4))) ? 3
				: (u3_t(value) > (uint_tail_mask<_tInt>() >> (8 * 5))) ? 2
				: (u3_t(value) > (uint_tail_mask<_tInt>() >> (8 * 6))) ? 1
				: 0												  
				)
			: szof(_tInt)
			);
	}
	tplt<tpnm _tInt>	nsix	u0_t		uint_value_width		(_tInt count)	{ return uint_tail_width(count) + 1; }
	tplt<tpnm _tInt>	ndsx	u0_t		uint_tail_multiplier	(_tInt count)	{
		return u0_t
			( (1 == szof(_tInt)) ? ((u0_t(count) > uint_tail_mask<_tInt>()) ? byte_at(count, 0) : count)
			: (2 == szof(_tInt)) ? ((u1_t(count) > uint_tail_mask<_tInt>()) ? byte_at(count, 1) : count)
			: (4 == szof(_tInt)) ? 
				( (u2_t(count) > (uint_tail_mask<_tInt>() >> (0 * 8))) ? byte_at(count, 3)
				: (u2_t(count) > (uint_tail_mask<_tInt>() >> (1 * 8))) ? byte_at(count, 2)
				: (u2_t(count) > (uint_tail_mask<_tInt>() >> (2 * 8))) ? byte_at(count, 1)
				: count																				
				)
			: (8 == szof(_tInt)) ? 
				( (u3_t(count) > (uint_tail_mask<_tInt>() >> (0 * 8))) ? byte_at(count, 7)
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (1 * 8))) ? byte_at(count, 6)
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (2 * 8))) ? byte_at(count, 5)
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (3 * 8))) ? byte_at(count, 4)
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (4 * 8))) ? byte_at(count, 3)
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (5 * 8))) ? byte_at(count, 2)
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (6 * 8))) ? byte_at(count, 1)
				: count								
				)
			: count
			);
	}
	tplt<tpnm _tInt>	ndsx	_tInt		uint_tail_base			(_tInt count)	{
		return _tInt
			( (1 == szof(_tInt)) ? ((u0_t(count) > uint_tail_mask<_tInt>()) ? count & 0xFFU : count)
			: (2 == szof(_tInt)) ? ((u1_t(count) > uint_tail_mask<_tInt>()) ? count & 0xFFU : count)
			: (4 == szof(_tInt)) ? 
				( (u2_t(count) > (uint_tail_mask<_tInt>() >> (0 * 8))) ? count & (0xFFFFFFFFU >> (1 * 8))
				: (u2_t(count) > (uint_tail_mask<_tInt>() >> (1 * 8))) ? count & (0xFFFFFFFFU >> (2 * 8))
				: (u2_t(count) > (uint_tail_mask<_tInt>() >> (2 * 8))) ? count & (0xFFFFFFFFU >> (3 * 8))
				: count
				)
			: (8 == szof(_tInt)) ? 
				( (u3_t(count) > (uint_tail_mask<_tInt>() >> (1 * 8))) ? count & (0xFFFFFFFFFFFFFFFFULL >> (1 * 8))
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (2 * 8))) ? count & (0xFFFFFFFFFFFFFFFFULL >> (2 * 8))
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (3 * 8))) ? count & (0xFFFFFFFFFFFFFFFFULL >> (3 * 8))
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (4 * 8))) ? count & (0xFFFFFFFFFFFFFFFFULL >> (4 * 8))
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (5 * 8))) ? count & (0xFFFFFFFFFFFFFFFFULL >> (5 * 8))
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (6 * 8))) ? count & (0xFFFFFFFFFFFFFFFFULL >> (6 * 8))
				: (u3_t(count) > (uint_tail_mask<_tInt>() >> (7 * 8))) ? count & (0xFFFFFFFFFFFFFFFFULL >> (7 * 8))
				: count
				)
			: count
			);
	}

#pragma pack(push, 1)
	tplt<tpnm _tInt = u2_t, u0_t widthField = uint_width_field_size<_tInt>()>
	struct packed_uint { 
		tydf _tInt	T;
		tydf cnst T	TConst;

		TConst		TailWidth		: widthField;
		TConst		Multiplier		: 8 - widthField;
		TConst		Tail			: max((u0_t)1U, u0_t((szof(T) - 1) * 8));

		inxp			packed_uint		()												: TailWidth{}, Multiplier{}, Tail{} {}
		inxp			packed_uint		(TConst & value)								: TailWidth{(T)uint_tail_width(value)}, Multiplier{(T)uint_tail_multiplier(value)}, Tail{(T)uint_tail_base(value)} {}
		inxp			packed_uint		(u0_t tailWidth, u0_t multiplier, T tail)	: TailWidth(tailWidth), Multiplier(multiplier), Tail(tail) {}

		tplt<tpnm TView>
		TView			cu8				()	const 				{ return {(u0_c*)(void*)this, ValueWidth()}; }

		ndix	u0_t	ValueWidth		()	const	noexcept	{ return u0_t(1 + TailWidth); }
		ndsc	T		Value			()	const	noexcept	{
			if(0 == TailWidth)
				return Multiplier;

			u3_t			tail			= 0; 
			memcpy(&tail, ((u0_c*)this) + 1, TailWidth); 
			return T((u3_t(Multiplier) << (TailWidth * 8)) + tail); 
		}
	};
#pragma pack(pop)
	tydf packed_uint<u1_t> pku1_t, packedu16; tdcs pku1_t pku1_c;
	tydf packed_uint<u2_t> pku2_t, packedu32; tdcs pku2_t pku2_c;
	tydf packed_uint<u3_t> pku3_t, packedu64; tdcs pku3_t pku3_c;

	//nsix ::gpk::packedu16	int_pack	(const u1_t value) { return {uint_tail_width(value), uint_tail_multiplier(value), uint_tail_base(value)}; } 
	//nsix ::gpk::packedu32	int_pack	(u2_c value) { return {uint_tail_width(value), uint_tail_multiplier(value), uint_tail_base(value)}; } 
	//nsix ::gpk::packedu64	int_pack	(const u3_t value) { return {uint_tail_width(value), uint_tail_multiplier(value), uint_tail_base(value)}; } 
	//nsix ::gpk::packedi16	int_pack	(const int16_t  value) { return {uint_tail_width(value), uint_tail_multiplier(value), uint_tail_base(value)}; } 
	//nsix ::gpk::packedi32	int_pack	(const int32_t  value) { return {uint_tail_width(value), uint_tail_multiplier(value), uint_tail_base(value)}; } 
	//nsix ::gpk::packedi64	int_pack	(const int64_t  value) { return {uint_tail_width(value), uint_tail_multiplier(value), uint_tail_base(value)}; } 
}

#endif // GPK_PACKED_INT_H
