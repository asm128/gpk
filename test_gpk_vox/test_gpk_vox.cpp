// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "test_gpk_vox.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"
#include "gpk_view_color.h"

#include "gpk_app_impl.h"
#include "gpk_raster_lh.h"
#include "gpk_chrono.h"

#include <GL\Gl.h>

stxp	uint32_t			ASCII_SCREEN_WIDTH		= 132	;
stxp	uint32_t			ASCII_SCREEN_HEIGHT		= 50	;

GPK_DEFINE_APPLICATION_ENTRY_POINT_MT(::SApplication, "Title");

// --- Cleanup application resources.
::gpk::err_t			cleanup		(::SApplication& app)											{
	// --- when the rendering context is no longer needed ...   
 	// make the rendering context not current  
	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	return 0;
}

::gpk::err_t			setup		(::SApplication & app)											{
	::gpk::SFramework				& framework									= app.Framework;
	::gpk::SWindow					& mainWindow								= framework.RootWindow;
	mainWindow.Size														= {1280, 720};
	if_true_e(::gpk::failed(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));

	framework.RootWindow.BackBuffer->resize(mainWindow.Size);

	::gpk::apod<uint8_t>			fileBytes;
	::gpk::astatic<char, 256>		pathNameData		= {"../gpk_data"};

	::gpk::astatic<char, 256>		folderNameVox		= {"vox"};

	uint64_t							timeStart			= ::gpk::timeCurrentInMs();
	::gpk::apobj<::gpk::SVOXData>	voxModels									= {};
	for(uint32_t iModel = 0; iModel < ::gpk::size(fileNames); ++iModel) {
		gpk::vcsc_t							fileName			= fileNames[iModel];
		char								pathToLoad[4096]	= {};
		sprintf_s(pathToLoad, "%s/%s/%s", pathNameData.Storage, folderNameVox.Storage, fileName.begin());
		do { if_fail_fe(::gpk::fileToMemory(pathToLoad, fileBytes)); } while(0);

		::gpk::vcu0_t							viewBytes			= fileBytes;
		do { if_fail_fe(voxModels[voxModels.push_back({})]->Load(viewBytes)); } while(0);
		app.VOXModelNames.push_back(fileName);
		gpk_vox_info_printf("Loaded %s.", pathToLoad);
	}

	app.VOXModelMaps.resize(voxModels.size());
	for(uint32_t iModel = 0; iModel < voxModels.size(); ++iModel) {
		gpk_vox_info_printf("Constructing %s.", app.VOXModelNames[iModel].begin());

		const ::gpk::pobj<::gpk::SVOXData>		& voxFile			= voxModels[iModel];
		::gpk::SVoxelMap<uint8_t>					& chunkMap			= app.VOXModelMaps[iModel];
		auto										coord				= voxFile->GetDimensions();
		chunkMap.Palette						= voxFile->GetBGRA();
		chunkMap.Dimensions						= voxFile->GetDimensions();
		::gpk::view<const ::gpk::SVOXVoxel>	voxels				= voxFile->GetXYZI();
		for(uint32_t iVoxel = 0; iVoxel < voxels.size(); ++iVoxel) {
			const ::gpk::SVOXVoxel						voxel				= voxels[iVoxel];
			chunkMap.SetValue({voxel.x, voxel.y, voxel.z}, voxel.i);
		}
	}

	app.VOXModels.resize(app.VOXModelMaps.size());
	for(uint32_t iModel = 0; iModel < app.VOXModelMaps.size(); ++iModel) 
		::gpk::geometryVoxelModel(app.VOXModels[iModel], app.VOXModelMaps[iModel]);

	uint64_t							timeStop			= ::gpk::timeCurrentInMs();
	warning_printf("Total time: %llu ms", timeStop - timeStart);

	return 0;
}

static	::gpk::err_t	processKeyboardEvent	(::SApplication & app, const ::gpk::SEventView<::gpk::EVENT_KEYBOARD> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_KEYBOARD_Down:
	case ::gpk::EVENT_KEYBOARD_SysDown:
		switch(screenEvent.Data[0]) {
		case VK_RETURN:
			if(GetAsyncKeyState(VK_MENU) & 0xFFF0)
				do { if_fail_fe(::gpk::fullScreenToggle(app.Framework.RootWindow)); } while(0);
			break;
		}
		break;
	}
	return 0;
}

static	::gpk::err_t	processSystemEvent	(::SApplication & app, const ::gpk::SEventSystem & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Keyboard	: if_true_e(::gpk::failed(::gpk::eventExtractAndHandle<::gpk::EVENT_KEYBOARD>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_KEYBOARD> & screenEvent) { return ::processKeyboardEvent(app, screenEvent); }))); break;
	}
	return 0;
}

::gpk::err_t			update		(::SApplication& app, bool systemRequestedExit)					{
	if_true_vif(1, systemRequestedExit, "%s", "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::gpk::SWindow					& mainWindow								= app.Framework.RootWindow;
	do { if_fail_fe(mainWindow.EventQueue.for_each([&app](const ::gpk::pobj<::gpk::SEventSystem> & sysEvent) { return ::processSystemEvent(app, *sysEvent); })); } while(0);

	::gpk::err_t																frameworkResult								= {};
	if_fail_fe(frameworkResult = ::gpk::updateFramework(app.Framework));
	if_true_vif(1, frameworkResult == 1, "%s", "Framework requested close. Terminating execution.");
	//-----------------------------
	::gpk::STimer																& timer										= app.Framework.Timer;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	::gpk::sleep(1);


	return 0;
}
namespace gpk
{
	template<typename _tCoord>
	static					::gpk::err_t									drawTriangle
		( ::gpk::gu2_t	& targetDepth
		, const ::gpk::minmaxf2_t		& fNearFar
		, const ::gpk::tri3<_tCoord>	& triangle
		, ::gpk::apod<::gpk::n2s1_t>		& out_Points
		) {
		int32_t																		pixelsDrawn									= 0;
		const ::gpk::n2<uint32_t>							& _targetMetrics							= targetDepth.metrics();
		::gpk::n2	<float>									areaMin										= {(float)::gpk::min(::gpk::min(triangle.A.x, triangle.B.x), triangle.C.x), (float)::gpk::min(::gpk::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::gpk::n2	<float>									areaMax										= {(float)::gpk::max(::gpk::max(triangle.A.x, triangle.B.x), triangle.C.x), (float)::gpk::max(::gpk::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		const float																	xStop										= ::gpk::min(areaMax.x, (float)_targetMetrics.x);
		for(float y = ::gpk::max(areaMin.y, 0.f), yStop = ::gpk::min(areaMax.y, (float)_targetMetrics.y); y < yStop; ++y)
		for(float x = ::gpk::max(areaMin.x, 0.f); x < xStop; ++x) {
			const ::gpk::n2<int32_t>							cellCurrent									= {(int32_t)x, (int32_t)y};
			const ::gpk::tri2<int32_t>						triangle2D									=
				{ {(int32_t)triangle.A.x, (int32_t)triangle.A.y}
				, {(int32_t)triangle.B.x, (int32_t)triangle.B.y}
				, {(int32_t)triangle.C.x, (int32_t)triangle.C.y}
				};
			{
				int32_t																		w0											= ::gpk::orient2d({triangle2D.B, triangle2D.C}, cellCurrent);	// Determine barycentric coordinates
				int32_t																		w1											= ::gpk::orient2d({triangle2D.C, triangle2D.A}, cellCurrent);
				int32_t																		w2											= ::gpk::orient2d({triangle2D.A, triangle2D.B}, cellCurrent);
				if(w0 <= -1 || w1 <= -1 || w2 <= -1) // ---- If p is on or inside all edges, render pixel.
					continue;
			}
			const ::gpk::n2f3_t												cellCurrentF								= {x, y};
			::gpk::tri<double>							proportions									=
				{ ::gpk::orient2d3d({triangle.C.f3(), triangle.B.f3()}, cellCurrentF)	// notice how having to type "template" every time before "Cast" totally defeats the purpose of the template. I really find this rule very stupid and there is no situation in which the compiler is unable to resolve it from the code it already has.
				, ::gpk::orient2d3d({triangle.A.f3(), triangle.C.f3()}, cellCurrentF)
				, ::gpk::orient2d3d({triangle.B.f3(), triangle.A.f3()}, cellCurrentF)	// Determine barycentric coordinates
				};
			double																		proportABC									= proportions.A + proportions.B + proportions.C; //(w0, w1, w2)
			if(proportABC == 0)
				continue;
			proportions.A															/= proportABC;
			proportions.B															/= proportABC;
			proportions.C															= 1.0 - (proportions.A + proportions.B);
			double																		finalZ
				= triangle.A.z * proportions.A
				+ triangle.B.z * proportions.B
				+ triangle.C.z * proportions.C
				;
			double																		depth										= ((finalZ - fNearFar.Min) / (fNearFar.Max - fNearFar.Min));
			if(depth >= 1 || depth <= 0) // discard from depth planes
				continue;
			uint32_t																	finalDepth									= (uint32_t)(depth * 0x00FFFFFFU);
			if (targetDepth[(uint32_t)y][(uint32_t)x] > (uint32_t)finalDepth) { // check against depth buffer
				targetDepth[(uint32_t)y][(uint32_t)x]									= finalDepth;
				out_Points.push_back(cellCurrent.Cast<int16_t>());
				++pixelsDrawn;
			}
		}
		return pixelsDrawn;
	}

	template<typename _tCoord, typename _tIndex>
	static	inline			::gpk::err_t									drawTriangleIndexed
		( ::gpk::gu2_t							& targetDepth
		, const ::gpk::minmaxf2_t				& fNearFar // fFar
		//, double								fNear
		, uint32_t								baseIndex
		, uint32_t								baseVertexIndex
		, ::gpk::view<const ::gpk::n3<_tCoord>>	coordList
		, ::gpk::view<const _tIndex>			indices
		, ::gpk::apod<::gpk::n2s1_t>				& out_Points
		) {
		return drawTriangle(targetDepth, fNearFar, ::gpk::tri3<_tCoord>{coordList[baseVertexIndex + indices[baseIndex + 0]], coordList[baseVertexIndex + indices[baseIndex + 1]], coordList[baseVertexIndex + indices[baseIndex + 2]]}, out_Points);
	}
}

static	::gpk::err_t	drawVoxelFaceGeometry
	( ::gpk::gu2_t							targetDepth
	, ::gpk::apod<::gpk::n2s1_t>				& Points
	, const ::gpk::n3f2_t					& voxelPos
	, const ::gpk::m4f2_t					& mWVP
	, const ::gpk::minmaxf2_t					& nearFar
	, const ::gpk::view<const ::gpk::n3f2_t>	verticesRaw
	, const ::gpk::vcu0_t						& indices
	) {
	::gpk::n3f2_t									vertices [4]				= {}; 
	for(uint32_t iVertex = 0; iVertex < 4; ++iVertex) {
		vertices[iVertex] = mWVP.Transform(verticesRaw[iVertex] + voxelPos); 
	}
	::gpk::drawTriangleIndexed<float, uint8_t>(targetDepth, nearFar, 0, 0, vertices, indices, Points); 
	::gpk::drawTriangleIndexed<float, uint8_t>(targetDepth, nearFar, 3, 0, vertices, indices, Points); 
	return 0;
}

struct SFragmentCache {
	::gpk::apod<::gpk::n2s1_t>	Points				[6]	= {};
	::gpk::apod<::gpk::trif2_t>	TriangleWeights		[6]	= {};	
	::gpk::g8bgra				TargetPixels;
	::gpk::gu2_t					TargetDepth	;
};

static	::gpk::err_t	drawVoxelFace
	( uint32_t					iFace
	, const ::gpk::n3f2_t		& voxelPos
	, const ::gpk::n3f2_t		& voxelCenter
	, const ::gpk::rgbaf		& cellColor
	, const ::gpk::m4f2_t		& mVP
	, const ::gpk::minmaxf2_t		& nearFar
	, const ::gpk::n3f2_t		& lightPosition
	, const double				lightFactorDistance
	, const ::gpk::rgbaf		& colorAmbient
	, ::SFragmentCache			& pixelCache
	) {
	pixelCache.Points			[iFace].clear();
	pixelCache.TriangleWeights	[iFace].clear();

	::gpk::view<const ::gpk::n3f2_t>	rawVertices				= {&::gpk::VOXEL_FACE_VERTICES[iFace].A, 4};
	::gpk::vcu0_t						rawIndices				= ::gpk::VOXEL_FACE_INDICES[iFace];

	::drawVoxelFaceGeometry(pixelCache.TargetDepth, pixelCache.Points[iFace], voxelPos, mVP, nearFar, rawVertices, rawIndices); 

	::gpk::rgbaf							faceColor	[6]				= {};
	faceColor[iFace]						= colorAmbient * lightFactorDistance;
	if(lightFactorDistance > 0) {
		const double							lightFactorDirectional		= ::gpk::max(0.0, (lightPosition - voxelCenter).Normalize().Dot(::gpk::VOXEL_NORMALS[iFace]));	
		::gpk::rgbaf						colorDiffuse				= cellColor * lightFactorDirectional;
		(faceColor[iFace] += colorDiffuse * lightFactorDistance).Clamp(); 
	}
	for(uint32_t iPoint = 0; iPoint < pixelCache.Points[iFace].size(); ++iPoint) {
		::gpk::n2s1_t					point					= pixelCache.Points[iFace][iPoint];
		::gpk::rgbaf						finalColor				= faceColor[iFace];
		pixelCache.TargetPixels[pixelCache.TargetPixels.metrics().y - 1 - point.y][point.x]	= finalColor;
	}

	return 0;
}

static	::gpk::err_t	drawVoxelModel						
	( const ::gpk::SVoxelGeometry	& voxelGeometry
	, const ::gpk::n3f2_t			& position
	, const ::gpk::m4f2_t			& mVP
	, const ::gpk::n3f2_t			& lightPosition
	, ::SFragmentCache				& pixelCache
	) {	
	for(uint32_t iFace = 0; iFace < 6; ++iFace) {
		::gpk::view<const ::gpk::SGeometryGroup>	faceSlices	= {(const ::gpk::SGeometryGroup*)voxelGeometry.GeometrySlices[iFace].begin(), voxelGeometry.GeometrySlices[iFace].size()};
		for(uint32_t iSlice = 0, countSlices = faceSlices.size(); iSlice < countSlices; ++iSlice) {
			// Clear out output
			::gpk::apod<::gpk::n2s1_t>		& facePixelCoords		= pixelCache.Points[iFace];
			::gpk::apod<::gpk::trif2_t>		& faceTriangleWeights	= pixelCache.TriangleWeights[iFace];
			::gpk::apod<::gpk::tri3f2_t>		trianglePositions		= {};
			::gpk::apod<::gpk::rangeu2_t>	triangleSlices			= {};
			::gpk::apod<uint32_t>			triangleIndices			= {};
			facePixelCoords		.clear();
			faceTriangleWeights	.clear();

			// Raster triangles
			const ::gpk::SGeometryGroup				& faceSlice			= faceSlices[iSlice];
			const ::gpk::SRenderMaterialPaletted	& sliceMaterial		= voxelGeometry.Materials[faceSlice.Material];
			for(uint32_t iIndex = 0, countIndices = faceSlice.Slice.Count; iIndex < countIndices; ++iIndex) {
				uint32_t						offsetPositionIndex		= voxelGeometry.Geometry.PositionIndices[iIndex];
				const ::gpk::tri3f2_t			triangle				= 
					{ mVP.Transform((voxelGeometry.Geometry.Positions[voxelGeometry.Geometry.PositionIndices[offsetPositionIndex + 0]] + position))
					, mVP.Transform((voxelGeometry.Geometry.Positions[voxelGeometry.Geometry.PositionIndices[offsetPositionIndex + 1]] + position))
					, mVP.Transform((voxelGeometry.Geometry.Positions[voxelGeometry.Geometry.PositionIndices[offsetPositionIndex + 2]] + position))
					};
				trianglePositions.push_back(triangle);
				::gpk::rangeu2_t					triangleSlice			= {facePixelCoords.size()};
				::gpk::drawTriangle(pixelCache.TargetDepth.metrics(), triangle, facePixelCoords, faceTriangleWeights, pixelCache.TargetDepth); 
				triangleSlice.Count			= facePixelCoords.size() - triangleSlice.Offset;
				if(triangleSlice.Count)
					triangleSlices.push_back(triangleSlice);
			}

			const double					lightFactorAmbient			= .075;
			const ::gpk::rgbaf				colorAmbient				= sliceMaterial.Diffuse * lightFactorAmbient;
			
			for(uint32_t iTriangle = 0; iTriangle < triangleSlices.size(); ++iTriangle) {
				const ::gpk::rangeu2_t			slice						= triangleSlices[iTriangle];
				// Process pixel fragments
				for(uint32_t iPoint = slice.Offset, pixelCount = slice.Count + slice.Offset; iPoint < pixelCount; ++iPoint) {
					const ::gpk::tri3f2_t			& triangle					= trianglePositions[iTriangle];
					const ::gpk::trif2_t				& triangleWeights			= faceTriangleWeights[iPoint];
					const ::gpk::n2s1_t				pixelPosIn2DSpace			= facePixelCoords[iPoint];
					const ::gpk::n3f2_t				pixelPosIn3DSpace			
						= triangle.A * triangleWeights.A
						+ triangle.B * triangleWeights.B
						+ triangle.C * triangleWeights.C
						;

					::gpk::n3f2_t					lightDistance				= lightPosition - pixelPosIn3DSpace;
					const double					lightFactorDistance			= ::gpk::clamped(1.0 - lightDistance.Length() * .001, 0.0, 1.0);
					::gpk::rgbaf					faceColor					= colorAmbient * lightFactorDistance;
					if(lightFactorDistance > 0 && lightFactorDistance < 0) {
						const double					lightFactorDirectional		= ::gpk::max(0.0, lightDistance.Normalize().Dot(::gpk::VOXEL_NORMALS[iFace]));	
						::gpk::rgbaf					colorDiffuse				= sliceMaterial.Diffuse * lightFactorDirectional;
						(faceColor += colorDiffuse * lightFactorDistance).Clamp(); 
					}

					::gpk::n2s1_t					point						= pixelCache.Points[iFace][iPoint];
					::gpk::rgbaf					finalColor					= faceColor;
					pixelCache.TargetPixels[pixelCache.TargetPixels.metrics().y - 1 - point.y][point.x]	= finalColor;
				}
			}
		}
	}
	return 0;
}
static	::gpk::err_t	drawVoxelModel						
	( const ::gpk::SVoxelMap<uint8_t>	& voxelMap	
	, const ::gpk::n3f2_t				& position
	, const ::gpk::m4f2_t				& mVP
	, const ::gpk::minmaxf2_t				& nearFar
	, const ::gpk::n3f2_t				& cameraPos
	, const ::gpk::n3f2_t				& cameraFront
	, const ::gpk::n3f2_t				& lightPosition
	, ::SFragmentCache					& pixelCache
	) {	
	const ::gpk::view<const ::gpk::SVoxel<uint8_t>>	voxels	= voxelMap.Voxels;
	::gpk::vc8bgra				rgba			= voxelMap.Palette;
	const ::gpk::n3u0_t			dimensions		= voxelMap.Dimensions;
	if(0 == rgba.size())
		rgba					= ::gpk::VOX_PALETTE_DEFAULT;

	::gpk::n3f2_t				vertices	[8]	= {};
	bool						culled			= true;
	for(uint32_t i = 0; i < 8; ++i) {
		vertices[i]				= ::gpk::VOXEL_VERTICES[i];
		vertices[i].Scale(dimensions.Cast<float>());
		vertices[i].x			+= cameraPos.x;
		vertices[i].z			+= cameraPos.z;
		const double				cameraDot		= (vertices[i] - cameraPos).Normalize().Dot(cameraFront);
		if(cameraDot > 0.01) {
			culled					= false;
			break;
		}
	}

	if(culled)
		return 0;

	for(uint32_t iVoxel = 0; iVoxel < voxels.size(); ++iVoxel) {
		const ::gpk::SVoxel<uint8_t>voxel			= voxels[iVoxel];
		const ::gpk::n3f2_t			voxelPos		= position + voxel.Position.Cast<float>();
		uint8_t						cellValue		= voxel.ColorIndex;
		uint8_t						cellValues	[6]	= {};
		//voxelMap.GetValue({voxel.Position.x, voxel.Position.y, voxel.Position.z}, cellValue);
		for(uint32_t iFace = 0; iFace < 6; ++iFace) {
			if(false == voxel.Limits(::gpk::VOXEL_FACE(iFace))) 
				voxelMap.GetValue((voxel.Position.Cast<int32_t>() + ::gpk::VOXEL_DELTAS[iFace].Cast<int32_t>()).Cast<uint8_t>(), cellValues[iFace]);
		}

		if(0 == cellValue)
			continue;

		if((voxelPos - cameraPos).Normalize().Dot(cameraFront) <= 0.5)
			continue;

		const ::gpk::rgbaf			cellColor		= {rgba[voxel.ColorIndex ? voxel.ColorIndex - 1 : 0]};
		bool						renderFaces	[6]	= {};
		bool						hasFace			= false;
		for(uint32_t iFace = 0; iFace < 6; ++iFace) {
			::gpk::bgra					color			= rgba[cellValues[iFace] ? cellValues[iFace] - 1 : 0];
			if(0 == cellValues[iFace] || color.a < 0xFF)
				hasFace = (renderFaces[iFace] = cameraFront.Dot(::gpk::VOXEL_NORMALS[iFace]) <= 0.35) || hasFace;
		}

		if(false == hasFace)
			continue;

		::gpk::n3f2_t				voxelCenter			= voxelPos + ::gpk::n3f2_t{.5f, .5f, .5f};
		const double				lightFactorDistance	= ::gpk::clamped(1.0 - (lightPosition - voxelCenter).Length() * .001, 0.0, 1.0);
		const double				lightFactorAmbient	= .075;
		const ::gpk::rgbaf			colorAmbient		= cellColor * lightFactorAmbient;

		for(uint32_t iFace = 0; iFace < 6; ++iFace) {
			if(false == renderFaces[iFace]) 
				continue;

			drawVoxelFace(iFace, voxelPos, voxelCenter, cellColor, mVP, nearFar, lightPosition, lightFactorDistance, colorAmbient, pixelCache);
		}
	}
	return 0;
}

struct SCamera {
	::gpk::n3f2_t			Position, Target;
};

::gpk::err_t			draw		(::SApplication& app)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework				& framework			= app.Framework;

	::gpk::pobj<::gpk::rtbgra8d32>	backBuffer		= app.BackBuffer;
	if(backBuffer.get_ref() && backBuffer.get_ref()->References > 2)
		return 0;

	backBuffer->resize(framework.RootWindow.Size);
	backBuffer->Color.View.fill(0x3060C0);
	memset(backBuffer->DepthStencil.begin(), (uint32_t)-1, backBuffer->DepthStencil.View.byte_count());

	::gpk::m4f2_t					projection									= {};
	::gpk::m4f2_t					viewMatrix									= {};
	projection.Identity();
	::gpk::SFrameInfo				& frameInfo									= framework.FrameInfo;
	const ::gpk::n3f2_t				tilt										= {10, };	// ? cam't remember what is this. Radians? Eulers?
	const ::gpk::n3f2_t				rotation									= {0, (float)frameInfo.FrameMeter.FrameNumber / 100, 0};

	::gpk::minmaxf2_t				nearFar										= {0.1f , 1000.0f};

	stxp const ::gpk::n3f2_t		cameraUp									= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
	::SCamera						camera										= {{100, 50, 0}, {25, 0, 25}};
	//camera.Position *= 2.0f;
	::gpk::n3f2_t					lightPos									= {150, 50, 0};
	static float					cameraRotation								= 0;
	cameraRotation				+= (float)framework.RootWindow.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	camera.Position	.RotateY(frameInfo.Seconds.Total * 0.1f);
	camera.Position.y *= (float)fabs(sin(frameInfo.Seconds.Total * .1f));
	//camera.Position	.RotateX(frameInfo.Microseconds.Total / 10000000.0f);
	lightPos		.RotateY(frameInfo.Seconds.Total * 1.f);

	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	const ::gpk::n2u2_t				& offscreenMetrics							= backBuffer->metrics();
	projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar);
	projection					= viewMatrix * projection;
	lightPos.x					+= 100;
	lightPos.y					*= (float)fabs(sin(frameInfo.Seconds.Total * .1f));

	const ::gpk::n2u1_t				screenCenter				= {(uint16_t)(offscreenMetrics.x / 2), (uint16_t)(offscreenMetrics.y / 2)};
	::gpk::m4f2_t					viewport									= {};
	viewport.ViewportLH(offscreenMetrics.Cast<uint16_t>());
	projection					= projection * viewport;

	::gpk::n3f2_t					cameraFront					= (camera.Target - camera.Position).Normalize();

	int32_t							zOffset						= 0;
	int32_t							xOffset						= 0;

	::SFragmentCache				pixelCache;
	pixelCache.TargetPixels		= backBuffer->Color;
	pixelCache.TargetDepth		= backBuffer->DepthStencil;
	bool							drawFromSource				= true;
	for(uint32_t iModel = 0; iModel < app.VOXModelMaps.size(); ++iModel) {
		const ::gpk::SVoxelMap<uint8_t>	& voxelMap					= app.VOXModelMaps[iModel];
		const ::gpk::n3f2_t				position					= {(float)xOffset, 0.0f, (float)zOffset};
		if(drawFromSource) 
			::drawVoxelModel(voxelMap, position, projection, nearFar, camera.Position, cameraFront, lightPos, pixelCache);
		else {
			const ::gpk::SVoxelGeometry		& voxelModel					= app.VOXModels[iModel];
			::drawVoxelModel(voxelModel, position, projection, lightPos, pixelCache);
		}

		if(zOffset < 300) 
			zOffset						+= voxelMap.Dimensions.z + 4;
		else {
			xOffset						+= ::gpk::max(uint16_t(voxelMap.Dimensions.x), uint16_t(30));
			zOffset						= 0;
		}
	}

	//::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	::std::swap(framework.RootWindow.BackBuffer, app.BackBuffer);
	//------------------------------------------------
	return 0;
}

