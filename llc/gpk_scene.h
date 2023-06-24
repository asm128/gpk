#include "gpk_array.h"
#include "gpk_matrix.h"
#include "gpk_tri2.h"
#include "gpk_apod_color.h"
#include "gpk_camera.h"

// TODO: Throw away
#ifndef GPK_SCENE_H_203492837
#define GPK_SCENE_H_203492837

namespace gpk
{
	template <typename _tUnit>
	struct SModelGeometry {
		::gpk::apod<::gpk::tri3<_tUnit>>	Positions									;
		::gpk::apod<::gpk::n3  <_tUnit>>	NormalsTriangle								;
		::gpk::apod<::gpk::tri3<_tUnit>>	NormalsVertex								;
		::gpk::apod<::gpk::tri2<_tUnit>>	UVs											;
	};

	template <typename _tElement, typename _tIndex>
	struct SModelBufferIndexed {
		::gpk::apod<_tIndex>	Indices										;
		::gpk::apod<_tElement>	Values										;
	};

	template <typename _tAxis, typename _tIndex>
	struct SModelGeometryIndexed {
		::gpk::SModelBufferIndexed<::gpk::n3	<_tAxis>, _tIndex>	Positions					;
		::gpk::SModelBufferIndexed<::gpk::n3	<_tAxis>, _tIndex>	NormalsTriangle				;
		::gpk::SModelBufferIndexed<::gpk::tri3	<_tAxis>, _tIndex>	NormalsVertex				;
		::gpk::SModelBufferIndexed<::gpk::tri2	<_tAxis>, _tIndex>	UVs							;
		::gpk::apod<_tIndex>										PositionRemap				;

	};

	template <typename _tUnit>
	struct SModelPivot {
		::gpk::n3	<_tUnit>	Scale		;
		::gpk::quat	<_tUnit>	Orientation	;
		::gpk::n3	<_tUnit>	Position	;
	};

	// --- Geometry generation: Cube.
	::gpk::error_t			generateCubePositions			(::gpk::apod<::gpk::tri3f32> & out_Positions);
	::gpk::error_t			generateCubeNormalsTriangle		(::gpk::apod<::gpk::n3f32  > & out_Normals	);
	::gpk::error_t			generateCubeNormalsVertex		(::gpk::apod<::gpk::tri3f32> & out_Normals	);
	::gpk::error_t			generateCubeUV					(::gpk::apod<::gpk::tri2f32> & out_UV		);
	::gpk::error_t			generateCubeGeometry
		( ::gpk::apod<::gpk::tri3f32> & out_Positions
		, ::gpk::apod<::gpk::n3f32  > & out_Normals
		, ::gpk::apod<::gpk::tri3f32> & out_NormalsVertex
		, ::gpk::apod<::gpk::tri2f32> & out_UV
		);
	stainli	::gpk::error_t	generateCubeGeometry			(::gpk::SModelGeometry<float>& out_Geometry)	{
		return ::gpk::generateCubeGeometry
			( out_Geometry.Positions
			, out_Geometry.NormalsTriangle
			, out_Geometry.NormalsVertex
			, out_Geometry.UVs
			);
	}

	::gpk::error_t			generateGridPositions			(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::tri3f32> & out_Positions	);
	::gpk::error_t			generateGridNormalsTriangle		(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::n3f32  > & out_Normals	);
	::gpk::error_t			generateGridNormalsVertex		(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::tri3f32> & out_Normals	);
	::gpk::error_t			generateGridUV					(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::tri2f32> & out_UV		);
	::gpk::error_t			generateGridGeometry
		( const ::gpk::n2u16			& gridMetrics
		, ::gpk::apod<::gpk::tri3f32>	& out_Positions
		, ::gpk::apod<::gpk::n3f32  >	& out_Normals
		, ::gpk::apod<::gpk::tri3f32>	& out_NormalsVertex
		, ::gpk::apod<::gpk::tri2f32>	& out_UV
		);
	stainli	::gpk::error_t	generateGridGeometry			(const ::gpk::n2u16	& gridMetrics, ::gpk::SModelGeometry<float>& out_Geometry)	{
		return ::gpk::generateGridGeometry
			( gridMetrics
			, out_Geometry.Positions
			, out_Geometry.NormalsTriangle
			, out_Geometry.NormalsVertex
			, out_Geometry.UVs
			);
	}


	struct SRenderCache {
		::gpk::apod<::gpk::n2i16>	TrianglePixelCoords			= {};
		::gpk::apod<::gpk::trif32>	TrianglePixelWeights		= {};
		::gpk::apod<::gpk::n2i16>	WireframePixelCoords		= {};

		::gpk::apod<::gpk::tri3f32>	TransformedNormalsVertex	= {};

		::gpk::ai32					Triangle3dIndices			= {};
		::gpk::apod<::gpk::tri3f32>	Triangle3dToDraw			= {};
		::gpk::apod<::gpk::tri3f32>	Triangle3dWorld				= {};

		uint32_t					TrianglesDrawn				= 0;
		uint32_t					PixelsDrawn					= 0;
		uint32_t					PixelsSkipped				= 0;
	};

	struct SColorsMaterial {
		::gpk::a8bgra				Ambient				= {};
		::gpk::a8bgra				Diffuse				= {};
		::gpk::a8bgra				Specular			= {};
	};

	struct SLightsDirectional {
		::gpk::apod<::gpk::n3f32>	Directions			= {};
		::gpk::SColorsMaterial		Colors				= {};
	};

	struct SLightsPoint {
		::gpk::apod<::gpk::n3f32>	Directions			= {};
		::gpk::apod<::gpk::n3f32>	Positions			= {};
		::gpk::apod<::gpk::n3f32>	Attenuation			= {};
		::gpk::SColorsMaterial		Colors				= {};
	};

	struct SLightsSpot {
		::gpk::apod<::gpk::n3f32>	Directions			= {};
		::gpk::apod<::gpk::n3f32>	Positions			= {};
		::gpk::apod<::gpk::n3f32>	Attenuation			= {};
		::gpk::af32					SpotPower			= {};
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
		int32_t						IndexTransform;
		int32_t						Geometry;
	};

	struct SSceneTransforms {
		::gpk::m4f32				Viewport								= {};
		::gpk::m4f32				ViewportInverse							= {};
		::gpk::m4f32				ViewportInverseCentered					= {};
		::gpk::m4f32				FieldOfView								= {};
		::gpk::m4f32				FinalProjection							= {};
		::gpk::m4f32				FinalProjectionInverse					= {};
		::gpk::m4f32				View									= {};
	};

	struct SScene {
		::gpk::apod<::gpk::SSceneObject>		Objects						= {};
		::gpk::SSceneGeometries					Geometries					= {};
		::gpk::SSceneLights						Lights						= {};
		::gpk::SSceneTransforms					Transforms					= {};

		::gpk::apod<::gpk::SModelPivot<float>>	ObjectPivots				= {};
		::gpk::apod<::gpk::m4f32>				ObjectTransform				= {};
		::gpk::apod<::gpk::m4f32>				ObjectOrientations			= {};
		::gpk::apod<::gpk::m4f32>				ObjectOrientationsInverse	= {};

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

	stainli	::gpk::error_t		sceneBoxAdd			(SScene& scene)	{
		const uint32_t					newIndex			= scene.ObjectPivots.size();
		if(errored(::gpk::resize
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

#endif // GPK_SCENE_H_203492837
