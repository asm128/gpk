#include "gpk_framework.h"
#include "gpk_gui.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

//struct SCameraVectors {
//						::cho::SCoord3<float>							Right, Up, Front;
//};

struct SApplication {
						::gpk::SFramework								Framework									;

						::gpk::SImage<::gpk::SColorBGRA>				TextureFont									= {};
						::gpk::SImageMonochrome<uint32_t>				TextureFontMonochrome						= {};
						::gpk::SGUI										GUI											= {};
						::gpk::STriangle3D	<float>						CubePositions[12]							= {};
						int32_t											IdViewport									= -1;

																		SApplication								(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423
