#include "gpk_view_array.h"

#ifndef GPK_ARRAY_STATIC_H_2983749823749826534465243
#define GPK_ARRAY_STATIC_H_2983749823749826534465243
namespace gpk
{
#pragma pack(push, 1)
	template<typename _tCell, uint32_t _sizeArray>
	struct array_static	{
							_tCell						Storage	[_sizeArray]						;

							operator					view_array<_tCell>							()												{ return {Storage, _sizeArray}; }
							operator					view_array<const _tCell>					()										const	noexcept	{ return {Storage, _sizeArray}; }

		inline				const _tCell&				operator[]									(uint32_t index)						const				{
			gthrow_if(index >= _sizeArray, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_sizeArray);
			return Storage[index];
		}
		inline				_tCell&						operator[]									(uint32_t index)											{
			gthrow_if(index >= _sizeArray, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_sizeArray);
			return Storage[index];
		}
		//constexpr										array_static								()															= default;
		//												array_static								(::std::initializer_list<_tCell> init)						{
		//	gthrow_if(init.size() > _sizeArray, "%s", "Failed to resize array! Why?");
		//	for(uint32_t i = 0, count = (uint32_t)init.size(); i < count; ++i)
		//		Storage[i]										= *(init.begin() + i);
		//}

		inline constexpr	uint32_t					size										()										const	noexcept	{ return _sizeArray; }
	};
#pragma pack(pop)
} // namespace

#endif // GPK_ARRAY_STATIC_H_2983749823749826534465243