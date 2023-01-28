#include "gpk_engine_rendercolor.h"
#include "gpk_ptr.h"
#include "gpk_matrix.h"

#ifndef GPK_ENGINE_RENDERNODE_H
#define GPK_ENGINE_RENDERNODE_H

namespace gpk 
{
#pragma pack(push, 1)
	// ------------------ Render Node
	struct SRenderNodeFlags {
		bool										NoAmbient			: 1;
		bool										NoDiffuse			: 1;
		bool										NoSpecular			: 1;
		bool										NoAlphaTest			: 1;
		bool										NoAlphaBlend		: 1;
		bool										NoDraw				: 1;
		//bool										NoTransformInherit	: 1;
	};

	struct SRenderNode {
		uint32_t									Mesh			= (uint32_t)-1;
		uint32_t									Slice			= (uint32_t)-1;
		uint32_t									Shader			= (uint32_t)-1;
		uint32_t									Skin			= (uint32_t)-1;
	};

	struct SRenderNodeTransforms {
		::gpk::SMatrix4<float>						World					= ::gpk::SMatrix4<float>::GetIdentity();
		::gpk::SMatrix4<float>						WorldInverse			= ::gpk::SMatrix4<float>::GetIdentity();
		::gpk::SMatrix4<float>						WorldInverseTranspose	= ::gpk::SMatrix4<float>::GetIdentity();
	};
	
	GDEFINE_ENUM_TYPE(LIGHT_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(LIGHT_TYPE, Directional	, 0);
	GDEFINE_ENUM_VALUE(LIGHT_TYPE, Point		, 1);
	GDEFINE_ENUM_VALUE(LIGHT_TYPE, Spot			, 2);

	struct SLightSpot {
		::gpk::SCoord3<float>						Direction;
		::gpk::SRenderColor							Color;
		float										SpotPower;
		float										Range;
	};

	struct SLightPoint {
		::gpk::SRenderColor							Color;
		float										Range;
	};

	struct SLightDirectional {
		::gpk::SCoord3<float>						Direction;
		::gpk::SRenderColor							Color;
	};

	struct SLight {
		::gpk::LIGHT_TYPE							Type;
		uint16_t									Index;
	};

	struct SCamera {
		::gpk::SCoord3<float>						Front;
		::gpk::SCoord3<float>						Right;
		::gpk::SCoord3<float>						Up;
		double										Angle;
	};
#pragma pack(pop)

	struct SRenderNodeManager {
		::gpk::apod<::gpk::SRenderNode				>			RenderNodes			= {};
		::gpk::apod<::gpk::SRenderNodeFlags			>			Flags				= {};
		::gpk::apod<::gpk::SRenderNodeTransforms	>			Transforms			= {};
		::gpk::apod<::gpk::SRenderNodeTransforms	>			BaseTransforms		= {};
		::gpk::apobj<::gpk::apod<::gpk::SLight	>	>			Lights				= {};
		::gpk::apobj<::gpk::apod<::gpk::SCamera	>	>			Cameras				= {};
			 
		// 	 
		::gpk::apobj<::gpk::apod<::gpk::SLightDirectional	>>	LightsDirectional	= {};
		::gpk::apobj<::gpk::apod<::gpk::SLightPoint			>>	LightsPoint			= {};
		::gpk::apobj<::gpk::apod<::gpk::SLightSpot			>>	LightsSpot			= {};

		const ::gpk::SRenderNode&								operator[]		(uint32_t index)							const	{ return RenderNodes[index]; }
		SRenderNode&											operator[]		(uint32_t index)									{ return RenderNodes[index]; }

		uint32_t												size			()											const	{ return RenderNodes.size(); }

		::gpk::error_t						Save			(::gpk::apod<ubyte_t> & output) const { 
			gpk_necs(::gpk::viewSave(output, RenderNodes	));
			gpk_necs(::gpk::viewSave(output, Flags			));
			gpk_necs(::gpk::viewSave(output, Transforms		));
			gpk_necs(::gpk::viewSave(output, BaseTransforms	));
			for(uint32_t iEntity = 0; iEntity < RenderNodes.size(); ++iEntity) {
				gpk_necall(::gpk::viewSave(output, Lights	[iEntity] ? ::gpk::v1<const ::gpk::SLight >{*Lights	[iEntity]} : ::gpk::v1<const ::gpk::SLight >{}), "iEntity: %i", iEntity);
				gpk_necall(::gpk::viewSave(output, Cameras	[iEntity] ? ::gpk::v1<const ::gpk::SCamera>{*Cameras[iEntity]} : ::gpk::v1<const ::gpk::SCamera>{}), "iEntity: %i", iEntity);
			}
			gpk_necs(output.append({(const ubyte_t*)&LightsDirectional.size(), 4}));
			for(uint32_t iEntity = 0; iEntity < LightsDirectional.size(); ++iEntity)
				gpk_necall(::gpk::viewSave(output, LightsDirectional[iEntity] ? ::gpk::v1<const ::gpk::SLightDirectional>{*LightsDirectional[iEntity]} : ::gpk::v1<const ::gpk::SLightDirectional>{}), "iEntity: %i", iEntity);

			gpk_necs(output.append({(const ubyte_t*)&LightsPoint.size(), 4}));
			for(uint32_t iEntity = 0; iEntity < LightsPoint.size(); ++iEntity)
				gpk_necall(::gpk::viewSave(output, LightsPoint[iEntity] ? ::gpk::v1<const ::gpk::SLightPoint>{*LightsPoint[iEntity]} : ::gpk::v1<const ::gpk::SLightPoint>{}), "iEntity: %i", iEntity);

			gpk_necs(output.append({(const ubyte_t*)&LightsSpot.size(), 4}));
			for(uint32_t iEntity = 0; iEntity < LightsSpot.size(); ++iEntity)
				gpk_necall(::gpk::viewSave(output, LightsSpot[iEntity] ? ::gpk::v1<const ::gpk::SLightSpot>{*LightsSpot[iEntity]} : ::gpk::v1<const ::gpk::SLightSpot>{}), "iEntity: %i", iEntity);

			info_printf("Saved %s, %i", "RenderNodes", RenderNodes.size());
			return 0;
		}

		::gpk::error_t						Load			(::gpk::vcub & input) {
			gpk_necs(::gpk::loadView(input, RenderNodes		));
			gpk_necs(::gpk::loadView(input, Flags			));
			gpk_necs(::gpk::loadView(input, Transforms		));
			gpk_necs(::gpk::loadView(input, BaseTransforms	));
			gpk_necall(Lights	.resize(RenderNodes.size()), "size: %i", RenderNodes.size());
			gpk_necall(Cameras	.resize(RenderNodes.size()), "size: %i", RenderNodes.size());
			for(uint32_t iEntity = 0; iEntity < RenderNodes.size(); ++iEntity) {
				if(!Lights	[iEntity]) Lights	[iEntity].create();
				if(!Cameras	[iEntity]) Cameras	[iEntity].create();
				gpk_necall(::gpk::loadView(input, *Lights	[iEntity]), "iEntity: %i", iEntity);
				gpk_necall(::gpk::loadView(input, *Cameras	[iEntity]), "iEntity: %i", iEntity);
			}

			gpk_necs(LightsDirectional.resize(*(uint32_t*)input.begin()));
			input											= {input.begin() + sizeof(uint32_t), input.size() - 4};
			for(uint32_t iEntity = 0; iEntity < LightsDirectional.size(); ++iEntity) {
				if(!LightsDirectional[iEntity])
					LightsDirectional[iEntity].create();
				gpk_necall(::gpk::loadView(input, *LightsDirectional[iEntity]), "iEntity: %i", iEntity);
			}

			gpk_necs(LightsPoint.resize(*(uint32_t*)input.begin()));
			input											= {input.begin() + sizeof(uint32_t), input.size() - 4};
			for(uint32_t iEntity = 0; iEntity < LightsPoint.size(); ++iEntity) {
				if(!LightsPoint[iEntity])
					LightsPoint[iEntity].create();
				gpk_necall(::gpk::loadView(input, *LightsPoint[iEntity]), "iEntity: %i", iEntity);
			}

			gpk_necs(LightsSpot.resize(*(uint32_t*)input.begin()));
			input											= {input.begin() + sizeof(uint32_t), input.size() - 4};
			for(uint32_t iEntity = 0; iEntity < LightsSpot.size(); ++iEntity) {
				if(!LightsSpot[iEntity])
					LightsSpot[iEntity].create();
				gpk_necall(::gpk::loadView(input, *LightsSpot[iEntity]), "iEntity: %i", iEntity);
			}
			return 0;
		}
		::gpk::error_t											Create		()	{
			Transforms			.push_back({});
			BaseTransforms		.push_back({});
			Lights				.push_back({});
			Cameras				.push_back({});
			Flags				.push_back({});
			return RenderNodes	.push_back({(uint32_t)-1});
		}

		::gpk::error_t											Clone		(uint32_t iNode)	{
			Transforms			.push_back(::gpk::SRenderNodeTransforms					{Transforms		[iNode]});
			BaseTransforms		.push_back(::gpk::SRenderNodeTransforms					{BaseTransforms	[iNode]});
			Lights				.push_back(::gpk::pobj<::gpk::apod<::gpk::SLight	>>	{Lights			[iNode]});
			Cameras				.push_back(::gpk::pobj<::gpk::apod<::gpk::SCamera	>>	{Cameras		[iNode]});
			Flags				.push_back(::gpk::SRenderNodeFlags						{Flags			[iNode]});
			return RenderNodes	.push_back(::gpk::SRenderNode							{RenderNodes	[iNode]});
		}

		::gpk::error_t											Delete		(uint32_t indexNode)	{
			Transforms			.remove_unordered(indexNode);
			BaseTransforms		.remove_unordered(indexNode);
			Lights				.remove_unordered(indexNode);
			Cameras				.remove_unordered(indexNode);
			Flags				.remove_unordered(indexNode);
			return RenderNodes	.remove_unordered(indexNode);
		}
	};
} // namespace

#endif
