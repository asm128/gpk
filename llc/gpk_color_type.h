/// Copyright 2009-2023 - asm128
#include "gpk_enum.h"

#ifndef GPK_COLOR_TYPE_H_23627
#define GPK_COLOR_TYPE_H_23627

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(COLOR_TYPE, u8);
	GDEFINE_ENUM_VALUE(COLOR_TYPE, GRAYSCALE		, 0); // Grayscale
	GDEFINE_ENUM_VALUE(COLOR_TYPE, RGB				, 2); // Red Green Blue 
	GDEFINE_ENUM_VALUE(COLOR_TYPE, PALETTE			, 3); // Palette
	GDEFINE_ENUM_VALUE(COLOR_TYPE, GRAYSCALE_ALPHA	, 4); // Grayscale Alpha 
	GDEFINE_ENUM_VALUE(COLOR_TYPE, RGBA				, 6); // Red Green Blue Alpha
	GDEFINE_ENUM_VALUE(COLOR_TYPE, BGR				, 7); // Blue Green Red
	GDEFINE_ENUM_VALUE(COLOR_TYPE, BGRA				, 8); // Blue Green Red Alpha
	GDEFINE_ENUM_VALUE(COLOR_TYPE, FILE				, 255); // Defined by file on load
#pragma pack(pop)
} // namespace

#endif // GPK_COLOR_TYPE_H_23627
