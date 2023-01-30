#include "gpk_engine_renderbuffer.h"
#include "gpk_engine_rendermesh.h"
#include "gpk_engine_rendernode.h"
#include "gpk_engine_rendersurface.h"

#include "gpk_font.h"
#include "gpk_raster_lh.h"

#include <functional>

#ifndef GPK_ENGINE_SCENE_H
#define GPK_ENGINE_SCENE_H

namespace gpk 
{
	
#pragma pack(push, 1)
	struct SEngineSceneConstants {
		::gpk::m4f					View					= {}; 
		::gpk::m4f					Perspective				= {}; 
		::gpk::m4f					Screen					= {}; 
		::gpk::m4f					VP						= {}; 
		::gpk::m4f					VPS						= {}; 
		::gpk::n3f					CameraPosition			= {}; 
		float						CameraAngle				= 0;
		::gpk::n3f					CameraFront				= {}; 
		float						PaddingB				= 0;
		::gpk::n3f					LightPosition			= {}; 
		float						PaddingC				= 0;
		::gpk::n3f					LightDirection			= {}; 
		float						LightSpotPower			= 0;
	};

	struct SRenderMaterial {
		::gpk::SRenderColor			Color;
		::gpk::n3f					Emission;
		float						SpecularPower;
	};

	struct SRenderNodeConstants {
		::gpk::m4f					Model;
		::gpk::m4f					ModelInverseTranspose;
		::gpk::m4f					MVP;
		::gpk::SRenderMaterial		Material;
	};
#pragma pack(pop)

	struct SPSIn {
		::gpk::n3f					WeightedPosition	;
		::gpk::n3f					WeightedNormal		;
		::gpk::n2f					WeightedUV			;
		::gpk::SRenderMaterial		Material			;
		::gpk::v2cbgra				Surface				; 
	};

	typedef ::gpk::error_t					(TFuncPixelShader)
		( const ::gpk::SEngineSceneConstants	& constants
		, const ::gpk::SPSIn					& inPS
		, ::gpk::bgra							& outputPixel
		);

	struct SVSCache {
		::gpk::apod<::gpk::STriangle<float>>	TriangleWeights			= {};
		::gpk::apod<::gpk::n2i16>				SolidPixelCoords		= {};
		::gpk::apod<::gpk::n3f>					WireframePixelCoords	= {};
	};

	struct SVSOutput {
		::gpk::apod<::gpk::STriangle3<float>>	PositionsWorld			= {};
		::gpk::apod<::gpk::STriangle3<float>>	PositionsScreen			= {};
		::gpk::apod<::gpk::STriangle3<float>>	Normals					= {};
		::gpk::apod<::gpk::STriangle3<float>>	NormalsScreen			= {};
		::gpk::apod<::gpk::STriangle2<float>>	UVs						= {};
	};

	struct SEngineRenderCache {
		::gpk::SVSOutput						VertexShaderOutput		= {};
		::gpk::SVSCache							VertexShaderCache		= {};
	};

	struct SSkin {
		::gpk::SRenderMaterial					Material;
		::gpk::apod<uint32_t>					Textures;

		::gpk::error_t						Save			(::gpk::apod<ubyte_t> & output) const { 
			gpk_necs(::gpk::savePOD(output, Material));
			gpk_necs(::gpk::viewSave(output, Textures));
			return 0;
		}
		::gpk::error_t						Load			(::gpk::vcub & input) {
			gpk_necs(::gpk::loadPOD(input, Material));
			gpk_necs(::gpk::loadView(input, Textures));
			return 0;
		}
	};

	typedef	::gpk::SLinearMap<::gpk::SSkin>							SSkinManager;
	typedef	::gpk::SLinearMap<::std::function<TFuncPixelShader>>	SShaderManager;

	struct SEngineGraphics {
		::gpk::SRenderBufferManager				Buffers					= {};
		::gpk::SSurfaceManager					Surfaces				= {};
		::gpk::SMeshManager						Meshes					= {};
		::gpk::SSkinManager						Skins					= {};

		::gpk::SShaderManager					Shaders					= {};
		::gpk::SRasterFontManager				Fonts					= {};

		::gpk::error_t							Save				(::gpk::apod<ubyte_t> & output)		const	{
			gpk_necs(Buffers	.Save(output));
			gpk_necs(Surfaces	.Save(output));
			gpk_necs(Meshes		.Save(output));
			gpk_necs(Skins		.Save(output));
			//gpk_necs(Shaders	.Save(output));
			//gpk_necs(Fonts	.Save(output));
			return 0;
		}
		::gpk::error_t							Load				(::gpk::vcub & input) {
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
		::gpk::pobj<::gpk::SEngineGraphics>		Graphics				= {};
		::gpk::SRenderNodeManager				ManagedRenderNodes		= {};
		::gpk::SEngineRenderCache				RenderCache				= {};


		::gpk::error_t						Save				(::gpk::apod<ubyte_t> & output)		const	{
			gpk_necs(Graphics			->Save(output));
			gpk_necs(ManagedRenderNodes	 .Save(output));
			//gpk_necs(RenderCache		 .Save(output));
			return 0;
		}
		::gpk::error_t						Load				(::gpk::vcub & input) {
			gpk_necs(Graphics			->Load(input));
			gpk_necs(ManagedRenderNodes	 .Load(input));
			//gpk_necs(RenderCache		 .Load(input));
			return 0;
		}
	};

	::gpk::error_t							drawScene
		( ::gpk::v2bgra							& backBufferColors
		, ::gpk::v2u32							& backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		);

} // namespace

#endif // GPK_ENGINE_SCENE_H