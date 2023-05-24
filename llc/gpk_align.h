#include "gpk_n2.h"
#include "gpk_bit.h"
#include "gpk_axis.h"

#ifndef GPK_ALIGN_H_928374982364923524
#define GPK_ALIGN_H_928374982364923524

namespace gpk
{
#pragma pack(push, 1)
	//------------------------------------------------------------------------------------------------------------
	template <typename TCoord, typename TTarget>
	::gpk::n2<TCoord>&		realignCoord
		( const ::gpk::n2<TTarget>	& targetSize
		, const ::gpk::n2<TCoord>	& coordToRealign
		, ::gpk::n2<TCoord>			& coordRealigned
		, ::gpk::ALIGN				align
		)																																					noexcept	{
		coordRealigned		= coordToRealign;
 			 if gbit_true(align, ::gpk::ALIGN_CENTER	) coordRealigned.x	+= (targetSize.x >> 1);
		else if gbit_true(align, ::gpk::ALIGN_RIGHT		) coordRealigned.x	= targetSize.x - coordToRealign.x;
												  
			 if gbit_true(align, ::gpk::ALIGN_VCENTER	) coordRealigned.y	+= (targetSize.y >> 1);
		else if gbit_true(align, ::gpk::ALIGN_BOTTOM	) coordRealigned.y	= targetSize.y - coordToRealign.y;
		return coordRealigned;
	}
#pragma pack(pop)
} // namespace

#endif // GPK_ALIGN_H_928374982364923524
