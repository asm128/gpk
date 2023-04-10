#include "gpk_geometry.h"
#include "gpk_color.h"
#include "gpk_camera.h"

#ifndef GPK_SCENE_H_203492837
#define GPK_SCENE_H_203492837

namespace gpk
{
	struct SColorsMaterial {
							::gpk::apod<::gpk::SColorBGRA>						Ambient										= {};
							::gpk::apod<::gpk::SColorBGRA>						Diffuse										= {};
							::gpk::apod<::gpk::SColorBGRA>						Specular									= {};
	};

	struct SLightsDirectional {
							::gpk::apod<::gpk::SCoord3<float>>					Directions									= {};
							::gpk::SColorsMaterial									Colors										= {};
	};

	struct SLightsPoint {
							::gpk::apod<::gpk::SCoord3<float>>					Directions									= {};
							::gpk::apod<::gpk::SCoord3<float>>					Positions									= {};
							::gpk::apod<::gpk::SCoord3<float>>					Attenuation									= {};
							::gpk::SColorsMaterial									Colors										= {};
	};

	struct SLightsSpot {
							::gpk::apod<::gpk::SCoord3<float>>					Directions									= {};
							::gpk::apod<::gpk::SCoord3<float>>					Positions									= {};
							::gpk::apod<::gpk::SCoord3<float>>					Attenuation									= {};
							::gpk::apod<float>									SpotPower									= {};
							::gpk::SColorsMaterial									Colors										= {};
	};

	struct SSceneGeometries {
							::gpk::apod<::gpk::SModelGeometry	<float>>		Polyhedron									= {};
							::gpk::SModelGeometry					<float>			Box											= {};
							::gpk::apod<::gpk::SModelGeometry	<float>>		Grid										= {};
							::gpk::apod<::gpk::SModelGeometry	<float>>		Models										= {};
	};

	struct SSceneLights	{
							::gpk::SLightsDirectional								LightsDirectional							= {};
							::gpk::SLightsPoint										LightsPoint									= {};
							::gpk::SLightsSpot										LightsSpot									= {};
	};

	struct SSceneCamera {
							::gpk::SCameraPoints									Points								;
							::gpk::SCameraVectors									Vectors								;
							::gpk::SMinMax<float>									NearFar								;
							double													Angle;
	};

	enum SCENE_OBJECT_TYPE
		{ SCENE_OBJECT_TYPE_BOX
		, SCENE_OBJECT_TYPE_GRID
		, SCENE_OBJECT_TYPE_POLYHEDRON
		, SCENE_OBJECT_TYPE_MODEL
		};

	struct SSceneObject {
							::gpk::SCENE_OBJECT_TYPE								Type;
							int32_t													IndexTransform;
							int32_t													Geometry;
	};

	struct SSceneTransforms {
							::gpk::SMatrix4<float>									Viewport									= {};
							::gpk::SMatrix4<float>									ViewportInverse								= {};
							::gpk::SMatrix4<float>									ViewportInverseCentered						= {};
							::gpk::SMatrix4<float>									FieldOfView									= {};
							::gpk::SMatrix4<float>									FinalProjection								= {};
							::gpk::SMatrix4<float>									FinalProjectionInverse						= {};
							::gpk::SMatrix4<float>									View										= {};
	};

	struct SScene {
							::gpk::apod<::gpk::SSceneObject>						Objects										= {};
							::gpk::SSceneGeometries									Geometries									= {};
							::gpk::SSceneLights										Lights										= {};
							::gpk::SSceneTransforms									Transforms									= {};

							::gpk::apod<::gpk::SModelPivot	<float>>				ObjectPivots								= {};
							::gpk::apod<::gpk::SMatrix4	<float>>					ObjectTransform								= {};
							::gpk::apod<::gpk::SMatrix4	<float>>					ObjectOrientations							= {};
							::gpk::apod<::gpk::SMatrix4	<float>>					ObjectOrientationsInverse					= {};

							::gpk::SSceneCamera										Camera										=
								{ ::gpk::SCameraPoints{{20, 1, 0}, {}}
								, ::gpk::SCameraVectors
									{ {1, 0, 0}
									, {0, 1, 0}
									, {0, 0, 1}
									}
								, ::gpk::SMinMax<float>
									{ 0.001f
									, 100.0f
									}
								, .25f
								};
	};

	stainli	::gpk::error_t											sceneBoxAdd									(SScene& scene)	{
			const uint32_t																newIndex									= scene.ObjectPivots.size();
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
