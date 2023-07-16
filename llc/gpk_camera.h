#include "gpk_minmax_n3.h"

#ifndef GPK_CAMERA_H_23627
#define GPK_CAMERA_H_23627

namespace gpk
{
#pragma pack(push, 1)
	struct SCameraPoints {
		::gpk::n3f32		Position	= {-0.5f, .05f, -.125f};
		::gpk::n3f32		Target		= {};
	};

	struct SCameraVectors {
		::gpk::n3f32		Front		= {1, 0, 0};
		::gpk::n3f32		Up			= {0, 1, 0};
		::gpk::n3f32		Right		= {0, 0, 1};
	};

	struct SCameraRange {
		::gpk::minmaxf32	NearFar		= {.01f, 1000.f};
		double				Angle		= ::gpk::math_pi * .25;
		double				Zoom		= 1.0;
	};
#pragma pack(pop)
} // namespace

#endif // GPK_CAMERA_H_23627
