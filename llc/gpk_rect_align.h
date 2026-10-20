#include "gpk_rect2.h"
#include "gpk_bit.h"
#include "gpk_axis.h"

#ifndef GPK_RECT_ALIGN_H_23627
#define GPK_RECT_ALIGN_H_23627

namespace gpk
{
	tplt<tpnm T, tpnm TTarget>
	rect2<T> &	realignRectangle
		( cnst n2<TTarget>	& targetSize
		, cnst rect2<T>		& rectangleToRealign
		, rect2<T>			& rectangleRealigned
		, ALIGN				align
		) nxpt {
		rectangleRealigned		= rectangleToRealign;
			 if(bit_true(align, ALIGN_HCENTER	)) rectangleRealigned.Offset.x += (T)(targetSize.x >> 1) - (rectangleRealigned.Size.x >> 1);
		else if(bit_true(align, ALIGN_RIGHT		)) rectangleRealigned.Offset.x =  (T) targetSize.x - (rectangleRealigned.Size.x + rectangleRealigned.Offset.x);

			 if(bit_true(align, ALIGN_VCENTER	)) rectangleRealigned.Offset.y += (T)(targetSize.y >> 1) - (rectangleRealigned.Size.y >> 1);
		else if(bit_true(align, ALIGN_BOTTOM		)) rectangleRealigned.Offset.y =  (T) targetSize.y - (rectangleRealigned.Size.y + rectangleRealigned.Offset.y);
		return rectangleRealigned;
	}
} // namespace

#endif // GPK_RECT_ALIGN_H_23627


