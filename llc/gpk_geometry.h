#include "gpk_geometry_buffers.h"
#include "gpk_enum.h"
#include "gpk_minmax.h"
#include "gpk_quat.h"

#ifndef GPK_GEOMETRY_H_23627
#define GPK_GEOMETRY_H_23627

namespace gpk
{
#ifdef GPK_ATMEL
	enum SHAPE_TYPE : u0_t
		{ SHAPE_TYPE_Custom		= 0
		, SHAPE_TYPE_Rectangle	= 1
		, SHAPE_TYPE_Circle		= 2
		, SHAPE_TYPE_Ring		= 3
		, SHAPE_TYPE_Cube		= 4
		, SHAPE_TYPE_Sphere		= 5
		, SHAPE_TYPE_Cylinder	= 6
		, SHAPE_TYPE_Torus		= 7
		};
#else
	GDEFINE_ENUM_TYPE(SHAPE_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Custom		, 0);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Rectangle	, 1);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Circle		, 2);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Ring			, 3);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cube			, 4);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Sphere		, 5);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cylinder		, 6);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Torus		, 7);
#endif

#pragma pack(push, 1)
	struct SParamsBox {
		::gpk::n3f2_t		Origin			= {.5f, .5f, .5f};
		::gpk::n3f2_t		HalfSizes		= {.5f, .5f, .5f};
		::gpk::quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsBox, Origin == other.Origin && HalfSizes == other.HalfSizes); 
	};

	struct SParamsSphere { 
		::gpk::n3f2_t		Origin			= {};
		::gpk::n2u1_t		CellCount		= {32, 32};
		bool				Reverse			= false;
		float				Circumference	= 1.0f;
		float				Radius			= .5f;
		::gpk::quatf2_t		Orientation		= {0, 0, 0, 1};
	
		GPK_DEFAULT_OPERATOR_C(SParamsSphere, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Circumference == other.Circumference && Radius == other.Radius);
	};

	struct SParamsDisc { 
		::gpk::n3f2_t		Origin			= {};
		u1_t			Slices			= 16;
		float				Circumference	= 1.0f;
		float				Radius			= .5f;
		::gpk::quatf2_t		Orientation		= {0, 0, 0, 1};
	
		GPK_DEFAULT_OPERATOR_C(SParamsDisc, Origin == other.Origin && Slices == other.Slices && Circumference == other.Circumference && Radius == other.Radius);
	};

	struct SParamsCircle : SParamsDisc {
		bool				Reverse			= false;

		constexpr			SParamsCircle	()							noexcept	= default;
		constexpr			SParamsCircle	(const SParamsCircle &)		noexcept	= default;
		constexpr			SParamsCircle	(const SParamsDisc & other)	noexcept	: SParamsDisc(other) {}

		GPK_DEFAULT_OPERATOR_C(SParamsCircle, SParamsDisc::operator==(other) && Reverse == other.Reverse);
	};

	struct SParamsRingSide { 
		::gpk::n3f2_t		Origin			= {};
		u1_t			Slices			= 16;
		bool				Reverse			= false;
		float				Circumference	= 1.0f;
		::gpk::minmaxf2_t	Radius			= {.45f, .5f};
		::gpk::quatf2_t		Orientation		= {0, 0, 0, 1};
	
		GPK_DEFAULT_OPERATOR_C(SParamsRingSide, Origin == other.Origin && Slices == other.Slices && Reverse == other.Reverse && Circumference == other.Circumference && Radius == other.Radius);
	};

	struct SParamsTube { 
		::gpk::n3f2_t		Origin			= {};
		::gpk::n2u1_t		CellCount		= {16, 1};
		bool				Reverse			= false;
		float				Circumference	= 1.0f;
		::gpk::minmaxf2_t	RadiusYMin		= {.45f, .5f};
		::gpk::minmaxf2_t	RadiusYMax		= {.45f, .5f};
		float				Height			= 1;
		::gpk::quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsTube, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Circumference == other.Circumference && RadiusYMin == other.RadiusYMin && RadiusYMax == other.RadiusYMax && Height == other.Height);
	};

	struct SParamsCylinderWall { 
		::gpk::n3f2_t		Origin			= {};
		::gpk::n2u1_t		CellCount		= {16, 2};
		bool				Reverse			= false;
		float				Circumference	= 1.0f;
		::gpk::minmaxf2_t	Radius			= {.5f, .5f};
		float				Height			= 1;
		::gpk::quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsCylinderWall, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Circumference == other.Circumference && Radius == other.Radius && Height == other.Height); 
	};

	struct SParamsGrid { 
		::gpk::n3f2_t		Origin			= {.5f, 0, .5f};
		::gpk::n2u1_t		CellCount		= {9, 9};
		bool				Reverse			= false;
		bool				Outward			= false;
		::gpk::n2f2_t		Size			= {1, 1};
		::gpk::quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsGrid, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Outward == other.Outward && Size == other.Size); 
	};

	struct SParamsHelix { 
		::gpk::n3f2_t		Origin			= {0, .5f};
		::gpk::n2u1_t		CellCount		= {9, 9};
		::gpk::minmaxf2_t	Radius			= {.5f, .5f};	// TODO: Not working
		float				Length			= 1;
		::gpk::quatf2_t		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR_C(SParamsHelix, Origin == other.Origin && CellCount == other.CellCount && Radius == other.Radius && Length == other.Length); 
	};

#pragma pack(pop)

#ifndef GPK_ATMEL
	::gpk::err_t		geometryBuildCircleSide		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCircle			& params);
	::gpk::err_t		geometryBuildDisc			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsDisc			& params);
	::gpk::err_t		geometryBuildBox			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsBox			& params);
	::gpk::err_t		geometryBuildSphere			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsSphere			& params);
	::gpk::err_t		geometryBuildCylinderWall	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCylinderWall	& params);
	::gpk::err_t		geometryBuildCylinder		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCylinderWall	& params);
	::gpk::err_t		geometryBuildGrid			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsGrid			& params);
	::gpk::err_t		geometryBuildHelixHalf		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix			& params);	// TODO: Rewrite
	::gpk::err_t		geometryBuildHelix			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix			& params);
	::gpk::err_t		geometryBuildFigure0		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix			& params);
	::gpk::err_t		geometryBuildRingSide		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsRingSide		& params);
	::gpk::err_t		geometryBuildRingFlat		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsRingSide		& params);
	::gpk::err_t		geometryBuildRing			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsTube			& params);
#endif
} // namespace

#endif // GPK_GEOMETRY_H_23627
