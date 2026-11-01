/// Copyright 2010-2024 - ogarnd
#include "gpk_typeint.h"

#ifndef GPK_BIT_H
#define GPK_BIT_H

namespace gpk
{
	tplTndsx	bool	bit_all			(cnst T state, cnst T bitsToTest)								nxpt	{ rtrn (state & bitsToTest) == bitsToTest; }
	tplTndsx	bool	bit_any			(cnst T state, cnst T bitsToTest)								nxpt	{ rtrn 0 != (state & bitsToTest); }
	tplTndsx	T		bit_clear		(cnst T state, cnst T bitsToClear)								nxpt	{ rtrn (state & bitsToClear)				? state & ~bitsToClear	: state; }
	tplTndsx	T		bit_set			(cnst T state, cnst T bitsToSet  )								nxpt	{ rtrn ((state & bitsToSet) != bitsToSet)	? state | bitsToSet	: state; }
	tplTndsx	T		bit_set			(cnst T state, cnst T bitsToSet, cnst bool value)				nxpt	{ rtrn value ? bit_set(state, bitsToSet) : bit_clear(state, bitsToSet); }
	tplTndsx	T		bit_test		(cnst T state, cnst T bitsToTest)								nxpt	{ rtrn bit_all(state, bitsToTest) ? state : (T)0; }
	tplTndsx	T		bit_set_masked	(cnst T state, cnst T mask, cnst T bitsToSet, cnst bool value)	nxpt	{ cnst T masked = T(mask & bitsToSet); rtrn masked ? bit_set(state, masked, value) : state; }
	tplTndsx	T		bit_test_masked	(cnst T state, cnst T mask, cnst T value)						nxpt	{ cnst T masked = T(mask & value); rtrn masked ? bit_test(state, masked) : (T)0; }
	tplTndsx	T		bit_true		(cnst T state, cnst T bitsToTest)								nxpt	{ rtrn bit_test(state, bitsToTest); }
	tplTndsx	T		bit_false		(cnst T state, cnst T bitsToTest)								nxpt	{
		cnst T		bitsThatMatch		= (state & bitsToTest);
		rtrn T((bitsThatMatch == bitsToTest) ? 0 : (state | bitsToTest) ? state | bitsToTest : -1);
	}
	tplTndsx	u2_t	bitsof			(cnst T&)		nxpt	{ rtrn szof(T) * BYTE_SIZE; }
	tplTndsx	u2_t	bitsof			()				nxpt	{ rtrn szof(T) * BYTE_SIZE; }

	tplt<u0_t... widths>
	ndsx u2_t bit_field_width_sum() nxpt { rtrn (u2_t(0) + ... + u2_t(widths)); }

	tplt<u2_t index, u0_t firstWidth, u0_t... remainingWidths>
	ndsx u2_t bit_field_offset() nxpt {
		static_assert(index < 1 + sizeof...(remainingWidths), "bit_field<> index out of range.");
		if constexpr(0 == index)
			rtrn 0;
		else
			rtrn firstWidth + bit_field_offset<index - 1, remainingWidths...>();
	}

	tplt<u2_t index, u0_t firstWidth, u0_t... remainingWidths>
	ndsx u0_t bit_field_width() nxpt {
		static_assert(index < 1 + sizeof...(remainingWidths), "bit_field<> index out of range.");
		if constexpr(0 == index)
			rtrn firstWidth;
		else
			rtrn bit_field_width<index - 1, remainingWidths...>();
	}

	tplt<tpnm _t, u0_t... fieldWidths>
	stct bit_field {
		tdfT(_t);
		static_assert(T(-1) > T(0), "bit_field<> requires an unsigned integer or flag type.");
		static_assert(((fieldWidths > 0) && ...), "bit_field<> field widths must be greater than zero.");
		static_assert(bit_field_width_sum<fieldWidths...>() <= bitsof<T>(), "bit_field<> fields exceed their storage type.");

		stxp u2_t				FIELD_COUNT		= sizeof...(fieldWidths);
		stxp u2_t				FIELD_BITS		= bit_field_width_sum<fieldWidths...>();

		T						Value			= {};

		inxp	bool			oper==			(cnst bit_field & other)			csnx	{ rtrn Value == other.Value; }
		inxp	bool			oper!=			(cnst bit_field & other)			csnx	{ rtrn Value != other.Value; }

		inxp					bit_field		()									nxpt	= default;
		inxp					bit_field		(T value)							nxpt	: Value(value) {}
		inxp	oper			T&				()									nxpt	{ rtrn Value; }
		inxp	oper	cnst	T&				()									csnx	{ rtrn Value; }
		inxp	bool			All				(T bitsToTest)						csnx	{ rtrn ::gpk::bit_all(Value, bitsToTest); }
		inxp	bool			Any				(T bitsToTest)						csnx	{ rtrn ::gpk::bit_any(Value, bitsToTest); }
		inln	bit_field&		Clear			(T bitsToClear)						nxpt	{ Value = ::gpk::bit_clear(Value, bitsToClear); rtrn *this; }
		inln	bit_field&		Set				(T bitsToSet, bool value = true)	nxpt	{ Value = ::gpk::bit_set(Value, bitsToSet, value); rtrn *this; }

		tplt<u2_t index>
		ndsx u2_t				Offset			()			nxpt { static_assert(index < FIELD_COUNT, "bit_field<> index out of range."); rtrn bit_field_offset<index, fieldWidths...>(); }
		tplt<u2_t index>
		ndsx u0_t				Width			()			nxpt { static_assert(index < FIELD_COUNT, "bit_field<> index out of range."); rtrn bit_field_width<index, fieldWidths...>(); }
		tplt<u2_t index>
		ndsx T					FieldMask		()			nxpt { rtrn T(T(-1) >> (bitsof<T>() - Width<index>())); }
		tplt<u2_t index>
		ndsx T					Mask			()			nxpt { rtrn T(FieldMask<index>() << Offset<index>()); }
		tplt<u2_t index>
		inxp T					Get				()			csnx { rtrn T((Value >> Offset<index>()) & FieldMask<index>()); }
		tplt<u2_t index>
		inxp bit_field&			Set				(T value)	nxpt {
			cnst T storageMask = Mask<index>();
			cnst T clearMask = T(T(-1) ^ storageMask);
			Value = T((Value & clearMask) | T(T(value & FieldMask<index>()) << Offset<index>()));
			rtrn *this;
		}

	};
	tplT ndxp	T		bit_make		(u0_t bitIndex)	nxpt	{
#ifdef GPK_DEBUG_ENABLED
//		if_true_tef(bitIndex >= (szof(T) * BYTE_SIZE), "bit index: %i", bitIndex);
#endif // GPK_DEBUG_ENABLED
		rtrn (T)(((T)1) << bitIndex);
	}
//#ifdef GPK_BIGENDIAN
//	sinx	u0_t		le_byte_at		(::gpk::u2_t bytes, u0_t byteIndex)	nxpt	{ return ((0xFF000000 >> (byteIndex * 8)) & bytes) >> ((szof(bytes) - byteIndex) * 8); }
//#else
//	sinx	u0_t		le_byte_at		(::gpk::u2_t bytes, u0_t byteIndex)	nxpt	{ return ((0xFF << (byteIndex * 8)) & bytes) >> (byteIndex * 8); }
//#endif
	tplT	sttc	T	reverse_bitfield	(T input, s2_c bitDepth)	{
		u2_c					sizeType			= u2_t(szof(T) * 8);
		if(bitDepth <= 0 || (u2_t)bitDepth > sizeType || sizeType % bitDepth)
			rtrn input;
		cnst T					mask				= ((T)(-1)) >> (sizeType - bitDepth);
		T						result				= 0;
		for(::gpk::u2_t iBit = 0; iBit < sizeType; iBit += bitDepth) {
			result				|= (input & mask) << (sizeType - bitDepth - iBit);
			input				>>= bitDepth;
		}
		rtrn result;
	}
	tplT	sttc	T	reverse_bits		(T input, u0_c width)	{
		if(0 == width || width > bitsof<T>())
			rtrn input;
		rtrn reverse_bitfield(input, 1) >> (bitsof<T>() - width);
	}
}

#endif // GPK_BIT_H
