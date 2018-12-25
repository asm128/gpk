#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog.h"
#include "gpk_camera.h"
#include "gpk_matrix.h"
#include "gpk_geometry.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SVertexCache {
							::gpk::array_pod<::gpk::STriangle3D<float>>							Triangle3dTransformed						= {};
							::gpk::array_pod<::gpk::SColorBGRA>									Triangle3dColorList							= {};
	};

	struct SCamera {
							::gpk::SCoord3<float>												Position;
							::gpk::SCoord3<float>												Target;
							::gpk::SCameraNearFar												NearFar							= {0.01f , 1000.0f};
	};

	struct SViewportScene {
		static constexpr	const ::gpk::SCoord3<float>											CameraUp						= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
							::gme::SCamera														Camera							= {{10, 5, 0}, {}};

							::gpk::SCoord3<float>												LightPos						= {10, 5, 0};

							::gpk::SMatrix4<float>												Projection						= {};
							::gpk::SMatrix4<float>												ViewMatrix						= {};
	};

	struct SApplication {
							::std::mutex														LockRender						;
							::std::mutex														LockViewport					;

							::gpk::SFramework													Framework;
							::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen						= {};
							::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>					PNGImages						= {};

							int32_t																IdExit							= -1;

							::gpk::SDialog														DialogMain						= {};
							int32_t																Slider							= -1;
							int32_t																NumericTuner					= -1;
							int32_t																CheckBox						= -1;
							int32_t																Viewport						= -1;
							//::gpk::STriangle3D<float>											CubePositions	[12]			= {};
							::gpk::SModelGeometryIndexed<float, uint8_t>						ModelGeometry;

							::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Buffer3D;
							SVertexCache														VertexCache;

							int32_t																IdFrameRateUpdate				= -1;
							int32_t																IdFrameRateRender				= -1;
							char																StringFrameRateUpdate	[256]	= {};
							char																StringFrameRateRender	[256]	= {};

							SViewportScene														Scene;

																								SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace


#endif // APPLICATION_H_2078934982734
