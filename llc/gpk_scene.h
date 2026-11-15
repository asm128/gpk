#include "gpk_array.h"
#include "gpk_matrix.h"
#include "gpk_matrix2.h"
#include "gpk_matrix3.h"
#include "gpk_tri2.h"
#include "gpk_apod_color.h"
#include "gpk_camera.h"
#include "gpk_ctti_enum.h"

// TODO: Throw away
#ifndef GPK_SCENE_H_23627
#define GPK_SCENE_H_23627

nmsp gpk
{
	tplt <tpnm _tUnit>
	struct SModelGeometry {
		apod<tri3<_tUnit>>	Positions									;
		apod<n3  <_tUnit>>	NormalsTriangle								;
		apod<tri3<_tUnit>>	NormalsVertex								;
		apod<tri2<_tUnit>>	UVs											;
	};

	tplt <tpnm _tElement, tpnm _tIndex>
	struct SModelBufferIndexed {
		apod<_tIndex>	Indices										;
		apod<_tElement>	Values										;
	};

	tplt <tpnm _tAxis, tpnm _tIndex>
	struct SModelGeometryIndexed {
		SModelBufferIndexed<n3	<_tAxis>, _tIndex>	Positions					;
		SModelBufferIndexed<n3	<_tAxis>, _tIndex>	NormalsTriangle				;
		SModelBufferIndexed<tri3	<_tAxis>, _tIndex>	NormalsVertex				;
		SModelBufferIndexed<tri2	<_tAxis>, _tIndex>	UVs							;
		apod<_tIndex>										PositionRemap				;

	};

	tplt <tpnm _tUnit>
	struct SModelPivot {
		n3	<_tUnit>	Scale		;
		quat	<_tUnit>	Orientation	;
		n3	<_tUnit>	Position	;
	};

	// --- Geometry generation: Cube.
	err_t 		generateCubePositions			(apod<tri3f2_t> & out_Positions);
	err_t 		generateCubeNormalsTriangle		(apod<n3f2_t  > & out_Normals	);
	err_t 		generateCubeNormalsVertex		(apod<tri3f2_t> & out_Normals	);
	err_t 		generateCubeUV					(apod<tri2f2_t> & out_UV		);
	err_t 		generateCubeGeometry
		( apod<tri3f2_t> & out_Positions
		, apod<n3f2_t  > & out_Normals
		, apod<tri3f2_t> & out_NormalsVertex
		, apod<tri2f2_t> & out_UV
		);
	stin	err_t generateCubeGeometry			(SModelGeometry<f2_t>& out_Geometry)	{
		return generateCubeGeometry
			( out_Geometry.Positions
			, out_Geometry.NormalsTriangle
			, out_Geometry.NormalsVertex
			, out_Geometry.UVs
			);
	}

	err_t 		generateGridPositions			(cnst n2u1_t & gridMetrics, apod<tri3f2_t> & out_Positions	);
	err_t 		generateGridNormalsTriangle		(cnst n2u1_t & gridMetrics, apod<n3f2_t  > & out_Normals	);
	err_t 		generateGridNormalsVertex		(cnst n2u1_t & gridMetrics, apod<tri3f2_t> & out_Normals	);
	err_t 		generateGridUV					(cnst n2u1_t & gridMetrics, apod<tri2f2_t> & out_UV		);
	err_t 		generateGridGeometry
		( cnst n2u1_t			& gridMetrics
		, apod<tri3f2_t>	& out_Positions
		, apod<n3f2_t  >	& out_Normals
		, apod<tri3f2_t>	& out_NormalsVertex
		, apod<tri2f2_t>	& out_UV
		);
	stin	err_t generateGridGeometry			(cnst n2u1_t & gridMetrics, SModelGeometry<f2_t>& out_Geometry)	{
		return generateGridGeometry
			( gridMetrics
			, out_Geometry.Positions
			, out_Geometry.NormalsTriangle
			, out_Geometry.NormalsVertex
			, out_Geometry.UVs
			);
	}


	struct SRenderCache {
		apod<n2s1_t>	TrianglePixelCoords			= {};
		apod<trif2_t>	TrianglePixelWeights		= {};
		apod<n2s1_t>	WireframePixelCoords		= {};

		apod<tri3f2_t>	TransformedNormalsVertex	= {};

		as2_t					Triangle3dIndices			= {};
		apod<tri3f2_t>	Triangle3dToDraw			= {};
		apod<tri3f2_t>	Triangle3dWorld				= {};

		u2_t					TrianglesDrawn				= 0;
		u2_t					PixelsDrawn					= 0;
		u2_t					PixelsSkipped				= 0;
	};

	struct SColorsMaterial {
		a8bgra				Ambient				= {};
		a8bgra				Diffuse				= {};
		a8bgra				Specular			= {};
	};

	struct SLightsDirectional {
		apod<n3f2_t>	Directions			= {};
		SColorsMaterial		Colors				= {};
	};

	struct SLightsPoint {
		apod<n3f2_t>	Directions			= {};
		apod<n3f2_t>	Positions			= {};
		apod<n3f2_t>	Attenuation			= {};
		SColorsMaterial		Colors				= {};
	};

	struct SLightsSpot {
		apod<n3f2_t>	Directions			= {};
		apod<n3f2_t>	Positions			= {};
		apod<n3f2_t>	Attenuation			= {};
		af2_t				SpotPower			= {};
		SColorsMaterial		Colors				= {};
	};

	struct SSceneGeometries {
		apod<SModelGeometry<f2_t>>	Polyhedron	= {};
		SModelGeometry<f2_t>				Box			= {};
		apod<SModelGeometry<f2_t>>	Grid		= {};
		apod<SModelGeometry<f2_t>>	Models		= {};
	};

	struct SSceneLights	{
		SLightsDirectional	LightsDirectional	= {};
		SLightsPoint			LightsPoint			= {};
		SLightsSpot			LightsSpot			= {};
	};

	struct SSceneCamera {
		SCameraPoints		Points								;
		SCameraVectors		Vectors								;
		minmax<f2_t>		NearFar								;
		f3_t						Angle;
	};

	CTTI_ENUM(SCENE_OBJECT_TYPE,
	enum SCENE_OBJECT_TYPE
		{ SCENE_OBJECT_TYPE_BOX
		, SCENE_OBJECT_TYPE_GRID
		, SCENE_OBJECT_TYPE_POLYHEDRON
		, SCENE_OBJECT_TYPE_MODEL
		};
	);
	struct SSceneObject {
		SCENE_OBJECT_TYPE	Type;
		s2_t						IndexTransform;
		s2_t						Geometry;
	};

	struct SSceneTransforms {
		m4f2_t				Viewport								= {};
		m4f2_t				ViewportInverse							= {};
		m4f2_t				ViewportInverseCentered					= {};
		m4f2_t				FieldOfView								= {};
		m4f2_t				FinalProjection							= {};
		m4f2_t				FinalProjectionInverse					= {};
		m4f2_t				View									= {};
	};

	struct SScene {
		apod<SSceneObject>		Objects						= {};
		SSceneGeometries					Geometries					= {};
		SSceneLights						Lights						= {};
		SSceneTransforms					Transforms					= {};

		apod<SModelPivot<f2_t>>	ObjectPivots				= {};
		apod<m4f2_t>				ObjectTransform				= {};
		apod<m4f2_t>				ObjectOrientations			= {};
		apod<m4f2_t>				ObjectOrientationsInverse	= {};

		SSceneCamera						Camera						=
			{ SCameraPoints{{20, 1, 0}, {}}
			, SCameraVectors
				{ {1, 0, 0}
				, {0, 1, 0}
				, {0, 0, 1}
				}
			, minmax<f2_t>
				{ 0.001f
				, 100.0f
				}
			, .25f
			};
	};

	stin	err_t 	sceneBoxAdd			(SScene& scene)	{
		u2_c					newIndex			= scene.ObjectPivots.size();
		if(failed(resize
			( newIndex + 1
			, scene.ObjectPivots
			, scene.ObjectTransform
			, scene.ObjectOrientations
			, scene.ObjectOrientationsInverse
			)))
		{
			resize
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

} // nmsp

#endif // GPK_SCENE_H_23627
