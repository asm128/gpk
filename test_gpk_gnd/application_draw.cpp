#include "gpk_bitmap_target.h"
#include "gpk_ro_rsw.h"
#include "gpk_dialog_controls.h"
#include "gpk_scene.h"
#include "gpk_ro_gnd.h"
#include "gpk_line3.h"


namespace gpk
{
	template<typename _tCoord>
	static	::gpk::err_t	drawTriangle
		( ::gpk::gu2_t					& targetDepth
		, const ::gpk::minmaxf2_t		& fNearFar
		, const ::gpk::tri3<_tCoord>	& triangle
		, ::gpk::apod<::gpk::n2s1_t>		& out_Points
		, ::gpk::apod<::gpk::trif2_t>	& triangleWeights
		) {
		int32_t						pixelsDrawn			= 0;
		const ::gpk::n2u2_t			& _targetMetrics	= targetDepth.metrics();
		::gpk::n2f2_t				areaMin				= {(float)::gpk::min(::gpk::min(triangle.A.x, triangle.B.x), triangle.C.x), (float)::gpk::min(::gpk::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::gpk::n2f2_t				areaMax				= {(float)::gpk::max(::gpk::max(triangle.A.x, triangle.B.x), triangle.C.x), (float)::gpk::max(::gpk::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		const float					xStop				= ::gpk::min(areaMax.x, (float)_targetMetrics.x);
		for(float y = ::gpk::max(areaMin.y, 0.f), yStop = ::gpk::min(areaMax.y, (float)_targetMetrics.y); y < yStop; ++y)
		for(float x = ::gpk::max(areaMin.x, 0.f); x < xStop; ++x) {
			const ::gpk::n2s2_t			cellCurrent			= {(int32_t)x, (int32_t)y};
			const ::gpk::tri2s2_t		triangle2D			=
				{ {(int32_t)triangle.A.x, (int32_t)triangle.A.y}
				, {(int32_t)triangle.B.x, (int32_t)triangle.B.y}
				, {(int32_t)triangle.C.x, (int32_t)triangle.C.y}
				};
			{
				int32_t						w0					= ::gpk::orient2d({triangle2D.B, triangle2D.A}, cellCurrent);	// Determine barycentric coordinates
				int32_t						w1					= ::gpk::orient2d({triangle2D.C, triangle2D.B}, cellCurrent);
				int32_t						w2					= ::gpk::orient2d({triangle2D.A, triangle2D.C}, cellCurrent);
				if(w0 <= -1 || w1 <= -1 || w2 <= -1) // ---- If p is on or inside all edges, render pixel.
					continue;
			}
			const ::gpk::n2<float>		cellCurrentF								= {x, y};
			::gpk::tri<float>			proportions									=
				{ ::gpk::orient2d3d({triangle.C, triangle.B}, cellCurrentF)	// notice how having to type "template" every time before "Cast" totally defeats the purpose of the template. I really find this rule very stupid and there is no situation in which the compiler is unable to resolve it from the code it already has.
				, ::gpk::orient2d3d({triangle.A, triangle.C}, cellCurrentF)
				, ::gpk::orient2d3d({triangle.B, triangle.A}, cellCurrentF)	// Determine barycentric coordinates
				};
			float						proportABC									= proportions.A + proportions.B + proportions.C; //(w0, w1, w2)
			if(proportABC <= 0)
				continue;
			proportions.A					/= proportABC;
			proportions.B					/= proportABC;
			proportions.C					= 1.0f - (proportions.A + proportions.B);
			float						finalZ
				= triangle.A.z * proportions.A
				+ triangle.B.z * proportions.B
				+ triangle.C.z * proportions.C
				;
			float																depth										= float((finalZ - fNearFar.Min) / (fNearFar.Max - fNearFar.Min));
			if(depth >= 1 || depth <= 0) // discard from depth planes
				continue;
			uint32_t															finalDepth									= (uint32_t)(depth * 0x00FFFFFFU);
			if (targetDepth[(uint32_t)y][(uint32_t)x] > (uint32_t)finalDepth) { // check against depth buffer
				targetDepth[(uint32_t)y][(uint32_t)x]							= finalDepth;
				triangleWeights.push_back({proportions.A, proportions.B, proportions.C});
				out_Points.push_back(cellCurrent.Cast<int16_t>());
				++pixelsDrawn;
			}
		}
		return pixelsDrawn;
	}
}
					::gpk::err_t										drawPixelGND
	( ::gpk::SRenderCache		& renderCache
	, ::gpk::bgra				& targetColorCell
	, const ::gpk::trif2_t		& pixelWeights
	, const ::gpk::tri3f2_t		& positions
	, const ::gpk::tri2f2_t		& uvs
	, const ::gpk::g8bgra		& textureColors
	, int32_t					iTriangle
	, const ::gpk::n3f3_t		& lightDir
	, const ::gpk::rgbaf		& diffuseColor
	, const ::gpk::rgbaf		& ambientColor
	, const ::gpk::view<const ::gpk::SLightInfoRSW>	& lights
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::gpk::tri3<float>												& normals									= renderCache.TransformedNormalsVertex[iTriangle];
	::gpk::tri3<double>													weightedNormals								=
		{ normals.A.Cast<double>() * pixelWeights.A
		, normals.B.Cast<double>() * pixelWeights.B
		, normals.C.Cast<double>() * pixelWeights.C
		};
	const ::gpk::n3<double>												interpolatedNormal							= (weightedNormals.A + weightedNormals.B + weightedNormals.C).Normalize();
	::gpk::rgbaf															directionalColor							= diffuseColor * interpolatedNormal.Dot(lightDir);
	const ::gpk::n2<uint32_t>												textureMetrics								= textureColors.metrics();
	const ::gpk::n2<double>												uv											=
		{ uvs.A.x * pixelWeights.A + uvs.B.x * pixelWeights.B + uvs.C.x * pixelWeights.C
		, uvs.A.y * pixelWeights.A + uvs.B.y * pixelWeights.B + uvs.C.y * pixelWeights.C
		};
	const ::gpk::tri3<double>												weightedPositions							=
		{ positions.A.Cast<double>() * pixelWeights.A
		, positions.B.Cast<double>() * pixelWeights.B
		, positions.C.Cast<double>() * pixelWeights.C
		};

	const ::gpk::n3<double>												interpolatedPosition						= weightedPositions.A + weightedPositions.B + weightedPositions.C;
	::gpk::bgra															interpolatedBGRA;
	::gpk::rgbaf															lightColor									= {0, 0, 0, 1}; //((::gpk::RED * pixelWeights.A) + (::gpk::GREEN * pixelWeights.B) + (::gpk::BLUE * pixelWeights.C));
	if( 0 == textureMetrics.x
	 ||	0 == textureMetrics.y
	 ) {
		for(uint32_t iLight = 0; iLight < lights.size(); ++iLight) {
			const ::gpk::SLightInfoRSW													& rswLight									= lights[iLight];
			::gpk::n3f2_t														rswColor									= rswLight.Color * (1.0 - (rswLight.Position.Cast<double>() - interpolatedPosition).Length() / 10.0);
			lightColor																+= ::gpk::rgbaf(rswColor.x, rswColor.y, rswColor.z, 1.0f) / 2.0;
		}
		interpolatedBGRA														= (directionalColor + lightColor + (ambientColor / 2.0)).Clamp();
	}
	else {
		const ::gpk::n2<int32_t>												uvcoords									=
			{ (int32_t)((uint32_t)(uv.x * textureMetrics.x) % textureMetrics.x)
			, (int32_t)((uint32_t)(uv.y * textureMetrics.y) % textureMetrics.y)
			};
		const ::gpk::rgbaf													& srcTexel									= textureColors[uvcoords.y][uvcoords.x];
		if (srcTexel == ::gpk::rgbaf{ ::gpk::bgra{0xFF, 0, 0xFF, 0xFF} })
			return 1;
		for(uint32_t iLight = 0, lightCount = lights.size(); iLight < lightCount; ++iLight) {
			const ::gpk::SLightInfoRSW													& rswLight									= lights[rand() % lights.size()];
			double																		distFactor									= 1.0 - (rswLight.Position.Cast<double>() - interpolatedPosition).Length() / 10.0;
			if(distFactor > 0) {
				::gpk::n3f2_t														rswColor									= rswLight.Color * distFactor;
				lightColor																+= srcTexel * ::gpk::rgbaf(rswColor.x, rswColor.y, rswColor.z, 1.0f) / 2.0;
			}
		}
		interpolatedBGRA														= (srcTexel * directionalColor + lightColor + srcTexel * (ambientColor / 2.0)).Clamp();
	}
	if( targetColorCell == interpolatedBGRA )
		return 1;

	targetColorCell															= interpolatedBGRA;
	return 0;
}

static				::gpk::err_t										transformTriangles
	( const ::gpk::view<::gpk::tri<uint32_t>>	& vertexIndexList
	, const ::gpk::view<::gpk::n3f2_t>				& vertices
	, const ::gpk::minmaxf2_t									& nearFar
	, const ::gpk::m4<float>									& xWorld
	, const ::gpk::m4<float>									& xWV
	, const ::gpk::m4<float>									& xProjection
	, const ::gpk::n2<int32_t>									& targetMetrics
	, ::gpk::SRenderCache											& out_transformed
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(uint32_t iTriangle = 0, triCount = vertexIndexList.size(); iTriangle < triCount; ++iTriangle) {
		const ::gpk::tri<uint32_t>										& vertexIndices								= vertexIndexList[iTriangle];
		::gpk::tri3<float>													triangle3DWorld								=
			{	vertices[vertexIndices.A]
			,	vertices[vertexIndices.B]
			,	vertices[vertexIndices.C]
			};
		::gpk::tri3<float>													transformedTriangle3D						= triangle3DWorld;
		::gpk::transform(transformedTriangle3D, xWV);
		// Check against far and near planes
		if(transformedTriangle3D.CulledZSpecial(nearFar))
			continue;
		float																		oldzA										= transformedTriangle3D.A.z;
		float																		oldzB										= transformedTriangle3D.B.z;
		float																		oldzC										= transformedTriangle3D.C.z;
		::gpk::transform(transformedTriangle3D, xProjection);
		transformedTriangle3D.A.z												= oldzA;
		transformedTriangle3D.B.z												= oldzB;
		transformedTriangle3D.C.z												= oldzC;
		// Check against screen limits
		if(transformedTriangle3D.CulledX({0, (float)targetMetrics.x})) continue;
		if(transformedTriangle3D.CulledY({0, (float)targetMetrics.y})) continue;
		if_fail_fe(out_transformed.Triangle3dToDraw.push_back(transformedTriangle3D));

		::gpk::transform(triangle3DWorld, xWorld);
		if_fail_fe(out_transformed.Triangle3dWorld.push_back(triangle3DWorld));
		if_fail_fe(out_transformed.Triangle3dIndices.push_back(iTriangle));
	}
	return 0;
}

static				::gpk::err_t										transformNormals
	( const ::gpk::view<::gpk::tri<uint32_t>>	& vertexIndexList
	, const ::gpk::view<::gpk::n3f2_t>				& normals
	, const ::gpk::m4<float>									& xWorld
	, ::gpk::SRenderCache											& renderCache
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		for(uint32_t iTriangle = 0, triCount = renderCache.Triangle3dIndices.size(); iTriangle < triCount; ++iTriangle) { // transform normals
			const ::gpk::tri<uint32_t>										& vertexIndices								= vertexIndexList[renderCache.Triangle3dIndices[iTriangle]];
			::gpk::tri3<float>													triangle3DWorldNormals						=
				{ normals[vertexIndices.A]
				, normals[vertexIndices.B]
				, normals[vertexIndices.C]
				};
			::gpk::tri3<float>													& vertNormalsTri							= renderCache.TransformedNormalsVertex[iTriangle];
			vertNormalsTri.A														= xWorld.TransformDirection(normals[vertexIndices.A]).Normalize(); // gndNode.Normals[vertexIndices.A]; //
			vertNormalsTri.B														= xWorld.TransformDirection(normals[vertexIndices.B]).Normalize(); // gndNode.Normals[vertexIndices.B]; //
			vertNormalsTri.C														= xWorld.TransformDirection(normals[vertexIndices.C]).Normalize(); // gndNode.Normals[vertexIndices.C]; //
			//vertNormalsTri.A.y *= -1;
			//vertNormalsTri.B.y *= -1;
			//vertNormalsTri.C.y *= -1;
		}

	return 0;
}

static	::gpk::err_t		drawTriangles
	( const ::gpk::view<::gpk::triu2_t>				& vertexIndexList
	, const ::gpk::view<::gpk::n3f2_t>				& vertices
	, const ::gpk::view<::gpk::n2f2_t>				& uvs
	, const ::gpk::g8bgra							& textureView
	, const ::gpk::minmaxf2_t						& nearFar
	, const ::gpk::n3f2_t							& lightDir
	, ::gpk::SRenderCache							& renderCache
	, ::gpk::gu2_t									& targetDepthView
	, ::gpk::g8bgra									& targetView
	, const ::gpk::rgbaf							& diffuseColor
	, const ::gpk::rgbaf							& ambientColor
	, const ::gpk::view<const ::gpk::SLightInfoRSW>	& lights
	, uint32_t										* pixelsDrawn
	, uint32_t										* pixelsSkipped
	, bool											wireframe
	) {	// ---
		//const ::gpk::n3f2_t			& lightDir									= app.LightDirection;
		for(uint32_t iTriangle = 0, triCount = renderCache.Triangle3dIndices.size(); iTriangle < triCount; ++iTriangle) { //
			renderCache.TrianglePixelCoords.clear();
			renderCache.TrianglePixelWeights.clear();
			const ::gpk::tri3<float>												& tri3DToDraw								= renderCache.Triangle3dToDraw[iTriangle];
			if_true_e(::gpk::failed(::gpk::drawTriangle(targetDepthView, nearFar, tri3DToDraw, renderCache.TrianglePixelCoords, renderCache.TrianglePixelWeights)));
			++renderCache.TrianglesDrawn;
			const ::gpk::tri<uint32_t>										& vertexIndices								= vertexIndexList[renderCache.Triangle3dIndices[iTriangle]];
			const ::gpk::tri3<float>												triangle3DPositions							=
				{ vertices[vertexIndices.A]
				, vertices[vertexIndices.B]
				, vertices[vertexIndices.C]
				};
			const ::gpk::tri2<float>												triangle3DUVs								=
				{ uvs[vertexIndices.A]
				, uvs[vertexIndices.B]
				, uvs[vertexIndices.C]
				};

			for(uint32_t iPixel = 0, pixCount = renderCache.TrianglePixelCoords.size(); iPixel < pixCount; ++iPixel) {
				const ::gpk::n2<int16_t>												& pixelCoord								= renderCache.TrianglePixelCoords	[iPixel];
				const ::gpk::tri<float>												& pixelWeights								= renderCache.TrianglePixelWeights	[iPixel];
				if(false == wireframe) {
					if(0 == ::drawPixelGND(renderCache, targetView[pixelCoord.y][pixelCoord.x], pixelWeights, triangle3DPositions, triangle3DUVs, textureView, iTriangle, lightDir.Cast<double>(), diffuseColor, ambientColor, ::gpk::view<const ::gpk::SLightInfoRSW>{lights.begin(), ::gpk::min(lights.size(), 8U)}))
						++*pixelsDrawn;
					else
						++*pixelsSkipped;
				}
			}
			if(wireframe) {
				if_true_e(::gpk::failed(::gpk::drawLine(targetView.metrics().Cast<uint16_t>(), ::gpk::line3<float>{renderCache.Triangle3dToDraw[iTriangle].A, renderCache.Triangle3dToDraw[iTriangle].B}, renderCache.WireframePixelCoords)));
				if_true_e(::gpk::failed(::gpk::drawLine(targetView.metrics().Cast<uint16_t>(), ::gpk::line3<float>{renderCache.Triangle3dToDraw[iTriangle].B, renderCache.Triangle3dToDraw[iTriangle].C}, renderCache.WireframePixelCoords)));
				if_true_e(::gpk::failed(::gpk::drawLine(targetView.metrics().Cast<uint16_t>(), ::gpk::line3<float>{renderCache.Triangle3dToDraw[iTriangle].C, renderCache.Triangle3dToDraw[iTriangle].A}, renderCache.WireframePixelCoords)));
			}
		}
	return 0;
}

					::gpk::err_t										updateTransforms							(::gpk::SSceneTransforms & transforms, ::gpk::SSceneCamera& camera, ::gpk::n2<uint32_t> targetMetrics)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::m4<float>														& finalProjection							= transforms.FinalProjection	;
	::gpk::m4<float>														& fieldOfView								= transforms.FieldOfView		;
	::gpk::m4<float>														& mviewport									= transforms.Viewport			;
	fieldOfView.FieldOfView(camera.Angle * ::gpk::math_pi, targetMetrics.x / (double)targetMetrics.y, camera.NearFar);
	mviewport.ViewportRH(targetMetrics.Cast<uint16_t>());
	const ::gpk::n2<int32_t>												screenCenter								= {(int32_t)targetMetrics.x / 2, (int32_t)targetMetrics.y / 2};
	finalProjection															= fieldOfView * mviewport;
	transforms.FinalProjectionInverse										= finalProjection.GetInverse();

	::gpk::m4<float>														& viewMatrix								= transforms.View;
	::gpk::SCameraVectors														& cameraVectors								= camera.Vectors;
	cameraVectors.Up														= {0, 1, 0};
	cameraVectors.Front														= (camera.Points.Target - camera.Points.Position).Normalize();
	cameraVectors.Right														= cameraVectors.Up		.Cross(cameraVectors.Front).Normalize();
	cameraVectors.Up														= cameraVectors.Front	.Cross(cameraVectors.Right).Normalize();
	viewMatrix.View3D(camera.Points.Position, cameraVectors.Right, cameraVectors.Up, cameraVectors.Front);
	//viewMatrix.LookAt(camera.Points.Position, {(app.GNDData.Metrics.Size.x / 2.0f), 0, -(app.GNDData.Metrics.Size.y / 2.0f)}, {0, 1, 0});
	return 0;
}

					::gpk::err_t										drawGND
	( ::gpk::SRenderCache							& renderCache
	, ::gpk::SSceneTransforms						& transforms
	, ::gpk::SSceneCamera							& camera
	, ::gpk::rtbgra8d32								& target
	, const ::gpk::SModelPivot<float>				& modelPivot
	, const ::gpk::n3f2_t							& lightDir
	, const ::gpk::SModelGND						& modelGND
	, const ::gpk::SRSWWorldLight					& directionalLight
	, const ::gpk::view<const ::gpk::img8bgra>		& textures
	, const ::gpk::view<const ::gpk::SLightInfoRSW>	& lights
	, bool											wireframe
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::g8bgra											& offscreen									= target.Color.View;
	const ::gpk::n2<uint32_t>												& offscreenMetrics							= offscreen.metrics();
	::updateTransforms(transforms, camera, offscreenMetrics);

	const ::gpk::m4<float>												& projection								= transforms.FinalProjection	;
	const ::gpk::m4<float>												& viewMatrix								= transforms.View				;

	::gpk::m4<float>														xRotation									= {};
	xRotation.Identity();
	::gpk::m4<float>														xWorld										= {};
	xWorld.Identity();
	const ::gpk::minmaxf2_t														& nearFar									= camera.NearFar;
	uint32_t																	& pixelsDrawn								= renderCache.PixelsDrawn	= 0;
	uint32_t																	& pixelsSkipped								= renderCache.PixelsSkipped	= 0;
	renderCache.WireframePixelCoords.clear();
	renderCache.TrianglesDrawn												= 0;
	const ::gpk::rgbaf													ambient										= {directionalLight.Ambient.x, directionalLight.Ambient.y, directionalLight.Ambient.z, 1};
	const ::gpk::rgbaf													diffuse										= {directionalLight.Diffuse.x, directionalLight.Diffuse.y, directionalLight.Diffuse.z, 1};
	xWorld		.Scale			(modelPivot.Scale, true);
	xRotation	.Identity();
	xRotation	.SetOrientation	(modelPivot.Orientation);
	xWorld																	= xWorld * xRotation;
	xWorld		.SetTranslation	(modelPivot.Position, false);
	for(uint32_t iGNDTexture = 0; iGNDTexture < textures.size(); ++iGNDTexture) {
		for(uint32_t iFacingDirection = 0; iFacingDirection < 6; ++iFacingDirection) {
			const ::gpk::g8bgra									& gndNodeTexture							= textures[iGNDTexture].View;
			const ::gpk::SModelNodeGND													& gndNode									= modelGND.Nodes[textures.size() * iFacingDirection + iGNDTexture];
			::gpk::clear
				( renderCache.Triangle3dWorld
				, renderCache.Triangle3dToDraw
				, renderCache.Triangle3dIndices
				);
			const ::gpk::m4<float>												xWV											= xWorld * viewMatrix;
			transformTriangles	(gndNode.VertexIndices, gndNode.Vertices, nearFar, xWorld, xWV, projection, offscreenMetrics.Cast<int32_t>(), renderCache);
			if_fail_fe(renderCache.TransformedNormalsVertex.resize(renderCache.Triangle3dIndices.size()));
			transformNormals	(gndNode.VertexIndices, gndNode.Normals, xWorld, renderCache);
			drawTriangles		(gndNode.VertexIndices, gndNode.Vertices, gndNode.UVs, gndNodeTexture, nearFar, lightDir, renderCache, target.DepthStencil.View, offscreen, diffuse, ambient, lights, &pixelsDrawn, &pixelsSkipped, wireframe);
		}
	}

	static	const ::gpk::bgra												color										= ::gpk::YELLOW;
	for(uint32_t iPixel = 0, pixCount = renderCache.WireframePixelCoords.size(); iPixel < pixCount; ++iPixel) {
		const ::gpk::n2<int16_t>												& pixelCoord								= renderCache.WireframePixelCoords[iPixel];
		if( offscreen[pixelCoord.y][pixelCoord.x] != color ) {
			offscreen[pixelCoord.y][pixelCoord.x]								= color;
			++pixelsDrawn;
		}
		else
			++pixelsSkipped;
	}
	return (::gpk::err_t)pixelsDrawn;
}
