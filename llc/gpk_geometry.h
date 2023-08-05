#include "gpk_geometry_buffers.h"
#include "gpk_enum.h"
#include "gpk_minmax.h"
#include "gpk_quat.h"

#ifndef GPK_GEOMETRY_H_23627
#define GPK_GEOMETRY_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE(SHAPE_TYPE, uint8_t);
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
		::gpk::n3f32		Origin			= {.5f, .5f, .5f};
		::gpk::n3f32		HalfSizes		= {.5f, .5f, .5f};
		::gpk::quatf32		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR(SParamsBox, Origin == other.Origin && HalfSizes == other.HalfSizes); 
	};

	struct SParamsSphere { 
		::gpk::n3f32		Origin			= {};
		::gpk::n2u16		CellCount		= {32, 32};
		bool				Reverse			= false;
		float				Circumference	= 1.0f;
		float				Radius			= .5f;
		::gpk::quatf32		Orientation		= {0, 0, 0, 1};
	
		GPK_DEFAULT_OPERATOR(SParamsSphere, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Circumference == other.Circumference && Radius == other.Radius);
	};

	struct SParamsDisc { 
		::gpk::n3f32		Origin			= {};
		uint16_t			Slices			= 16;
		float				Circumference	= 1.0f;
		float				Radius			= .5f;
		::gpk::quatf32		Orientation		= {0, 0, 0, 1};
	
		GPK_DEFAULT_OPERATOR(SParamsDisc, Origin == other.Origin && Slices == other.Slices && Circumference == other.Circumference && Radius == other.Radius);
	};

	struct SParamsCircle : SParamsDisc {
		bool				Reverse			= false;

		constexpr			SParamsCircle	()							noexcept	= default;
		constexpr			SParamsCircle	(const SParamsCircle &)		noexcept	= default;
		constexpr			SParamsCircle	(const SParamsDisc & other)	noexcept	: SParamsDisc(other) {}

		GPK_DEFAULT_OPERATOR(SParamsCircle, SParamsDisc::operator==(other) && Reverse == other.Reverse);
	};

	struct SParamsRingSide { 
		::gpk::n3f32		Origin			= {};
		uint16_t			Slices			= 16;
		bool				Reverse			= false;
		float				Circumference	= 1.0f;
		::gpk::minmaxf32	Radius			= {.45f, .5f};
		::gpk::quatf32		Orientation		= {0, 0, 0, 1};
	
		GPK_DEFAULT_OPERATOR(SParamsRingSide, Origin == other.Origin && Slices == other.Slices && Reverse == other.Reverse && Circumference == other.Circumference && Radius == other.Radius);
	};

	struct SParamsTube { 
		::gpk::n3f32		Origin			= {};
		::gpk::n2u16		CellCount		= {16, 1};
		bool				Reverse			= false;
		float				Circumference	= 1.0f;
		::gpk::minmaxf32	RadiusYMin		= {.45f, .5f};
		::gpk::minmaxf32	RadiusYMax		= {.45f, .5f};
		float				Height			= 1;
		::gpk::quatf32		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR(SParamsTube, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Circumference == other.Circumference && RadiusYMin == other.RadiusYMin && RadiusYMax == other.RadiusYMax && Height == other.Height);
	};

	struct SParamsCylinderWall { 
		::gpk::n3f32		Origin			= {};
		::gpk::n2u16		CellCount		= {16, 2};
		bool				Reverse			= false;
		float				Circumference	= 1.0f;
		::gpk::minmaxf32	Radius			= {.5f, .5f};
		float				Height			= 1;
		::gpk::quatf32		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR(SParamsCylinderWall, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Circumference == other.Circumference && Radius == other.Radius && Height == other.Height); 
	};

	struct SParamsGrid { 
		::gpk::n3f32		Origin			= {.5f, 0, .5f};
		::gpk::n2u16		CellCount		= {9, 9};
		bool				Reverse			= false;
		bool				Outward			= false;
		::gpk::n2f32		Size			= {1, 1};
		::gpk::quatf32		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR(SParamsGrid, Origin == other.Origin && CellCount == other.CellCount && Reverse == other.Reverse && Outward == other.Outward && Size == other.Size); 
	};

	struct SParamsHelix { 
		::gpk::n3f32		Origin			= {0, .5f};
		::gpk::n2u16		CellCount		= {9, 9};
		::gpk::minmaxf32	Radius			= {.5f, .5f};	// TODO: Not working
		float				Length			= 1;
		::gpk::quatf32		Orientation		= {0, 0, 0, 1};

		GPK_DEFAULT_OPERATOR(SParamsHelix, Origin == other.Origin && CellCount == other.CellCount && Radius == other.Radius && Length == other.Length); 
	};

#pragma pack(pop)
	::gpk::error_t		geometryBuildCircleSide		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCircle			& params);
	::gpk::error_t		geometryBuildDisc			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsDisc			& params);
	::gpk::error_t		geometryBuildBox			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsBox			& params);
	::gpk::error_t		geometryBuildSphere			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsSphere			& params);
	::gpk::error_t		geometryBuildCylinderWall	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCylinderWall	& params);
	::gpk::error_t		geometryBuildCylinder		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCylinderWall	& params);
	::gpk::error_t		geometryBuildGrid			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsGrid			& params);
	::gpk::error_t		geometryBuildHelixHalf		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix			& params);	// TODO: Rewrite
	::gpk::error_t		geometryBuildHelix			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix			& params);
	::gpk::error_t		geometryBuildFigure0		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix			& params);
	::gpk::error_t		geometryBuildRingSide		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsRingSide		& params);
	::gpk::error_t		geometryBuildRingFlat		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsRingSide		& params);
	::gpk::error_t		geometryBuildRing			(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsTube			& params);

} // namespace

#endif // GPK_GEOMETRY_H_23627
