#include "gpk_engine_rendercolor.h"
#include "gpk_array_ptr.h"
#include "gpk_matrix.h"
#include "gpk_matrix2.h"
#include "gpk_matrix3.h"
#include "gpk_camera.h"

#ifndef GPK_ENGINE_RENDERNODE_H_23627
#define GPK_ENGINE_RENDERNODE_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	// ------------------ Render Node
	struct SRenderNodeFlags {
		bool					NoAmbient			: 1;
		bool					NoDiffuse			: 1;
		bool					NoSpecular			: 1;
		bool					NoAlphaTest			: 1;
		bool					NoAlphaBlend		: 1;
		bool					NoDraw				: 1;
		//bool					NoTransformInherit	: 1;
	};
	struct SRenderNode {
		u2_t				Mesh			= (u2_t)-1;
		u2_t				Slice			= (u2_t)-1;
		u2_t				Shader			= (u2_t)-1;
		u2_t				Skin			= (u2_t)-1;
	};
	struct SRenderMaterial {
		::gpk::SRenderColor		Color			= {};
		::gpk::n3f2_t			Emission		= {};
		f2_t					SpecularPower	= {};
	};
	stxp size_t		SRENDERMATERIALSIZE = sizeof(SRenderMaterial);

	struct SRenderNodeConstants {
		::gpk::SRenderMaterial	Material				= {};
		::gpk::m4f2_t			MVP						= ::gpk::m4f2_t::GetIdentity();
		::gpk::m4f2_t			Model					= ::gpk::m4f2_t::GetIdentity();
		::gpk::m4f2_t			ModelInverse			= ::gpk::m4f2_t::GetIdentity();
		::gpk::m4f2_t			ModelInverseTranspose	= ::gpk::m4f2_t::GetIdentity();
		::gpk::n3f2_t			NodeSize				= {}; 
		f2_t					PaddingC				= 0;
	};
	stxp size_t		SRENDERNODECONSTANTSSIZE = sizeof(SRenderNodeConstants);


	GDEFINE_ENUM_TYPE(LIGHT_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(LIGHT_TYPE, Directional	, 0);
	GDEFINE_ENUM_VALUE(LIGHT_TYPE, Point		, 1);
	GDEFINE_ENUM_VALUE(LIGHT_TYPE, Spot			, 2);

	struct SLightSpot {
		::gpk::n3f2_t			Direction;
		::gpk::SRenderColor		Color;
		f2_t					SpotPower;
		f2_t					Range;
	};
	struct SLightPoint {
		::gpk::SRenderColor		Color;
		f2_t					Range;
	};
	struct SLightDirectional {
		::gpk::n3f2_t			Direction;
		::gpk::SRenderColor		Color;
	};
	struct SLight {
		::gpk::LIGHT_TYPE		Type;
		u1_t				Index;
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

		const ::gpk::SRenderNode&			operator[]		(u2_t index)							const	{ return RenderNodes[index]; }
		SRenderNode&						operator[]		(u2_t index)									{ return RenderNodes[index]; }

		u2_t							size			()											const	{ return RenderNodes.size(); }

		gpk::err_t 					Save			(::gpk::apod<u0_t> & output) const { 
			if_fail_fe(gpk::saveView(output, RenderNodes	));
			if_fail_fe(gpk::saveView(output, Flags			));
			if_fail_fe(gpk::saveView(output, Transforms		));
			if_fail_fe(gpk::saveView(output, BaseTransforms	));
			for(u2_t iEntity = 0; iEntity < RenderNodes.size(); ++iEntity) {
				if_fail_fef(gpk::saveView(output, Lights	[iEntity] ? ::gpk::v1<const ::gpk::SLight >{*Lights	[iEntity]} : ::gpk::v1<const ::gpk::SLight >{}), "iEntity: %i", iEntity);
				if_fail_fef(gpk::saveView(output, Cameras	[iEntity] ? ::gpk::v1<const ::gpk::SCamera>{*Cameras[iEntity]} : ::gpk::v1<const ::gpk::SCamera>{}), "iEntity: %i", iEntity);
			}
			if_fail_fe(output.append({(const u0_t*)&LightsDirectional.size(), 4}));
			for(u2_t iEntity = 0; iEntity < LightsDirectional.size(); ++iEntity)
				if_fail_fef(gpk::saveView(output, LightsDirectional[iEntity] ? ::gpk::v1<const ::gpk::SLightDirectional>{*LightsDirectional[iEntity]} : ::gpk::v1<const ::gpk::SLightDirectional>{}), "iEntity: %i", iEntity);

			if_fail_fe(output.append({(const u0_t*)&LightsPoint.size(), 4}));
			for(u2_t iEntity = 0; iEntity < LightsPoint.size(); ++iEntity)
				if_fail_fef(gpk::saveView(output, LightsPoint[iEntity] ? ::gpk::v1<const ::gpk::SLightPoint>{*LightsPoint[iEntity]} : ::gpk::v1<const ::gpk::SLightPoint>{}), "iEntity: %i", iEntity);

			if_fail_fe(output.append({(const u0_t*)&LightsSpot.size(), 4}));
			for(u2_t iEntity = 0; iEntity < LightsSpot.size(); ++iEntity)
				if_fail_fef(gpk::saveView(output, LightsSpot[iEntity] ? ::gpk::v1<const ::gpk::SLightSpot>{*LightsSpot[iEntity]} : ::gpk::v1<const ::gpk::SLightSpot>{}), "iEntity: %i", iEntity);

			info_printf("Saved %s, %i", "RenderNodes", RenderNodes.size());
			return 0;
		}

		gpk::err_t 					Load			(::gpk::vcu0_t & input) {
			if_fail_fe(gpk::loadView(input, RenderNodes		));
			if_fail_fe(gpk::loadView(input, Flags			));
			if_fail_fe(gpk::loadView(input, Transforms		));
			if_fail_fe(gpk::loadView(input, BaseTransforms	));
			if_fail_fef(Lights	.resize(RenderNodes.size()), "size: %i", RenderNodes.size());
			if_fail_fef(Cameras	.resize(RenderNodes.size()), "size: %i", RenderNodes.size());
			for(u2_t iEntity = 0; iEntity < RenderNodes.size(); ++iEntity) {
				if(!Lights	[iEntity]) 
					if_true_vef(-1, 0 == Lights	[iEntity].create(), "iEntity: %i", iEntity);
				if(!Cameras	[iEntity]) 
					if_true_vef(-1, 0 == Cameras	[iEntity].create(), "iEntity: %i", iEntity);
				if_fail_fef(gpk::loadView(input, *Lights	[iEntity]), "iEntity: %i", iEntity);
				if_fail_fef(gpk::loadView(input, *Cameras	[iEntity]), "iEntity: %i", iEntity);
			}

			if_fail_fe(LightsDirectional.resize(*(u2_t*)input.begin()));
			input								= {input.begin() + sizeof(u2_t), input.size() - 4};
			for(u2_t iEntity = 0; iEntity < LightsDirectional.size(); ++iEntity) {
				if(!LightsDirectional[iEntity])
					if_true_vef(-1, 0 == LightsDirectional[iEntity].create(), "iEntity: %i", iEntity);
				if_fail_fef(gpk::loadView(input, *LightsDirectional[iEntity]), "iEntity: %i", iEntity);
			}

			if_fail_fe(LightsPoint.resize(*(u2_t*)input.begin()));
			input								= {input.begin() + sizeof(u2_t), input.size() - 4};
			for(u2_t iEntity = 0; iEntity < LightsPoint.size(); ++iEntity) {
				if(!LightsPoint[iEntity])
					if_true_vef(-1, 0 == LightsPoint[iEntity].create(), "iEntity: %i", iEntity);
				if_fail_fef(gpk::loadView(input, *LightsPoint[iEntity]), "iEntity: %i", iEntity);
			}

			if_fail_fe(LightsSpot.resize(*(u2_t*)input.begin()));
			input								= {input.begin() + sizeof(u2_t), input.size() - 4};
			for(u2_t iEntity = 0; iEntity < LightsSpot.size(); ++iEntity) {
				if(!LightsSpot[iEntity])
					if_true_vef(-1, 0 == LightsSpot[iEntity].create(), "iEntity: %i", iEntity);
				if_fail_fef(gpk::loadView(input, *LightsSpot[iEntity]), "iEntity: %i", iEntity);
			}
			return 0;
		}
		gpk::err_t 					Create		()	{
			if_fail_fe(Transforms		.push_back({}));
			if_fail_fe(BaseTransforms	.push_back({}));
			if_fail_fe(Lights			.push_back({}));
			if_fail_fe(Cameras		.push_back({}));
			if_fail_fe(Flags			.push_back({}));
			return RenderNodes.push_back({(u2_t)-1});
		}

		gpk::err_t 					Clone		(u2_t iNode)	{
			if_fail_fe(Transforms		.push_back(::gpk::SRenderNodeConstants					{Transforms		[iNode]}));
			if_fail_fe(BaseTransforms	.push_back(::gpk::SRenderNodeConstants					{BaseTransforms	[iNode]}));
			if_fail_fe(Lights			.push_back(::gpk::pobj<::gpk::apod<::gpk::SLight	>>	{Lights			[iNode]}));
			if_fail_fe(Cameras		.push_back(::gpk::pobj<::gpk::apod<::gpk::SCamera	>>	{Cameras		[iNode]}));
			if_fail_fe(Flags			.push_back(::gpk::SRenderNodeFlags						{Flags			[iNode]}));
			return RenderNodes.push_back(::gpk::SRenderNode{RenderNodes	[iNode]});
		}

		gpk::err_t 					Delete		(u2_t indexNode)	{
			if_fail_fe(Transforms			.remove_unordered(indexNode));
			if_fail_fe(BaseTransforms		.remove_unordered(indexNode));
			if_fail_fe(Lights				.remove_unordered(indexNode));
			if_fail_fe(Cameras			.remove_unordered(indexNode));
			if_fail_fe(Flags				.remove_unordered(indexNode));
			return RenderNodes.remove_unordered(indexNode);
		}
	};
} // namespace

#endif // GPK_ENGINE_RENDERNODE_H_23627
