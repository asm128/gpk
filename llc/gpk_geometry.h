#include "gpk_geometry_buffers.h"
#include "gpk_enum.h"
#include "gpk_minmax.h"
#include "gpk_quat.h"

#ifndef GPK_GEOMETRY_H_23627
#define GPK_GEOMETRY_H_23627

nmsp gpk
{
	GDEFINE_ENUM_TYPE(SHAPE_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Custom		, 0);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Rectangle	, 1);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Circle		, 2);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Ring			, 3);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cube			, 4);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Sphere		, 5);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cylinder		, 6);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Torus		, 7);

#pragma pack(push, 1)
	struct SParamsBox {
		n3f2_t		Origin			= {.5f, .5f, .5f};
		n3f2_t		HalfSizes		= {.5f, .5f, .5f};
		quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsBox, Origin == other.Origin && HalfSizes == other.HalfSizes); 
	};

	struct SParamsSphere { 
		n3f2_t		Origin			= {};
		n2u1_t		CellCount		= {32, 32};
		bool				Reverse			= false;
		f2_t				Circumference	= 1.0f;
		f2_t				Radius			= .5f;
		quatf2_t		Orientation		= {0, 0, 0, 1};
	
		GPK_DEFAULT_OPERATOR_C(SParamsSphere, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Circumference == other.Circumference && Radius == other.Radius);
	};

	struct SParamsDisc { 
		n3f2_t		Origin			= {};
		u1_t			Slices			= 16;
		f2_t				Circumference	= 1.0f;
		f2_t				Radius			= .5f;
		quatf2_t		Orientation		= {0, 0, 0, 1};
	
		GPK_DEFAULT_OPERATOR_C(SParamsDisc, Origin == other.Origin && Slices == other.Slices && Circumference == other.Circumference && Radius == other.Radius);
	};

	struct SParamsCircle : SParamsDisc {
		bool				Reverse			= false;

		cxpr			SParamsCircle	()							nxpt	= default;
		cxpr			SParamsCircle	(const SParamsCircle &)		nxpt	= default;
		cxpr			SParamsCircle	(const SParamsDisc & other)	nxpt	: SParamsDisc(other) {}

		GPK_DEFAULT_OPERATOR_C(SParamsCircle, SParamsDisc::oper==(other) && Reverse == other.Reverse);
	};

	struct SParamsRingSide { 
		n3f2_t		Origin			= {};
		u1_t			Slices			= 16;
		bool				Reverse			= false;
		f2_t				Circumference	= 1.0f;
		minmaxf2_t	Radius			= {.45f, .5f};
		quatf2_t		Orientation		= {0, 0, 0, 1};
	
		GPK_DEFAULT_OPERATOR_C(SParamsRingSide, Origin == other.Origin && Slices == other.Slices && Reverse == other.Reverse && Circumference == other.Circumference && Radius == other.Radius);
	};

	struct SParamsTube { 
		n3f2_t		Origin			= {};
		n2u1_t		CellCount		= {16, 1};
		bool				Reverse			= false;
		f2_t				Circumference	= 1.0f;
		minmaxf2_t	RadiusYMin		= {.45f, .5f};
		minmaxf2_t	RadiusYMax		= {.45f, .5f};
		f2_t				Height			= 1;
		quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsTube, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Circumference == other.Circumference && RadiusYMin == other.RadiusYMin && RadiusYMax == other.RadiusYMax && Height == other.Height);
	};

	struct SParamsCylinderWall { 
		n3f2_t		Origin			= {};
		n2u1_t		CellCount		= {16, 2};
		bool				Reverse			= false;
		f2_t				Circumference	= 1.0f;
		minmaxf2_t	Radius			= {.5f, .5f};
		f2_t				Height			= 1;
		quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsCylinderWall, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Circumference == other.Circumference && Radius == other.Radius && Height == other.Height); 
	};

	struct SParamsGrid { 
		n3f2_t		Origin			= {.5f, 0, .5f};
		n2u1_t		CellCount		= {9, 9};
		bool				Reverse			= false;
		bool				Outward			= false;
		n2f2_t		Size			= {1, 1};
		quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsGrid, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Outward == other.Outward && Size == other.Size); 
	};

	struct SParamsHelix { 
		n3f2_t		Origin			= {0, .5f};
		n2u1_t		CellCount		= {9, 9};
		minmaxf2_t	Radius			= {.5f, .5f};	// TODO: Not working
		f2_t				Length			= 1;
		quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsHelix, Origin == other.Origin && CellCount == other.CellCount && Radius == other.Radius && Length == other.Length); 
	};

#pragma pack(pop)

#ifndef GPK_ATMEL
	gpk::err_t 	geometryBuildCircleSide		(SGeometryBuffers & geometry, const SParamsCircle			& params);
	gpk::err_t 	geometryBuildDisc			(SGeometryBuffers & geometry, const SParamsDisc			& params);
	gpk::err_t 	geometryBuildBox			(SGeometryBuffers & geometry, const SParamsBox			& params);
	gpk::err_t 	geometryBuildSphere			(SGeometryBuffers & geometry, const SParamsSphere			& params);
	gpk::err_t 	geometryBuildCylinderWall	(SGeometryBuffers & geometry, const SParamsCylinderWall	& params);
	gpk::err_t 	geometryBuildCylinder		(SGeometryBuffers & geometry, const SParamsCylinderWall	& params);
	gpk::err_t 	geometryBuildGrid			(SGeometryBuffers & geometry, const SParamsGrid			& params);
	gpk::err_t 	geometryBuildHelixHalf		(SGeometryBuffers & geometry, const SParamsHelix			& params);	// TODO: Rewrite
	gpk::err_t 	geometryBuildHelix			(SGeometryBuffers & geometry, const SParamsHelix			& params);
	gpk::err_t 	geometryBuildFigure0		(SGeometryBuffers & geometry, const SParamsHelix			& params);
	gpk::err_t 	geometryBuildRingSide		(SGeometryBuffers & geometry, const SParamsRingSide		& params);
	gpk::err_t 	geometryBuildRingFlat		(SGeometryBuffers & geometry, const SParamsRingSide		& params);
	gpk::err_t 	geometryBuildRing			(SGeometryBuffers & geometry, const SParamsTube			& params);
#endif
} // nmsp

#endif // GPK_GEOMETRY_H_23627
