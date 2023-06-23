#include "gpk_apod_n3.h"
#include "gpk_apod_n2.h"

#ifndef GPK_GEOMETRYBUFFERS_H_2023611
#define GPK_GEOMETRYBUFFERS_H_2023611

namespace gpk
{
	struct SGeometryBuffers {
		::gpk::an3f32	Positions;
		::gpk::an3f32	Normals;
		::gpk::an2f32	TextureCoords;
		::gpk::au32		Colors;
		::gpk::au32		PositionIndices;
	};
} // gpk

#endif // GPK_TRIANGLESINDEXED_H_2023611