#include "gpk_bitmap_target.h"
#include "gpk_ro_rsw.h"
#include "gpk_dialog_controls.h"
#include "gpk_scene.h"
#include "gpk_ro_gnd.h"

					::gpk::error_t										drawPixelGND									
	( ::gpk::SRenderCache									& renderCache
	, ::gpk::SColorBGRA										& targetColorCell
	, const ::gpk::STriangleWeights<double>					& pixelWeights	
	, const ::gpk::STriangle3D<float>						& positions
	, const ::gpk::STriangle2D<float>						& uvs
	, const ::gpk::view_grid<::gpk::SColorBGRA>				& textureColors
	, int32_t												iTriangle
	, const ::gpk::SCoord3<double>							& lightDir
	, const ::gpk::SColorFloat								& diffuseColor								
	, const ::gpk::SColorFloat								& ambientColor								
	, const ::gpk::view_array<const ::gpk::SLightInfoRSW>	& lights
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SColorFloat															lightColor									= {0, 0, 0, 1}; //((::gpk::RED * pixelWeights.A) + (::gpk::GREEN * pixelWeights.B) + (::gpk::BLUE * pixelWeights.C));
	const ::gpk::STriangle3D<float>												& normals									= renderCache.TransformedNormalsVertex[iTriangle];
	::gpk::STriangle3D<double>													weightedNormals								= 
		{ normals.A.Cast<double>() * pixelWeights.A
		, normals.B.Cast<double>() * pixelWeights.B
		, normals.C.Cast<double>() * pixelWeights.C
		};
	const ::gpk::SCoord3<double>												interpolatedNormal							= (weightedNormals.A + weightedNormals.B + weightedNormals.C).Normalize();
	::gpk::SColorFloat															directionalColor							= diffuseColor * interpolatedNormal.Dot(lightDir);
	const ::gpk::SCoord2<uint32_t>												textureMetrics								= textureColors.metrics();
	::gpk::SCoord2<double>														uv											= 
		{ uvs.A.x * pixelWeights.A + uvs.B.x * pixelWeights.B + uvs.C.x * pixelWeights.C
		, uvs.A.y * pixelWeights.A + uvs.B.y * pixelWeights.B + uvs.C.y * pixelWeights.C
		};
	::gpk::STriangle3D<double>													weightedPositions							= 
		{ positions.A.Cast<double>() * pixelWeights.A//((::gpk::RED * pixelWeights.A) + (::gpk::GREEN * pixelWeights.B) + (::gpk::BLUE * pixelWeights.C));
		, positions.B.Cast<double>() * pixelWeights.B
		, positions.C.Cast<double>() * pixelWeights.C
		};

	const ::gpk::SCoord3<double>												interpolatedPosition						= weightedPositions.A + weightedPositions.B + weightedPositions.C;
	::gpk::SColorBGRA															interpolatedBGRA;
	if( 0 == textureMetrics.x
	 ||	0 == textureMetrics.y
	 ) {
		for(uint32_t iLight = 0; iLight < lights.size(); ++iLight) {
			const ::gpk::SLightInfoRSW													& rswLight									= lights[iLight];
			::gpk::SCoord3<float>														rswColor									= rswLight.Color * (1.0 - (rswLight.Position.Cast<double>() - interpolatedPosition).Length() / 10.0);
			lightColor																+= ::gpk::SColorFloat(rswColor.x, rswColor.y, rswColor.z, 1.0f) / 2.0;
		}
		interpolatedBGRA														= directionalColor + lightColor + ambientColor;
	}
	else {
		const ::gpk::SCoord2<int32_t>												uvcoords									= 
			{ (int32_t)((uint32_t)(uv.x * textureMetrics.x) % textureMetrics.x)
			, (int32_t)((uint32_t)(uv.y * textureMetrics.y) % textureMetrics.y)
			};
		const ::gpk::SColorFloat													& srcTexel									= textureColors[uvcoords.y][uvcoords.x];
		if(srcTexel == ::gpk::SColorBGRA{0xFF, 0, 0xFF, 0xFF}) 
			return 1;
		for(uint32_t iLight = 0, lightCount = lights.size(); iLight < lightCount; ++iLight) {
			const ::gpk::SLightInfoRSW													& rswLight									= lights[rand() % lights.size()];
			double																		distFactor									= 1.0 - (rswLight.Position.Cast<double>() - interpolatedPosition).Length() / 10.0;
			if(distFactor > 0) {
				::gpk::SCoord3<float>														rswColor									= rswLight.Color * distFactor;
				lightColor																+= srcTexel * ::gpk::SColorFloat(rswColor.x, rswColor.y, rswColor.z, 1.0f) / 2.0;
			}
		}
		interpolatedBGRA														= srcTexel * directionalColor + lightColor + srcTexel * ambientColor;
	}
	if( targetColorCell == interpolatedBGRA ) 
		return 1;

	targetColorCell															= interpolatedBGRA;
	return 0;
}

static				::gpk::error_t										transformTriangles							
	( const ::gpk::view_array<::gpk::STriangleWeights<uint32_t>>	& vertexIndexList
	, const ::gpk::view_array<::gpk::SCoord3<float>>				& vertices
	, const ::gpk::SCameraNearFar									& nearFar
	, const ::gpk::SMatrix4<float>									& xWorld
	, const ::gpk::SMatrix4<float>									& xWV
	, const ::gpk::SMatrix4<float>									& xProjection
	, const ::gpk::SCoord2<int32_t>									& targetMetrics
	, ::gpk::SRenderCache											& out_transformed
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	for(uint32_t iTriangle = 0, triCount = vertexIndexList.size(); iTriangle < triCount; ++iTriangle) {
		const ::gpk::STriangleWeights<uint32_t>										& vertexIndices								= vertexIndexList[iTriangle];
		::gpk::STriangle3D<float>													triangle3DWorld								= 
			{	vertices[vertexIndices.A]
			,	vertices[vertexIndices.B]
			,	vertices[vertexIndices.C]
			};
		::gpk::STriangle3D<float>													transformedTriangle3D						= triangle3DWorld;
		::gpk::transform(transformedTriangle3D, xWV);
		// Check against far and near planes
		if(transformedTriangle3D.CulledZSpecial({(float)nearFar.Near, (float)nearFar.Far}))
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
		gpk_necall(out_transformed.Triangle3dToDraw		.push_back(transformedTriangle3D)	, "Out of memory?");
		::gpk::transform(triangle3DWorld, xWorld);
		gpk_necall(out_transformed.Triangle3dWorld		.push_back(triangle3DWorld)			, "Out of memory?");
		gpk_necall(out_transformed.Triangle3dIndices	.push_back(iTriangle)				, "Out of memory?");
	}
	return 0;
}

static				::gpk::error_t										transformNormals
	( const ::gpk::view_array<::gpk::STriangleWeights<uint32_t>>	& vertexIndexList
	, const ::gpk::view_array<::gpk::SCoord3<float>>				& normals
	, const ::gpk::SMatrix4<float>									& xWorld
	, ::gpk::SRenderCache											& renderCache
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		for(uint32_t iTriangle = 0, triCount = renderCache.Triangle3dIndices.size(); iTriangle < triCount; ++iTriangle) { // transform normals
			const ::gpk::STriangleWeights<uint32_t>										& vertexIndices								= vertexIndexList[renderCache.Triangle3dIndices[iTriangle]];
			::gpk::STriangle3D<float>													triangle3DWorldNormals						= 
				{ normals[vertexIndices.A]
				, normals[vertexIndices.B]
				, normals[vertexIndices.C]
				};
			::gpk::STriangle3D<float>													& vertNormalsTri							= renderCache.TransformedNormalsVertex[iTriangle];
			vertNormalsTri.A														= xWorld.TransformDirection(normals[vertexIndices.A]).Normalize(); // gndNode.Normals[vertexIndices.A]; //
			vertNormalsTri.B														= xWorld.TransformDirection(normals[vertexIndices.B]).Normalize(); // gndNode.Normals[vertexIndices.B]; //
			vertNormalsTri.C														= xWorld.TransformDirection(normals[vertexIndices.C]).Normalize(); // gndNode.Normals[vertexIndices.C]; //
			//vertNormalsTri.A.y *= -1;
			//vertNormalsTri.B.y *= -1;
			//vertNormalsTri.C.y *= -1;
		}
	
	return 0;
}

static				::gpk::error_t										drawTriangles
	( const ::gpk::view_array<::gpk::STriangleWeights<uint32_t>>	& vertexIndexList
	, const ::gpk::view_array<::gpk::SCoord3<float>>				& vertices
	, const ::gpk::view_array<::gpk::SCoord2<float>>				& uvs
	, const ::gpk::view_grid	<::gpk::SColorBGRA>					& textureView
	, const ::gpk::SCameraNearFar									& nearFar
	, const ::gpk::SCoord3<float>									& lightDir
	, ::gpk::SRenderCache											& renderCache
	, ::gpk::view_grid<uint32_t>									& targetDepthView
	, ::gpk::view_grid<::gpk::SColorBGRA>							& targetView
	, const ::gpk::SColorFloat										& diffuseColor								
	, const ::gpk::SColorFloat										& ambientColor								
	, const ::gpk::view_array<const ::gpk::SLightInfoRSW>			& lights
	, uint32_t														* pixelsDrawn
	, uint32_t														* pixelsSkipped
	, bool															wireframe
	) {	// --- 
		//const ::gpk::SCoord3<float>													& lightDir									= app.LightDirection;
		for(uint32_t iTriangle = 0, triCount = renderCache.Triangle3dIndices.size(); iTriangle < triCount; ++iTriangle) { // 
			renderCache.TrianglePixelCoords.clear();
			renderCache.TrianglePixelWeights.clear();
			const ::gpk::STriangle3D<float>												& tri3DToDraw								= renderCache.Triangle3dToDraw[iTriangle];
			error_if(errored(::gpk::drawTriangle(targetDepthView, nearFar, tri3DToDraw, renderCache.TrianglePixelCoords, renderCache.TrianglePixelWeights)), "Not sure if these functions could ever fail");
			++renderCache.TrianglesDrawn;
			const ::gpk::STriangleWeights<uint32_t>										& vertexIndices								= vertexIndexList[renderCache.Triangle3dIndices[iTriangle]];
			const ::gpk::STriangle3D<float>												triangle3DPositions							= 
				{ vertices[vertexIndices.A]
				, vertices[vertexIndices.B]
				, vertices[vertexIndices.C]
				};
			const ::gpk::STriangle2D<float>												triangle3DUVs								= 
				{ uvs[vertexIndices.A]
				, uvs[vertexIndices.B]
				, uvs[vertexIndices.C]
				};

			for(uint32_t iPixel = 0, pixCount = renderCache.TrianglePixelCoords.size(); iPixel < pixCount; ++iPixel) {
				const ::gpk::SCoord2<int32_t>												& pixelCoord								= renderCache.TrianglePixelCoords	[iPixel];
				const ::gpk::STriangleWeights<double>										& pixelWeights								= renderCache.TrianglePixelWeights	[iPixel];
				if(false == wireframe) {
					if(0 == ::drawPixelGND(renderCache, targetView[pixelCoord.y][pixelCoord.x], pixelWeights, triangle3DPositions, triangle3DUVs, textureView, iTriangle, lightDir.Cast<double>(), diffuseColor, ambientColor, ::gpk::view_array<const ::gpk::SLightInfoRSW>{lights.begin(), ::gpk::min(lights.size(), 8U)}))
						++*pixelsDrawn;
					else
						++*pixelsSkipped;
				}
			}
			if(wireframe) {
				error_if(errored(::gpk::drawLine(targetView.metrics(), ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].A, renderCache.Triangle3dToDraw[iTriangle].B}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
				error_if(errored(::gpk::drawLine(targetView.metrics(), ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].B, renderCache.Triangle3dToDraw[iTriangle].C}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
				error_if(errored(::gpk::drawLine(targetView.metrics(), ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].C, renderCache.Triangle3dToDraw[iTriangle].A}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
			}
		} 
	return 0;
}

					::gpk::error_t										updateTransforms							(::gpk::SSceneTransforms & transforms, ::gpk::SSceneCamera& camera, ::gpk::SCoord2<uint32_t> targetMetrics)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SMatrix4<float>														& finalProjection							= transforms.FinalProjection	;
	::gpk::SMatrix4<float>														& fieldOfView								= transforms.FieldOfView		;
	::gpk::SMatrix4<float>														& mviewport									= transforms.Viewport			;
	::gpk::SMatrix4<float>														& viewportInverse							= transforms.ViewportInverse	;
	::gpk::SMatrix4<float>														& viewportInverseCentered					= transforms.ViewportInverse	;
	fieldOfView.FieldOfView(camera.Angle * ::gpk::math_pi, targetMetrics.x / (double)targetMetrics.y, camera.NearFar.Near, camera.NearFar.Far);
	mviewport.Viewport(targetMetrics, camera.NearFar.Far, camera.NearFar.Near);
	viewportInverse															= mviewport.GetInverse();
	const ::gpk::SCoord2<int32_t>												screenCenter								= {(int32_t)targetMetrics.x / 2, (int32_t)targetMetrics.y / 2};
	viewportInverseCentered													= viewportInverse;
	viewportInverseCentered._41												+= screenCenter.x;
	viewportInverseCentered._42												+= screenCenter.y;
	finalProjection															= fieldOfView * viewportInverseCentered;
	transforms.FinalProjectionInverse										= finalProjection.GetInverse();

	::gpk::SMatrix4<float>														& viewMatrix								= transforms.View;
	::gpk::SCameraVectors														& cameraVectors								= camera.Vectors;
	cameraVectors.Up														= {0, 1, 0};
	cameraVectors.Front														= (camera.Points.Target - camera.Points.Position).Normalize();
	cameraVectors.Right														= cameraVectors.Up		.Cross(cameraVectors.Front).Normalize();
	cameraVectors.Up														= cameraVectors.Front	.Cross(cameraVectors.Right).Normalize();
	viewMatrix.View3D(camera.Points.Position, cameraVectors.Right, cameraVectors.Up, cameraVectors.Front);
	//viewMatrix.LookAt(camera.Points.Position, {(app.GNDData.Metrics.Size.x / 2.0f), 0, -(app.GNDData.Metrics.Size.y / 2.0f)}, {0, 1, 0});
	return 0;	
}

					::gpk::error_t										drawGND										
	( ::gpk::SRenderCache												& renderCache
	, ::gpk::SSceneTransforms											& transforms
	, ::gpk::SSceneCamera												& camera
	, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>					& target
	, const ::gpk::SModelPivot<float>									& modelPivot
	, const ::gpk::SCoord3<float>										& lightDir
	, const ::gpk::SModelGND											& modelGND
	, const ::gpk::SRSWWorldLight										& directionalLight
	, const ::gpk::view_array<const ::gpk::SImage<::gpk::SColorBGRA>>	& textures
	, const ::gpk::view_array<const ::gpk::SLightInfoRSW>				& lights
	, bool																wireframe
	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::view_grid<::gpk::SColorBGRA>											& offscreen									= target.Color.View;
	const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= offscreen.metrics();
	::updateTransforms(transforms, camera, offscreenMetrics);

	const ::gpk::SMatrix4<float>												& projection								= transforms.FinalProjection	;
	const ::gpk::SMatrix4<float>												& viewMatrix								= transforms.View				;

	::gpk::SMatrix4<float>														xRotation									= {};
	xRotation.Identity();
	::gpk::SMatrix4<float>														xWorld										= {};
	xWorld.Identity();
	const ::gpk::SCameraNearFar													& nearFar									= camera.NearFar;
	uint32_t																	& pixelsDrawn								= renderCache.PixelsDrawn	= 0;
	uint32_t																	& pixelsSkipped								= renderCache.PixelsSkipped	= 0;
	renderCache.WireframePixelCoords.clear();
	renderCache.TrianglesDrawn												= 0;
	const ::gpk::SColorFloat													ambient										= {directionalLight.Ambient.x, directionalLight.Ambient.y, directionalLight.Ambient.z, 1};
	const ::gpk::SColorFloat													diffuse										= {directionalLight.Diffuse.x, directionalLight.Diffuse.y, directionalLight.Diffuse.z, 1};
	xWorld		.Scale			(modelPivot.Scale, true);
	xRotation	.Identity();
	xRotation	.SetOrientation	(modelPivot.Orientation);
	xWorld																	= xWorld * xRotation;
	xWorld		.SetTranslation	(modelPivot.Position, false);
	for(uint32_t iGNDTexture = 0; iGNDTexture < textures.size(); ++iGNDTexture) {
		for(uint32_t iFacingDirection = 0; iFacingDirection < 6; ++iFacingDirection) {
			const ::gpk::view_grid<::gpk::SColorBGRA>									& gndNodeTexture							= textures[iGNDTexture].View;
			const ::gpk::SModelNodeGND													& gndNode									= modelGND.Nodes[textures.size() * iFacingDirection + iGNDTexture];
			::gpk::clear
				( renderCache.Triangle3dWorld
				, renderCache.Triangle3dToDraw		
				, renderCache.Triangle3dIndices		
				);
			const ::gpk::SMatrix4<float>												xWV											= xWorld * viewMatrix;
			transformTriangles	(gndNode.VertexIndices, gndNode.Vertices, nearFar, xWorld, xWV, projection, offscreenMetrics.Cast<int32_t>(), renderCache);
			gpk_necall(renderCache.TransformedNormalsVertex.resize(renderCache.Triangle3dIndices.size()), "Out of memory?");
			transformNormals	(gndNode.VertexIndices, gndNode.Normals, xWorld, renderCache);
			drawTriangles		(gndNode.VertexIndices, gndNode.Vertices, gndNode.UVs, gndNodeTexture, nearFar, lightDir, renderCache, target.DepthStencil.View, offscreen, diffuse, ambient, lights, &pixelsDrawn, &pixelsSkipped, wireframe);
		}
	}

	static constexpr const ::gpk::SColorBGRA									color										= ::gpk::YELLOW;
	for(uint32_t iPixel = 0, pixCount = renderCache.WireframePixelCoords.size(); iPixel < pixCount; ++iPixel) {
		const ::gpk::SCoord2<int32_t>												& pixelCoord								= renderCache.WireframePixelCoords[iPixel];
		if( offscreen[pixelCoord.y][pixelCoord.x] != color ) {
			offscreen[pixelCoord.y][pixelCoord.x]								= color;
			++pixelsDrawn;
		}
		else
			++pixelsSkipped;
	}
	return (::gpk::error_t)pixelsDrawn;
}
