#include "gpk_apod_n3.h"
#include "gpk_apod_n2.h"

#ifndef GPK_GEOMETRYBUFFERS_H_23627
#define GPK_GEOMETRYBUFFERS_H_23627

namespace gpk
{
	struct SGeometryBuffers {
		::gpk::an3f2_t	Positions;
		::gpk::an3f2_t	Normals;
		::gpk::an2f2_t	TextureCoords;
		::gpk::au2_t	Colors;
		::gpk::au2_t	PositionIndices;
	};
} // gpk

#endif // GPK_GEOMETRYBUFFERS_H_23627

