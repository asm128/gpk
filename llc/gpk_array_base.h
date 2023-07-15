#include "gpk_view.h"
#include "gpk_safe.h"
#include "gpk_memory.h"
#include "gpk_size.h"

#ifndef GPK_ARRAY_BASE_H_23627
#define GPK_ARRAY_BASE_H_23627

namespace gpk
{
	template<typename... _Args>	
	void						clear					(_Args&&... args)							{ const int32_t results[] = {args.clear()..., 0}; (void)results; }

	template<typename... _tArgs>
	::gpk::error_t				resize					(uint32_t newSize, _tArgs&&... args)		{
		const uint32_t					oldSizes	[]			= {args.size	()..., 0};
		const ::gpk::error_t			results		[]			= {args.resize	(newSize)..., 0};
		for(uint32_t i = 0; i < ::gpk::size(results); ++i)
			if(errored(results[i])) {
				error_printf("Failed to set container %i to size: %i. Out of memory?", i, (int32_t)newSize);
				int32_t							j						= 0;
				const int32_t					dummy	[]				= {args.resize(oldSizes[j++])..., 0};
				(void)dummy;
				return -1;
			}
		return newSize;
	}

	template<typename... _tArgs>
	::gpk::error_t				resize_obj				(uint32_t newSize, _tArgs&&... args)		{
		const uint32_t					oldSizes	[]			= {args.size	()..., 0};
		const ::gpk::error_t			results		[]			= {args.resize	(newSize,{})..., 0};
		for(uint32_t i = 0; i < ::gpk::size(results); ++i)
			if(errored(results[i])) {
				error_printf("Failed to set container %i to size: %i. Out of memory?", i, (int32_t)newSize);
				int32_t							j						= 0;
				const int32_t					dummy	[]				= {args.resize(oldSizes[j++])..., 0};
				(void)dummy;
				return -1;
			}
		return newSize;
	}

	// Base for arrays that keeps track of its actual size.
#pragma pack(push, 1)
	template<typename T>
	class array_base : public view<T> {
		static	uint32_t		calc_reserve_count		(const uint32_t newCountRequested, uint32_t & outCount)	noexcept	{ 
			stacxpr	uint32_t			MAX_COUNT				= 0x7FFFFFFFU;
			if(MAX_COUNT < newCountRequested)
				return (uint32_t)-1;

			const uint32_t				desiredCount			= newCountRequested + (newCountRequested >> 2);
			return outCount = (desiredCount > MAX_COUNT || desiredCount < newCountRequested) ? MAX_COUNT : desiredCount; 
		}

	protected:
		using					view<T>::Data;

		typedef					array_base<T>			TArray;
		uint32_t				Size					: 31;
		uint32_t				NoAlloc					: 1;
//		uint32_t				Offset					= 0;

		inline					~array_base				()							noexcept	{ if(0 == NoAlloc) ::gpk::safe_gpk_free(Data); }

		inlcxpr					array_base				()							noexcept	: Size(0), NoAlloc(0) {}
		inlcxpr					array_base				(const TArray &	 other)		noexcept	= delete;
		inlcxpr					array_base				(const TArray && other)		noexcept	= delete;

		TArray&					operator =				(const TArray &	 other)					= delete;
		TArray&					operator =				(const TArray && other)					= delete;
		// This helper method is used to prevent redundancies. It returns a safe integer of the same or a higher value than the one passed as argument.
		static	::gpk::error_t	alloc_with_reserve		(const uint32_t newCount, T*& reserved)	noexcept	{ 
			uint32_t					newSize;
			gpk_necall(calc_reserve_count(newCount, newSize), "Allocation too large. newCount: %i", newCount);
			const uint32_t				bytesToAllocate			= ::gpk::size<T>(newSize) + 2;
			ree_if(0 == (reserved = (T*)::gpk::gpk_malloc(bytesToAllocate)), "Failed to allocate buffer with size: %u.", bytesToAllocate); 
			return (::gpk::error_t)newSize;
		}
	}; // array_base
#pragma pack(pop)
}

#endif // GPK_ARRAY_BASE_H_23627
