#include "gpk_view.h"
#include "gpk_safe.h"
#include "gpk_memory.h"
#include "gpk_size.h"

#ifndef GPK_ARRAY_BASE_H_230518
#define GPK_ARRAY_BASE_H_230518

namespace gpk
{
	template<typename... _Args>	void	clear					(_Args&&... args)						{ const int32_t results[] = {args.clear()..., 0}; (void)results; }
	template<typename... _Args>	
	::gpk::error_t						resize					(uint32_t newSize, _Args&&... args)		{
		const uint32_t							oldSizes	[]			= {args.size	()			..., 0};
		const ::gpk::error_t					results		[]			= {args.resize	(newSize)	..., 0};
		for(uint32_t i = 0; i < ::gpk::min(::gpk::size(results), ::gpk::size(oldSizes)); ++i)
			if(errored(results[i])) {
				error_printf("Failed to set container %i to size: %i. Out of memory?", i, (int32_t)newSize);
				int32_t									j						= 0;
				const int32_t							dummy	[]				= {args.resize(oldSizes[j++])..., 0};
				(void)dummy;
				return -1;
			}
		return newSize;
	}

	// Base for arrays that keeps track of its actual size.
#pragma pack(push, 1)
	template<typename T>
	struct array_base : public view<T> {
		using				view<T>::byte_count;
	private:
		stincxp	uint32_t	calc_reserve_size		(const uint32_t newCount)	noexcept	{ return ::gpk::clamp(newCount, newCount + (newCount >> 2), 0xBFFFFFFFU); }

	protected:
		using				view<T>::Data;

		typedef				array_base<T>			TArray;
		uint32_t			Size					= 0;
//		uint32_t			Offset					= 0;

		inline				~array_base				()							noexcept	{ ::gpk::safe_gpk_free(Data); }
		
		inlcxpr				array_base				()							noexcept	= default;
		inlcxpr				array_base				(const TArray &	 other)		noexcept	= delete;
		inlcxpr				array_base				(const TArray && other)		noexcept	= delete;

		TArray&				operator =				(const TArray &	 other)					= delete;
		TArray&				operator =				(const TArray && other)					= delete;
		// This helper method is used to prevent redundancies. It returns a safe integer of the same or a higher value than the one passed as argument.
		static	inline	T*	alloc_with_reserve		(const uint32_t newCount, uint32_t & reservedSize)	noexcept	{ 
			const uint32_t			newSize					= calc_reserve_size(newCount);
			T						* newData				= (T*)::gpk::gpk_malloc(::gpk::size<T>(newSize) + 2); 
			reservedSize		= newSize;
			return newData;
		}
	}; // array_base
#pragma pack(pop)
}

#endif // GPK_ARRAY_BASE_H_230518
