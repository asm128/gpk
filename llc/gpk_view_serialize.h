#include "gpk_view.h"

#include "gpk_packed_int.h"

#ifndef GPK_VIEW_SERIALIZE_H_23627
#define GPK_VIEW_SERIALIZE_H_23627

namespace gpk
{

#pragma pack(push, 1)

	nodstxp	packedu32	viewHeader	(uint32_t payloadSize) { return {tail_width(payloadSize), tail_multiplier(payloadSize), tail_base(payloadSize)}; }
#pragma pack(pop)

	tplt<tpnm T, tpnm TByte>
	::gpk::error_t				viewRead			(::gpk::view<T> & headerToRead, ::gpk::view<TByte> input)	{
		const packedu32					& header			= *(const packedu32*)input.begin();
		const uint32_t					counterWidth		= header.ValueWidth();
		ree_if(input.size() < counterWidth, "Invalid input size: %u", input.size());

		const uint32_t					elementCount		= header.Value();
		const uint32_t					dataSize			= sizeof(T) * elementCount;
		ree_if(dataSize > (input.size() - counterWidth), "Invalid input size: %u. Expected: %u.", input.size(), dataSize);

		headerToRead				= {(input.size() > counterWidth) ? (T*)&input[counterWidth] : 0, elementCount};
		return counterWidth + dataSize;
	}
	tplt<tpnm T>	stainli	::gpk::error_t	viewRead		(::gpk::view<const T> & headerToRead, const ::gpk::vcu8 & input)	{ return viewRead<const T, const uint8_t>(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewRead		(::gpk::view<const T> & headerToRead, const ::gpk::vci8 & input)	{ return viewRead<const T, const int8_t >(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewRead		(::gpk::view<const T> & headerToRead, const ::gpk::vcc  & input)	{ return viewRead<const T, const char   >(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewRead		(::gpk::view<T> & headerToRead, ::gpk::vu8 input)					{ return viewRead<T, uint8_t>(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewRead		(::gpk::view<T> & headerToRead, ::gpk::vi8 input)					{ return viewRead<T, int8_t >(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewRead		(::gpk::view<T> & headerToRead, ::gpk::vc  input)					{ return viewRead<T, char   >(headerToRead, input); }

	tplt<tpnm T, tpnm TByte>
	::gpk::error_t				viewReadLegacy		(::gpk::view<T> & headerToRead, ::gpk::view<TByte> input)	{
		const uint32_t					counterWidth		= sizeof(uint32_t);
		ree_if(input.size() < counterWidth, "Invalid input size: %u", input.size());

		const uint32_t					elementCount		= *(const uint32_t*)input.begin();
		const uint32_t					dataSize			= elementCount * sizeof(T);
		ree_if(dataSize > (input.size() - counterWidth), "Invalid input size: %u. Expected: %u", input.size(), dataSize);

		headerToRead				= {(input.size() > counterWidth) ? (T*)&input[counterWidth] : 0, elementCount};
		return counterWidth + dataSize;
	}
	tplt<tpnm T>	stainli	::gpk::error_t	viewReadLegacy	(::gpk::view<const T> & headerToRead, const ::gpk::vcu8 & input)	{ return viewReadLegacy<const T, const uint8_t>(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewReadLegacy	(::gpk::view<const T> & headerToRead, const ::gpk::vci8 & input)	{ return viewReadLegacy<const T, const int8_t >(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewReadLegacy	(::gpk::view<const T> & headerToRead, const ::gpk::vcc  & input)	{ return viewReadLegacy<const T, const char   >(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewReadLegacy	(::gpk::view<T> & headerToRead, ::gpk::vu8 input)					{ return viewReadLegacy<T, uint8_t>(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewReadLegacy	(::gpk::view<T> & headerToRead, ::gpk::vi8 input)					{ return viewReadLegacy<T, int8_t >(headerToRead, input); }
	tplt<tpnm T>	stainli	::gpk::error_t	viewReadLegacy	(::gpk::view<T> & headerToRead, ::gpk::vc  input)					{ return viewReadLegacy<T, char   >(headerToRead, input); }

	tplt<tpnm _tPOD> 
	::gpk::error_t				loadPOD				(::gpk::vcu8 & input, _tPOD & output) { 
		rees_if(input.byte_count() < sizeof(_tPOD));
		memcpy(&output, input.begin(), sizeof(_tPOD));
		gpk_necs(input.slice(input, sizeof(_tPOD)));
		return sizeof(_tPOD);
	}
	tplt<tpnm _tPOD>	stainli	::gpk::error_t	loadPOD		(::gpk::vci8 & input, _tPOD & output) { return loadPOD (*(::gpk::vcu8*)& input, output); }
	tplt<tpnm _tPOD>	stainli	::gpk::error_t	loadPOD		(::gpk::vcc  & input, _tPOD & output) { return loadPOD (*(::gpk::vcu8*)& input, output); }

	tplt<tpnm _tPOD> 
	::gpk::error_t				loadView			(::gpk::vcu8 & input, ::gpk::view<const _tPOD> & output) { 
		uint32_t						bytesRead				= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(output, input)); 
		gpk_necs(input.slice(input, bytesRead));
		return 0;
	}
	tplt<tpnm _tPOD>	stainli	::gpk::error_t	loadView	(::gpk::vci8 & input, ::gpk::view<_tPOD> & output) { return loadView(*(::gpk::vcu8*)& input, output); }
	tplt<tpnm _tPOD>	stainli	::gpk::error_t	loadView	(::gpk::vcc  & input, ::gpk::view<_tPOD> & output) { return loadView(*(::gpk::vcu8*)& input, output); }
} // namespace

#endif // GPK_VIEW_SERIALIZE_H_23627
