#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_dialog_controls.h"
#include "gpk_matrix.h"
#include "gpk_bitmap_target.h"

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

// Vertex coordinates for cube faces
static constexpr	const ::gpk::STriangle3D<float>					modelPositionsVertices	[12]						=
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

static constexpr	const ::gpk::STriangle3D<float>						modelNormalVectors		[12]						=
	{ {{0.0f, 0.0f, -1.0f}	, {0.0f, 0.0f, -1.0f}	, {0.0f, 0.0f, -1.0f}}	// Right	- first			?? I have no idea if this is correct lol
	, {{0.0f, 0.0f, -1.0f}	, {0.0f, 0.0f, -1.0f}	, {0.0f, 0.0f, -1.0f}}	// Right	- second		?? I have no idea if this is correct lol
//		{					, , 										}
	, {{-1.0f, 0.0f, 0.0f}	, {-1.0f, 0.0f, 0.0f}	, {-1.0f, 0.0f, 0.0f}}	// Back		- first			?? I have no idea if this is correct lol
	, {{-1.0f, 0.0f, 0.0f}	, {-1.0f, 0.0f, 0.0f}	, {-1.0f, 0.0f, 0.0f}}	// Back		- second		?? I have no idea if this is correct lol
//		{					, , 										}
	, {{0.0f, -1.0f, 0.0f}	, {0.0f, -1.0f, 0.0f}	, {0.0f, -1.0f, 0.0f}}	// Bottom	- first			?? I have no idea if this is correct lol
	, {{0.0f, -1.0f, 0.0f}	, {0.0f, -1.0f, 0.0f}	, {0.0f, -1.0f, 0.0f}}	// Bottom	- second		?? I have no idea if this is correct lol
//		{					, , 										}
	, {{0.0f, 0.0f, 1.0f}	, {0.0f, 0.0f, 1.0f}	, {0.0f, 0.0f, 1.0f}}	// Left		- first
	, {{0.0f, 0.0f, 1.0f}	, {0.0f, 0.0f, 1.0f}	, {0.0f, 0.0f, 1.0f}}	// Left		- second
//		{					, , 										}
	, {{1.0f, 0.0f, 0.0f}	, {1.0f, 0.0f, 0.0f}	, {1.0f, 0.0f, 0.0f}}	// Front	- first
	, {{1.0f, 0.0f, 0.0f}	, {1.0f, 0.0f, 0.0f}	, {1.0f, 0.0f, 0.0f}}	// Front	- second
//		{					, , 										}
	, {{0.0f, 1.0f, 0.0f}	, {0.0f, 1.0f, 0.0f}	, {0.0f, 1.0f, 0.0f}}	// Top		- first
	, {{0.0f, 1.0f, 0.0f}	, {0.0f, 1.0f, 0.0f}	, {0.0f, 1.0f, 0.0f}}	// Top		- second
	};

template<typename _tIndex, typename _tValue>
					::gpk::error_t									indexValues									(const ::gpk::view_array<const _tValue> & in_values, ::gpk::array_pod<_tIndex> & out_indices, ::gpk::array_pod<_tValue> & out_values, ::gpk::array_pod<_tIndex> & out_remap){
	for(uint32_t iValue = 0; iValue < in_values.size(); ++iValue) {
		bool																	bFound										= false;
		const _tValue															& in_value									= in_values[iValue];
		for(uint32_t iValueIndexed = 0; iValueIndexed < out_values.size(); ++iValueIndexed) {
			const _tValue															& out_value									= out_values[iValueIndexed];
			if(in_value == out_value) {
				bFound																= true;
				out_indices	.push_back((_tIndex)iValueIndexed);
				out_remap	.push_back((_tIndex)iValue);
				break;
			}
		}
		if(false == bFound) {
			int32_t																	newIndex;
			gpk_necall(newIndex = out_values.push_back(in_value), "%s", "Out of memory?");
			out_indices	.push_back((_tIndex)newIndex);
			out_remap	.push_back((_tIndex)iValue);
		}
	}
	return 0;
}

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
	{
		app.IdExit															= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlExit								= gui.Controls.Controls[app.IdExit];
		controlExit.Area													= {{}, {64, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_CENTER_BOTTOM;
		::gpk::SControlText														& controlText								= gui.Controls.Text[app.IdExit];
		controlText.Text													= "Exit";
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}

	{
		app.IdFrameRateUpdate												= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlExit								= gui.Controls.Controls[app.IdFrameRateUpdate];
		controlExit.Area													= {{}, {384, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_BOTTOM_LEFT;
		::gpk::SControlText														& controlText								= gui.Controls.Text[app.IdFrameRateUpdate];
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, app.IdFrameRateUpdate, -1);
	}

	{
		app.IdFrameRateRender												= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlExit								= gui.Controls.Controls[app.IdFrameRateRender];
		controlExit.Area													= {{}, {384, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
		::gpk::SControlText														& controlText								= gui.Controls.Text[app.IdFrameRateRender];
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, app.IdFrameRateRender, -1);
	}

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
	::gpk::array_pod<uint8_t>												& remap										= app.ModelGeometry.PositionRemap;
	::indexValues({&modelPositionsVertices[0].A, ::gpk::size(modelPositionsVertices) * 3}, app.ModelGeometry.Positions.Indices, app.ModelGeometry.Positions.Values, remap);
	for(uint32_t iVertex = 0; iVertex < app.ModelGeometry.Positions.Values.size(); ++iVertex) {
		::gpk::SCoord3<float>													& vertexToTransform					= app.ModelGeometry.Positions.Values[iVertex];
		vertexToTransform													-= cubeCenter;
	}
	app.ModelGeometry.NormalsVertex.Values									= modelNormalVectors;
	return 0;
}

					::gpk::error_t									update										(::gme::SApplication & app, bool exitSignal)	{
	static ::gpk::STimer													timer;
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

	//------------------------------------------------
	::gpk::SFrameInfo														& frameInfo									= framework.FrameInfo;
	::gme::SViewportScene													& scene										= app.Scene;
	::gpk::SCameraNearFar													& nearFar									= scene.Camera.NearFar;
	{
		::gpk::mutex_guard														lockViewport								(app.LockViewport);
		scene.Projection.Identity();
		static	float															cameraRotation								= 0;
		scene.Camera																= {{10, 5, 0}, {}};
		scene.LightPos															= scene.Camera.Position;
		cameraRotation														+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
		//camera.Position	.RotateY(cameraRotation);
		scene.Camera.Position	.RotateY(frameInfo.Microseconds.Total / 1000000.0f);
		scene.LightPos		.RotateY(frameInfo.Microseconds.Total /  500000.0f);
		scene.ViewMatrix.LookAt(scene.Camera.Position, scene.Camera.Target, scene.CameraUp);
		::gpk::ptr_obj<::gpk::SDialogViewport>									viewport									= {};
		app.DialogMain.Controls[app.Viewport].as(viewport);
		const ::gpk::SCoord2<uint32_t>											& offscreenMetrics							= gui.Controls.Controls[viewport->IdClient].Area.Size.Cast<uint32_t>();
		scene.Projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far );
		scene.Projection															= scene.ViewMatrix * scene.Projection;
		scene.LightPos.Normalize();

		::gpk::SMatrix4<float>													mViewport									= {};
		mViewport._11														= 2.0f / offscreenMetrics.x;
		mViewport._22														= 2.0f / offscreenMetrics.y;
		mViewport._33														= 1.0f / (float)(nearFar.Far - nearFar.Near);
		mViewport._43														= (float)(-nearFar.Near * ( 1.0f / (nearFar.Far - nearFar.Near) ));
		mViewport._44														= 1.0f;
		scene.Projection													= scene.Projection * mViewport.GetInverse();
	}

	timer.Frame();
	sprintf_s(app.StringFrameRateUpdate, "Last frame time (update): %fs.", (float)timer.LastTimeSeconds);
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	gui.Controls.Text[app.IdFrameRateUpdate].Text							= app.StringFrameRateUpdate;
	::gpk::controlMetricsInvalidate(gui, app.IdFrameRateUpdate);
	return 0;
}


					::gpk::error_t									draw										(::gme::SApplication & app)							{
	static ::gpk::STimer													timer;
	::gpk::SFramework														& framework									= app.Framework;
	::gpk::SGUI																& gui										= *framework.GUI;

	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		buffer3d									= app.Buffer3D;
	::gpk::ptr_obj<::gpk::SDialogViewport>									viewport									= {};
	app.DialogMain.Controls[app.Viewport].as(viewport);
	const ::gpk::SCoord2<uint32_t>											& offscreenMetrics							= gui.Controls.Controls[viewport->IdClient].Area.Size.Cast<uint32_t>();
	buffer3d->resize(offscreenMetrics, {0, 0, 0, 0}, (uint32_t)-1);

	::gpk::array_pod<::gpk::STriangle3D<float>>								& triangle3dList							= app.VertexCache.Triangle3dTransformed	;
	::gpk::array_pod<::gpk::SColorBGRA>										& triangle3dColorList						= app.VertexCache.Triangle3dColorList	;
	const uint32_t															countTriangles								= app.ModelGeometry.Positions.Indices.size() / 3;
	triangle3dList		.resize(countTriangles);
	triangle3dColorList	.resize(countTriangles * 3);
	::gpk::SMatrix4<float>													projection									;
	::gme::SCamera															camera										;
	{
		::gpk::mutex_guard														lockViewport								(app.LockViewport);
		projection															= app.Scene.Projection;
		camera																= app.Scene.Camera;
	}

	::gpk::SCoord3<float>													& lightPos									= app.Scene.LightPos;
	for(uint32_t iTriangle = 0; iTriangle < countTriangles; ++iTriangle) {
		::gpk::STriangle3D<float>												& transformedTriangle						= triangle3dList[iTriangle];
		transformedTriangle													= 
			{ app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 0]]
			, app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 1]]
			, app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 2]]
			};
		::gpk::transform(transformedTriangle, projection);
	}
	::gpk::array_pod<::gpk::STriangle2D<int32_t>>							triangle2dList								= {};
	triangle2dList.resize(countTriangles);
	const ::gpk::SCoord2<int32_t>											screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
	for(uint32_t iTriangle = 0; iTriangle < countTriangles; ++iTriangle) { // Maybe the scale
		::gpk::STriangle3D<float>												& transformedTriangle3D						= triangle3dList[iTriangle];
		::gpk::STriangle2D<int32_t>												& transformedTriangle2D						= triangle2dList[iTriangle];
		transformedTriangle2D.A												= {(int32_t)transformedTriangle3D.A.x, (int32_t)transformedTriangle3D.A.y};
		transformedTriangle2D.B												= {(int32_t)transformedTriangle3D.B.x, (int32_t)transformedTriangle3D.B.y};
		transformedTriangle2D.C												= {(int32_t)transformedTriangle3D.C.x, (int32_t)transformedTriangle3D.C.y};
		::gpk::translate(transformedTriangle2D, screenCenter);

	}

	::gpk::view_array<::gpk::SCoord3<float>>								normals										= {(::gpk::SCoord3<float>*)app.ModelGeometry.NormalsVertex.Values.begin(), app.ModelGeometry.NormalsVertex.Values.size() * 3};
	for(uint32_t iVertex = 0; iVertex < app.ModelGeometry.Positions.Indices.size(); ++iVertex) {
		double																	lightFactor									= normals[iVertex].Dot(lightPos);
		triangle3dColorList[iVertex]										= ::gpk::RED * lightFactor;
	}
	::gpk::array_pod<::gpk::SCoord2<int32_t>>								wireframePixelCoords;
	::gpk::array_pod<::gpk::SCoord2<int32_t>>								trianglePixelCoords;
	::gpk::SCoord3<float>													cameraFront										= (camera.Target - camera.Position).Normalize();
	::gpk::array_pod<::gpk::STriangleWeights<double>>						triangleWeights;

	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																	lightFactor									= normals[iTriangle * 3].Dot(cameraFront);
		if(lightFactor > 0)
			continue;
		//error_if(errored(::gpk::drawTriangle(buffer3d->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])), "Not sure if these functions could ever fail");
		trianglePixelCoords.clear();
		::gpk::STriangle3D<float>												transformedTriangle3D						= {};
		transformedTriangle3D.A												= {(float)triangle2dList[iTriangle].A.x, (float)triangle2dList[iTriangle].A.y, triangle3dList[iTriangle].A.z};
		transformedTriangle3D.B												= {(float)triangle2dList[iTriangle].B.x, (float)triangle2dList[iTriangle].B.y, triangle3dList[iTriangle].B.z};
		transformedTriangle3D.C												= {(float)triangle2dList[iTriangle].C.x, (float)triangle2dList[iTriangle].C.y, triangle3dList[iTriangle].C.z};
		error_if(errored(::gpk::drawTriangle(buffer3d->DepthStencil.View, camera.NearFar, transformedTriangle3D, trianglePixelCoords, triangleWeights)), "Not sure if these functions could ever fail");
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord)
			buffer3d->Color.View[trianglePixelCoords[iCoord].y][trianglePixelCoords[iCoord].x] = triangle3dColorList[iTriangle * 3];

		::gpk::drawLine(offscreenMetrics, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine2D<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	}
	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		::gpk::drawPixelLight(buffer3d->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::GREEN, 0.05f, 1.5);

	gui.Controls.Controls[viewport->IdClient].Image						= buffer3d->Color.View; 

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
	timer.Frame();
	sprintf_s(app.StringFrameRateRender, "Last frame time (render): %fs.", (float)timer.LastTimeSeconds);
	gui.Controls.Text[app.IdFrameRateRender].Text							= app.StringFrameRateRender;
	::gpk::controlMetricsInvalidate(gui, app.IdFrameRateRender);
	return 0;
}
