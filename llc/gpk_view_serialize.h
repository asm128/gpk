#include "gpk_view.h"

#ifndef GPK_VIEW_SERIALIZE_H_23627
#define GPK_VIEW_SERIALIZE_H_23627

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

	struct SSerializedViewHeader : packed_int<uint32_t> { 
		using packed_int<uint32_t>::Value		; 
		using packed_int<uint32_t>::ValueWidth	; 
		using packed_int<uint32_t>::packed_int	; 

		inlcxpr	SSerializedViewHeader() noexcept = default;
		inlcxpr	SSerializedViewHeader
			( uint8_t tailWidth		
			, uint8_t multiplier	
			, uint32_t tail			
			) : packed_int{tailWidth, multiplier, tail} {} 

		tplt<tpnm T>
		ndincxp uint32_t			DataSize			()	const	noexcept	{ return ElementCount() * sizeof(T); }
		ndincxp	uint8_t				CounterWidth		() 	const	noexcept	{ return ValueWidth(); }
		nodinli	uint32_t			ElementCount		()	const	noexcept	{ return Value(); }

	};

	nodstxp	SSerializedViewHeader	viewHeader	(uint32_t payloadSize) { return {tail_width(payloadSize), tail_multiplier(payloadSize), tail_base(payloadSize)}; }
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
