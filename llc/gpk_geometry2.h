#include "gpk_enum.h"

#ifndef GPK_GEOMETRY2_H
#define GPK_GEOMETRY2_H

namespace gpk
{
#ifdef GPK_ATMEL
	enum SHAPE_2D_TYPE : uint8_t
		{ SHAPE_2D_TYPE_Custom		= 0
		, SHAPE_2D_TYPE_Point		= 1
		, SHAPE_2D_TYPE_Line		= 2
		, SHAPE_2D_TYPE_Rectangle	= 3
		, SHAPE_2D_TYPE_Circle		= 4
		, SHAPE_2D_TYPE_Triangle	= 5
		, SHAPE_2D_TYPE_Ring		= 6
		};
#else
	GDEFINE_ENUM_TYPE(SHAPE_2D_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(SHAPE_2D_TYPE, Custom	, 0);
	GDEFINE_ENUM_VALUE(SHAPE_2D_TYPE, Point		, 1);
	GDEFINE_ENUM_VALUE(SHAPE_2D_TYPE, Line		, 2);
	GDEFINE_ENUM_VALUE(SHAPE_2D_TYPE, Rectangle	, 3);
	GDEFINE_ENUM_VALUE(SHAPE_2D_TYPE, Circle	, 4);
	GDEFINE_ENUM_VALUE(SHAPE_2D_TYPE, Triangle	, 5);
	GDEFINE_ENUM_VALUE(SHAPE_2D_TYPE, Ring		, 6);
#endif
} // namespace

#endif // GPK_GEOMETRY2_H