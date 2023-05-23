#include "gpk_n3.h"

#ifndef GPK_CAMERA_H_20934702938492
#define GPK_CAMERA_H_20934702938492

namespace gpk
{
#pragma pack(push, 1)
	struct SCameraPoints {
		::gpk::n3f32	Position	;
		::gpk::n3f32	Target		;
	};

	struct SCameraVectors {
		::gpk::n3f32	Front		;
		::gpk::n3f32	Up			;
		::gpk::n3f32	Right		;
	};

	struct SCameraRange {
		SNearFar		NearFar		;
		double			Angle		;
	};
#pragma pack(pop)
} // namespace

#endif // GPK_CAMERA_H_20934702938492
