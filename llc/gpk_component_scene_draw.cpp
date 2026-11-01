#include "gpk_component_scene.h"
#include "gpk_raster_lh.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();
GPK_USING_N2();
GPK_USING_N3();

static	::gpk::err_t	drawOrderedVertices
	( ::gpk::SNodeRenderer					& renderer
	, const ::gpk::SRenderNode				& nodeToDraw
	, ::gpk::m4f2_t				matrixWVP
	, ::gpk::g8bgra	target_image
	, ::gpk::grid<u2_t>			target_depth
	) {

	::gpk::tri3f2_t							triangleTransformed		= {};
	::gpk::tri2<s1_t>					triangleFinal			= {};
	::gpk::bgra							nodeColor				= (nodeToDraw.Color >= 0) ? renderer.Colors[nodeToDraw.Color] : ::gpk::bgra{0xFFFF00FF};
	if(nodeToDraw.PerFaceNormal) {
		if(nodeToDraw.VertexColor < 0) {
			::gpk::drawTriangle(target_image, triangleFinal, nodeColor);
		}
		else if(nodeToDraw.PerFaceColor) {
			::gpk::apod<::gpk::n3f2_t>		& nodeVertices			= renderer.Vertices[nodeToDraw.Vertices];
			::gpk::apod<::gpk::n3f2_t>		& nodeNormals			= renderer.Normals [nodeToDraw.Normals];
			for(u2_t iTriangle = 0; iTriangle < nodeVertices.size() / 3; ++iTriangle) {
				::gpk::bgra							triangleColor			= (nodeToDraw.VertexColor >= 0) ? renderer.VertexColors[nodeToDraw.VertexColor][iTriangle] : nodeColor;
				;
				triangleTransformed.A = nodeVertices[iTriangle * 3 + 0];
				triangleTransformed.B = nodeVertices[iTriangle * 3 + 1];
				triangleTransformed.C = nodeVertices[iTriangle * 3 + 2];
				::gpk::transform(triangleTransformed, matrixWVP);

				::gpk::m4f2_t						matrixViewport			= {};
				matrixViewport.ViewportLH(target_image.metrics().u1());
				::gpk::transform(triangleTransformed, matrixViewport);

				renderer.RenderCache.PixelCoordBuffer.clear();
				renderer.RenderCache.TriangleWeightBuffer.clear();
				if_true_vef(-1, ::gpk::drawTriangle(target_image.metrics(), triangleTransformed, renderer.RenderCache.PixelCoordBuffer, renderer.RenderCache.TriangleWeightBuffer, target_depth), "%s", "Maybe the pixel caches weren't cleared properly");
				triangleColor							= triangleColor * nodeNormals[iTriangle].Dot(renderer.Lights[0].Direction);
				for(u2_t iPixelCoord = 0; iPixelCoord < renderer.RenderCache.PixelCoordBuffer.size(); ++iPixelCoord) {
					::gpk::n2s1_c				pixelCoord				= renderer.RenderCache.PixelCoordBuffer[iPixelCoord];
					target_image[pixelCoord.y][pixelCoord.x]	= triangleColor;
				}
			}
		}
		else {
		}
	}
	else {
		if(nodeToDraw.VertexColor < 0) {}
		else if(nodeToDraw.PerFaceColor) {}
		else {}
	}
	return 0;
}

::gpk::err_t			gpk::nodeRendererDrawNode
	( ::gpk::SNodeRenderer					& renderer
	, u2_t								iNode
	, const ::gpk::m4f2_t			& view
	, const ::gpk::m4f2_t			& projection
	, const ::gpk::m4f2_t			& viewProjection
	, ::gpk::g8bgra	target_image
	, ::gpk::grid<u2_t>			target_depth
	) {
	if(0 == target_image.size())
		return 1;
	const ::gpk::SRenderNode					& nodeToDraw			= renderer.Nodes[iNode];
	if_true_vwf(2, -1 == nodeToDraw.Vertices, "No vertices to draw for node %i. Better skip this call for invisible nodes.", iNode);

	const ::gpk::m4f2_t				& matrixWorld			= (-1 == nodeToDraw.Transform) ? ::gpk::m4f2_t::GetIdentity() : renderer.Transforms[nodeToDraw.Transform].Matrix;
	const ::gpk::m4f2_t				& matrixWorldInverse	= (-1 == nodeToDraw.Transform) ? ::gpk::m4f2_t::GetIdentity() : renderer.Transforms[nodeToDraw.Transform].MatrixInverse;
	::gpk::m4f2_t						matrixWVP				= matrixWorld * viewProjection;
	if(nodeToDraw.Indices >= 0) { // draw indexed triangles

	}
	else { // draw ordered vertices
		::drawOrderedVertices(renderer, nodeToDraw, matrixWVP, target_image, target_depth);
	}
	(void)nodeToDraw		;
	(void)matrixWorld		;
	(void)matrixWorldInverse;
	(void)view				;
	(void)projection		;
	(void)target_depth		;
	return 0;
}

::gpk::err_t			gpk::nodeRendererDraw
	( ::gpk::SNodeRenderer					& renderer
	, s2_t								iCamera
	, ::gpk::g8bgra	target_image
	, ::gpk::grid<u2_t>			target_depth
	, bool									drawHidden
	) {
	if(0 == target_image.size())
		return 1;
	memset(target_depth.begin(), 0xFFFFFFFFU, sizeof(u2_t) * target_depth.size());
	::gpk::m4f2_t						matrixView				= ::gpk::m4f2_t::GetIdentity();
	::gpk::m4f2_t						matrixProjection		= ::gpk::m4f2_t::GetIdentity();
	::gpk::m4f2_t						matrixVP				= ::gpk::m4f2_t::GetIdentity();
	const double								aspectRatio				= target_image.metrics().x / (double)target_image.metrics().y;
	::gpk::SCamera								& camera				= renderer.Cameras[iCamera];
	matrixView.LookAt(camera.Position, camera.Target, camera.Up);
	matrixProjection.FieldOfView(camera.Angle, aspectRatio, camera.ClipPlanes.Min, camera.ClipPlanes.Max);
	renderer.RenderCache.NodesToRender.clear();
	matrixVP								= matrixView * matrixProjection;

	for(u2_t iNode = 0; iNode < renderer.Nodes.size(); ++iNode) {
		const ::gpk::SRenderNode					& nodeToRender			= renderer.Nodes[iNode];
		if(drawHidden || false == nodeToRender.Hidden) {
			renderer.RenderCache.NodesToRender.push_back(iNode);
			::gpk::au2_t						& nodeLights			= renderer.RenderCache.NodeLights[renderer.RenderCache.NodeLights.push_back({})];
			::gpk::n3f2_t						nodePosition			= (-1 == nodeToRender.Transform) ? ::gpk::n3f2_t{} : renderer.Transforms[nodeToRender.Transform].Matrix.GetTranslation();
			for(u2_t iLight = 0; iLight < renderer.Lights.size(); ++iLight) {
				const ::gpk::SLight							& light					= renderer.Lights[iLight];
				if(light.Type & ::gpk::GLIGHT_STATE_ENABLED) {
					if((light.Position - nodePosition).LengthSquared() < light.RangeSquared) {
						nodeLights.push_back(iLight);
					}
				}
			}
		}
	}
	for(u2_t iNode = 0; iNode < renderer.RenderCache.NodesToRender.size(); ++iNode) {
		const u2_t												indexNode				= renderer.RenderCache.NodesToRender[iNode];
		if_fail_ef(::gpk::nodeRendererDrawNode(renderer, indexNode, matrixView, matrixProjection, matrixVP, target_image, target_depth), "Failed to render node %i", indexNode);
	}
	return 0;
}
