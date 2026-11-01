#include "gpk_array.h"
#include "gpk_matrix.h"
#include "gpk_matrix2.h"
#include "gpk_matrix3.h"
#include "gpk_tri2.h"
#include "gpk_apod_color.h"
#include "gpk_camera.h"

// TODO: Throw away
#ifndef GPK_SCENE_H_23627
#define GPK_SCENE_H_23627

namespace gpk
{
	tplt <tpnm _tUnit>
	struct SModelGeometry {
		::gpk::apod<::gpk::tri3<_tUnit>>	Positions									;
		::gpk::apod<::gpk::n3  <_tUnit>>	NormalsTriangle								;
		::gpk::apod<::gpk::tri3<_tUnit>>	NormalsVertex								;
		::gpk::apod<::gpk::tri2<_tUnit>>	UVs											;
	};

	tplt <tpnm _tElement, tpnm _tIndex>
	struct SModelBufferIndexed {
		::gpk::apod<_tIndex>	Indices										;
		::gpk::apod<_tElement>	Values										;
	};

	tplt <tpnm _tAxis, tpnm _tIndex>
	struct SModelGeometryIndexed {
		::gpk::SModelBufferIndexed<::gpk::n3	<_tAxis>, _tIndex>	Positions					;
		::gpk::SModelBufferIndexed<::gpk::n3	<_tAxis>, _tIndex>	NormalsTriangle				;
		::gpk::SModelBufferIndexed<::gpk::tri3	<_tAxis>, _tIndex>	NormalsVertex				;
		::gpk::SModelBufferIndexed<::gpk::tri2	<_tAxis>, _tIndex>	UVs							;
		::gpk::apod<_tIndex>										PositionRemap				;

	};

	tplt <tpnm _tUnit>
	struct SModelPivot {
		::gpk::n3	<_tUnit>	Scale		;
		::gpk::quat	<_tUnit>	Orientation	;
		::gpk::n3	<_tUnit>	Position	;
	};

	// --- Geometry generation: Cube.
	::gpk::err_t			generateCubePositions			(::gpk::apod<::gpk::tri3f2_t> & out_Positions);
	::gpk::err_t			generateCubeNormalsTriangle		(::gpk::apod<::gpk::n3f2_t  > & out_Normals	);
	::gpk::err_t			generateCubeNormalsVertex		(::gpk::apod<::gpk::tri3f2_t> & out_Normals	);
	::gpk::err_t			generateCubeUV					(::gpk::apod<::gpk::tri2f2_t> & out_UV		);
	::gpk::err_t			generateCubeGeometry
		( ::gpk::apod<::gpk::tri3f2_t> & out_Positions
		, ::gpk::apod<::gpk::n3f2_t  > & out_Normals
		, ::gpk::apod<::gpk::tri3f2_t> & out_NormalsVertex
		, ::gpk::apod<::gpk::tri2f2_t> & out_UV
		);
	stin	::gpk::err_t	generateCubeGeometry			(::gpk::SModelGeometry<float>& out_Geometry)	{
		return ::gpk::generateCubeGeometry
			( out_Geometry.Positions
			, out_Geometry.NormalsTriangle
			, out_Geometry.NormalsVertex
			, out_Geometry.UVs
			);
	}

	::gpk::err_t			generateGridPositions			(const ::gpk::n2u1_t & gridMetrics, ::gpk::apod<::gpk::tri3f2_t> & out_Positions	);
	::gpk::err_t			generateGridNormalsTriangle		(const ::gpk::n2u1_t & gridMetrics, ::gpk::apod<::gpk::n3f2_t  > & out_Normals	);
	::gpk::err_t			generateGridNormalsVertex		(const ::gpk::n2u1_t & gridMetrics, ::gpk::apod<::gpk::tri3f2_t> & out_Normals	);
	::gpk::err_t			generateGridUV					(const ::gpk::n2u1_t & gridMetrics, ::gpk::apod<::gpk::tri2f2_t> & out_UV		);
	::gpk::err_t			generateGridGeometry
		( const ::gpk::n2u1_t			& gridMetrics
		, ::gpk::apod<::gpk::tri3f2_t>	& out_Positions
		, ::gpk::apod<::gpk::n3f2_t  >	& out_Normals
		, ::gpk::apod<::gpk::tri3f2_t>	& out_NormalsVertex
		, ::gpk::apod<::gpk::tri2f2_t>	& out_UV
		);
	stin	::gpk::err_t	generateGridGeometry			(const ::gpk::n2u1_t & gridMetrics, ::gpk::SModelGeometry<float>& out_Geometry)	{
		return ::gpk::generateGridGeometry
			( gridMetrics
			, out_Geometry.Positions
			, out_Geometry.NormalsTriangle
			, out_Geometry.NormalsVertex
			, out_Geometry.UVs
			);
	}


	struct SRenderCache {
		::gpk::apod<::gpk::n2s1_t>	TrianglePixelCoords			= {};
		::gpk::apod<::gpk::trif2_t>	TrianglePixelWeights		= {};
		::gpk::apod<::gpk::n2s1_t>	WireframePixelCoords		= {};

		::gpk::apod<::gpk::tri3f2_t>	TransformedNormalsVertex	= {};

		::gpk::as2_t					Triangle3dIndices			= {};
		::gpk::apod<::gpk::tri3f2_t>	Triangle3dToDraw			= {};
		::gpk::apod<::gpk::tri3f2_t>	Triangle3dWorld				= {};

		u2_t					TrianglesDrawn				= 0;
		u2_t					PixelsDrawn					= 0;
		u2_t					PixelsSkipped				= 0;
	};

	struct SColorsMaterial {
		::gpk::a8bgra				Ambient				= {};
		::gpk::a8bgra				Diffuse				= {};
		::gpk::a8bgra				Specular			= {};
	};

	struct SLightsDirectional {
		::gpk::apod<::gpk::n3f2_t>	Directions			= {};
		::gpk::SColorsMaterial		Colors				= {};
	};

	struct SLightsPoint {
		::gpk::apod<::gpk::n3f2_t>	Directions			= {};
		::gpk::apod<::gpk::n3f2_t>	Positions			= {};
		::gpk::apod<::gpk::n3f2_t>	Attenuation			= {};
		::gpk::SColorsMaterial		Colors				= {};
	};

	struct SLightsSpot {
		::gpk::apod<::gpk::n3f2_t>	Directions			= {};
		::gpk::apod<::gpk::n3f2_t>	Positions			= {};
		::gpk::apod<::gpk::n3f2_t>	Attenuation			= {};
		::gpk::af2_t				SpotPower			= {};
		::gpk::SColorsMaterial		Colors				= {};
	};

	struct SSceneGeometries {
		::gpk::apod<::gpk::SModelGeometry<float>>	Polyhedron	= {};
		::gpk::SModelGeometry<float>				Box			= {};
		::gpk::apod<::gpk::SModelGeometry<float>>	Grid		= {};
		::gpk::apod<::gpk::SModelGeometry<float>>	Models		= {};
	};

	struct SSceneLights	{
		::gpk::SLightsDirectional	LightsDirectional	= {};
		::gpk::SLightsPoint			LightsPoint			= {};
		::gpk::SLightsSpot			LightsSpot			= {};
	};

	struct SSceneCamera {
		::gpk::SCameraPoints		Points								;
		::gpk::SCameraVectors		Vectors								;
		::gpk::minmax<float>		NearFar								;
		double						Angle;
	};

	enum SCENE_OBJECT_TYPE
		{ SCENE_OBJECT_TYPE_BOX
		, SCENE_OBJECT_TYPE_GRID
		, SCENE_OBJECT_TYPE_POLYHEDRON
		, SCENE_OBJECT_TYPE_MODEL
		};

	struct SSceneObject {
		::gpk::SCENE_OBJECT_TYPE	Type;
		s2_t						IndexTransform;
		s2_t						Geometry;
	};

	struct SSceneTransforms {
		::gpk::m4f2_t				Viewport								= {};
		::gpk::m4f2_t				ViewportInverse							= {};
		::gpk::m4f2_t				ViewportInverseCentered					= {};
		::gpk::m4f2_t				FieldOfView								= {};
		::gpk::m4f2_t				FinalProjection							= {};
		::gpk::m4f2_t				FinalProjectionInverse					= {};
		::gpk::m4f2_t				View									= {};
	};

	struct SScene {
		::gpk::apod<::gpk::SSceneObject>		Objects						= {};
		::gpk::SSceneGeometries					Geometries					= {};
		::gpk::SSceneLights						Lights						= {};
		::gpk::SSceneTransforms					Transforms					= {};

		::gpk::apod<::gpk::SModelPivot<float>>	ObjectPivots				= {};
		::gpk::apod<::gpk::m4f2_t>				ObjectTransform				= {};
		::gpk::apod<::gpk::m4f2_t>				ObjectOrientations			= {};
		::gpk::apod<::gpk::m4f2_t>				ObjectOrientationsInverse	= {};

		::gpk::SSceneCamera						Camera						=
			{ ::gpk::SCameraPoints{{20, 1, 0}, {}}
			, ::gpk::SCameraVectors
				{ {1, 0, 0}
				, {0, 1, 0}
				, {0, 0, 1}
				}
			, ::gpk::minmax<float>
				{ 0.001f
				, 100.0f
				}
			, .25f
			};
	};

	stin	::gpk::err_t		sceneBoxAdd			(SScene& scene)	{
		const u2_t					newIndex			= scene.ObjectPivots.size();
		if(failed(::gpk::resize
			( newIndex + 1
			, scene.ObjectPivots
			, scene.ObjectTransform
			, scene.ObjectOrientations
			, scene.ObjectOrientationsInverse
			)))
		{
			::gpk::resize
				( newIndex + 1
				, scene.ObjectPivots
				, scene.ObjectTransform
				, scene.ObjectOrientations
				, scene.ObjectOrientationsInverse
				);
			return -1;
		}
		return newIndex;
	}

} // namespace

#endif // GPK_SCENE_H_23627
