#include "gpk_coord.h"

#ifndef GPK_CAMERA_H_20934702938492
#define GPK_CAMERA_H_20934702938492

namespace gpk
{
	struct SCameraPoints {
							::gpk::n3<float>							Position	;
							::gpk::n3<float>							Target		;
	};

	struct SCameraVectors {
							::gpk::n3<float>							Front		;
							::gpk::n3<float>							Up			;
							::gpk::n3<float>							Right		;
	};

	struct SCameraRange {
							SNearFar										NearFar		;
							double											Angle		;
	};
} // namespace

#endif // GPK_CAMERA_H_20934702938492
