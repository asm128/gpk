#include "gpk_n2.h"
#include "gpk_bit.h"
#include "gpk_axis.h"

#ifndef GPK_ALIGN_H_23627
#define GPK_ALIGN_H_23627

namespace gpk
{
#pragma pack(push, 1)
	//------------------------------------------------------------------------------------------------------------
	tplt <tpnm TCoord, tpnm TTarget>
	::gpk::n2<TCoord>&		realignCoord
		( const ::gpk::n2<TTarget>	& targetSize
		, const ::gpk::n2<TCoord>	& coordToRealign
		, ::gpk::n2<TCoord>			& coordRealigned
		, ::gpk::ALIGN				align
		)																		noexcept	{
		coordRealigned		= coordToRealign;
 			 if (::gpk::bit_true(align, ::gpk::ALIGN_CENTER)) coordRealigned.x += (targetSize.x >> 1);
		else if (::gpk::bit_true(align, ::gpk::ALIGN_RIGHT )) coordRealigned.x = targetSize.x - 1 - coordToRealign.x;
											  
			 if (::gpk::bit_true(align, ::gpk::ALIGN_VCENTER)) coordRealigned.y += (targetSize.y >> 1);
		else if (::gpk::bit_true(align, ::gpk::ALIGN_BOTTOM )) coordRealigned.y = targetSize.y - 1 - coordToRealign.y;
		return coordRealigned;
	}
#pragma pack(pop)
} // namespace

#endif // GPK_ALIGN_H_23627
