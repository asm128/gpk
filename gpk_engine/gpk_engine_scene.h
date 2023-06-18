#include "gpk_engine_renderbuffer.h"
#include "gpk_engine_rendermesh.h"
#include "gpk_engine_rendernode.h"
#include "gpk_engine_rendersurface.h"

#include "gpk_font.h"
#include "gpk_raster_lh.h"
#include "gpk_grid_color.h"
#include "gpk_apod_n2.h"
#include "gpk_apod_n3.h"
#include "gpk_apod_tri.h"

#include <functional>

#ifndef GPK_ENGINE_SCENE_H
#define GPK_ENGINE_SCENE_H

namespace gpk 
{
	
#pragma pack(push, 1)
	struct SEngineSceneConstants {
		::gpk::m4f32			View					= {}; 
		::gpk::m4f32			Perspective				= {}; 
		::gpk::m4f32			Screen					= {}; 
		::gpk::m4f32			VP						= {}; 
		::gpk::m4f32			VPS						= {}; 
		::gpk::n3f32			CameraPosition			= {}; 
		float					CameraAngle				= 0;
		::gpk::n3f32			CameraFront				= {}; 
		float					PaddingB				= 0;
		::gpk::n3f32			LightPosition			= {}; 
		float					PaddingC				= 0;
		::gpk::n3f32			LightDirection			= {}; 
		float					LightSpotPower			= 0;
	};

	struct SRenderMaterial {
		::gpk::SRenderColor		Color;
		::gpk::n3f32			Emission;
		float					SpecularPower;
	};

	struct SRenderNodeConstants {
		::gpk::m4f32			Model;
		::gpk::m4f32			ModelInverseTranspose;
		::gpk::m4f32			MVP;
		::gpk::SRenderMaterial	Material;
	};
#pragma pack(pop)

	struct SPSIn {
		::gpk::n3f32			WeightedPosition	;
		::gpk::n3f32			WeightedNormal		;
		::gpk::n2f32			WeightedUV			;
		::gpk::SRenderMaterial	Material			;
		::gpk::gc8bgra			Surface				; 
	};

	typedef ::gpk::error_t	(TFuncPixelShader)
		( const ::gpk::SEngineSceneConstants	& constants
		, const ::gpk::SPSIn					& inPS
		, ::gpk::bgra							& outputPixel
		);

	struct SVSCache {
		::gpk::atrif32			TriangleWeights			= {};
		::gpk::an2i16			SolidPixelCoords		= {};
		::gpk::an3f32			WireframePixelCoords	= {};
	};

	struct SVSOutput {
		::gpk::apod<::gpk::tri3f32>	PositionsWorld			= {};
		::gpk::apod<::gpk::tri3f32>	PositionsScreen			= {};
		::gpk::apod<::gpk::tri3f32>	Normals					= {};
		::gpk::apod<::gpk::tri3f32>	NormalsScreen			= {};
		::gpk::apod<::gpk::tri2f32>	UVs						= {};
	};

	struct SEngineRenderCache {
		::gpk::SVSOutput			VertexShaderOutput		= {};
		::gpk::SVSCache				VertexShaderCache		= {};
	};

	struct SSkin {
		::gpk::SRenderMaterial		Material;
		::gpk::au32					Textures;

		::gpk::error_t						Save			(::gpk::au8 & output) const { 
			gpk_necs(::gpk::savePOD(output, Material));
			gpk_necs(::gpk::saveView(output, Textures));
			return 0;
		}
		::gpk::error_t						Load			(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadPOD(input, Material));
			gpk_necs(::gpk::loadView(input, Textures));
			return 0;
		}
	};

	typedef	::gpk::SLinearNamedMap<::gpk::SSkin>						SSkinManager;
	typedef	::gpk::SLinearNamedMap<::std::function<TFuncPixelShader>>	SShaderManager;

	struct SEngineGraphics {
		::gpk::SRenderBufferManager	Buffers		= {};
		::gpk::SSurfaceManager		Surfaces	= {};
		::gpk::SMeshManager			Meshes		= {};
		::gpk::SSkinManager			Skins		= {};

		::gpk::SShaderManager		Shaders		= {};
		::gpk::SRasterFontManager	Fonts		= {};

		::gpk::error_t				Save		(::gpk::au8 & output)		const	{
			gpk_necs(Buffers	.Save(output));
			gpk_necs(Surfaces	.Save(output));
			gpk_necs(Meshes		.Save(output));
			gpk_necs(Skins		.Save(output));
			//gpk_necs(Shaders	.Save(output));
			//gpk_necs(Fonts	.Save(output));
			return 0;
		}
		::gpk::error_t				Load		(::gpk::vcu8 & input) {
			gpk_necs(Buffers	.Load(input));
			gpk_necs(Surfaces	.Load(input));
			gpk_necs(Meshes		.Load(input));
			gpk_necs(Skins		.Load(input));
			//gpk_necs(Shaders	.Load(input));
			//gpk_necs(Fonts	.Load(input));
			return 0;
		}
	};

	struct SEngineScene {
		::gpk::pobj<::gpk::SEngineGraphics>	Graphics				= {};
		::gpk::SRenderNodeManager			RenderNodes				= {};
		::gpk::SEngineRenderCache			RenderCache				= {};

		::gpk::error_t						Clone					(uint32_t iRenderNode, bool cloneSkin, bool cloneSurfaces, bool cloneShaders) {
			uint32_t							iNewNode					= RenderNodes.Clone(iRenderNode);
			uint32_t							idShaderSource				= RenderNodes[iRenderNode].Shader;
			if(cloneShaders && idShaderSource < Graphics->Shaders.size()) {
				gpk_necs(RenderNodes[iNewNode].Shader = Graphics->Shaders.Clone(idShaderSource));
			}

			uint32_t							idSkinSource		= RenderNodes[iNewNode].Skin;
			if(cloneSkin && idSkinSource < Graphics->Skins.size()) {
				uint32_t							idSkin;
				gpk_necs(idSkin = Graphics->Skins.Clone(idSkinSource));
				RenderNodes[iNewNode].Skin	= idSkin;
				if(cloneSurfaces) {
					if(Graphics->Skins[idSkin]) {
						::gpk::SSkin						& newSkin			= *Graphics->Skins[idSkin];
						for(uint32_t iTexture = 0; iTexture < newSkin.Textures.size(); ++iTexture)
							newSkin.Textures[iTexture]		= Graphics->Surfaces.Clone(newSkin.Textures[iTexture]);
					}
				}
			}
			return iNewNode;
		}

		::gpk::error_t						Save					(::gpk::au8 & output)		const	{
			gpk_necs(Graphics		->Save(output));
			gpk_necs(RenderNodes	 .Save(output));
			//gpk_necs(RenderCache	 .Save(output));
			return 0;
		}
		::gpk::error_t						Load					(::gpk::vcu8 & input) {
			gpk_necs(Graphics		->Load(input));
			gpk_necs(RenderNodes	 .Load(input));
			//gpk_necs(RenderCache	 .Load(input));
			return 0;
		}
	};

	::gpk::error_t							drawScene
		( ::gpk::g8bgra							& backBufferColors
		, ::gpk::gu32							& backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		);

} // namespace

#endif // GPK_ENGINE_SCENE_H