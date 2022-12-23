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
		float										CameraAngle				= 0;	// this padding is necessary because GPU shader apis require aligned members
		::gpk::SCoord3<float>						CameraFront				= {}; 
		float										PaddingB				= 0;
		::gpk::SCoord3<float>						LightPosition			= {}; 
		float										PaddingC				= 0;
		::gpk::SCoord3<float>						LightDirection			= {}; 
		float										LightSpotPower			= 0;
	};
	static constexpr size_t SEngineSceneConstants_SIze = sizeof(SEngineSceneConstants);
	struct SRenderMaterial {
		::gpk::SRenderColor							Color;
		::gpk::SCoord3<float>						Emission;
		float										SpecularPower;
	};

	struct SRenderNodeConstants {
		::gpk::SMatrix4<float>						Model;
		::gpk::SMatrix4<float>						ModelInverseTranspose;
		::gpk::SMatrix4<float>						MVP;
		::gpk::SRenderMaterial						Material;
	};
#pragma pack(pop)

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
		::gpk::apod<uint32_t>						Textures;
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
		::gpk::pobj<::gpk::SEngineGraphics>			Graphics				= {};
		::gpk::SRenderNodeManager					ManagedRenderNodes		= {};
		::gpk::SEngineRenderCache					RenderCache				= {};
	};

	::gpk::error_t							drawScene
		( ::gpk::view_grid<::gpk::SColorBGRA>		& backBufferColors
		, ::gpk::view_grid<uint32_t>				& backBufferDepth
		, ::gpk::SEngineRenderCache					& renderCache
		, const ::gpk::SEngineScene					& scene
		, const ::gpk::SEngineSceneConstants		& constants
		);

} // namespace

#endif // GPK_ENGINE_SCENE_H