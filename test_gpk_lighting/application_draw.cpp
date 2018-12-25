#include "application.h"
#include "gpk_bitmap_target.h"
#include "gpk_dialog_controls.h"

					::gpk::error_t										drawPixel									
	( ::SRenderCache															& renderCache
	, ::gpk::SColorBGRA															& targetColorCell
	, const ::gpk::STriangleWeights<double>										& pixelWeights	
	, const ::gpk::STriangle2D<float>											& uvGrid
	, const ::gpk::view_grid<::gpk::SColorBGRA>									& textureColors
	, int32_t																	iTriangle
	, const ::gpk::SCoord3<double>												& lightDir	

	) {	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::gpk::SColorBGRA														ambientColor								= {0xF, 0xF, 0xF, 0xFF};	//(((::gpk::DARKRED * pixelWeights.A) + (::gpk::DARKGREEN * pixelWeights.B) + (::gpk::DARKBLUE * pixelWeights.C))) * .1;
	const ::gpk::SColorFloat													interpolatedColor							= {1, 1, 1, 1}; //((::gpk::RED * pixelWeights.A) + (::gpk::GREEN * pixelWeights.B) + (::gpk::BLUE * pixelWeights.C));
	::gpk::STriangle3D<float>													weightedNormals								= renderCache.TransformedNormalsVertex[iTriangle]; //((::gpk::RED * pixelWeights.A) + (::gpk::GREEN * pixelWeights.B) + (::gpk::BLUE * pixelWeights.C));
	weightedNormals.A														*= pixelWeights.A;
	weightedNormals.B														*= pixelWeights.B;
	weightedNormals.C														*= pixelWeights.C;
	const ::gpk::SCoord3<double>												interpolatedNormal							= (weightedNormals.A + weightedNormals.B + weightedNormals.C).Cast<double>().Normalize();
	const ::gpk::SColorFloat													finalColor									= interpolatedColor * interpolatedNormal.Dot(lightDir);
	const ::gpk::SCoord2<uint32_t>												textureMetrics								= textureColors.metrics();
	
	::gpk::SCoord2<double>														uv											= 
		{ uvGrid.A.x * pixelWeights.A + uvGrid.B.x * pixelWeights.B + uvGrid.C.x * pixelWeights.C
		, uvGrid.A.y * pixelWeights.A + uvGrid.B.y * pixelWeights.B + uvGrid.C.y * pixelWeights.C
		};
	::gpk::SColorBGRA															interpolatedBGRA;
	if( 0 == textureMetrics.x
	 ||	0 == textureMetrics.y
	 ) 
		interpolatedBGRA													= finalColor + ambientColor;
	else {
		const ::gpk::SCoord2<int32_t>														uvcoords									= 
			{ (int32_t)((uint32_t)(uv.x * textureMetrics.x) % textureMetrics.x)
			, (int32_t)((uint32_t)(uv.y * textureMetrics.y) % textureMetrics.y)
			};
		const ::gpk::SColorBGRA														& srcTexel									= textureColors[uvcoords.y][uvcoords.x];
		if(srcTexel == ::gpk::SColorBGRA{0xFF, 0, 0xFF, 0xFF}) {
			return 1;
		}
		//interpolatedBGRA														= finalColor + ambientColor;
		interpolatedBGRA														= ::gpk::SColorFloat(srcTexel) * finalColor + ambientColor;
	}
	if( targetColorCell == interpolatedBGRA ) 
		return 1;

	targetColorCell															= interpolatedBGRA;
	return 0;
}

					::gpk::error_t										drawGrids									(::gme::SApplication& applicationInstance, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>& target)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework::TOffscreen												& offscreen									= target.Color;
	::gpk::SImage<uint32_t>														& offscreenDepth							= target.DepthStencil;
	const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= offscreen.View.metrics();
	::SRenderCache																& renderCache								= applicationInstance.RenderCache;
	const ::gpk::SMatrix4<float>												& projection								= applicationInstance.Scene.Transforms.FinalProjection	;
	const ::gpk::SMatrix4<float>												& viewMatrix								= applicationInstance.Scene.Transforms.View				;

	::gpk::SMatrix4<float>														xViewProjection								= viewMatrix * projection;
	::gpk::SMatrix4<float>														xWorld										= {};
	::gpk::SMatrix4<float>														xRotation									= {};
	const double																& fFar										= applicationInstance.Scene.Camera.NearFar.Far	;
	const double																& fNear										= applicationInstance.Scene.Camera.NearFar.Near	;
	int32_t																		& pixelsDrawn								= applicationInstance.RenderCache.PixelsDrawn	= 0;
	int32_t																		& pixelsSkipped								= applicationInstance.RenderCache.PixelsSkipped	= 0;
	renderCache.WireframePixelCoords.clear();
	renderCache.TrianglesDrawn												= 0;
	const ::gpk::SCoord2<int32_t>												offscreenMetricsI							= offscreenMetrics.Cast<int32_t>();
	const ::gpk::SCoord3<float>													screenCenter								= {offscreenMetricsI.x / 2.0f, offscreenMetricsI.y / 2.0f, };
	for(uint32_t iGrid = 0; iGrid < 1; ++iGrid) {
		xWorld		.Scale			(applicationInstance.GridPivot.Scale, true);
		//xRotation	.SetOrientation	((applicationInstance.GridPivot.Orientation + ::gpk::SQuaternion<float>{0, (float)(iGrid / ::gpk::math_2pi), 0, 0}).Normalize());
		xRotation	.SetOrientation	(applicationInstance.GridPivot.Orientation.Normalize());
		xWorld																	= xWorld * xRotation;
		xWorld		.SetTranslation	(applicationInstance.GridPivot.Position, false);
		::gpk::clear
			( renderCache.Triangle3dWorld
			, renderCache.Triangle3dToDraw		
			, renderCache.Triangle3dIndices		
			);
		const ::gpk::SMatrix4<float>												xWV											= xWorld * viewMatrix;
		//const ::gpk::SCoord3<float>													& cameraFront								= applicationInstance.Scene.Camera.Vectors.Front;
		//const ::gpk::SMatrix4<float>												finalTransform								= xWorld * xViewProjection;
		for(uint32_t iTriangle = 0, triCount = applicationInstance.Grid.Positions.size(); iTriangle < triCount; ++iTriangle) {
			::gpk::STriangle3D<float>													triangle3DWorld								= applicationInstance.Grid.Positions[iTriangle];
			::gpk::STriangle3D<float>													transformedTriangle3D						= triangle3DWorld;
			::gpk::transform(transformedTriangle3D, xWV);
			if( transformedTriangle3D.A.z >= fFar
			 && transformedTriangle3D.B.z >= fFar	
			 && transformedTriangle3D.C.z >= fFar) 
				continue;
			if( (transformedTriangle3D.A.z <= fNear) // && transformedTriangle3D.B.z <= (fNear + .01) )  	
			 || (transformedTriangle3D.B.z <= fNear) // && transformedTriangle3D.C.z <= (fNear + .01) )  	
			 || (transformedTriangle3D.C.z <= fNear) // && transformedTriangle3D.A.z <= (fNear + .01) )  	
			 ) 
				continue;

			float																		oldzA										= transformedTriangle3D.A.z;
			float																		oldzB										= transformedTriangle3D.B.z;
			float																		oldzC										= transformedTriangle3D.C.z;
			::gpk::transform(transformedTriangle3D, projection);
			transformedTriangle3D.A.z												= oldzA;
			transformedTriangle3D.B.z												= oldzB;
			transformedTriangle3D.C.z												= oldzC;
			//::gpk::translate(transformedTriangle3D, screenCenter);
			// Check against far and near planes
			// Check against screen limits
			if(transformedTriangle3D.A.x < 0 && transformedTriangle3D.B.x < 0 && transformedTriangle3D.C.x < 0) continue;
			if(transformedTriangle3D.A.y < 0 && transformedTriangle3D.B.y < 0 && transformedTriangle3D.C.y < 0) continue;
			if( transformedTriangle3D.A.x >= offscreenMetricsI.x
			 && transformedTriangle3D.B.x >= offscreenMetricsI.x
			 && transformedTriangle3D.C.x >= offscreenMetricsI.x
			 )
				continue;
			if( transformedTriangle3D.A.y >= offscreenMetricsI.y
			 && transformedTriangle3D.B.y >= offscreenMetricsI.y
			 && transformedTriangle3D.C.y >= offscreenMetricsI.y
			 )
				continue;
			gpk_necall(renderCache.Triangle3dToDraw		.push_back(transformedTriangle3D)	, "Out of memory?");
			::gpk::transform(triangle3DWorld, xWorld);
			gpk_necall(renderCache.Triangle3dWorld		.push_back(triangle3DWorld)			, "Out of memory?");
			gpk_necall(renderCache.Triangle3dIndices	.push_back(iTriangle)				, "Out of memory?");
		}
		gpk_necall(::gpk::resize(renderCache.Triangle3dIndices.size()
			, renderCache.TransformedNormalsTriangle
			, renderCache.TransformedNormalsVertex		
			, renderCache.Triangle3dColorList			
			), "Out of memory?");

		const ::gpk::SCoord3<float>													& lightDir									= applicationInstance.LightDirection;
		for(uint32_t iTriangle = 0, triCount = renderCache.Triangle3dIndices.size(); iTriangle < triCount; ++iTriangle) { // transform normals
			::gpk::SCoord3<float>														& transformedNormalTri						= renderCache.TransformedNormalsTriangle[iTriangle];
			transformedNormalTri													= xWorld.TransformDirection(applicationInstance.Grid.NormalsTriangle[renderCache.Triangle3dIndices[iTriangle]]).Normalize();
			const double																lightFactor									= fabs(transformedNormalTri.Dot(lightDir));
			renderCache.Triangle3dColorList[iTriangle]								= ::gpk::LIGHTGRAY * lightFactor;
			const ::gpk::STriangle3D<float>												& vertNormalsTriOrig						= applicationInstance.Grid.NormalsVertex[renderCache.Triangle3dIndices[iTriangle]];
			::gpk::STriangle3D<float>													& vertNormalsTri							= renderCache.TransformedNormalsVertex[iTriangle];
			vertNormalsTri.A														= xWorld.TransformDirection(vertNormalsTriOrig.A).Normalize();
			vertNormalsTri.B														= xWorld.TransformDirection(vertNormalsTriOrig.B).Normalize();
			vertNormalsTri.C														= xWorld.TransformDirection(vertNormalsTriOrig.C).Normalize();
		}

		for(uint32_t iTriangle = 0, triCount = renderCache.Triangle3dIndices.size(); iTriangle < triCount; ++iTriangle) { // 
			const double																cameraFactor								= renderCache.TransformedNormalsTriangle[iTriangle].Dot(applicationInstance.Scene.Camera.Vectors.Front);
			if(cameraFactor > .65)
				continue;
			renderCache.TrianglePixelCoords.clear();
			renderCache.TrianglePixelWeights.clear();
			offscreenDepth;
			::gpk::ptr_obj<::gpk::SDialogCheckBox>			checkbox;
			applicationInstance.DialogMain.Controls[applicationInstance.CheckBox].as(checkbox);
			if(false == checkbox->Checked)
				error_if(errored(::gpk::drawTriangle(offscreenDepth.View, {fNear, fFar}, renderCache.Triangle3dToDraw[iTriangle], renderCache.TrianglePixelCoords, renderCache.TrianglePixelWeights)), "Not sure if these functions could ever fail");
			//if(0 != renderCache.TrianglePixelCoords.size())
			++renderCache.TrianglesDrawn;
			const int32_t																iGridTri										= renderCache.Triangle3dIndices[iTriangle];
			const ::gpk::STriangle2D<float>												& uvGrid										= applicationInstance.Grid.UVs[iGridTri];
			for(uint32_t iPixel = 0, pixCount = renderCache.TrianglePixelCoords.size(); iPixel < pixCount; ++iPixel) {
				const ::gpk::SCoord2<int32_t>												& pixelCoord								= renderCache.TrianglePixelCoords	[iPixel];
				const ::gpk::STriangleWeights<double>										& pixelWeights								= renderCache.TrianglePixelWeights	[iPixel];
				if(0 == ::drawPixel(renderCache, offscreen.View[pixelCoord.y][pixelCoord.x], pixelWeights, uvGrid, applicationInstance.TextureGrid.View, iTriangle, lightDir.Cast<double>()))
					++pixelsDrawn;
				else
					++pixelsSkipped;
			}
			if(checkbox->Checked) {
				error_if(errored(::gpk::drawLine(offscreenMetrics, ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].A, renderCache.Triangle3dToDraw[iTriangle].B}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
				error_if(errored(::gpk::drawLine(offscreenMetrics, ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].B, renderCache.Triangle3dToDraw[iTriangle].C}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
				error_if(errored(::gpk::drawLine(offscreenMetrics, ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].C, renderCache.Triangle3dToDraw[iTriangle].A}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
			}
			// draw normals
			//error_if(errored(::gpk::drawLine(offscreenMetrics, ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].A, renderCache.Triangle3dToDraw[iTriangle].A + (renderCache.TransformedNormalsTriangle[iTriangle])}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
			//error_if(errored(::gpk::drawLine(offscreenMetrics, ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].B, renderCache.Triangle3dToDraw[iTriangle].B + (renderCache.TransformedNormalsTriangle[iTriangle])}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
			//error_if(errored(::gpk::drawLine(offscreenMetrics, ::gpk::SLine3D<float>{renderCache.Triangle3dToDraw[iTriangle].C, renderCache.Triangle3dToDraw[iTriangle].C + (renderCache.TransformedNormalsTriangle[iTriangle])}, renderCache.WireframePixelCoords)), "Not sure if these functions could ever fail");
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
	return pixelsDrawn;
}

