#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_dialog_controls.h"
#include "gpk_matrix.h"
#include "gpk_bitmap_target.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

// Vertex coordinates for cube faces
static constexpr	const ::gpk::STriangle3D<float>					geometryCube	[12]						=
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

					::gpk::error_t									cleanup										(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
					::gpk::error_t									setup										(::gme::SApplication & app)						{
	app.Buffer3D.create();
	::gpk::SFramework														& framework									= app.Framework;
	::gpk::SDisplay															& mainWindow								= framework.MainDisplay;
	app.Framework.GUI													= app.DialogMain.GUI;
	app.DialogMain.Input												= framework.Input;
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?????!?!?!?!?");
	::gpk::SGUI																& gui										= *framework.GUI;
	gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
	app.IdExit															= ::gpk::controlCreate(gui);
	::gpk::SControl															& controlExit								= gui.Controls.Controls[app.IdExit];
	controlExit.Area													= {{}, {64, 20}};
	controlExit.Border													= {10, 10, 10, 10};
	controlExit.Margin													= {1, 1, 1, 1};
	controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
	::gpk::SControlText														& controlText								= gui.Controls.Text[app.IdExit];
	controlText.Text													= "Exit";
	controlText.Align													= ::gpk::ALIGN_CENTER;
	::gpk::SControlConstraints												& controlConstraints						= gui.Controls.Constraints[app.IdExit];
	controlConstraints.AttachSizeToControl								= {app.IdExit, -1};
	::gpk::controlSetParent(gui, app.IdExit, -1);

	::gpk::ptr_obj<::gpk::SDialogTuner>										tuner										= {};
	app.NumericTuner													= ::gpk::tunerCreate(app.DialogMain, tuner);
	tuner->ValueLimits.Min												= 100;
	tuner->ValueLimits.Max												= 200;
	::gpk::SGUIControlTable													 & controlTable								= gui.Controls;
	controlTable.Controls[tuner->IdGUIControl].Area.Offset				= {128, 192};
	controlTable.Controls[tuner->IdGUIControl].Area.Size.x				= 128;
	controlTable.Controls[tuner->IdGUIControl].Area.Size.y				=  20;
	::gpk::tunerSetValue(*tuner, 0);

	::gpk::ptr_obj<::gpk::SDialogSlider>									slider										= {};
	app.Slider															= ::gpk::sliderCreate(app.DialogMain, slider);
	slider->ValueLimits.Min												= 0;
	slider->ValueLimits.Max												= 255;
	controlTable.Controls[slider->IdGUIControl].Area.Offset				= {128, 128};
	controlTable.Controls[slider->IdGUIControl].Area.Size.x				= 128;
	controlTable.Controls[slider->IdGUIControl].Area.Size.y				= 8;

	::gpk::ptr_obj<::gpk::SDialogCheckBox>									checkbox									= {};
	app.CheckBox														= ::gpk::checkBoxCreate(app.DialogMain, checkbox);
	controlTable.Controls[checkbox->IdGUIControl].Area.Offset			= {128, 256};

	::gpk::ptr_obj<::gpk::SDialogViewport>									viewport									= {};
	app.Viewport														= ::gpk::viewportCreate(app.DialogMain, viewport);
	controlTable.Controls	[viewport->IdGUIControl	].Area.Offset			= {320, 128};
	controlTable.Controls	[viewport->IdGUIControl	].Area.Size				= {640, 480};
	controlTable.States		[viewport->IdClient		].ImageInvertY			= true;

	static constexpr const ::gpk::SCoord3<float>							cubeCenter									= {0.5f, 0.5f, 0.5f};
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		::gpk::STriangle3D<float>												& transformedTriangle						= app.CubePositions[iTriangle];
		transformedTriangle													= geometryCube[iTriangle];
		transformedTriangle.A												-= cubeCenter;
		transformedTriangle.B												-= cubeCenter;
		transformedTriangle.C												-= cubeCenter;
	}

	return 0;
}

					::gpk::error_t									update										(::gme::SApplication & app, bool exitSignal)	{
	//::gpk::STimer															timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock										(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
	}
	::gpk::SFramework														& framework									= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");

	::gpk::SGUI																& gui										= *framework.GUI;
	::gpk::array_pod<uint32_t>												controlsToProcess							= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	for(uint32_t iProcessable = 0, countControls = controlsToProcess.size(); iProcessable < countControls; ++iProcessable) {
		uint32_t																iControl									= controlsToProcess[iProcessable];
		const ::gpk::SControlState												& controlState								= gui.Controls.States[iControl];
		if(controlState.Execute) {
			info_printf("Executed %u.", iControl);
			if(iControl == (uint32_t)app.IdExit)
				return 1;
		}
	}

	app.DialogMain.Update();
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

static constexpr	const ::gpk::SCoord3<float>						geometryCubeNormals	[12]						=
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

					::gpk::error_t									draw										(::gme::SApplication & app)							{
	// -- 

	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		buffer3d									= app.Buffer3D;
	::gpk::SFramework														& framework									= app.Framework;
	::gpk::SGUI																& gui										= *framework.GUI;
	::gpk::ptr_obj<::gpk::SDialogViewport>									viewport									= {};
	app.DialogMain.Controls[app.Viewport].as(viewport);
	buffer3d->resize(gui.Controls.Controls[viewport->IdClient].Area.Size.Cast<uint32_t>(), {0, 0, 0, 0}, (uint32_t)-1);

	//------------------------------------------------
	::gpk::array_pod<::gpk::STriangle3D<float>>								& triangle3dList							= app.VertexCache.Triangle3dList		;
	::gpk::array_pod<::gpk::SColorBGRA>										& triangle3dColorList						= app.VertexCache.Triangle3dColorList	;
	triangle3dList		.resize(12);
	triangle3dColorList	.resize(12);
	::gpk::SMatrix4<float>													& projection								= app.Scene.Projection;
	::gpk::SMatrix4<float>													& viewMatrix								= app.Scene.ViewMatrix;
	::gpk::SFrameInfo														& frameInfo									= framework.FrameInfo;
	::gpk::SCameraRange														& nearFar									= app.Scene.Camera.NearFar;
	const ::gpk::SCoord3<float>												& cameraUp									= app.Scene.CameraUp;
	::gme::SCamera															& camera									= app.Scene.Camera;
	::gpk::SCoord3<float>													& lightPos									= app.Scene.LightPos;
	projection.Identity();
	const ::gpk::SCoord3<float>												tilt										= {10, };	// ? cam't remember what is this. Radians? Eulers?
	const ::gpk::SCoord3<float>												rotation									= {0, (float)frameInfo.FrameNumber / 100, 0};

	static	float															cameraRotation								= 0;
	camera																= {{10, 5, 0}, {}};
	lightPos															= camera.Position;
	cameraRotation														+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	camera.Position	.RotateY(frameInfo.Microseconds.Total / 1000000.0f);
	lightPos		.RotateY(frameInfo.Microseconds.Total /  500000.0f);
	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	const ::gpk::SCoord2<uint32_t>											& offscreenMetrics							= buffer3d->Color.View.metrics();
	projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far );
	projection															= viewMatrix * projection;
	lightPos.Normalize();

	::gpk::SMatrix4<float>													mViewport									= {};
	mViewport._11														= 2.0f / offscreenMetrics.x;
	mViewport._22														= 2.0f / offscreenMetrics.y;
	mViewport._33														= 1.0f / (float)(nearFar.Far - nearFar.Near);
	mViewport._43														= (float)(-nearFar.Near * ( 1.0f / (nearFar.Far - nearFar.Near) ));
	mViewport._44														= 1.0f;
	projection															= projection * mViewport.GetInverse();
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		::gpk::STriangle3D<float>												& transformedTriangle						= triangle3dList[iTriangle];
		transformedTriangle													= app.CubePositions[iTriangle];
		::gpk::transform(transformedTriangle, projection);
	}
	::gpk::array_pod<::gpk::STriangle2D<int32_t>>							triangle2dList								= {};
	triangle2dList.resize(12);
	const ::gpk::SCoord2<int32_t>											screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) { // Maybe the scale
		::gpk::STriangle3D<float>												& transformedTriangle3D						= triangle3dList[iTriangle];
		::gpk::STriangle2D<int32_t>												& transformedTriangle2D						= triangle2dList[iTriangle];
		transformedTriangle2D.A												= {(int32_t)transformedTriangle3D.A.x, (int32_t)transformedTriangle3D.A.y};
		transformedTriangle2D.B												= {(int32_t)transformedTriangle3D.B.x, (int32_t)transformedTriangle3D.B.y};
		transformedTriangle2D.C												= {(int32_t)transformedTriangle3D.C.x, (int32_t)transformedTriangle3D.C.y};
		::gpk::translate(transformedTriangle2D, screenCenter);
	}

	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																	lightFactor									= geometryCubeNormals[iTriangle].Dot(lightPos);
		triangle3dColorList[iTriangle]									= ::gpk::RED * lightFactor;
	}
	::gpk::array_pod<::gpk::SCoord2<int32_t>>								trianglePixelCoords;
	::gpk::array_pod<::gpk::SCoord2<int32_t>>								wireframePixelCoords;
	::gpk::SCoord3<float>													cameraFront										= (camera.Target - camera.Position).Normalize();
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																	lightFactor									= geometryCubeNormals[iTriangle].Dot(cameraFront);
		if(lightFactor > 0)
			continue;
		error_if(errored(::gpk::drawTriangle(buffer3d->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])), "Not sure if these functions could ever fail");
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	}
	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		::gpk::drawPixelLight(buffer3d->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::GREEN, 0.05f, 1.5);

	gui.Controls.Controls[viewport->IdClient].Image							= buffer3d->Color.View; 

	// --- 
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, ::gpk::LIGHTGRAY, 0xFFFFFFFFU);
	{
		::gpk::mutex_guard														lock										(app.Framework.LockGUI);
		::gpk::guiDraw(*app.DialogMain.GUI, target->Color.View);
	}
	{
		::gpk::mutex_guard														lock										(app.LockRender);
		app.Offscreen														= target;
	}
	return 0;
}
