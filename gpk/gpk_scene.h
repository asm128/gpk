#include "gpk_geometry.h"
#include "gpk_color.h"
#include "gpk_camera.h"

#ifndef GPK_SCENE_H_203492837
#define GPK_SCENE_H_203492837

namespace gpk
{
	struct SColorsMaterial {
							::gpk::array_pod<::gpk::SColorBGRA>						Ambient										= {};
							::gpk::array_pod<::gpk::SColorBGRA>						Diffuse										= {};
							::gpk::array_pod<::gpk::SColorBGRA>						Specular									= {};
	};

	struct SLightsDirectional {
							::gpk::array_pod<::gpk::SCoord3<float>>					Directions									= {};
							::gpk::SColorsMaterial									Colors										= {};
	};

	struct SLightsPoint {
							::gpk::array_pod<::gpk::SCoord3<float>>					Directions									= {};
							::gpk::array_pod<::gpk::SCoord3<float>>					Positions									= {};
							::gpk::array_pod<::gpk::SCoord3<float>>					Attenuation									= {};
							::gpk::SColorsMaterial									Colors										= {};
	};

	struct SLightsSpot {
							::gpk::array_pod<::gpk::SCoord3<float>>					Directions									= {};
							::gpk::array_pod<::gpk::SCoord3<float>>					Positions									= {};
							::gpk::array_pod<::gpk::SCoord3<float>>					Attenuation									= {};
							::gpk::array_pod<float>									SpotPower									= {};
							::gpk::SColorsMaterial									Colors										= {};
	};

	struct SSceneGeometries {
							::gpk::array_pod<::gpk::SModelGeometry	<float>>		Polyhedron									= {};
							::gpk::SModelGeometry					<float>			Box											= {};
							::gpk::array_pod<::gpk::SModelGeometry	<float>>		Grid										= {};
							::gpk::array_pod<::gpk::SModelGeometry	<float>>		Models										= {};
	};

	struct SSceneLights	{
							::gpk::SLightsDirectional								LightsDirectional							= {};
							::gpk::SLightsPoint										LightsPoint									= {};
							::gpk::SLightsSpot										LightsSpot									= {};
	};

	struct SSceneCamera {
							::gpk::SCameraPoints									Points								;
							::gpk::SCameraRange										Range								;
							::gpk::SCameraVectors									Vectors								;
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
							int32_t													IndexGeometry;
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
							::gpk::array_pod<::gpk::SSceneObject>					Objects										= {};
							::gpk::SSceneGeometries									Geometries									= {};
							::gpk::SSceneLights										Lights										= {};
							::gpk::SSceneTransforms									Transforms									= {};

							::gpk::array_pod<::gpk::SModelPivot	<float>>			ObjectPivots								= {};
							::gpk::array_pod<::gpk::SMatrix4	<float>>			ObjectTransform								= {};
							::gpk::array_pod<::gpk::SMatrix4	<float>>			ObjectOrientations							= {};
							::gpk::array_pod<::gpk::SMatrix4	<float>>			ObjectOrientationsInverse					= {};

							::gpk::SSceneCamera										Camera										=
								{ ::gpk::SCameraPoints{{20, 1, 0}, {}}
								, ::gpk::SCameraRange
									{ 0.01
									, 100.0
									}
								, ::gpk::SCameraVectors
									{ {1, 0, 0}
									, {0, 1, 0}
									, {0, 0, 1}
									}
								};

							double    CameraAngle = .25;
	};

	static inline		::gpk::error_t											sceneBoxAdd									(SScene& scene)	{
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
