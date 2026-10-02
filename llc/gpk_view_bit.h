#include "gpk_log.h"
#include "gpk_eval.h"

#ifndef GPK_ARRAY_VIEW_BIT_H_23627
#define GPK_ARRAY_VIEW_BIT_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt <tpnm _tInt>
	struct bit_proxy {
		tydf	_tInt			T;

		T							& Element;
		u0_t						Offset;

		oper					bool		()				cnst	{ return Element & (1ULL << Offset); }
		bit_proxy&				oper=		(bool value)			{ value ? Element |= (1ULL << Offset) : Element &= ~(1ULL << Offset); return *this; }
	};

	tplt<tpnm _tInt>	ndsx	u0_t		bit_offset_field_size	() 	{
		return
			( (szof(_tInt) > 4) ? 6
			: (szof(_tInt) > 2) ? 5
			: (szof(_tInt) > 1) ? 4
			: 3
			);
	}

	tplt<tpnm _tInt, u0_t offsetField = bit_offset_field_size<_tInt>()>
	struct bit_iterator {
		tydf	_tInt			T;

		stxp	u2_t			ELEMENT_BITS	= szof(T) * 8;

		T							* Begin			= 0;
		T							* End			= 0;
		T							* Element		= 0;
		u0_t					Offset			: offsetField;
		u0_t					Stop			: offsetField;

		u2_t					Limit		()		cnst	{ return Begin ? u2_t((End - Begin) * ELEMENT_BITS - (Stop ? ELEMENT_BITS - Stop : 0)) : 0; }
		u2_t					Index		()		cnst	{ return Begin ? u2_t((Element - Begin) * ELEMENT_BITS + Offset) : 0; }

		bit_proxy<T>			oper*		()				{ if_true_tef(Index() >= Limit(), "Invalid index:%" GPK_FMT_U2 ", size:%" GPK_FMT_U2 ".", Index(), Limit()); return {*Element, (u0_t)Offset}; }
		bool					oper*		()		cnst	{ if_true_tef(Index() >= Limit(), "Invalid index:%" GPK_FMT_U2 ", size:%" GPK_FMT_U2 ".", Index(), Limit()); return (*Element) & (1ULL << Offset); }

		inline	oper			bool		()							cnst	{ if_true_tef(Index() >= Limit(), "Invalid index:%" GPK_FMT_U2 ", size:%" GPK_FMT_U2 ".", Index(), Limit()); return (*Element) & (1ULL << Offset); }
		inxp	bool			oper==		(cnst bit_iterator & other)	csnx	{ rtrn Element == other.Element && Offset == other.Offset; }
		inxp	bool			oper!=		(cnst bit_iterator & other)	csnx	{ rtrn Element != other.Element || Offset != other.Offset; }

		bit_iterator			oper++		(int)			{ bit_iterator result (*this); ++(*this); return result; }
		bit_iterator			oper--		(int)			{ bit_iterator result (*this); --(*this); return result; }
		bit_iterator&			oper++		()				{ if_true_tef(Index() >= Limit(), "Invalid index:%" GPK_FMT_U2 ", size:%" GPK_FMT_U2 ".", Index(), Limit()); if(0 == ++Offset) ++Element; return *this; }
		bit_iterator&			oper--		()				{ if_true_tef(0 == Index(), "Invalid index:%" GPK_FMT_U2 ", size:%" GPK_FMT_U2 ".", Index(), Limit()); if(0 == Offset) { --Element; Offset = ELEMENT_BITS - 1; } else --Offset; return *this; }
		inline	bit_iterator&	oper=		(bool value)	{ if_true_tef(Index() >= Limit(), "Invalid index:%" GPK_FMT_U2 ", size:%" GPK_FMT_U2 ".", Index(), Limit()); value ? *Element |= (1ULL << Offset) : *Element &= ~(1ULL << Offset); return *this; }
	};

	tplt<tpnm _tInt>
	class view_bit {
	protected:
		// Properties / Member Variables
		_tInt				* Data			= 0;
		u2_t				Count			= 0;
	public:
		tydf	_tInt					T;
		tydf	bit_iterator<T>			TIter;
		tydf	bit_iterator<cnst T>	TIterConst;
		tydf	TIter					iterator;

		stxp	u0_t		ELEMENT_BITS	= szof(T) * 8;

		// Constructors
		inxp				view_bit		()								nxpt	= default;
		inline				view_bit		(T * data, u2_t bitCount)				: Data(data), Count(bitCount) { if_true_tef(bitCount && 0 == data, "bitCount(%" GPK_FMT_U2 ")", bitCount); }
		tplN0u	inxp		view_bit		(T (&data)[N])					nxpt	: Data(data), Count(N * ELEMENT_BITS)								{}
		tplN0u	inln		view_bit		(u2_t bitCount, T (&data)[N])			: Data(data), Count(::gpk::min(u2_t(N * ELEMENT_BITS), bitCount))	{ if_true_tef(bitCount > (N * ELEMENT_BITS), "max(%" GPK_FMT_U2 "), bitCount(%" GPK_FMT_U2 ")", N * ELEMENT_BITS, bitCount); }
		// Operators
		bit_proxy<T>		oper[]		(u2_t index)			{ if_true_tef(index >= Count, GPK_FMT_GE_U2, index, Count); u2_c offsetRow = index / ELEMENT_BITS, offsetBit = index % ELEMENT_BITS; return {Data[offsetRow], (u0_t)offsetBit}; }
		bool				oper[]		(u2_t index)	cnst	{ if_true_tef(index >= Count, GPK_FMT_GE_U2, index, Count); u2_c offsetRow = index / ELEMENT_BITS, offsetBit = index % ELEMENT_BITS; return Data[offsetRow] & (1ULL << offsetBit); }
		// Methods
		inln	TIter		begin			()			nxpt	{ return Count ? TIter{Data, Data + round_up(Count, ELEMENT_BITS), Data, 0, Count % ELEMENT_BITS} : TIter{}; }
		inln	TIter		end				()			nxpt	{ return Count ? TIter{Data, Data + round_up(Count, ELEMENT_BITS), Data + Count / ELEMENT_BITS, Count % ELEMENT_BITS, Count % ELEMENT_BITS} : TIter{}; }
		//
		inxp	TIterConst	begin			()	cnst	nxpt	{ return Count ? TIterConst{Data, Data + round_up(Count, ELEMENT_BITS), Data, 0, Count % ELEMENT_BITS} : TIterConst{}; }
		inxp	TIterConst	end				()	cnst	nxpt	{ return Count ? TIterConst{Data, Data + round_up(Count, ELEMENT_BITS), Data + Count / ELEMENT_BITS, Count % ELEMENT_BITS, Count % ELEMENT_BITS} : TIterConst{}; }

		inxp	u2_c&		size			()	cnst	nxpt	{ return Count; }
	};
#pragma pack(pop)

	tplT	using					vbit		= ::gpk::view_bit<T>;
	tydf	::gpk::view_bit<u0_t>	vbitu0_t, vbitu8	;
	tydf	::gpk::view_bit<u1_t>	vbitu1_t, vbitu16	;
	tydf	::gpk::view_bit<u2_t>	vbitu2_t, vbitu32	;
	tydf	::gpk::view_bit<u3_t>	vbitu3_t, vbitu64	;
	tplT	err_t		reverse_bits		(::gpk::view_bit<T> toReverse)											{
		u2_c				countBits			= toReverse.size() / 2;
		u2_c				lastBitIndex		= toReverse.size() - 1;
		for(u2_t iBit = 0; iBit < countBits; ++iBit) {
			u2_c				iRev				= lastBitIndex - iBit;
			cnst bool					current				= toReverse[iBit];
			toReverse[iBit]			= (bool)toReverse[iRev];
			toReverse[iRev]			= current;
		}
		return 0;
	}

} // namespace

#endif // GPK_ARRAY_VIEW_BIT_H_23627
