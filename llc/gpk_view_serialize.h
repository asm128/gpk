#include "gpk_view.h"

#include "gpk_packed_int.h"

#ifndef GPK_VIEW_SERIALIZE_H_23627
#define GPK_VIEW_SERIALIZE_H_23627

namespace gpk
{
	tplT		err_t	loadPOD			(vcu0_t & input, T & output) { 
		rees_if(input.byte_count() < szof(T));
		memcpy(&output, input.begin(), szof(T));
		gpk_necs(input.slice(input, szof(T)));
		rtrn szof(T);
	}
	tplTstin	err_t	loadPOD			(vcs0_t & input, T & output)	{ rtrn loadPOD (*(vcu0_t*)& input, output); }
	tplTstin	err_t	loadPOD			(vcsc_t & input, T & output)	{ rtrn loadPOD (*(vcu0_t*)& input, output); }
	//
	tplT		err_t	loadUInt		(vcu0_t & input, T & output)	{
		cnst packed_uint<T>		& packedInput	= *(cnst packed_uint<T>*)input.begin(); 
		if_true_ve(-1, packedInput.ValueWidth() > input.size()); 
		output					= packedInput.Value(); 
		gpk_necs(input.slice(input, packedInput.ValueWidth())); 
		rtrn packedInput.ValueWidth();
	}
	tplTstin	err_t	loadUInt		(vcs0_t & input, T & output)	{ rtrn loadUInt (*(vcu0_t*)& input, output); }
	tplTstin	err_t	loadUInt		(vcsc_t  & input, T & output)	{ rtrn loadUInt (*(vcu0_t*)& input, output); }
	//
	tplt<tpnm T, tpnm TByte>
	err_t					viewRead		(view<T> & headerToRead, view<TByte> input)	{
		pku2_c					& header		= *(pku2_c*)input.begin();
		u2_c					counterWidth	= header.ValueWidth();
		ree_if(input.size() < counterWidth, GPK_FMT_LT_U2, input.size(), counterWidth);
		u2_c					elementCount	= header.Value();
		u2_c					dataSize		= szof(T) * elementCount;
		ree_if(dataSize > (input.size() - counterWidth), "%" GPK_FMT_U2 " > (%" GPK_FMT_U2 "-%" GPK_FMT_U2 ").", dataSize, input.size(), counterWidth);
		headerToRead			= {(input.size() > counterWidth) ? (T*)&input[counterWidth] : 0, elementCount};
		rtrn counterWidth + dataSize;
	}
	tplTstin	err_t	viewRead		(view<cnst T> & headerToRead, cnst vcu0_t & input)	{ rtrn viewRead<cnst T, u0_c>(headerToRead, input); }
	tplTstin	err_t	viewRead		(view<cnst T> & headerToRead, cnst vcs0_t & input)	{ rtrn viewRead<cnst T, s0_c>(headerToRead, input); }
	tplTstin	err_t	viewRead		(view<cnst T> & headerToRead, cnst vcsc_t  & input)	{ rtrn viewRead<cnst T, sc_c>(headerToRead, input); }
	tplTstin	err_t	viewRead		(view<T> & headerToRead, vu8 input)					{ rtrn viewRead<T, u0_t>(headerToRead, input); }
	tplTstin	err_t	viewRead		(view<T> & headerToRead, vi8 input)					{ rtrn viewRead<T, s0_t>(headerToRead, input); }
	tplTstin	err_t	viewRead		(view<T> & headerToRead, vc  input)					{ rtrn viewRead<T, sc_t>(headerToRead, input); }
	//
	tplT		err_t	loadView		(vcu0_t & input, view<cnst T> & output) { 
		u2_t					bytesRead		= 0;
		gpk_necs(bytesRead = viewRead(output, input)); 
		gpk_necs(input.slice(input, bytesRead));
		rtrn 0;
	}
	tplTstin	err_t	loadView	(vcs0_t & input, view<T> & output) { rtrn loadView(*(vcu0_t*)& input, output); }
	tplTstin	err_t	loadView	(vcsc_t & input, view<T> & output) { rtrn loadView(*(vcu0_t*)& input, output); }
	//
	tplTOutTIn	err_t	viewReadLegacy	(view<TOut> & headerToRead, view<TIn> input)	{
		stxp		u2_c		counterWidth	= szof(u2_t);
		ree_if(input.size() < counterWidth, "Invalid input size: %" GPK_FMT_U2 "", input.size());
		u2_c					elementCount	= *(u2_c*)input.begin();
		u2_c					dataSize		= elementCount * szof(TOut);
		ree_if(dataSize > (input.size() - counterWidth), "Invalid input size: %" GPK_FMT_U2 ". Expected: %" GPK_FMT_U2 "", input.size(), dataSize);
		headerToRead	= {(input.size() > counterWidth) ? (TOut*)&input[counterWidth] : 0, elementCount};
		rtrn counterWidth + dataSize;
	}
	tplTstin	err_t	viewReadLegacy	(view<cnst T> & headerToRead, vcu0_c & input)	{ rtrn viewReadLegacy<cnst T, u0_c>(headerToRead, input); }
	tplTstin	err_t	viewReadLegacy	(view<cnst T> & headerToRead, vcs0_c & input)	{ rtrn viewReadLegacy<cnst T, s0_c>(headerToRead, input); }
	tplTstin	err_t	viewReadLegacy	(view<cnst T> & headerToRead, vcsc_c & input)	{ rtrn viewReadLegacy<cnst T, sc_c>(headerToRead, input); }
	tplTstin	err_t	viewReadLegacy	(view<T> & headerToRead, vu0_t input)			{ rtrn viewReadLegacy<T, u0_t>(headerToRead, input); }
	tplTstin	err_t	viewReadLegacy	(view<T> & headerToRead, vs0_t input)			{ rtrn viewReadLegacy<T, s0_t>(headerToRead, input); }
	tplTstin	err_t	viewReadLegacy	(view<T> & headerToRead, vsc_t input)			{ rtrn viewReadLegacy<T, sc_t>(headerToRead, input); }
} // namespace

#endif // GPK_VIEW_SERIALIZE_H_23627
