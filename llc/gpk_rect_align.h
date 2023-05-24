#include "gpk_rect2.h"
#include "gpk_bit.h"
#include "gpk_axis.h"

#ifndef GPK_RECT_ALIGN_H_928374982364923524
#define GPK_RECT_ALIGN_H_928374982364923524

namespace gpk
{
	//------------------------------------------------------------------------------------------------------------
	template <typename T, typename TTarget>
	::gpk::rect2<T>&		realignRectangle
		(	const ::gpk::n2<TTarget>	& targetSize
		,	const ::gpk::rect2<T>		& rectangleToRealign
		,	::gpk::rect2<T>				& rectangleRealigned
		,	ALIGN						align
		)	noexcept	{
		rectangleRealigned															= rectangleToRealign;
			 if(::gpk::bit_true(align, ::gpk::ALIGN_HCENTER	))	{ rectangleRealigned.Offset.x += (T)(targetSize.x >> 1)	- (rectangleRealigned.Size.x >> 1); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_RIGHT	))	{ rectangleRealigned.Offset.x =  (T) targetSize.x		- (rectangleRealigned.Size.x + rectangleRealigned.Offset.x); }
		else													{}

			 if(::gpk::bit_true(align, ::gpk::ALIGN_VCENTER	))	{ rectangleRealigned.Offset.y += (T)(targetSize.y >> 1)	- (rectangleRealigned.Size.y >> 1); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_BOTTOM	))	{ rectangleRealigned.Offset.y =  (T) targetSize.y		- (rectangleRealigned.Size.y + rectangleRealigned.Offset.y); }
		else													{}
		return rectangleRealigned;
	}

	template <typename T>
	::gpk::rect2<T>&		dockRectangle
		(	const ::gpk::rect2<T>	& rectangleToDockTo
		,	const ::gpk::rect2<T>	& rectangleToDock
		,	::gpk::rect2<T>			& rectangleDocked
		,	ALIGN					align
		)	noexcept	{
		rectangleDocked		= rectangleToDock;
			 /*if(::gpk::bit_true(align, ::gpk::ALIGN_HCENTER	))	{ rectangleDocked.Offset.x = (rectangleToDockTo.Size.x >> 1)	- (rectangleDocked.Size.x >> 1)	+ rectangleDocked.Offset.x; }
		else*/if(::gpk::bit_true(align, ::gpk::ALIGN_RIGHT	))	{ rectangleDocked.Offset.x = rectangleToDockTo.Size.x  - (rectangleDocked.Size.x		+ rectangleDocked.Offset.x); }
		else													{ rectangleDocked.Offset.x = rectangleToDockTo.Size.x; }

			 if(::gpk::bit_true(align, ::gpk::ALIGN_VCENTER	))	{ rectangleDocked.Offset.y = (rectangleToDockTo.Size.y >> 1)	- (rectangleDocked.Size.y >> 1)	+ rectangleDocked.Offset.y; }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_BOTTOM	))	{ rectangleDocked.Offset.y = rectangleToDockTo.Size.y			- (rectangleDocked.Size.y		+ rectangleDocked.Offset.y); }
		else													{}
		return rectangleDocked;
	}
} // namespace

#endif // GPK_RECT_ALIGN_H_928374982364923524
