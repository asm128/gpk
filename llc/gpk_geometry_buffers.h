#include "gpk_apod_n3.h"
#include "gpk_apod_n2.h"

#ifndef GPK_GEOMETRYBUFFERS_H_23627
#define GPK_GEOMETRYBUFFERS_H_23627

nmsp gpk
{
	struct SGeometryBuffers {
		a3f2_t	Positions;
		a3f2_t	Normals;
		a2f2_t	TextureCoords;
		au2_t	Colors;
		au2_t	PositionIndices;
	};
} // gpk

#endif // GPK_GEOMETRYBUFFERS_H_23627