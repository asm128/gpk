#include "application.h"
#include "gpk_bitmap_target.h"
#include "gpk_ro_rsw.h"
#include "gpk_dialog_controls.h"

					::gpk::error_t										drawPixelGND									
	( ::SRenderCache										& renderCache
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
	 //||	0 != textureMetrics.y
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
		//interpolatedBGRA														= finalColor + ambientColor;
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
	, double														fFar
	, double														fNear
	, const ::gpk::SMatrix4<float>									& xWorld
	, const ::gpk::SMatrix4<float>									& xWV
	, const ::gpk::SMatrix4<float>									& xProjection
	, const ::gpk::SCoord2<int32_t>									& targetMetrics
	, ::SRenderCache												& out_transformed
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
		if( transformedTriangle3D.A.z >= fFar
		 && transformedTriangle3D.B.z >= fFar	
		 && transformedTriangle3D.C.z >= fFar) 
			continue;
		if( (transformedTriangle3D.A.z <= fNear)
		 || (transformedTriangle3D.B.z <= fNear)
		 || (transformedTriangle3D.C.z <= fNear)
		 ) 
		 continue;

		float																		oldzA										= transformedTriangle3D.A.z;
		float																		oldzB										= transformedTriangle3D.B.z;
		float																		oldzC										= transformedTriangle3D.C.z;

		::gpk::transform(transformedTriangle3D, xProjection);
		transformedTriangle3D.A.z												= oldzA;
		transformedTriangle3D.B.z												= oldzB;
		transformedTriangle3D.C.z												= oldzC;
		// Check against screen limits
		if(transformedTriangle3D.A.x < 0 && transformedTriangle3D.B.x < 0 && transformedTriangle3D.C.x < 0) continue;
		if(transformedTriangle3D.A.y < 0 && transformedTriangle3D.B.y < 0 && transformedTriangle3D.C.y < 0) continue;
		if( transformedTriangle3D.A.x >= targetMetrics.x
		 && transformedTriangle3D.B.x >= targetMetrics.x
		 && transformedTriangle3D.C.x >= targetMetrics.x
		 )
			continue;
		if( transformedTriangle3D.A.y >= targetMetrics.y
		 && transformedTriangle3D.B.y >= targetMetrics.y
		 && transformedTriangle3D.C.y >= targetMetrics.y
		 )
			continue;
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
	, ::SRenderCache												& renderCache
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
	, double														fFar
	, double														fNear
	, const ::gpk::SCoord3<float>									& lightDir
	, ::SRenderCache												& renderCache
	, ::gpk::view_grid<uint32_t>									& targetDepthView
	, ::gpk::view_grid<::gpk::SColorBGRA>							& targetView
	, const ::gpk::SColorFloat										& diffuseColor								
	, const ::gpk::SColorFloat										& ambientColor								
	, const ::gpk::view_array<::gpk::SLightInfoRSW>					& lights
	, uint32_t														* pixelsDrawn
	, uint32_t														* pixelsSkipped
	, bool															wireframe
	) {	// --- 
		//const ::gpk::SCoord3<float>													& lightDir									= app.LightDirection;
		for(uint32_t iTriangle = 0, triCount = renderCache.Triangle3dIndices.size(); iTriangle < triCount; ++iTriangle) { // 
			renderCache.TrianglePixelCoords.clear();
			renderCache.TrianglePixelWeights.clear();
			const ::gpk::STriangle3D<float>												& tri3DToDraw								= renderCache.Triangle3dToDraw[iTriangle];
			error_if(errored(::gpk::drawTriangle(targetDepthView, {fNear, fFar}, tri3DToDraw, renderCache.TrianglePixelCoords, renderCache.TrianglePixelWeights)), "Not sure if these functions could ever fail");
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

			//if(vertexIndices.C == -1) {
				for(uint32_t iPixel = 0, pixCount = renderCache.TrianglePixelCoords.size(); iPixel < pixCount; ++iPixel) {
					const ::gpk::SCoord2<int32_t>												& pixelCoord								= renderCache.TrianglePixelCoords	[iPixel];
					const ::gpk::STriangleWeights<double>										& pixelWeights								= renderCache.TrianglePixelWeights	[iPixel];
					if(false == wireframe) {
						if(0 == ::drawPixelGND(renderCache, targetView[pixelCoord.y][pixelCoord.x], pixelWeights, triangle3DPositions, triangle3DUVs, textureView, iTriangle, lightDir.Cast<double>(), diffuseColor, ambientColor, ::gpk::view_array<const ::gpk::SLightInfoRSW>{lights.begin(), 8U}))
							++*pixelsDrawn;
						else
							++*pixelsSkipped;
					}
				}
			//}
			if(wireframe) {
				error_if(errored(::gpk::drawLine(targetView.metrics(), ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].A, renderCache.Triangle3dToDraw[iTriangle].B}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
				error_if(errored(::gpk::drawLine(targetView.metrics(), ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].B, renderCache.Triangle3dToDraw[iTriangle].C}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
				error_if(errored(::gpk::drawLine(targetView.metrics(), ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].C, renderCache.Triangle3dToDraw[iTriangle].A}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
			}
		} 
	return 0;
}

					::gpk::error_t										drawGND										(::gme::SApplication& app, ::gpk::SRenderTarget<::gpk::SFramework::TTexel, uint32_t> & target, bool wireframe)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework::TOffscreen												& offscreen									= target.Color;
	::gpk::SImage<uint32_t>														& offscreenDepth							= target.DepthStencil;
	const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= offscreen.View.metrics();
	::memset(offscreenDepth	.Texels.begin(), -1, sizeof(uint32_t)								* offscreenDepth	.Texels.size());	// Clear target.
	::memset(offscreen		.Texels.begin(), 0, sizeof(::gpk::SFramework::TOffscreen::TTexel)	* offscreen			.Texels.size());	// Clear target.
	for(uint32_t y = 0, yMax = offscreenMetrics.y; y < yMax; ++y) {
		const uint8_t																colorHeight									= (uint8_t)(y / 10);
		for(uint32_t x = 0, xMax = offscreenMetrics.x; x < xMax; ++x)
			offscreen.View[y][x]													= {colorHeight, 0, 0, 0xFF};
	}

	{
		::gpk::SMatrix4<float>														& finalProjection							= app.Scene.Transforms.FinalProjection	;
		::gpk::SMatrix4<float>														& fieldOfView								= app.Scene.Transforms.FieldOfView		;
		::gpk::SMatrix4<float>														& mviewport									= app.Scene.Transforms.Viewport			;
		::gpk::SMatrix4<float>														& viewportInverse							= app.Scene.Transforms.ViewportInverse	;
		::gpk::SMatrix4<float>														& viewportInverseCentered					= app.Scene.Transforms.ViewportInverse	;
		fieldOfView.FieldOfView(app.Scene.Camera.Angle * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, app.Scene.Camera.NearFar.Near, app.Scene.Camera.NearFar.Far);
		mviewport.Viewport(offscreenMetrics, app.Scene.Camera.NearFar.Far, app.Scene.Camera.NearFar.Near);
		viewportInverse															= mviewport.GetInverse();
		const ::gpk::SCoord2<int32_t>												screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
		viewportInverseCentered													= viewportInverse;
		viewportInverseCentered._41												+= screenCenter.x;
		viewportInverseCentered._42												+= screenCenter.y;
		finalProjection															= fieldOfView * viewportInverseCentered;
		app.Scene.Transforms.FinalProjectionInverse								= finalProjection.GetInverse();

		::gpk::SMatrix4<float>														& viewMatrix								= app.Scene.Transforms.View;
		const ::gpk::SCameraPoints													& camera									= app.Scene.Camera.Points;
		::gpk::SCameraVectors														& cameraVectors								= app.Scene.Camera.Vectors;
		cameraVectors.Up														= {0, 1, 0};
		cameraVectors.Front														= (camera.Target - camera.Position).Normalize();
		cameraVectors.Right														= cameraVectors.Up		.Cross(cameraVectors.Front).Normalize();
		cameraVectors.Up														= cameraVectors.Front	.Cross(cameraVectors.Right).Normalize();
		//viewMatrix.View3D(camera.Position, cameraVectors.Right, cameraVectors.Up, cameraVectors.Front);
		viewMatrix.LookAt(camera.Position, {(app.GNDData.Metrics.Size.x / 2.0f), 0, -(app.GNDData.Metrics.Size.y / 2.0f)}, {0, 1, 0});
	}

	::SRenderCache																& renderCache								= app.RenderCache;

	const ::gpk::SMatrix4<float>												& projection								= app.Scene.Transforms.FinalProjection	;
	const ::gpk::SMatrix4<float>												& viewMatrix								= app.Scene.Transforms.View				;

	//::gpk::SMatrix4<float>														xViewProjection								= viewMatrix * projection;
	//::gpk::SMatrix4<float>														xRotation									= {};
	//xRotation.Identity();
	::gpk::SMatrix4<float>														xWorld										= {};
	xWorld.Identity();
	const double																& fFar										= app.Scene.Camera.NearFar.Far	;
	const double																& fNear										= app.Scene.Camera.NearFar.Near	;
	uint32_t																	& pixelsDrawn								= app.RenderCache.PixelsDrawn	= 0;
	uint32_t																	& pixelsSkipped								= app.RenderCache.PixelsSkipped	= 0;
	renderCache.WireframePixelCoords.clear();
	renderCache.TrianglesDrawn												= 0;
	const ::gpk::SCoord2<int32_t>												offscreenMetricsI							= offscreenMetrics.Cast<int32_t>();
	const ::gpk::SCoord3<float>													screenCenter								= {offscreenMetricsI.x / 2.0f, offscreenMetricsI.y / 2.0f, };
	const ::gpk::SColorFloat													ambient										= 
		{ app.RSWData.Light.Ambient.x
		, app.RSWData.Light.Ambient.y
		, app.RSWData.Light.Ambient.z
		, 1
		};
	const ::gpk::SColorFloat													diffuse										= 
		{ app.RSWData.Light.Diffuse.x
		, app.RSWData.Light.Diffuse.y
		, app.RSWData.Light.Diffuse.z
		, 1
		};
	::gpk::STimer																timerMark									= {};
	for(uint32_t iGNDTexture = 0; iGNDTexture < app.GNDData.TextureNames.size(); ++iGNDTexture) {
		for(uint32_t iFacingDirection = 0; iFacingDirection < 6; ++iFacingDirection) {
			const ::gpk::view_grid<::gpk::SColorBGRA>									& gndNodeTexture							= app.TexturesGND	[iGNDTexture].View;
			const ::gpk::SModelNodeGND													& gndNode									= app.GNDModel.Nodes[app.GNDData.TextureNames.size() * iFacingDirection + iGNDTexture];
			xWorld		.Scale			(app.GridPivot.Scale, true);
			//xRotation	.SetOrientation	(app.GridPivot.Orientation.Normalize());
			//xWorld																	= xWorld * xRotation;
			xWorld		.SetTranslation	(app.GridPivot.Position, false);
			::gpk::clear
				( renderCache.Triangle3dWorld
				, renderCache.Triangle3dToDraw		
				, renderCache.Triangle3dIndices		
				);
			const ::gpk::SMatrix4<float>												xWV											= xWorld * viewMatrix;
			transformTriangles(gndNode.VertexIndices, gndNode.Vertices, fFar, fNear, xWorld, xWV, projection, offscreenMetricsI, renderCache);
			//timerMark.Frame(); info_printf("First iteration: %f.", timerMark.LastTimeSeconds);
			gpk_necall(renderCache.TransformedNormalsVertex.resize(renderCache.Triangle3dIndices.size()), "Out of memory?");
			transformNormals(gndNode.VertexIndices, gndNode.Normals, xWorld, renderCache);
			//timerMark.Frame(); info_printf("Second iteration: %f.", timerMark.LastTimeSeconds);
			const ::gpk::SCoord3<float>													& lightDir									= app.LightDirection;
			drawTriangles(gndNode.VertexIndices, gndNode.Vertices, gndNode.UVs, gndNodeTexture, fFar, fNear, lightDir, renderCache, offscreenDepth.View, offscreen.View, diffuse, ambient, app.RSWData.RSWLights, &pixelsDrawn, &pixelsSkipped, wireframe);
			//timerMark.Frame(); info_printf("Third iteration: %f.", timerMark.LastTimeSeconds);
		}
	}

	static constexpr const ::gpk::SColorBGRA									color										= ::gpk::YELLOW;
	for(uint32_t iPixel = 0, pixCount = renderCache.WireframePixelCoords.size(); iPixel < pixCount; ++iPixel) {
		const ::gpk::SCoord2<int32_t>												& pixelCoord								= renderCache.WireframePixelCoords[iPixel];
		if( offscreen.View[pixelCoord.y][pixelCoord.x] != color ) {
			offscreen.View[pixelCoord.y][pixelCoord.x]								= color;
			++pixelsDrawn;
		}
		else
			++pixelsSkipped;
	}
	return (::gpk::error_t)pixelsDrawn;
}
