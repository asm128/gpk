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
		m4f2_t			View					= {}; 
		m4f2_t			Perspective				= {}; 
		m4f2_t			Screen					= {}; 
		m4f2_t			VP						= {}; 
		m4f2_t			VPS						= {}; 
		n3f2_t			CameraPosition			= {};
		f2_t			CameraAngle				= 0;
		n3f2_t			CameraFront				= {}; 
		f2_t			Time					= 0;
		n3f2_t			LightDirection			= {}; 
		f2_t			LightSpotPower			= 0;
		n3f2_t			LightPosition			= {}; 
		f2_t			PaddingB				= 0;
	};
	stxp size_t		SENGINESCENECONSTANTSSIZE = sizeof(SEngineSceneConstants);


	struct SPSIn {
		const SRenderNodeConstants & NodeConstants;
		n3f2_t			WeightedPosition	;
		n3f2_t			WeightedNormal		;
		n2f2_t			WeightedUV			;
		SRenderMaterial	Material			;
		gc8bgra			Surface				; 
	};

	stxp size_t		SPSINSIZE = sizeof(SPSIn);
#pragma pack(pop)

	typedef err_t (TFuncPixelShader)
		( const SEngineSceneConstants	& constants
		, const SPSIn					& inPS
		, bgra							& outputPixel
		);

	struct SVSCache {
		atrif2_t			TriangleWeights			= {};
		an2s1_t				SolidPixelCoords		= {};
		an3f2_t				WireframePixelCoords	= {};
	};

	struct SVSOutput {
		atri3f2_t			PositionsWorld			= {};
		atri3f2_t			PositionsScreen			= {};
		atri3f2_t			Normals					= {};
		atri3f2_t			NormalsScreen			= {};
		atri2f2_t			UVs						= {};
	};

	struct SEngineRenderCache {
		SVSOutput			VertexShaderOutput		= {};
		SVSCache			VertexShaderCache		= {};
	};

	struct SSkin {
		SRenderMaterial		Material;
		au2_t				Textures;

		err_t 				Save			(au0_t & output) const { 
			if_fail_fe(savePOD(output, Material));
			if_fail_fe(saveView(output, Textures));
			return 0;
		}
		err_t 				Load			(vcu0_t & input) {
			if_fail_fe(loadPOD(input, Material));
			if_fail_fe(loadView(input, Textures));
			return 0;
		}
	};

	typedef	gpk_linear_map_pobj<SSkin>								SSkinManager;
	typedef	gpk_linear_map_pobj<function<TFuncPixelShader>>	SShaderManager;

	struct SEngineGraphics {
		SRenderBufferManager	Buffers		= {};
		SSurfaceManager			Surfaces	= {};
		SMeshManager			Meshes		= {};
		SSkinManager			Skins		= {};

		SShaderManager			Shaders		= {};
		SRasterFontManager		Fonts		= {};

		err_t 					Save		(au0_t & output)		const	{
			if_fail_fe(Buffers	.Save(output));
			if_fail_fe(Surfaces	.Save(output));
			if_fail_fe(Meshes		.Save(output));
			if_fail_fe(Skins		.Save(output));
			//if_fail_fe(Shaders	.Save(output));
			//if_fail_fe(Fonts	.Save(output));
			return 0;
		}
		err_t 				Load			(vcu0_t & input) {
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
		pobj<SEngineGraphics>	Graphics				= {};
		SRenderNodeManager		RenderNodes				= {};
		SEngineRenderCache		RenderCache				= {};

		err_t 				CreateRenderNode		(const SGeometryBuffers & geometry, vcst_t name, bool createSkin);

		err_t 				Load					(vcu0_t & input) {
			if_fail_fe(Graphics		->Load(input));
			if_fail_fe(RenderNodes	 .Load(input));
			//if_fail_fe(RenderCache	 .Load(input));
			return 0;
		}

		err_t 				Save					(au0_t & output)	const	{
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

		err_t 				Clone					(u2_t iRenderNode, bool cloneSkin, bool cloneSurfaces, bool cloneShaders) {
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
						SSkin						& newSkin			= *Graphics->Skins[idSkin];
						for(u2_t iTexture = 0; iTexture < newSkin.Textures.size(); ++iTexture) {
							newSkin.Textures[iTexture]			= Graphics->Surfaces.Clone(newSkin.Textures[iTexture]);
						}
					}
				}
			}
			return iNewNode;
		}

	};

	err_t 						drawScene
		( g8bgra				backBufferColors
		, gu2_t				backBufferDepth
		, SEngineRenderCache	& renderCache
		, const SEngineScene	& scene
		, const n3f2_t		& cameraPosition
		, const n3f2_t		& cameraTarget
		, const n3f2_t		& cameraUp
		, const minmaxf2_t	& nearFar
		);

} // namespace

#endif // GPK_ENGINE_SCENE_H