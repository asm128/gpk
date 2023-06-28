#include "gpk_view.h"

#ifndef GPK_VIEW_SERIALIZE_H_23627
#define GPK_VIEW_SERIALIZE_H_23627

namespace gpk
{
	stacxpr	uint8_t				tail_width			(uint32_t count) 	{
		//	 if(count > 0x3FFFFFFF) return 4; else 
			 if(count > 0x003FFFFF) return 3;
		else if(count > 0x00003FFF) return 2;
		else if(count > 0x0000003F) return 1;
		else
			return 0;
	}
	stincxp	uint8_t				header_width		(uint32_t count)	{ return tail_width(count) + 1; }

	stacxpr	uint8_t				tail_multiplier		(uint32_t count)	{
		//	 if(count > 0x3FFFFFFF) return (count & 0x3F00000000) >> 32; else 
			 if(count > 0x003FFFFF) return uint8_t((count & 0x3F000000) >> 24);
		else if(count > 0x00003FFF) return uint8_t((count & 0x003F0000) >> 16);
		else if(count > 0x0000003F) return uint8_t((count & 0x00003F00) >> 8);
		else
			return (uint8_t)count;
	}
	stacxpr	uint32_t			tail_base			(uint32_t count)	{
		//	 if(count > 0x3FFFFFFF) return (count & 0xFFFFFFFFU) >> 8; else 
			 if(count > 0x003FFFFF) return count & 0x00FFFFFFU;
		else if(count > 0x00003FFF) return count & 0x0000FFFFU;
		else if(count > 0x0000003F) return count & 0x000000FFU;
		else
			return 0;
	}

#pragma pack(push, 1)
	struct SSerializedViewHeader { 
		uint8_t						TailWidth			: 2;
		uint8_t						Multiplier			: 6;
		uint32_t					Tail				= 0;

		inlcxpr uint32_t			ActualWidth			() 	const	noexcept	{ return TailWidth + 1; }
		cnstxpr	uint32_t			ActualCount			()	const	noexcept	{
			switch(TailWidth) {
			case 0: return Multiplier;
			case 1: return (uint32_t(Multiplier) << 8) + *(uint8_t*)&Tail;
			case 2: return (uint32_t(Multiplier) << 16) + *(uint16_t*)&Tail;
			case 3: return (uint32_t(Multiplier) << 24) + (((*(uint32_t*)this) & 0xFFFFFF00U) >> 8);
			//case 4: return Multiplier * Tail;
			}
			return Multiplier;
		}
	};
#pragma pack(pop)

	template<typename T>
	::gpk::error_t				viewReadLegacy		(::gpk::view<const T> & headerToRead, const ::gpk::vcu8 & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t					elementCount		= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(T)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(T));
		headerToRead				= {(input.size() > sizeof(uint32_t)) ? (const T*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(T);
	}
	template<typename T>
	::gpk::error_t				viewReadLegacy		(::gpk::view<T> & headerToRead, const ::gpk::vu8 & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t					elementCount		= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(T)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(T));
		headerToRead				= {(input.size() > sizeof(uint32_t)) ? (T*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(T);
	}
	template<typename T>
	::gpk::error_t				viewRead			(::gpk::view<const T> & headerToRead, const ::gpk::vcu8 & input)	{
		const SSerializedViewHeader		& header			= *(const SSerializedViewHeader*)input.begin();
		const uint32_t					elementCount		= header.ActualCount();
		const uint32_t					counterWidth		= header.ActualWidth();
		ree_if(input.size() < counterWidth, "Invalid input size: %u", input.size());
		ree_if((elementCount * sizeof(T)) > (input.size() - counterWidth), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(T));
		headerToRead				= {(input.size() > counterWidth) ? (const T*)&input[counterWidth] : 0, elementCount};
		return counterWidth + headerToRead.size() * sizeof(T);
	}
	template<typename T>	::gpk::error_t	viewRead	(::gpk::view<const T> & headerToRead, const ::gpk::vci8 & input)	{ return viewRead(headerToRead, *(const ::gpk::vcu8*)&input); }
	template<typename T>	::gpk::error_t	viewRead	(::gpk::view<const T> & headerToRead, const ::gpk::vcc  & input)	{ return viewRead(headerToRead, *(const ::gpk::vcu8*)&input); }

	template<typename T>
	::gpk::error_t				viewRead			(::gpk::view<T> & headerToRead, const ::gpk::vu8 & input)	{
		const uint32_t					elementCount		= ((const SSerializedViewHeader*)input.begin())->ActualCount();
		const uint32_t					counterWidth		= ((const SSerializedViewHeader*)input.begin())->ActualWidth();
		ree_if(input.size() < counterWidth, "Invalid input size: %u", input.size());
		ree_if((elementCount * sizeof(T)) > (input.size() - counterWidth), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(T));
		headerToRead				= {(input.size() > counterWidth) ? (T*)&input[counterWidth] : 0, elementCount};
		return counterWidth + headerToRead.size() * sizeof(T);
	}
	template<typename T>	::gpk::error_t	viewRead	(::gpk::view<T> & headerToRead, const ::gpk::vi8 & input)			{ return viewRead(headerToRead, *(const ::gpk::vu8*)&input); }
	template<typename T>	::gpk::error_t	viewRead	(::gpk::view<T> & headerToRead, const ::gpk::vc  & input)			{ return viewRead(headerToRead, *(const ::gpk::vu8*)&input); }

	template<typename _tPOD> 
	::gpk::error_t				loadPOD				(::gpk::vcu8 & input, _tPOD & output) { 
		::gpk::view<const _tPOD>		readView			= {}; 
		uint32_t						bytesRead			= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(readView, input)); 
		gpk_necs(input.slice(input, bytesRead));
		output						= readView[0]; 
		return 0;
	}
	template<typename _tPOD> ::gpk::error_t	loadPOD		(::gpk::vci8 & input, _tPOD & output) { return loadPOD (*(::gpk::vcu8*)& input, output); }
	template<typename _tPOD> ::gpk::error_t	loadPOD		(::gpk::vcc  & input, _tPOD & output) { return loadPOD (*(::gpk::vcu8*)& input, output); }

	template<typename _tPOD> 
	::gpk::error_t				loadView			(::gpk::vcu8 & input, ::gpk::view<const _tPOD> & output) { 
		uint32_t						bytesRead				= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(output, input)); 
		gpk_necs(input.slice(input, bytesRead));
		return 0;
	}
	template<typename _tPOD> ::gpk::error_t	loadView	(::gpk::vci8 & input, ::gpk::view<_tPOD> & output) { return loadView(*(::gpk::vcu8*)& input, output); }
	template<typename _tPOD> ::gpk::error_t	loadView	(::gpk::vcc  & input, ::gpk::view<_tPOD> & output) { return loadView(*(::gpk::vcu8*)& input, output); }
} // namespace

#endif // GPK_VIEW_SERIALIZE_H_23627
