#include "gpk_array_pod.h"
#include "gpk_n3.h"

#ifndef GPK_GEOMETRY_H
#define GPK_GEOMETRY_H

namespace gpk
{
	struct STrianglesIndexed {
		::gpk::apod<::gpk::n3f32>	Positions;
		::gpk::apod<::gpk::n3f32>	Normals;
		::gpk::apod<::gpk::n2f32>	TextureCoords;
		::gpk::au32					PositionIndices;
	};

#pragma pack(push, 1)
	struct SParamsBox {
		::gpk::n3f32		Origin			= {.5f, .5f, .5f};
		::gpk::n3f32		HalfSizes		= {.5f, .5f, .5f};

		GPK_DEFAULT_OPERATOR(SParamsBox, Origin == other.Origin && HalfSizes == other.HalfSizes); 
	};

	struct SParamsSphere { 
		::gpk::n3f32		Origin			= {};
		::gpk::n2u16		CellCount		= {2, 2};
		bool				Reverse			= false;
		float				Radius			= .5f;
	
		GPK_DEFAULT_OPERATOR(SParamsSphere, Origin == other.Origin && Radius == other.Radius && CellCount == other.CellCount && Reverse == other.Reverse); 
	};

	struct SParamsCylinder { 
		::gpk::n3f32		Origin			= {0, .5f};
		::gpk::n2u16		CellCount		= {2, 2};
		bool				Reverse			= false;
		float				DiameterRatio	= 1.0f;
		::gpk::minmaxf32	Radius			= {.5f, .5f};

		GPK_DEFAULT_OPERATOR(SParamsCylinder, Origin == other.Origin && Radius == other.Radius && CellCount == other.CellCount && Reverse == other.Reverse && DiameterRatio == other.DiameterRatio); 
	};

	struct SParamsGrid { 
		::gpk::n2f32		Origin			= {.5f, .5f};
		::gpk::n2u16		CellCount		= {2, 2};
		bool				Outward			= false;
		::gpk::n2f32		Size			= {1, 1};

		GPK_DEFAULT_OPERATOR(SParamsGrid, Origin == other.Origin && CellCount == other.CellCount && Outward == other.Outward); 
	};																														
#pragma pack(pop)


	::gpk::error_t	geometryBuildBox		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsBox		& params);
	::gpk::error_t	geometryBuildSphere		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsSphere	& params);
	::gpk::error_t	geometryBuildCylinder	(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsCylinder	& params);
	::gpk::error_t	geometryBuildGrid		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsGrid		& params);



} // namespace

#endif // GPK_GEOMETRY_H
