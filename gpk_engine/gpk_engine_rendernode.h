#include "gpk_engine_rendercolor.h"
#include "gpk_array_ptr.h"
#include "gpk_matrix.h"
#include "gpk_camera.h"

#ifndef GPK_ENGINE_RENDERNODE_H_23627
#define GPK_ENGINE_RENDERNODE_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	// ------------------ Render Node
	struct SRenderNodeFlags {
		bool						NoAmbient			: 1;
		bool						NoDiffuse			: 1;
		bool						NoSpecular			: 1;
		bool						NoAlphaTest			: 1;
		bool						NoAlphaBlend		: 1;
		bool						NoDraw				: 1;
		//bool						NoTransformInherit	: 1;
	};

	struct SRenderNode {
		uint32_t					Mesh			= (uint32_t)-1;
		uint32_t					Slice			= (uint32_t)-1;
		uint32_t					Shader			= (uint32_t)-1;
		uint32_t					Skin			= (uint32_t)-1;
	};

	struct SRenderMaterial {
		::gpk::SRenderColor		Color			= {};
		::gpk::n3f32			Emission		= {};
		float					SpecularPower	= {};
	};
	stacxpr size_t		SRENDERMATERIALSIZE = sizeof(SRenderMaterial);

	struct SRenderNodeConstants {
		::gpk::SRenderMaterial	Material				= {};
		::gpk::m4f32			MVP						= ::gpk::m4f32::GetIdentity();
		::gpk::m4f32			Model					= ::gpk::m4f32::GetIdentity();
		::gpk::m4f32			ModelInverse			= ::gpk::m4f32::GetIdentity();
		::gpk::m4f32			ModelInverseTranspose	= ::gpk::m4f32::GetIdentity();
		::gpk::n3f32			NodeSize				= {}; 
		float					PaddingC				= 0;
	};
	stacxpr size_t		SRENDERNODECONSTANTSSIZE = sizeof(SRenderNodeConstants);


	GDEFINE_ENUM_TYPE(LIGHT_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(LIGHT_TYPE, Directional	, 0);
	GDEFINE_ENUM_VALUE(LIGHT_TYPE, Point		, 1);
	GDEFINE_ENUM_VALUE(LIGHT_TYPE, Spot			, 2);

	struct SLightSpot {
		::gpk::n3f32			Direction;
		::gpk::SRenderColor		Color;
		float					SpotPower;
		float					Range;
	};

	struct SLightPoint {
		::gpk::SRenderColor		Color;
		float					Range;
	};

	struct SLightDirectional {
		::gpk::n3f32			Direction;
		::gpk::SRenderColor		Color;
	};

	struct SLight {
		::gpk::LIGHT_TYPE		Type;
		uint16_t				Index;
	};

	struct SCamera : SCameraPoints, SCameraRange, SCameraVectors {};
#pragma pack(pop)

	struct SRenderNodeManager {
		::gpk::apod<::gpk::SRenderNode>				RenderNodes			= {};
		::gpk::apod<::gpk::SRenderNodeFlags>		Flags				= {};
		::gpk::apod<::gpk::SRenderNodeConstants>	Transforms			= {};
		::gpk::apod<::gpk::SRenderNodeConstants>	BaseTransforms		= {};
		//
		::gpk::apobj<::gpk::apod<::gpk::SLight>>			Lights				= {};	// Map each light to either LightsDirectional, LightsPoint or LightsSpot
		::gpk::apobj<::gpk::apod<::gpk::SCamera>>			Cameras				= {};	// Currently one camera type
		//
		::gpk::apobj<::gpk::apod<::gpk::SLightDirectional>>	LightsDirectional	= {};
		::gpk::apobj<::gpk::apod<::gpk::SLightPoint>>		LightsPoint			= {};
		::gpk::apobj<::gpk::apod<::gpk::SLightSpot>>		LightsSpot			= {};

		const ::gpk::SRenderNode&			operator[]		(uint32_t index)							const	{ return RenderNodes[index]; }
		SRenderNode&						operator[]		(uint32_t index)									{ return RenderNodes[index]; }

		uint32_t							size			()											const	{ return RenderNodes.size(); }

		::gpk::error_t						Save			(::gpk::apod<uint8_t> & output) const { 
			gpk_necs(::gpk::saveView(output, RenderNodes	));
			gpk_necs(::gpk::saveView(output, Flags			));
			gpk_necs(::gpk::saveView(output, Transforms		));
			gpk_necs(::gpk::saveView(output, BaseTransforms	));
			for(uint32_t iEntity = 0; iEntity < RenderNodes.size(); ++iEntity) {
				gpk_necall(::gpk::saveView(output, Lights	[iEntity] ? ::gpk::v1<const ::gpk::SLight >{*Lights	[iEntity]} : ::gpk::v1<const ::gpk::SLight >{}), "iEntity: %i", iEntity);
				gpk_necall(::gpk::saveView(output, Cameras	[iEntity] ? ::gpk::v1<const ::gpk::SCamera>{*Cameras[iEntity]} : ::gpk::v1<const ::gpk::SCamera>{}), "iEntity: %i", iEntity);
			}
			gpk_necs(output.append({(const uint8_t*)&LightsDirectional.size(), 4}));
			for(uint32_t iEntity = 0; iEntity < LightsDirectional.size(); ++iEntity)
				gpk_necall(::gpk::saveView(output, LightsDirectional[iEntity] ? ::gpk::v1<const ::gpk::SLightDirectional>{*LightsDirectional[iEntity]} : ::gpk::v1<const ::gpk::SLightDirectional>{}), "iEntity: %i", iEntity);

			gpk_necs(output.append({(const uint8_t*)&LightsPoint.size(), 4}));
			for(uint32_t iEntity = 0; iEntity < LightsPoint.size(); ++iEntity)
				gpk_necall(::gpk::saveView(output, LightsPoint[iEntity] ? ::gpk::v1<const ::gpk::SLightPoint>{*LightsPoint[iEntity]} : ::gpk::v1<const ::gpk::SLightPoint>{}), "iEntity: %i", iEntity);

			gpk_necs(output.append({(const uint8_t*)&LightsSpot.size(), 4}));
			for(uint32_t iEntity = 0; iEntity < LightsSpot.size(); ++iEntity)
				gpk_necall(::gpk::saveView(output, LightsSpot[iEntity] ? ::gpk::v1<const ::gpk::SLightSpot>{*LightsSpot[iEntity]} : ::gpk::v1<const ::gpk::SLightSpot>{}), "iEntity: %i", iEntity);

			info_printf("Saved %s, %i", "RenderNodes", RenderNodes.size());
			return 0;
		}

		::gpk::error_t						Load			(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadView(input, RenderNodes		));
			gpk_necs(::gpk::loadView(input, Flags			));
			gpk_necs(::gpk::loadView(input, Transforms		));
			gpk_necs(::gpk::loadView(input, BaseTransforms	));
			gpk_necall(Lights	.resize(RenderNodes.size()), "size: %i", RenderNodes.size());
			gpk_necall(Cameras	.resize(RenderNodes.size()), "size: %i", RenderNodes.size());
			for(uint32_t iEntity = 0; iEntity < RenderNodes.size(); ++iEntity) {
				if(!Lights	[iEntity]) 
					ree_if(0 == Lights	[iEntity].create(), "iEntity: %i", iEntity);
				if(!Cameras	[iEntity]) 
					ree_if(0 == Cameras	[iEntity].create(), "iEntity: %i", iEntity);
				gpk_necall(::gpk::loadView(input, *Lights	[iEntity]), "iEntity: %i", iEntity);
				gpk_necall(::gpk::loadView(input, *Cameras	[iEntity]), "iEntity: %i", iEntity);
			}

			gpk_necs(LightsDirectional.resize(*(uint32_t*)input.begin()));
			input								= {input.begin() + sizeof(uint32_t), input.size() - 4};
			for(uint32_t iEntity = 0; iEntity < LightsDirectional.size(); ++iEntity) {
				if(!LightsDirectional[iEntity])
					ree_if(0 == LightsDirectional[iEntity].create(), "iEntity: %i", iEntity);
				gpk_necall(::gpk::loadView(input, *LightsDirectional[iEntity]), "iEntity: %i", iEntity);
			}

			gpk_necs(LightsPoint.resize(*(uint32_t*)input.begin()));
			input								= {input.begin() + sizeof(uint32_t), input.size() - 4};
			for(uint32_t iEntity = 0; iEntity < LightsPoint.size(); ++iEntity) {
				if(!LightsPoint[iEntity])
					ree_if(0 == LightsPoint[iEntity].create(), "iEntity: %i", iEntity);
				gpk_necall(::gpk::loadView(input, *LightsPoint[iEntity]), "iEntity: %i", iEntity);
			}

			gpk_necs(LightsSpot.resize(*(uint32_t*)input.begin()));
			input								= {input.begin() + sizeof(uint32_t), input.size() - 4};
			for(uint32_t iEntity = 0; iEntity < LightsSpot.size(); ++iEntity) {
				if(!LightsSpot[iEntity])
					ree_if(0 == LightsSpot[iEntity].create(), "iEntity: %i", iEntity);
				gpk_necall(::gpk::loadView(input, *LightsSpot[iEntity]), "iEntity: %i", iEntity);
			}
			return 0;
		}
		::gpk::error_t						Create		()	{
			gpk_necs(Transforms		.push_back({}));
			gpk_necs(BaseTransforms	.push_back({}));
			gpk_necs(Lights			.push_back({}));
			gpk_necs(Cameras		.push_back({}));
			gpk_necs(Flags			.push_back({}));
			return RenderNodes.push_back({(uint32_t)-1});
		}

		::gpk::error_t						Clone		(uint32_t iNode)	{
			gpk_necs(Transforms		.push_back(::gpk::SRenderNodeConstants					{Transforms		[iNode]}));
			gpk_necs(BaseTransforms	.push_back(::gpk::SRenderNodeConstants					{BaseTransforms	[iNode]}));
			gpk_necs(Lights			.push_back(::gpk::pobj<::gpk::apod<::gpk::SLight	>>	{Lights			[iNode]}));
			gpk_necs(Cameras		.push_back(::gpk::pobj<::gpk::apod<::gpk::SCamera	>>	{Cameras		[iNode]}));
			gpk_necs(Flags			.push_back(::gpk::SRenderNodeFlags						{Flags			[iNode]}));
			return RenderNodes.push_back(::gpk::SRenderNode{RenderNodes	[iNode]});
		}

		::gpk::error_t						Delete		(uint32_t indexNode)	{
			gpk_necs(Transforms			.remove_unordered(indexNode));
			gpk_necs(BaseTransforms		.remove_unordered(indexNode));
			gpk_necs(Lights				.remove_unordered(indexNode));
			gpk_necs(Cameras			.remove_unordered(indexNode));
			gpk_necs(Flags				.remove_unordered(indexNode));
			return RenderNodes.remove_unordered(indexNode);
		}
	};
} // namespace

#endif // GPK_ENGINE_RENDERNODE_H_23627
