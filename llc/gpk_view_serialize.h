#include "gpk_view.h"

#ifndef GPK_VIEW_SERIALIZE_H_23627
#define GPK_VIEW_SERIALIZE_H_23627

namespace gpk
{
	stacxpr	uint8_t				tail_width			(uint32_t count) 	{
		//return (count > 0x3FFFFFFF) ? 4
		return uint8_t
			( (count > 0x003FFFFF) ? 3
			: (count > 0x00003FFF) ? 2
			: (count > 0x0000003F) ? 1
			: 0
			);
	}
	
	stincxp	uint8_t				header_width		(uint32_t count)	{ return tail_width(count) + 1; }

	stacxpr	uint8_t				tail_multiplier		(uint32_t count)	{
		//return (count > 0x3FFFFFFF) ? (count & 0x3F00000000) >> 32
		return uint8_t
			( (count > 0x003FFFFF) ? ((count & 0x3F000000) >> 24)
			: (count > 0x00003FFF) ? ((count & 0x003F0000) >> 16)
			: (count > 0x0000003F) ? ((count & 0x00003F00) >> 8)
			: count
			);
	}
	stacxpr	uint32_t			tail_base			(uint32_t count)	{
		//return (count > 0x3FFFFFFF) ? (count & 0xFFFFFFFFU) >> 8
		return (count > 0x003FFFFF) ? count & 0x00FFFFFFU
			 : (count > 0x00003FFF) ? count & 0x0000FFFFU
			 : (count > 0x0000003F) ? count & 0x000000FFU
			 : 0;
	}
#pragma pack(push, 1)
	struct SSerializedViewHeader { 
		uint8_t						TailWidth			: 2;
		uint8_t						Multiplier			: 6;
		uint32_t					Tail				= 0;

		inlcxpr uint8_t				CounterWidth		() 	const	noexcept	{ return TailWidth + 1; }
		cnstxpr	uint32_t			ElementCount		()	const	noexcept	{
			return (0 == TailWidth) ? Multiplier
				 : (1 == TailWidth) ? (uint32_t(Multiplier) <<  8) + *(uint8_t *)&Tail
				 : (2 == TailWidth) ? (uint32_t(Multiplier) << 16) + *(uint16_t*)&Tail
				 : (3 == TailWidth) ? (uint32_t(Multiplier) << 24) + (((*(uint32_t*)this) & 0xFFFFFF00U) >> 8)
				//: (4 == TailWidth) ? 
				 : Multiplier 
				 ;
		}

		tplt<tpnm T>
		cnstxpr	uint32_t			DataSize			()	const	noexcept	{ return ElementCount() * sizeof(T); }
	};
#pragma pack(pop)
	tplt<tpnm T, tpnm TByte>
	::gpk::error_t				viewRead			(::gpk::view<T> & headerToRead, ::gpk::view<TByte> input)	{
		const SSerializedViewHeader		& header			= *(const SSerializedViewHeader*)input.begin();
		const uint32_t					counterWidth		= header.CounterWidth();
		ree_if(input.size() < counterWidth, "Invalid input size: %u", input.size());

		const uint32_t					elementCount		= header.ElementCount();
		const uint32_t					dataSize			= header.DataSize<T>();
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
		::gpk::view<const _tPOD>		readView			= {}; 
		uint32_t						bytesRead			= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(readView, input)); 
		gpk_necs(input.slice(input, bytesRead));
		output						= readView[0]; 
		return 0;
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
