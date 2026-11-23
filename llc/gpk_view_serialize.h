#include "gpk_view.h"

#include "gpk_packed_int.h"

#ifndef GPK_VIEW_SERIALIZE_H_23627
#define GPK_VIEW_SERIALIZE_H_23627

nmsp gpk
{
    tplt<tpnm T, tpnm TInput = u0_t> requires(szof(TInput) == 1)
    err_t       loadPOD            (view<cnst TInput> & input, T & output) {
        if_true_fe(input.byte_count() < szof(T));
        memcpy(&output, input.template as<u0_c>().begin(), szof(T));
        rtrn input.slice(input, szof(T));
    }
    //
    tplt<tpnm T, u0_t widthField = uint_width_field_size<T>()>
    err_t				loadPacked		(vcu0_t & input, T & output) {
        if_zero_fe(input.size());
        cnst packed_uint<T, widthField>
                                & packedInput	= *(cnst packed_uint<T, widthField>*)input.begin();
        u0_c                    valueWidth		= packedInput.ValueWidth();
        if_fail_fe(input.slice(input, valueWidth));
        output				= packedInput.Value();
        rtrn valueWidth;
    }
    //
    tplt<tpnm T, tpnm TByte>
    err_t				viewRead		(view<T> & headerToRead, view<TByte> input)	{
        if_zero_fe(input.size());
        vcu0_t					byteInput		= input.bytes();
        u2_t					elementCount	= 0;
        err_t					counterWidth	= 0;
        if_fail_fe(counterWidth	= loadPacked(byteInput, elementCount));
        if_true_fef(elementCount > byteInput.byte_count(), GPK_FMT_GT_U2, elementCount, byteInput.byte_count());
        u2_c					dataSize		= szof(T) * elementCount;
        if_true_fef(dataSize > byteInput.size(), GPK_FMT_GT_U2, dataSize, byteInput.size());
        headerToRead			= {elementCount ? (T*)byteInput.begin() : 0, elementCount};
        rtrn counterWidth + dataSize;
    }
    tplTstin	err_t	viewRead		(view<cnst T> & headerToRead, vcu0_c & input)	{ rtrn viewRead<cnst T, u0_c>(headerToRead, input); }
    tplTstin	err_t	viewRead		(view<cnst T> & headerToRead, vcs0_c  & input)	{ rtrn viewRead<cnst T, s0_c>(headerToRead, input); }
    tplTstin	err_t	viewRead		(view<T> & headerToRead, vu0_t input)					{ rtrn viewRead<T, u0_t>(headerToRead, input); }
    tplTstin	err_t	viewRead		(view<T> & headerToRead, vs0_t input)					{ rtrn viewRead<T, s0_t>(headerToRead, input); }
    //
    tplT		err_t	loadView		(vcu0_t & input, view<cnst T> & output) { 
        err_t					bytesRead		= 0;
        if_fail_fe(bytesRead = viewRead(output, input)); 
        if_fail_fe(input.slice(input, bytesRead));
        rtrn 0;
    }
    tplTstin	err_t	loadView	(vcs0_t & input, view<T> & output) { rtrn loadView(*(vcu0_t*)& input, output); }
    tplTInTOut err_t load(view<cnst TIn> & input, view<TOut> & output) { rtrn loadView(input, output); }
    tplTInTOut err_t load(view<cnst TIn> & input, TOut & output) { rtrn loadPOD(input, output); }
    tplTInTArgs requires(sizeof...(TArgs) > 1)
    err_t load(view<cnst TIn> & input, TArgs &... output) {
        err_t result = 0;
        ((result = failed(result) ? result : load(input, output)), ...);
        rtrn failed(result) ? result : 0;
    }
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
} // nmsp

#endif // GPK_VIEW_SERIALIZE_H_23627
