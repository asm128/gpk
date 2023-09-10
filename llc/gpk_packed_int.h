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
#pragma pack(push, 1)
	tplt<tpnm _tInt = int32_t, uint8_t widthField = 2>
	struct packed_int { 
		typedef _tInt	T;
		uint8_t			TailWidth		: widthField;
		uint8_t			Multiplier		: 8 - widthField;
		T				Tail			= 0;

		inlcxpr			packed_int		(uint8_t tailWidth = 0, uint8_t multiplier = 0, T tail = 0) 
			:TailWidth(tailWidth), Multiplier(multiplier), Tail(tail) {}
		ndincxp	uint8_t	ValueWidth		() 	const	noexcept	{ return TailWidth + 1; }
		nodscrd	T		Value			()	const	noexcept	{
			switch(TailWidth) {
			default:
			case 0: return Multiplier;
			case 1: { T tail = 0; memcpy(&tail, ((const char*)this) + 1, 1); return T((T(Multiplier) << (1 * 8)) + tail); }
			case 2: { T tail = 0; memcpy(&tail, ((const char*)this) + 1, 2); return T((T(Multiplier) << (2 * 8)) + tail); }
			case 3: { T tail = 0; memcpy(&tail, ((const char*)this) + 1, 3); return T((T(Multiplier) << (3 * 8)) + tail); }
			}
		}

	};

	typedef packed_int<uint32_t>	packedu32;
	typedef packed_int<int32_t>		packedi32;
	typedef packed_int<uint64_t>	packedu64;
	typedef packed_int<int64_t>		packedi64;
#pragma pack(pop)

}

#endif // GPK_PACKED_INT_H
