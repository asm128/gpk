// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "gpk_bitmap_target.h"
#include "gpk_bitmap_file.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"

#include "gpk_app_impl.h"

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "Title");

					::SApplication										* g_ApplicationInstance						= 0;

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& applicationInstance)											{
	const ::gpk::SCoord2<uint32_t>												newSize										= applicationInstance.Framework.MainDisplay.Size;
	::gpk::updateSizeDependentTarget(applicationInstance.Framework.MainDisplayOffscreen->Color, newSize);
	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& applicationInstance)											{
	::gpk::SDisplayPlatformDetail												& displayDetail								= applicationInstance.Framework.MainDisplay.PlatformDetail;
	::gpk::mainWindowDestroy(applicationInstance.Framework.MainDisplay);
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	g_ApplicationInstance													= 0;
	return 0;
}

// Vertex coordinates for cube faces
static constexpr const ::gpk::STriangle3D<float>						geometryCube	[12]						=
	{ {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}	// Right	- first			?? I have no idea if this is correct lol
	, {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Right	- second		?? I have no idea if this is correct lol

	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}	// Back		- first			?? I have no idea if this is correct lol
	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}	// Back		- second		?? I have no idea if this is correct lol

	, {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}	// Bottom	- first			?? I have no idea if this is correct lol
	, {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Bottom	- second		?? I have no idea if this is correct lol

	, {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Left		- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}	// Left		- second

	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}	// Front	- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Front	- second

	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}	// Top		- first
	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}}	// Top		- second
	};

					::gpk::error_t										setup										(::SApplication& applicationInstance)											{
	g_ApplicationInstance													= &applicationInstance;
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::SDisplay																& mainWindow								= framework.MainDisplay;
	framework.Input.create();
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	static constexpr	const char												bmpFileName2	[]							= "Codepage-437-24.bmp";
	error_if(errored(::gpk::bmpFileLoad((::gpk::view_const_string)bmpFileName2, applicationInstance.TextureFont				)), "Failed to load bitmap from file: %s.", bmpFileName2);
	const ::gpk::SCoord2<uint32_t>												& textureFontMetrics						= applicationInstance.TextureFont.View.metrics();
	applicationInstance.TextureFontMonochrome.resize(textureFontMetrics);
	for(uint32_t y = 0, yMax = textureFontMetrics.y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = textureFontMetrics.x; x < xMax; ++x)
		applicationInstance.TextureFontMonochrome.View[y * textureFontMetrics.x + x]
		=	0 != applicationInstance.TextureFont.View[y][x].r
		||	0 != applicationInstance.TextureFont.View[y][x].g
		||	0 != applicationInstance.TextureFont.View[y][x].b
		;

	static constexpr const ::gpk::SCoord3<float>								cubeCenter									= {0.5f, 0.5f, 0.5f};
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		::gpk::STriangle3D<float>													& transformedTriangle						= applicationInstance.CubePositions[iTriangle];
		transformedTriangle														= geometryCube[iTriangle];
		transformedTriangle.A													-= cubeCenter;
		transformedTriangle.B													-= cubeCenter;
		transformedTriangle.C													-= cubeCenter;
	}
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	return 0;
}

					::gpk::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::gpk::error_t																frameworkResult								= ::gpk::updateFramework(applicationInstance.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	//-----------------------------
	::gpk::STimer																& timer										= applicationInstance.Framework.Timer;
	::gpk::SDisplay																& mainWindow								= applicationInstance.Framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

static constexpr const ::gpk::SCoord3<float>						geometryCubeNormals	[12]						=
	{ {0.0f, 0.0f, -1.0f}	// Right	- first			?? I have no idea if this is correct lol
	, {0.0f, 0.0f, -1.0f}	// Right	- second		?? I have no idea if this is correct lol

	, {-1.0f, 0.0f, 0.0f}	// Back		- first			?? I have no idea if this is correct lol
	, {-1.0f, 0.0f, 0.0f}	// Back		- second		?? I have no idea if this is correct lol

	, {0.0f, -1.0f, 0.0f}	// Bottom	- first			?? I have no idea if this is correct lol
	, {0.0f, -1.0f, 0.0f}	// Bottom	- second		?? I have no idea if this is correct lol

	, {0.0f, 0.0f, 1.0f}	// Left		- first
	, {0.0f, 0.0f, 1.0f}	// Left		- second

	, {1.0f, 0.0f, 0.0f}	// Front	- first
	, {1.0f, 0.0f, 0.0f}	// Front	- second

	, {0.0f, 1.0f, 0.0f}	// Top		- first
	, {0.0f, 1.0f, 0.0f}	// Top		- second
	};

struct SCamera {
						::gpk::SCoord3<float>								Position, Target;
};


					::gpk::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= applicationInstance.Framework;
	::gpk::SFramework::TOffscreen												& offscreen									= framework.MainDisplayOffscreen->Color;

	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			backBuffer;
	backBuffer->resize(offscreen.metrics(), 0xFF000080, (uint32_t)-1);

	//------------------------------------------------
	::gpk::array_pod<::gpk::STriangle3D<float>>									triangle3dList								= {};
	::gpk::array_pod<::gpk::SColorBGRA>											triangle3dColorList							= {};
	triangle3dList.resize(12);
	triangle3dColorList.resize(12);
	::gpk::SMatrix4<float>														projection									= {};
	::gpk::SMatrix4<float>														viewMatrix									= {};
	projection.Identity();
	::gpk::SFrameInfo															& frameInfo									= framework.FrameInfo;
	const ::gpk::SCoord3<float>													tilt										= {10, };	// ? cam't remember what is this. Radians? Eulers?
	const ::gpk::SCoord3<float>													rotation									= {0, (float)frameInfo.FrameNumber / 100, 0};

	::gpk::SCameraNearFar														nearFar										= {0.01f , 1000.0f};

	static constexpr const ::gpk::SCoord3<float>								cameraUp									= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
	::SCamera																	camera										= {{10, 5, 0}, {}};
	::gpk::SCoord3<float>														lightPos									= {10, 5, 0};
	static float																cameraRotation								= 0;
	cameraRotation															+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	camera.Position	.RotateY(frameInfo.Microseconds.Total / 1000000.0f);
	lightPos		.RotateY(frameInfo.Microseconds.Total /  500000.0f * -2);
	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= offscreen.View.metrics();
	projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far );
	projection																= viewMatrix * projection;
	lightPos.Normalize();

	::gpk::SMatrix4<float>														viewport									= {};
	viewport._11															= 2.0f / offscreenMetrics.x;
	viewport._22															= 2.0f / offscreenMetrics.y;
	viewport._33															= 1.0f / (float)(nearFar.Far - nearFar.Near);
	viewport._43															= (float)(-nearFar.Near * ( 1.0f / (nearFar.Far - nearFar.Near) ));
	viewport._44															= 1.0f;
	projection																= projection * viewport.GetInverse();
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		::gpk::STriangle3D<float>													& transformedTriangle						= triangle3dList[iTriangle];
		transformedTriangle														= applicationInstance.CubePositions[iTriangle];
		::gpk::transform(transformedTriangle, projection);
	}
	::gpk::array_pod<::gpk::STriangle2D<int32_t>>								triangle2dList								= {};
	triangle2dList.resize(12);
	const ::gpk::SCoord2<int32_t>												screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) { // Maybe the scale
		::gpk::STriangle3D<float>													& transformedTriangle3D						= triangle3dList[iTriangle];
		::gpk::STriangle2D<int32_t>													& transformedTriangle2D						= triangle2dList[iTriangle];
		transformedTriangle2D.A													= {(int32_t)transformedTriangle3D.A.x, (int32_t)transformedTriangle3D.A.y};
		transformedTriangle2D.B													= {(int32_t)transformedTriangle3D.B.x, (int32_t)transformedTriangle3D.B.y};
		transformedTriangle2D.C													= {(int32_t)transformedTriangle3D.C.x, (int32_t)transformedTriangle3D.C.y};
		::gpk::translate(transformedTriangle2D, screenCenter);
	}

	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																		lightFactor									= geometryCubeNormals[iTriangle].Dot(lightPos);
		triangle3dColorList[iTriangle]											= ::gpk::RED * lightFactor;
	}
	::gpk::array_pod<::gpk::SCoord2<int32_t>>										trianglePixelCoords;
	::gpk::array_pod<::gpk::SCoord2<int32_t>>										wireframePixelCoords;
	::gpk::SCoord3<float> cameraFront = (camera.Target - camera.Position).Normalize();
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																		lightFactor									= geometryCubeNormals[iTriangle].Dot(cameraFront);
		if(lightFactor > 0)
			continue;
		error_if(errored(::gpk::drawTriangle(backBuffer->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])), "Not sure if these functions could ever fail");
		//::gpk::drawLine(offscreen.View, (::gpk::SColorBGRA)::gpk::GREEN, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B});
		//::gpk::drawLine(offscreen.View, (::gpk::SColorBGRA)::gpk::GREEN, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C});
		//::gpk::drawLine(offscreen.View, (::gpk::SColorBGRA)::gpk::GREEN, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A});
		//trianglePixelCoords.clear();
		//error_if(errored(::gpk::drawTriangle(offscreenMetrics, triangle2dList[iTriangle], trianglePixelCoords)), "Not sure if these functions could ever fail");
		//for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord)
		//	::gpk::drawPixelLight(offscreen.View, trianglePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::BLUE, 0.05f, 2.5);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	}
	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::GREEN, 0.05f, 1.5);

	::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	//framework.MainDisplayOffscreen = backBuffer;
	//------------------------------------------------
	return 0;																																									
}
	
