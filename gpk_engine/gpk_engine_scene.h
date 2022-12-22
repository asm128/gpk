#include "gpk_engine_renderbuffer.h"
#include "gpk_engine_rendermesh.h"
#include "gpk_engine_rendernode.h"
#include "gpk_engine_rendersurface.h"
#include "gpk_engine_container.h"

#include "gpk_font.h"
#include "gpk_raster_lh.h"

#include <functional>

#ifndef GPK_ENGINE_SCENE_H
#define GPK_ENGINE_SCENE_H

namespace gpk 
{
	struct SVSCache {
		::gpk::array_pod<::gpk::STriangle<float>>	TriangleWeights			= {};
		::gpk::array_pod<::gpk::SCoord2<int16_t>>	SolidPixelCoords		= {};
		::gpk::array_pod<::gpk::SCoord3<float>>		WireframePixelCoords	= {};
	};

	struct SVSOutput {
		::gpk::array_pod<::gpk::STriangle3<float>>	PositionsWorld			= {};
		::gpk::array_pod<::gpk::STriangle3<float>>	PositionsScreen			= {};
		::gpk::array_pod<::gpk::STriangle3<float>>	Normals					= {};
		::gpk::array_pod<::gpk::STriangle3<float>>	NormalsScreen			= {};
		::gpk::array_pod<::gpk::STriangle2<float>>	UVs						= {};
	};

	struct SEngineRenderCache {
		::gpk::SVSOutput							OutputVertexShader		= {};
		::gpk::SVSCache								CacheVertexShader		= {};
	};
	
#pragma pack(push, 1)
	struct SEngineSceneConstants {
		::gpk::SMatrix4<float>						View					= {}; 
		::gpk::SMatrix4<float>						Perspective				= {}; 
		::gpk::SMatrix4<float>						Screen					= {}; 
		::gpk::SMatrix4<float>						VP						= {}; 
		::gpk::SMatrix4<float>						VPS						= {}; 
		::gpk::SCoord3<float>						CameraPosition			= {}; 
		float a = 0;
		::gpk::SCoord3<float>						CameraFront				= {}; 
		float b = 0;
		::gpk::SCoord3<float>						LightPosition			= {}; 
		float c = 0;
		::gpk::SCoord3<float>						LightDirection			= {}; 
		float d = 0;
		::gpk::SNearFar								NearFar 				= {.0001f, 10.0f}; 
		::gpk::SNearFar								Padding					= {.0001f, 10.0f}; 
	};
#pragma pack(pop)

	struct SEngineScene;

	typedef int32_t								(TFuncEffect)
		( ::gpk::view_grid<::gpk::SColorBGRA>		backBufferColors
		, ::gpk::view_grid<uint32_t>				backBufferDepth
		, ::gpk::SEngineRenderCache					& renderCache
		, const ::gpk::SEngineScene					& scene
		, const ::gpk::SEngineSceneConstants		& constants
		, int32_t									iRenderNode
		);

	struct SRenderMaterial {
		::gpk::SRenderColor							Color;
		float										SpecularPower;
	};

	struct SPSIn {
		::gpk::SCoord3<float>						WeightedPosition	;
		::gpk::SCoord3<float>						WeightedNormal		;
		::gpk::SCoord2<float>						WeightedUV			;
		::gpk::SRenderMaterial						Material			;
		::gpk::view_grid<const ::gpk::SColorBGRA>	Surface				; 
	};

	typedef ::gpk::error_t						(TFuncPixelShader)
		( const ::gpk::SEngineSceneConstants		& constants
		, const ::gpk::SPSIn						& inPS
		, ::gpk::SColorBGRA							& outputPixel
		);

	struct SSkin {
		::gpk::SRenderMaterial						Material;
		::gpk::array_pod<uint32_t>					Textures;
	};

	typedef	::gpk::SLinearMap<::gpk::SSkin>							SSkinManager;
	typedef	::gpk::SLinearMap<::std::function<TFuncPixelShader>>	SShaderManager;

	struct SEngineGraphics {
		::gpk::SRenderBufferManager					Buffers					= {};
		::gpk::SSurfaceManager						Surfaces				= {};
		::gpk::SMeshManager							Meshes					= {};
		::gpk::SSkinManager							Skins					= {};
		::gpk::SShaderManager						Shaders					= {};

		::gpk::SRasterFontManager					Fonts					= {};
	};

	struct SEngineScene {
		::gpk::ptr_obj<::gpk::SEngineGraphics>		Graphics				= {};
		::gpk::SRenderNodeManager					ManagedRenderNodes		= {};
		::gpk::SEngineRenderCache					RenderCache				= {};
	};

	::gpk::error_t								drawScene
		( ::gpk::view_grid<::gpk::SColorBGRA>		& backBufferColors
		, ::gpk::view_grid<uint32_t>				& backBufferDepth
		, ::gpk::SEngineRenderCache					& renderCache
		, const ::gpk::SEngineScene					& scene
		, const ::gpk::SEngineSceneConstants		& constants
		);

} // namespace

#endif // GPK_ENGINE_SCENE_H