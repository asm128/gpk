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
		::gpk::apod <::gpk::SRenderNode					>		RenderNodes			= {};
		::gpk::apod <::gpk::SRenderNodeFlags				>		Flags				= {};
		::gpk::apod <::gpk::SRenderNodeTransforms			>		Transforms			= {};
		::gpk::apod <::gpk::SRenderNodeTransforms			>		BaseTransforms		= {};
		::gpk::array_pobj<::gpk::apod<::gpk::SLight	>	>		Lights				= {};
		::gpk::array_pobj<::gpk::apod<::gpk::SCamera	>	>		Cameras				= {};

		// 
		::gpk::array_pobj<::gpk::apod<::gpk::SLightDirectional	>>	LightsDirectional	= {};
		::gpk::array_pobj<::gpk::apod<::gpk::SLightPoint		>>	LightsPoint			= {};
		::gpk::array_pobj<::gpk::apod<::gpk::SLightSpot		>>	LightsSpot			= {};

		::gpk::error_t											Create		()	{
			Transforms			.push_back({});
			BaseTransforms		.push_back({});
			Lights				.push_back({});
			Cameras				.push_back({});
			Flags				.push_back({});
			return RenderNodes	.push_back({(uint32_t)-1});
		}

		::gpk::error_t											Clone		(uint32_t iNode)	{
			Transforms			.push_back(::gpk::SRenderNodeTransforms							{Transforms		[iNode]});
			BaseTransforms		.push_back(::gpk::SRenderNodeTransforms							{BaseTransforms	[iNode]});
			Lights				.push_back(::gpk::pobj<::gpk::apod<::gpk::SLight	>>	{Lights			[iNode]});
			Cameras				.push_back(::gpk::pobj<::gpk::apod<::gpk::SCamera	>>	{Cameras		[iNode]});
			Flags				.push_back(::gpk::SRenderNodeFlags								{Flags			[iNode]});
			return RenderNodes	.push_back(::gpk::SRenderNode									{RenderNodes	[iNode]});
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
