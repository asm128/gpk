#include "gpk_color.h"

#ifndef GPK_ENGINE_RENDERCOLOR_H_23627
#define GPK_ENGINE_RENDERCOLOR_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	struct SRenderColor {
		::gpk::rgbaf							Diffuse		= ::gpk::RED;
		::gpk::rgbaf							Ambient		= ::gpk::GREEN;
		::gpk::rgbaf							Specular	= ::gpk::LIGHTGRAY;
	};
#pragma pack(pop)
} // namespace

#endif // GPK_ENGINE_RENDERCOLOR_H_23627
