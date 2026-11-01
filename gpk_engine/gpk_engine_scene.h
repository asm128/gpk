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
		::gpk::m4f2_t			View					= {}; 
		::gpk::m4f2_t			Perspective				= {}; 
		::gpk::m4f2_t			Screen					= {}; 
		::gpk::m4f2_t			VP						= {}; 
		::gpk::m4f2_t			VPS						= {}; 
		::gpk::n3f2_t			CameraPosition			= {};
		float					CameraAngle				= 0;
		::gpk::n3f2_t			CameraFront				= {}; 
		float					Time					= 0;
		::gpk::n3f2_t			LightDirection			= {}; 
		float					LightSpotPower			= 0;
		::gpk::n3f2_t			LightPosition			= {}; 
		float					PaddingB				= 0;
	};
	stxp size_t		SENGINESCENECONSTANTSSIZE = sizeof(SEngineSceneConstants);


	struct SPSIn {
		const ::gpk::SRenderNodeConstants & NodeConstants;
		::gpk::n3f2_t			WeightedPosition	;
		::gpk::n3f2_t			WeightedNormal		;
		::gpk::n2f2_t			WeightedUV			;
		::gpk::SRenderMaterial	Material			;
		::gpk::gc8bgra			Surface				; 
	};

	stxp size_t		SPSINSIZE = sizeof(SPSIn);
#pragma pack(pop)

	typedef ::gpk::err_t	(TFuncPixelShader)
		( const ::gpk::SEngineSceneConstants	& constants
		, const ::gpk::SPSIn					& inPS
		, ::gpk::bgra							& outputPixel
		);

	struct SVSCache {
		::gpk::atrif2_t			TriangleWeights			= {};
		::gpk::an2s1_t			SolidPixelCoords		= {};
		::gpk::an3f2_t			WireframePixelCoords	= {};
	};

	struct SVSOutput {
		::gpk::atri3f2_t			PositionsWorld			= {};
		::gpk::atri3f2_t			PositionsScreen			= {};
		::gpk::atri3f2_t			Normals					= {};
		::gpk::atri3f2_t			NormalsScreen			= {};
		::gpk::atri2f2_t			UVs						= {};
	};

	struct SEngineRenderCache {
		::gpk::SVSOutput		VertexShaderOutput		= {};
		::gpk::SVSCache			VertexShaderCache		= {};
	};

	struct SSkin {
		::gpk::SRenderMaterial	Material;
		::gpk::au2_t				Textures;

		::gpk::err_t			Save			(::gpk::au0_t & output) const { 
			if_fail_fe(gpk::savePOD(output, Material));
			if_fail_fe(gpk::saveView(output, Textures));
			return 0;
		}
		::gpk::err_t			Load			(::gpk::vcu0_t & input) {
			if_fail_fe(gpk::loadPOD(input, Material));
			if_fail_fe(gpk::loadView(input, Textures));
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

		::gpk::err_t				Save		(::gpk::au0_t & output)		const	{
			if_fail_fe(Buffers	.Save(output));
			if_fail_fe(Surfaces	.Save(output));
			if_fail_fe(Meshes		.Save(output));
			if_fail_fe(Skins		.Save(output));
			//if_fail_fe(Shaders	.Save(output));
			//if_fail_fe(Fonts	.Save(output));
			return 0;
		}
		::gpk::err_t				Load		(::gpk::vcu0_t & input) {
			if_fail_fe(Buffers	.Load(input));
			if_fail_fe(Surfaces	.Load(input));
			if_fail_fe(Meshes		.Load(input));
			if_fail_fe(Skins		.Load(input));
			//if_fail_fe(Shaders	.Load(input));
			//if_fail_fe(Fonts	.Load(input));
			return 0;
		}
	};

	struct SEngineScene {
		::gpk::pobj<SEngineGraphics>	Graphics				= {};
		::gpk::SRenderNodeManager		RenderNodes				= {};
		::gpk::SEngineRenderCache		RenderCache				= {};

		::gpk::err_t					CreateRenderNode		(const ::gpk::SGeometryBuffers & geometry, ::gpk::view<sc_c> name, bool createSkin);

		::gpk::err_t					Load					(::gpk::vcu0_t & input) {
			if_fail_fe(Graphics		->Load(input));
			if_fail_fe(RenderNodes	 .Load(input));
			//if_fail_fe(RenderCache	 .Load(input));
			return 0;
		}

		::gpk::err_t					Save					(::gpk::au0_t & output)	const	{
			if(Graphics) {
				if_fail_fe(Graphics->Save(output));
			}
			else {
				SEngineGraphics{}.Save(output);
			}
			if_fail_fe(RenderNodes.Save(output));
			//if_fail_fe(RenderCache.Save(output));
			return 0;
		}

		::gpk::err_t					Clone					(u2_t iRenderNode, bool cloneSkin, bool cloneSurfaces, bool cloneShaders) {
			const u2_t						iNewNode				= RenderNodes.Clone(iRenderNode);
			const u2_t						idShaderSource			= RenderNodes[iRenderNode].Shader;
			if(cloneShaders && idShaderSource < Graphics->Shaders.size())
				if_fail_fe(RenderNodes[iNewNode].Shader = Graphics->Shaders.Clone(idShaderSource));

			const u2_t						idSkinSource			= RenderNodes[iNewNode].Skin;
			if(cloneSkin && idSkinSource < Graphics->Skins.size()) {
				u2_t							idSkin;
				if_fail_fe(idSkin = Graphics->Skins.Clone(idSkinSource));
				RenderNodes[iNewNode].Skin		= idSkin;
				if(cloneSurfaces) {
					if(Graphics->Skins[idSkin]) {
						::gpk::SSkin						& newSkin			= *Graphics->Skins[idSkin];
						for(u2_t iTexture = 0; iTexture < newSkin.Textures.size(); ++iTexture) {
							newSkin.Textures[iTexture]			= Graphics->Surfaces.Clone(newSkin.Textures[iTexture]);
						}
					}
				}
			}
			return iNewNode;
		}

	};

	::gpk::err_t							drawScene
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