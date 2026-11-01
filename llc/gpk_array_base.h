#include "gpk_view.h"
#include "gpk_safe.h"
#include "gpk_memory.h"
#include "gpk_size.h"

#include "gpk_std_initializer_list.h"

#ifndef GPK_ARRAY_BASE_H_23627
#define GPK_ARRAY_BASE_H_23627

namespace gpk
{
	tpl_vtArgs	err_t			clear					(_tArgs&&... args)				nxpt	{ const s2_t results[] = {args.clear()..., 0}; for(cnst s2_t result : results) if(result < 0) rtrn result; rtrn 0; }
	tpl_vtArgs	err_t			resize					(u2_t newSize, _tArgs&&... args)		{
		u2_c					oldSizes	[]			= {args.size	()..., 0};
		const gpk::err_t 		results		[]			= {args.resize	(newSize)..., 0};
		for(u2_t i = 0; i < ::gpk::size(results); ++i)
			if_failed(results[i]) {
				error_printf("Failed to set container %" GPK_FMT_S2 " to size: %" GPK_FMT_S2 ". Out of memory?", i, (s2_t)newSize);
				s2_t							j						= 0;
				const s2_t					dummy	[]				= {args.resize(oldSizes[j++])..., 0};
				(void)dummy;
				return -1;
			}
		return newSize;
	}
	tpl_vtArgs	err_t			resize_obj				(u2_t newSize, _tArgs&&... args)		{
		u2_c					oldSizes	[]			= {args.size	()..., 0};
		const gpk::err_t 		results		[]			= {args.resize	(newSize,{})..., 0};
		for(u2_t i = 0; i < ::gpk::size(results); ++i)
			if_failed(results[i]) {
				error_printf("Failed to set container %" GPK_FMT_S2 " to size: %" GPK_FMT_S2 ". Out of memory?", i, (s2_t)newSize);
				s2_t							j						= 0;
				const s2_t					dummy	[]				= {args.resize(oldSizes[j++])..., 0};
				(void)dummy;
				return -1;
			}
		return newSize;
	}
	// Base for arrays that keeps track of its actual size.
#pragma pack(push, 1)
	tplT class array_base : public view<T> {
		static	u2_t		calc_reserve_count		(u2_c newCountRequested, u2_t & outCount)	noexcept	{ 
			stxp	u2_t			MAX_COUNT				= 0x3FFFFFFFU;
			if(MAX_COUNT < newCountRequested)
				return (u2_t)-1;
#if defined (GPK_ARDUINO) || defined(GPK_ESP32) || defined(GPK_ESP8266) || defined(ESP32)
			u2_c				desiredCount			= 2 + newCountRequested;//: + (newCountRequested >> 8);
#else
			u2_c				desiredCount			= 2 + newCountRequested + (newCountRequested >> 2);
#endif
			return outCount = (desiredCount > MAX_COUNT || desiredCount < newCountRequested) ? MAX_COUNT : desiredCount; 
		}

	protected:
		using					view<T>::Data;
		tydf					array_base<T>			TArray;

		u2_t				Size					: 30;
		u2_t				NoAlloc					: 2;
//		u2_t				Offset					= 0;

		inline					~array_base				()							noexcept	{ if(0 == NoAlloc) ::gpk::safe_gpk_free(Data); }

		inxp					array_base				()							noexcept	: Size(0), NoAlloc(0) {}
		inxp					array_base				(const TArray &	 other)		noexcept	= delete;
		inxp					array_base				(const TArray && other)		noexcept	= delete;

		TArray&					oper =				(const TArray &	 other)					= delete;
		TArray&					oper =				(const TArray && other)					= delete;
		// This helper method is used to prevent redundancies. It returns a safe integer of the same or a higher value than the one passed as argument.
		static	gpk::err_t alloc_with_reserve		(u2_c newCount, T* & reserved)	noexcept	{ 
			u2_t					newSize					= 0;
			if_fail_fef(calc_reserve_count(newCount, newSize), "Too large. newCount: %" GPK_FMT_S2 ".", newCount);
			u2_c				bytesToAllocate			= szof(T) * (newSize + 1);
			if(bool(reserved = (T*)::gpk::gpk_malloc(bytesToAllocate)))
				return (::gpk::err_t)newSize;
			error_printf("failed to reserve %" GPK_FMT_U2 " bytes.", bytesToAllocate);
			GPK_PLATFORM_CRT_CHECK_MEMORY();
			return -1;
		}
	}; // array_base
#pragma pack(pop)
}

#endif // GPK_ARRAY_BASE_H_23627
