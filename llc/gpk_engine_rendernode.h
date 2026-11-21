#include "gpk_engine_rendercolor.h"
#include "gpk_array_ptr.h"
#include "gpk_matrix.h"
#include "gpk_matrix2.h"
#include "gpk_matrix3.h"
#include "gpk_camera.h"

#ifndef GPK_ENGINE_RENDERNODE_H_23627
#define GPK_ENGINE_RENDERNODE_H_23627

nmsp gpk 
{
#pragma pack(push, 1)
    // ------------------ Render Node
    struct SRenderNodeFlags {
        b8_t					NoAmbient			: 1;
        b8_t					NoDiffuse			: 1;
        b8_t					NoSpecular			: 1;
        b8_t					NoAlphaTest			: 1;
        b8_t					NoAlphaBlend		: 1;
        b8_t					NoDraw				: 1;
        //b8_t					NoTransformInherit	: 1;
    };
    struct SRenderNode {
        u2_t				Mesh			= (u2_t)-1;
        u2_t				Slice			= (u2_t)-1;
        u2_t				Shader			= (u2_t)-1;
        u2_t				Skin			= (u2_t)-1;
    };
    struct SRenderMaterial {
        SRenderColor		Color			= {};
        n3f2_t			Emission		= {};
        f2_t					SpecularPower	= {};
    };
    stxp size_t		SRENDERMATERIALSIZE = sizeof(SRenderMaterial);

    struct SRenderNodeConstants {
        SRenderMaterial	Material				= {};
        m4f2_t			MVP						= m4f2_t::GetIdentity();
        m4f2_t			Model					= m4f2_t::GetIdentity();
        m4f2_t			ModelInverse			= m4f2_t::GetIdentity();
        m4f2_t			ModelInverseTranspose	= m4f2_t::GetIdentity();
        n3f2_t			NodeSize				= {}; 
        f2_t					PaddingC				= 0;
    };
    stxp size_t		SRENDERNODECONSTANTSSIZE = sizeof(SRenderNodeConstants);


    GDEFINE_ENUM_TYPE(LIGHT_TYPE, u0_t);
    GDEFINE_ENUM_VALUE(LIGHT_TYPE, Directional	, 0);
    GDEFINE_ENUM_VALUE(LIGHT_TYPE, Point		, 1);
    GDEFINE_ENUM_VALUE(LIGHT_TYPE, Spot			, 2);

    struct SLightSpot {
        n3f2_t			Direction;
        SRenderColor		Color;
        f2_t					SpotPower;
        f2_t					Range;
    };
    struct SLightPoint {
        SRenderColor		Color;
        f2_t					Range;
    };
    struct SLightDirectional {
        n3f2_t			Direction;
        SRenderColor		Color;
    };
    struct SLight {
        LIGHT_TYPE		Type;
        u1_t				Index;
    };
    struct SCamera : SCameraPoints, SCameraRange, SCameraVectors {};
#pragma pack(pop)

    struct SRenderNodeManager {
        apod<SRenderNode>				RenderNodes			= {};
        apod<SRenderNodeFlags>			Flags				= {};
        apod<SRenderNodeConstants>		Transforms			= {};
        apod<SRenderNodeConstants>		BaseTransforms		= {};
        //
        apobj<apod<SLight>>				Lights				= {};	// Map each light to either LightsDirectional, LightsPoint or LightsSpot
        apobj<apod<SCamera>>			Cameras				= {};	// Currently one camera type
        //
        apobj<apod<SLightDirectional>>	LightsDirectional	= {};
        apobj<apod<SLightPoint>>		LightsPoint			= {};
        apobj<apod<SLightSpot>>			LightsSpot			= {};

        cnst SRenderNode&				oper[]		(u2_t index)							cnst	{ return RenderNodes[index]; }
        SRenderNode&					oper[]		(u2_t index)									{ return RenderNodes[index]; }

        u2_t							size			()											cnst	{ return RenderNodes.size(); }

        err_t 					Save			(apod<u0_t> & output) cnst { 
            if_fail_fe(saveView(output, RenderNodes	));
            if_fail_fe(saveView(output, Flags			));
            if_fail_fe(saveView(output, Transforms		));
            if_fail_fe(saveView(output, BaseTransforms	));
            for(u2_t iEntity = 0; iEntity < RenderNodes.size(); ++iEntity) {
                if_fail_fef(saveView(output, Lights	[iEntity] ? view<cnst SLight >{*Lights	[iEntity]} : view<cnst SLight >{}), "iEntity: %i", iEntity);
                if_fail_fef(saveView(output, Cameras	[iEntity] ? view<cnst SCamera>{*Cameras[iEntity]} : view<cnst SCamera>{}), "iEntity: %i", iEntity);
            }
            if_fail_fe(output.append({(cnst u0_t*)&LightsDirectional.size(), 4}));
            for(u2_t iEntity = 0; iEntity < LightsDirectional.size(); ++iEntity)
                if_fail_fef(saveView(output, LightsDirectional[iEntity] ? view<cnst SLightDirectional>{*LightsDirectional[iEntity]} : view<cnst SLightDirectional>{}), "iEntity: %i", iEntity);

            if_fail_fe(output.append({(cnst u0_t*)&LightsPoint.size(), 4}));
            for(u2_t iEntity = 0; iEntity < LightsPoint.size(); ++iEntity)
                if_fail_fef(saveView(output, LightsPoint[iEntity] ? view<cnst SLightPoint>{*LightsPoint[iEntity]} : view<cnst SLightPoint>{}), "iEntity: %i", iEntity);

            if_fail_fe(output.append({(cnst u0_t*)&LightsSpot.size(), 4}));
            for(u2_t iEntity = 0; iEntity < LightsSpot.size(); ++iEntity)
                if_fail_fef(saveView(output, LightsSpot[iEntity] ? view<cnst SLightSpot>{*LightsSpot[iEntity]} : view<cnst SLightSpot>{}), "iEntity: %i", iEntity);

            info_printf("Saved %s, %i", "RenderNodes", RenderNodes.size());
            return 0;
        }

        err_t 					Load			(vcu0_t & input) {
            if_fail_fe(loadView(input, RenderNodes		));
            if_fail_fe(loadView(input, Flags			));
            if_fail_fe(loadView(input, Transforms		));
            if_fail_fe(loadView(input, BaseTransforms	));
            if_fail_fef(Lights	.resize(RenderNodes.size()), "size: %i", RenderNodes.size());
            if_fail_fef(Cameras	.resize(RenderNodes.size()), "size: %i", RenderNodes.size());
            for(u2_t iEntity = 0; iEntity < RenderNodes.size(); ++iEntity) {
                if(!Lights	[iEntity]) 
                    if_true_vef(-1, 0 == Lights	[iEntity].create(), "iEntity: %i", iEntity);
                if(!Cameras	[iEntity]) 
                    if_true_vef(-1, 0 == Cameras	[iEntity].create(), "iEntity: %i", iEntity);
                if_fail_fef(loadView(input, *Lights	[iEntity]), "iEntity: %i", iEntity);
                if_fail_fef(loadView(input, *Cameras	[iEntity]), "iEntity: %i", iEntity);
            }

            if_fail_fe(LightsDirectional.resize(*(u2_t*)input.begin()));
            input								= {input.begin() + sizeof(u2_t), input.size() - 4};
            for(u2_t iEntity = 0; iEntity < LightsDirectional.size(); ++iEntity) {
                if(!LightsDirectional[iEntity])
                    if_true_vef(-1, 0 == LightsDirectional[iEntity].create(), "iEntity: %i", iEntity);
                if_fail_fef(loadView(input, *LightsDirectional[iEntity]), "iEntity: %i", iEntity);
            }

            if_fail_fe(LightsPoint.resize(*(u2_t*)input.begin()));
            input								= {input.begin() + sizeof(u2_t), input.size() - 4};
            for(u2_t iEntity = 0; iEntity < LightsPoint.size(); ++iEntity) {
                if(!LightsPoint[iEntity])
                    if_true_vef(-1, 0 == LightsPoint[iEntity].create(), "iEntity: %i", iEntity);
                if_fail_fef(loadView(input, *LightsPoint[iEntity]), "iEntity: %i", iEntity);
            }

            if_fail_fe(LightsSpot.resize(*(u2_t*)input.begin()));
            input								= {input.begin() + sizeof(u2_t), input.size() - 4};
            for(u2_t iEntity = 0; iEntity < LightsSpot.size(); ++iEntity) {
                if(!LightsSpot[iEntity])
                    if_true_vef(-1, 0 == LightsSpot[iEntity].create(), "iEntity: %i", iEntity);
                if_fail_fef(loadView(input, *LightsSpot[iEntity]), "iEntity: %i", iEntity);
            }
            return 0;
        }
        err_t 					Create		()	{
            if_fail_fe(Transforms		.push_back({}));
            if_fail_fe(BaseTransforms	.push_back({}));
            if_fail_fe(Lights			.push_back({}));
            if_fail_fe(Cameras		.push_back({}));
            if_fail_fe(Flags			.push_back({}));
            return RenderNodes.push_back({(u2_t)-1});
        }

        err_t 					Clone		(u2_t iNode)	{
            if_fail_fe(Transforms		.push_back(SRenderNodeConstants					{Transforms		[iNode]}));
            if_fail_fe(BaseTransforms	.push_back(SRenderNodeConstants					{BaseTransforms	[iNode]}));
            if_fail_fe(Lights			.push_back(pobj<apod<SLight	>>	{Lights			[iNode]}));
            if_fail_fe(Cameras		.push_back(pobj<apod<SCamera	>>	{Cameras		[iNode]}));
            if_fail_fe(Flags			.push_back(SRenderNodeFlags						{Flags			[iNode]}));
            return RenderNodes.push_back(SRenderNode{RenderNodes	[iNode]});
        }

        err_t 					Delete		(u2_t indexNode)	{
            if_fail_fe(Transforms			.remove_unordered(indexNode));
            if_fail_fe(BaseTransforms		.remove_unordered(indexNode));
            if_fail_fe(Lights				.remove_unordered(indexNode));
            if_fail_fe(Cameras			.remove_unordered(indexNode));
            if_fail_fe(Flags				.remove_unordered(indexNode));
            return RenderNodes.remove_unordered(indexNode);
        }
    };
} // nmsp

#endif // GPK_ENGINE_RENDERNODE_H_23627
