#include "gpk_color.h"

#ifndef GPK_ENGINE_RENDERCOLOR_H
#define GPK_ENGINE_RENDERCOLOR_H

namespace gpk 
{
#pragma pack(push, 1)
	struct SRenderColor {
		::gpk::SColorFloat							Ambient;
		::gpk::SColorFloat							Diffuse;
		::gpk::SColorFloat							Specular;
	};
#pragma pack(pop)
} // namespace

#endif
