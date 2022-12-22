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
	struct SEngineRenderCache {
		::gpk::SVSOutput						OutputVertexShader		= {};
		::gpk::SVSCache							CacheVertexShader		= {};
	};
	
	struct SEngineSceneConstants {
		::gpk::SMatrix4<float>					View				= {}; 
		::gpk::SMatrix4<float>					Perspective			= {}; 
		::gpk::SMatrix4<float>					Projection			= {}; 
		::gpk::SMatrix4<float>					Screen				= {}; 
		::gpk::SCoord3<float>					CameraPosition		= {}; 
		::gpk::SCoord3<float>					CameraFront			= {}; 
		::gpk::SCoord3<float>					LightPosition		= {}; 
		::gpk::SCoord3<float>					LightDirection		= {}; 
		::gpk::SNearFar							NearFar 			= {.1f, 50.0f}; 
	};

	struct SEngineScene;

	typedef int32_t							(TFuncEffect)
		( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
		, ::gpk::view_grid<uint32_t>			backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		, int32_t								iRenderNode
		);

	struct SRenderMaterial {
		::gpk::SRenderColor						Color;
		float									SpecularPower;
	};

	struct SSkin {
		::gpk::SRenderMaterial					Material;
		::gpk::array_pod<uint32_t>				Textures;
	};

	typedef	::gpk::SLinearMap<::gpk::SSkin>					SSkinManager;
	typedef	::gpk::SLinearMap<::std::function<TFuncEffect>>	SShaderManager;

	struct SEngineGraphics {
		::gpk::SRenderBufferManager				Buffers			= {};
		::gpk::SSurfaceManager					Surfaces		= {};
		::gpk::SMeshManager						Meshes			= {};
		::gpk::SSkinManager						Skins			= {};
		::gpk::SShaderManager					Shaders			= {};

		::gpk::SRasterFontManager				Fonts			= {};
	};

	struct SEngineScene {
		::gpk::ptr_obj<::gpk::SEngineGraphics>	Graphics				= {};
		::gpk::SRenderNodeManager				ManagedRenderNodes		= {};
		::gpk::SEngineRenderCache				RenderCache				= {};
	};

	::gpk::error_t							drawScene									
		( ::gpk::view_grid<::gpk::SColorBGRA>	& backBufferColors
		, ::gpk::view_grid<uint32_t>			& backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		);

} // namespace

#endif // GPK_ENGINE_SCENE_H