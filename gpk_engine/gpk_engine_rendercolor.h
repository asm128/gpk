#include "gpk_color.h"

#ifndef GPK_ENGINE_RENDERCOLOR_H_23627
#define GPK_ENGINE_RENDERCOLOR_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	struct SRenderColor {
		::gpk::rgbaf							Diffuse;
		::gpk::rgbaf							Ambient;
		::gpk::rgbaf							Specular;
	};
#pragma pack(pop)
} // namespace

#endif // GPK_ENGINE_RENDERCOLOR_H_23627
