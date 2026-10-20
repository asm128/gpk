#include "gpk_view.h"

#include "gpk_packed_int.h"

#ifndef GPK_VIEW_SERIALIZE_H_23627
#define GPK_VIEW_SERIALIZE_H_23627

namespace gpk
{
	tplT		err_t	loadPOD			(vcu0_t & input, T & output) { 
		if_true_fe(input.byte_count() < szof(T));
		memcpy(&output, input.begin(), szof(T));
		if_fail_fe(input.slice(input, szof(T)));
		rtrn szof(T);
	}
	tplTstin	err_t	loadPOD			(vcs0_t  & input, T & output)	{ rtrn loadPOD (*(vcu0_t*)& input, output); }
	//
	tplt<tpnm T, u0_t widthField = uint_width_field_size<T>()>
	err_t				loadPacked		(vcu0_t & input, T & output) {
		if_zero_fe(input.size());
		cnst packed_uint<T, widthField>	& packedInput	= *(cnst packed_uint<T, widthField>*)input.begin();
		u0_c					valueWidth		= packedInput.ValueWidth();
		if_fail_fe(input.slice(input, valueWidth));
		output									= packedInput.Value();
		rtrn valueWidth;
	}
	//
	tplT		err_t	loadUInt		(vcu0_t & input, T & output)	{ rtrn loadPacked(input, output); }
	tplTstin	err_t	loadUInt		(vcs0_t  & input, T & output)	{ rtrn loadUInt (*(vcu0_t*)& input, output); }
	//
	tplt<tpnm T, tpnm TByte>
	err_t				viewRead		(view<T> & headerToRead, view<TByte> input)	{
		if_zero_fe(input.size());
		vcu0_t					byteInput		= {(u0_c*)input.begin(), input.byte_count()};
		u2_t					elementCount	= 0;
		err_t					counterWidth	= 0;
		if_fail_fe(counterWidth	= loadPacked(byteInput, elementCount));
		if_true_fef(elementCount > byteInput.size() / szof(T), GPK_FMT_GT_U2, elementCount, byteInput.size() / szof(T));
		u2_c					dataSize		= szof(T) * elementCount;
		if_true_fef(dataSize > byteInput.size(), GPK_FMT_GT_U2, dataSize, byteInput.size());
		headerToRead			= {elementCount ? (T*)byteInput.begin() : 0, elementCount};
		rtrn counterWidth + dataSize;
	}
	tplTstin	err_t	viewRead		(view<cnst T> & headerToRead, cnst vcu0_t & input)	{ rtrn viewRead<cnst T, u0_c>(headerToRead, input); }
	tplTstin	err_t	viewRead		(view<cnst T> & headerToRead, cnst vcs0_t  & input)	{ rtrn viewRead<cnst T, s0_c>(headerToRead, input); }
	tplTstin	err_t	viewRead		(view<T> & headerToRead, vu8 input)					{ rtrn viewRead<T, u0_t>(headerToRead, input); }
	tplTstin	err_t	viewRead		(view<T> & headerToRead, vs0_t input)					{ rtrn viewRead<T, s0_t>(headerToRead, input); }
	//
	tplT		err_t	loadView		(vcu0_t & input, view<cnst T> & output) { 
		err_t					bytesRead		= 0;
		if_fail_fe(bytesRead = viewRead(output, input)); 
		if_fail_fe(input.slice(input, bytesRead));
		rtrn 0;
	}
	tplTstin	err_t	loadView	(vcs0_t & input, view<T> & output) { rtrn loadView(*(vcu0_t*)& input, output); }
	//
	tplTInTOut requires(szof(TIn) == 1 && (false == is_cnst<TIn>::Value || is_cnst<TOut>::Value))
	err_t				viewPascal	(view<TOut> & headerToRead, view<TIn> input)	{
		stxp		u2_c		counterWidth	= szof(u2_t);
		if_true_fef(input.size() < counterWidth, "Invalid input size: %" GPK_FMT_U2 "", input.size());
		u2_t					elementCount	= {};
		memcpy(&elementCount, input.begin(), counterWidth);
		if_true_fef(elementCount > (input.size() - counterWidth) / szof(TOut), "Invalid input size: %" GPK_FMT_U2 ". Element count: %" GPK_FMT_U2 "", input.size(), elementCount);
		u2_c					dataSize		= elementCount * szof(TOut);
		headerToRead	= {elementCount ? reinterpret_cast<TOut*>(&input[counterWidth]) : 0, elementCount};
		rtrn counterWidth + dataSize;
	}
	tplTstin	err_t	viewPascal	(view<cnst T> & headerToRead, vcu0_c input)	{ rtrn viewPascal<u0_c, cnst T>(headerToRead, input); }
	tplTstin	err_t	viewPascal	(view<cnst T> & headerToRead, vcs0_t input)	{ rtrn viewPascal<s0_c, cnst T>(headerToRead, input); }
	tplTstin	err_t	viewPascal	(view<cnst T> & headerToRead, vcst_t input)	{ rtrn viewPascal<sc_c, cnst T>(headerToRead, input); }
	tplTstin	err_t	viewPascal	(view<T> & headerToRead, vu0_t input)		{ rtrn viewPascal<u0_t, T>(headerToRead, input); }
	tplTstin	err_t	viewPascal	(view<T> & headerToRead, vs0_t input)		{ rtrn viewPascal<s0_t, T>(headerToRead, input); }
} // namespace

#endif // GPK_VIEW_SERIALIZE_H_23627


