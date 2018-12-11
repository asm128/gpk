#include "gpk_coord.h"

#ifndef GPK_CAMERA_H_20934702938492
#define GPK_CAMERA_H_20934702938492

namespace gpk
{
	struct SCameraPoints {
							::gpk::SCoord3<float>							Position	;
							::gpk::SCoord3<float>							Target		;
	};

	struct SCameraVectors {
							::gpk::SCoord3<float>							Front		;
							::gpk::SCoord3<float>							Up			;
							::gpk::SCoord3<float>							Right		;
	};
	struct SCameraRange {
							double											Near		;
							double											Far			;
//							double											Angle		;
	};
} // namespace

#endif // GPK_CAMERA_H_20934702938492
