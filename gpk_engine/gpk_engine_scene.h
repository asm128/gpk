#include "gpk_engine_renderbuffer.h"
#include "gpk_engine_rendermesh.h"
#include "gpk_engine_rendernode.h"
#include "gpk_engine_rendersurface.h"

#include "gpk_font.h"
#include "gpk_raster_lh.h"
#include "gpk_grid_color.h"
#include "gpk_apod_tri.h"
#include "gpk_apod_tri2.h"
#include "gpk_apod_tri3.h"

#include <functional>

#ifndef GPK_ENGINE_SCENE_H_23627
#define GPK_ENGINE_SCENE_H_23627

namespace gpk 
{
	
#pragma pack(push, 1)
	struct SEngineSceneConstants {
		::gpk::m4f32			View					= {}; 
		::gpk::m4f32			Perspective				= {}; 
		::gpk::m4f32			Screen					= {}; 
		::gpk::m4f32			VP						= {}; 
		::gpk::m4f32			VPS						= {}; 
		::gpk::n3f2_t			CameraPosition			= {};
		float					CameraAngle				= 0;
		::gpk::n3f2_t			CameraFront				= {}; 
		float					Time					= 0;
		::gpk::n3f2_t			LightDirection			= {}; 
		float					LightSpotPower			= 0;
		::gpk::n3f2_t			LightPosition			= {}; 
		float					PaddingB				= 0;
	};
	stacxpr size_t		SENGINESCENECONSTANTSSIZE = sizeof(SEngineSceneConstants);


	struct SPSIn {
		const ::gpk::SRenderNodeConstants & NodeConstants;
		::gpk::n3f2_t			WeightedPosition	;
		::gpk::n3f2_t			WeightedNormal		;
		::gpk::n2f2_t			WeightedUV			;
		::gpk::SRenderMaterial	Material			;
		::gpk::gc8bgra			Surface				; 
	};

	stacxpr size_t		SPSINSIZE = sizeof(SPSIn);
#pragma pack(pop)

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
		::gpk::atri3f32			PositionsWorld			= {};
		::gpk::atri3f32			PositionsScreen			= {};
		::gpk::atri3f32			Normals					= {};
		::gpk::atri3f32			NormalsScreen			= {};
		::gpk::atri2f32			UVs						= {};
	};

	struct SEngineRenderCache {
		::gpk::SVSOutput		VertexShaderOutput		= {};
		::gpk::SVSCache			VertexShaderCache		= {};
	};

	struct SSkin {
		::gpk::SRenderMaterial	Material;
		::gpk::au2_t				Textures;

		::gpk::error_t			Save			(::gpk::au0_t & output) const { 
			gpk_necs(gpk::savePOD(output, Material));
			gpk_necs(gpk::saveView(output, Textures));
			return 0;
		}
		::gpk::error_t			Load			(::gpk::vcu0_t & input) {
			gpk_necs(gpk::loadPOD(input, Material));
			gpk_necs(gpk::loadView(input, Textures));
			return 0;
		}
	};

	typedef	::gpk::gpk_linear_map_pobj<::gpk::SSkin>						SSkinManager;
	typedef	::gpk::gpk_linear_map_pobj<::std::function<TFuncPixelShader>>	SShaderManager;

	struct SEngineGraphics {
		::gpk::SRenderBufferManager	Buffers		= {};
		::gpk::SSurfaceManager		Surfaces	= {};
		::gpk::SMeshManager			Meshes		= {};
		::gpk::SSkinManager			Skins		= {};

		::gpk::SShaderManager		Shaders		= {};
		::gpk::SRasterFontManager	Fonts		= {};

		::gpk::error_t				Save		(::gpk::au0_t & output)		const	{
			gpk_necs(Buffers	.Save(output));
			gpk_necs(Surfaces	.Save(output));
			gpk_necs(Meshes		.Save(output));
			gpk_necs(Skins		.Save(output));
			//gpk_necs(Shaders	.Save(output));
			//gpk_necs(Fonts	.Save(output));
			return 0;
		}
		::gpk::error_t				Load		(::gpk::vcu0_t & input) {
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
		::gpk::pobj<SEngineGraphics>	Graphics				= {};
		::gpk::SRenderNodeManager		RenderNodes				= {};
		::gpk::SEngineRenderCache		RenderCache				= {};

		::gpk::error_t					CreateRenderNode		(const ::gpk::SGeometryBuffers & geometry, ::gpk::vcsc_c name, bool createSkin);

		::gpk::error_t					Load					(::gpk::vcu0_t & input) {
			gpk_necs(Graphics		->Load(input));
			gpk_necs(RenderNodes	 .Load(input));
			//gpk_necs(RenderCache	 .Load(input));
			return 0;
		}

		::gpk::error_t					Save					(::gpk::au0_t & output)	const	{
			if(Graphics) 
				gpk_necs(Graphics->Save(output));
			else {
				SEngineGraphics{}.Save(output);
			}
			gpk_necs(RenderNodes.Save(output));
			//gpk_necs(RenderCache.Save(output));
			return 0;
		}

		::gpk::error_t					Clone					(uint32_t iRenderNode, bool cloneSkin, bool cloneSurfaces, bool cloneShaders) {
			const uint32_t						iNewNode				= RenderNodes.Clone(iRenderNode);
			const uint32_t						idShaderSource			= RenderNodes[iRenderNode].Shader;
			if(cloneShaders && idShaderSource < Graphics->Shaders.size())
				gpk_necs(RenderNodes[iNewNode].Shader = Graphics->Shaders.Clone(idShaderSource));

			const uint32_t						idSkinSource			= RenderNodes[iNewNode].Skin;
			if(cloneSkin && idSkinSource < Graphics->Skins.size()) {
				uint32_t							idSkin;
				gpk_necs(idSkin = Graphics->Skins.Clone(idSkinSource));
				RenderNodes[iNewNode].Skin		= idSkin;
				if(cloneSurfaces) {
					if(Graphics->Skins[idSkin]) {
						::gpk::SSkin						& newSkin			= *Graphics->Skins[idSkin];
						for(uint32_t iTexture = 0; iTexture < newSkin.Textures.size(); ++iTexture) {
							newSkin.Textures[iTexture]			= Graphics->Surfaces.Clone(newSkin.Textures[iTexture]);
						}
					}
				}
			}
			return iNewNode;
		}

	};

	::gpk::error_t							drawScene
		( ::gpk::g8bgra				backBufferColors
		, ::gpk::gu32				backBufferDepth
		, ::gpk::SEngineRenderCache	& renderCache
		, const ::gpk::SEngineScene	& scene
		, const ::gpk::n3f2_t		& cameraPosition
		, const ::gpk::n3f2_t		& cameraTarget
		, const ::gpk::n3f2_t		& cameraUp
		, const ::gpk::minmaxf2_t	& nearFar
		);

} // namespace

#endif // GPK_ENGINE_SCENE_H