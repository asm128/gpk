/// Copyright 2010-2017 - asm128
#include "gpk_eval.h"
#include <malloc.h>
#include <memory.h>
#include "gpk_auto_handler.h"

#ifndef GPK_MEMORY_H__92836409283642038462309846
#define GPK_MEMORY_H__92836409283642038462309846

#define GPK_MALLOC_CUSTOM_ALIGN					16
#ifdef GPK_MALLOC_CUSTOM_ALIGN
#	define GPK_MALLOC_ALIGN						GPK_MALLOC_CUSTOM_ALIGN
#else
#	define GPK_MALLOC_ALIGN						sizeof(void*)
#endif

//#define GREF_CUSTOM_ALIGN						16
//#ifdef GREF_CUSTOM_ALIGN
//#	define BASETYPE_ALIGN						GREF_CUSTOM_ALIGN
//#else
//#	define BASETYPE_ALIGN						sizeof(void*)
//#endif

//#define	gpk_calc_align_address		(alignment, address)	((alignment - ((alignment - 1) & (uintptr_t)address)) & (alignment - 1))	// returns the difference between an origin address and the next aligned addres. The alignment must always be a power of two.
//#define	gpk_calc_align_address_4	(address)				gpk_calc_align_address( 4, address)	// (0x04 - (0x03 & (uintptr_t)address) & 0x03)
//#define	gpk_calc_align_address_8	(address)				gpk_calc_align_address( 8, address)	// (0x08 - (0x07 & (uintptr_t)address) & 0x07)
//#define	gpk_calc_align_address_16	(address)				gpk_calc_align_address(16, address)	// (0x10 - (0x0F & (uintptr_t)address) & 0x0F)
//#define	gpk_calc_align_address_32	(address)				gpk_calc_align_address(32, address)	// (0x20 - (0x1F & (uintptr_t)address) & 0x1F)
//#define	gpk_calc_align_address_64	(address)				gpk_calc_align_address(64, address)	// (0x40 - (0x3F & (uintptr_t)address) & 0x3F)

namespace gpk
{
	static constexpr		const uintptr_t															calc_align_address			(uintptr_t alignment, uintptr_t address)			noexcept	{ return ((alignment - ((alignment - 1) & address)) & (alignment - 1)); }
	static inline constexpr	const uintptr_t															calc_align_address_4		(uintptr_t address)									noexcept	{ return calc_align_address( 4, address); }
	static inline constexpr	const uintptr_t															calc_align_address_8		(uintptr_t address)									noexcept	{ return calc_align_address( 8, address); }
	static inline constexpr	const uintptr_t															calc_align_address_16		(uintptr_t address)									noexcept	{ return calc_align_address(16, address); }
	static inline constexpr	const uintptr_t															calc_align_address_32		(uintptr_t address)									noexcept	{ return calc_align_address(32, address); }
	static inline constexpr	const uintptr_t															calc_align_address_64		(uintptr_t address)									noexcept	{ return calc_align_address(64, address); }

#if defined(GPK_WINDOWS)
	static inline			void																	gpk_free					(void* ptr)											noexcept	{ _aligned_free(ptr);									}
#else
	static inline			void																	gpk_free					(void* ptr)											noexcept	{ ::free(ptr);											}
#endif
#if defined(GPK_WINDOWS)
	static inline			void*																	gpk_malloc					(size_t size)										noexcept	{ byte_t* p = (byte_t*)_aligned_malloc(size + 1, GPK_MALLOC_ALIGN); if(p) (p[size] = 0); return p; }
#elif defined(GPK_LINUX) || defined(GPK_ANDROID)
	static inline			void*																	gpk_malloc					(size_t size)										noexcept	{ byte_t* p = (byte_t*)::memalign(GPK_MALLOC_ALIGN, size); if(p) (p[size] = 0); return p; }
#else
	static inline			void*																	gpk_malloc					(size_t size)										noexcept	{ byte_t* p = (byte_t*)::malloc(size + GPK_MALLOC_ALIGN); int offset = calc_align_address(GPK_MALLOC_ALIGN, p); if(p) (p[size] = 0); return p; }
#endif

	template<typename _typePtr>
	static inline			void																	safe_gpk_free				(_typePtr &p)										noexcept	{
		_typePtr																							_pepe						= p;
		p																								= 0;
#if defined(GPK_WINDOWS)
		_aligned_free((void*)_pepe);
#elif defined(GPK_LINUX) || defined(GPK_ANDROID)
		free((void*)_pepe);
#endif
	}

	struct auto_gpk_free : public ::gpk::auto_handler<void*, nullptr>					{ using TWrapper::auto_handler; inline ~auto_gpk_free() noexcept { close(); } inline void close() noexcept { safe_gpk_free(Handle); } };

#define GREF_PAGE_SIZE_MAX (4096)
	template<typename _tBase>	static inline constexpr		uint32_t								get_page_size				()																noexcept	{ return (uint32_t)(sizeof(_tBase) <= GREF_PAGE_SIZE_MAX) ? GREF_PAGE_SIZE_MAX/sizeof(_tBase) : 1; };
	template<typename _tBase>	static inline constexpr		uint32_t								get_type_size				()																noexcept	{ return (uint32_t) sizeof(_tBase); }
	template<typename _tBase>	static inline constexpr		uint32_t								get_type_size				(const _tBase&)													noexcept	{ return (uint32_t) sizeof(_tBase); }
	template<typename _tBase>	static inline constexpr		uint32_t								get_type_size_padded		(uint32_t paddingInBytes)										noexcept	{ return paddingInBytes ? (uint32_t) ( (sizeof(_tBase) / paddingInBytes) + one_if(sizeof(_tBase) % paddingInBytes) ) * paddingInBytes : sizeof(_tBase); }
	template <typename _tBase>	static inline constexpr		uint32_t								get_type_align				()																noexcept	{
		return (uint32_t)
			(	(0 == (sizeof(_tBase) % 32))	? 32
			:	(0 == (sizeof(_tBase) % 16))	? 16
			:	(0 == (sizeof(_tBase) % 8 ))	? 8
			:	(0 == (sizeof(_tBase) % 4 ))	? 4
			:	(0 == (sizeof(_tBase) % 2 ))	? 2
			:	1
			);
	}
	template <typename _tBase>	static inline constexpr		uint32_t								get_type_align_multiplier	()																noexcept	{
		return (uint32_t)
			(	(0 == (sizeof(_tBase) % 32)) ? sizeof(_tBase) / 32
			:	(0 == (sizeof(_tBase) % 16)) ? sizeof(_tBase) / 16
			:	(0 == (sizeof(_tBase) %  8)) ? sizeof(_tBase) /  8
			:	(0 == (sizeof(_tBase) %  4)) ? sizeof(_tBase) /  4
			:	(0 == (sizeof(_tBase) %  2)) ? sizeof(_tBase) /  2
			:	sizeof(_tBase)
			);
	}
	template <typename _tBase>	static inline				int32_t									podcmp						(const _tBase* pA, const _tBase* pB)							noexcept	{
		if(0 == pA)
			return (0 == pB) ? 0 : 0x7FFFFFFF;
		else if(0 == pB)
			return 0x7FFFFFFF;

		return memcmp(pA, pB, sizeof(_tBase));
	}

	template <typename _tBase>	static inline				_tBase*									chkcpy						(_tBase* destination, const _tBase* source, uint32_t count)		noexcept	{
			for (uint32_t i = 0; i < count; ++i)
				if (destination[i] != source[i])
					destination[i]  = source[i];
			return destination;
	}

	template <typename _tBase>								_tBase*									podcpy						(_tBase* dest, const _tBase* source)							noexcept	{
		static constexpr	const uint32_t																	dataMultiplier				= (uint32_t)get_type_align_multiplier<_tBase>();
		static constexpr	const uint32_t																	typeAlign					= (uint32_t)get_type_align<_tBase>();
		switch (typeAlign) {
		case 32: chkcpy((uint64_t*)dest, (const uint64_t*)source, dataMultiplier * 4);	break;
		case 16: chkcpy((uint64_t*)dest, (const uint64_t*)source, dataMultiplier * 2);	break;
		case  8: chkcpy((uint64_t*)dest, (const uint64_t*)source, dataMultiplier);		break;
		case  4: chkcpy((uint32_t*)dest, (const uint32_t*)source, dataMultiplier);		break;
		case  2: chkcpy((uint16_t*)dest, (const uint16_t*)source, dataMultiplier);		break;
		default:
			if( memcmp(dest, source, sizeof(_tBase)) )
				memcpy(dest, source, sizeof(_tBase));
			break;
		}
		return dest;
	}

	template <typename _tBase, size_t _sizeArray>			_tBase*									memcpy_s					(_tBase (&dest)[_sizeArray], const _tBase (&src)[_sizeArray])	noexcept	{ ::memcpy(dest, src, sizeof(_tBase) * _sizeArray); return dest; }
}	// namespace

#endif // GPK_MEMORY_H__92836409283642038462309846
