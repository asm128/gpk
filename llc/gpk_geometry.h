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
		::gpk::n3f32	Center			= {.5f, .5f, .5f};
		::gpk::n3f32	HalfSizes		= {.5f, .5f, .5f};

		GPK_DEFAULT_OPERATOR(SParamsBox, HalfSizes == other.HalfSizes && Center == other.Center); 
	};

	struct SParamsSphere { 
		::gpk::n3f32		Center		= {};
		float				Radius		= .5f;
		uint16_t			Stacks		= 24;
		uint16_t			Slices		= 24;
		bool				Reverse		= false;
	
		GPK_DEFAULT_OPERATOR(SParamsSphere, Center == other.Center && Radius == other.Radius && Stacks == other.Stacks && Slices == other.Slices && Reverse == other.Reverse); 
	};

	struct SParamsCylinder { 
		::gpk::n3f32		Center			= {0, .5f};
		float				DiameterRatio	= 1.0f;
		::gpk::minmaxf32	Radius			= {.5f, .5f};
		uint16_t			Stacks			= 1, Slices = 16;
		bool				Reverse			= false;

		GPK_DEFAULT_OPERATOR(SParamsCylinder, Center == other.Center && Radius == other.Radius && Stacks == other.Stacks && Slices == other.Slices && Reverse == other.Reverse && DiameterRatio == other.DiameterRatio); 
	};

	struct SParamsGrid { 
		::gpk::n2f32	Center				= {.5f, .5f};
		::gpk::n2u16	CellCount			= {2, 2};
		bool			ReverseTriangles	= false;

		GPK_DEFAULT_OPERATOR(SParamsGrid, Center == other.Center && CellCount == other.CellCount && ReverseTriangles == other.ReverseTriangles); 
	};																														
#pragma pack(pop)


	::gpk::error_t	geometryBuildBox		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsBox		& params);
	::gpk::error_t	geometryBuildSphere		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsSphere	& params);
	::gpk::error_t	geometryBuildCylinder	(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsCylinder	& params);
	::gpk::error_t	geometryBuildGrid		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsGrid		& params);



} // namespace

#endif // GPK_GEOMETRY_H
