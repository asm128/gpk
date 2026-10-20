#include "gpk_n3.h"
#include "gpk_minmax.h"

#ifndef GPK_CAMERA_H_23627
#define GPK_CAMERA_H_23627

namespace gpk
{
	stct SCameraPoints {
		n3f2_t	Position	= {-0.45f, 0.05f, -0.125f};
		n3f2_t	Target		= {};
	};

	stct SCameraVectors {
		n3f2_t	Front		= {1, 0, 0};
		n3f2_t	Up			= {0, 1, 0};
		n3f2_t	Right		= {0, 0, 1};
	};

	stct SCameraRange {
		minmaxf2_t	NearFar	= {0.0001f, 100.0f};
		f3_t		Angle	= math_pi * 0.25;
		f3_t		Zoom	= 1.0;
	};
} // namespace

#endif // GPK_CAMERA_H_23627


