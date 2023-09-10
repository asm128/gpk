#include "gpk_typeint.h"

#ifndef GPK_PACKED_INT_H
#define GPK_PACKED_INT_H

namespace gpk 
{
	nodstxp	uint8_t				tail_width			(uint32_t count) 	{
		//return (count > 0x3FFFFFFF) ? 4
		return uint8_t
			( (count > 0x003FFFFF) ? 3
			: (count > 0x00003FFF) ? 2
			: (count > 0x0000003F) ? 1
			: 0
			);
	}
	
	ndstinx	uint8_t				header_width		(uint32_t count)	{ return tail_width(count) + 1; }

	nodstxp	uint8_t				tail_multiplier		(uint32_t count)	{
		//return (count > 0x3FFFFFFF) ? (count & 0x3F00000000) >> 32
		return uint8_t
			( (count > 0x003FFFFF) ? ((count & 0x3F000000) >> 24)
			: (count > 0x00003FFF) ? ((count & 0x003F0000) >> 16)
			: (count > 0x0000003F) ? ((count & 0x00003F00) >> 8)
			: count
			);
	}
	nodstxp	uint32_t			tail_base			(uint32_t count)	{
		//return (count > 0x3FFFFFFF) ? (count & 0xFFFFFFFFU) >> 8
		return (count > 0x003FFFFF) ? count & 0x00FFFFFFU
			 : (count > 0x00003FFF) ? count & 0x0000FFFFU
			 : (count > 0x0000003F) ? count & 0x000000FFU
			 : 0;
	}
	tplt<tpnm _tInt>
	nodstxp	uint8_t			width_field_size	()		{ 
		return (sizeof(_tInt) > 8) ? 4
			: (sizeof(_tInt) > 4) ? 3
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

	typedef packed_int<uint32_t>	packedu32;
	typedef packed_int<int32_t>		packedi32;
	typedef packed_int<uint64_t>	packedu64;
	typedef packed_int<int64_t>		packedi64;

	ndstinx	::gpk::packedu32	pack_int	(uint32_t value) { return {tail_width(value), tail_multiplier(value), tail_base(value)}; }
	ndstinx	::gpk::packedu32	int_pack	(uint32_t value) { return {tail_width(value), tail_multiplier(value), tail_base(value)}; }
	ndstinx	::gpk::packedu32	int_unpack	(uint32_t value) { return {tail_width(value), tail_multiplier(value), tail_base(value)}; }
}

#endif // GPK_PACKED_INT_H
