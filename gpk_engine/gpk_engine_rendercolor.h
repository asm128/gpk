#include "gpk_color.h"

#ifndef GPK_ENGINE_RENDERCOLOR_H_23627
#define GPK_ENGINE_RENDERCOLOR_H_23627

nmsp gpk 
{
#pragma pack(push, 1)
	struct SRenderColor {
		::gpk::rgbaf							Diffuse		= ::gpk::RED;
		::gpk::rgbaf							Ambient		= ::gpk::GREEN;
		::gpk::rgbaf							Specular	= ::gpk::LIGHTGRAY;
	};
#pragma pack(pop)
} // nmsp

#endif // GPK_ENGINE_RENDERCOLOR_H_23627
