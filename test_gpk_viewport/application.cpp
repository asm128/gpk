#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_dialog_controls.h"
#include "gpk_matrix.h"
#include "gpk_bitmap_target.h"
#include "gpk_raster_lh.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

// Vertex coordinates for cube faces
stxp	const ::gpk::tri3<float>					modelPositionsVertices	[12]						=
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

stxp	const ::gpk::tri3<float>						modelNormalVectors		[12]						=
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
					::gpk::err_t									indexValues									(const ::gpk::view<const _tValue> & in_values, ::gpk::apod<_tIndex> & out_indices, ::gpk::apod<_tValue> & out_values, ::gpk::apod<_tIndex> & out_remap){
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
			do { if_fail_fef(newIndex = out_values.push_back(in_value), "%s", "Out of memory?");
			out_indices	.push_back((_tIndex)newIndex);
			out_remap	.push_back((_tIndex)iValue);
		}
	}
	return 0;
}

::gpk::err_t			cleanup		(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
::gpk::err_t			setup		(::gme::SApplication & app)						{
	app.Buffer3D.create();
	::gpk::SFramework				& framework									= app.Framework;
	::gpk::SWindow					& mainWindow								= framework.RootWindow;
	app.Framework.GUI													= app.DialogMain.GUI;
	app.DialogMain.Input												= mainWindow.Input;
	framework.RootWindow.Size												= {1280, 720};
	if_true_e(::gpk::failed(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	::gpk::SGUI						& gui										= *framework.GUI;
	gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
	{
		app.IdExit															= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement															& controlExit								= gui.Controls.Placement[app.IdExit];
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
		::gpk::SControlPlacement															& controlExit								= gui.Controls.Placement[app.IdFrameRateUpdate];
		controlExit.Area													= {{}, {384, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_TOP_LEFT;
		::gpk::SControlText														& controlText								= gui.Controls.Text[app.IdFrameRateUpdate];
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, app.IdFrameRateUpdate, -1);
	}

	{
		app.IdFrameRateRender												= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement															& controlExit								= gui.Controls.Placement[app.IdFrameRateRender];
		controlExit.Area													= {{}, {384, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_TOP_RIGHT;
		::gpk::SControlText														& controlText								= gui.Controls.Text[app.IdFrameRateRender];
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, app.IdFrameRateRender, -1);
	}

	::gpk::pobj<::gpk::SDialogTuner<uint8_t>>						tuner						= {};
	app.NumericTuner											= ::gpk::tunerCreate(app.DialogMain, tuner);
	tuner->ValueLimits.Min										= 100;
	tuner->ValueLimits.Max										= 200;
	::gpk::SControlTable											 & controlTable								= gui.Controls;
	controlTable.Placement[tuner->IdGUIControl].Area.Offset		= {128, 192};
	controlTable.Placement[tuner->IdGUIControl].Area.Size.x		= 128;
	controlTable.Placement[tuner->IdGUIControl].Area.Size.y		=  20;
	tuner->SetValue(0);

	::gpk::pobj<::gpk::SDialogSlider>							slider										= {};
	app.Slider													= ::gpk::sliderCreate(app.DialogMain, slider);
	slider->ValueLimits.Min										= 0;
	slider->ValueLimits.Max										= 255;
	controlTable.Placement[slider->IdGUIControl].Area.Offset	= {128, 128};
	controlTable.Placement[slider->IdGUIControl].Area.Size.x	= 128;
	controlTable.Placement[slider->IdGUIControl].Area.Size.y	= 8;

	::gpk::pobj<::gpk::SDialogCheckBox>									checkbox									= {};
	app.CheckBox														= ::gpk::checkBoxCreate(app.DialogMain, checkbox);
	controlTable.Placement[checkbox->IdGUIControl].Area.Offset			= {128, 256};

	::gpk::pobj<::gpk::SDialogViewport>									viewport									= {};
	app.Viewport														= ::gpk::viewportCreate(app.DialogMain, viewport);
	controlTable.Placement	[viewport->IdGUIControl	].Area.Offset			= {320, 128};
	controlTable.Placement	[viewport->IdGUIControl	].Area.Size				= {640, 480};
	controlTable.Images		[viewport->IdClient		].ImageInvertY			= true;

	stxp const ::gpk::n3f2_t							cubeCenter									= {0.5f, 0.5f, 0.5f};
	::gpk::array_pod<uint8_t>												& remap										= app.ModelGeometry.PositionRemap;
	::indexValues({&modelPositionsVertices[0].A, ::gpk::size(modelPositionsVertices) * 3}, app.ModelGeometry.Positions.Indices, app.ModelGeometry.Positions.Values, remap);
	for(uint32_t iVertex = 0; iVertex < app.ModelGeometry.Positions.Values.size(); ++iVertex) {
		::gpk::n3f2_t													& vertexToTransform					= app.ModelGeometry.Positions.Values[iVertex];
		vertexToTransform													-= cubeCenter;
	}
	app.ModelGeometry.NormalsVertex.Values									= modelNormalVectors;
	return 0;
}

::gpk::err_t			update				(::gme::SApplication & app, bool exitSignal)	{
	static ::gpk::STimer		timer;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard			lock			(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	::gpk::SFramework			& framework		= app.Framework;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui			= *framework.GUI;
	::gpk::acid					toProcess		= {};
	if(1 == ::gpk::guiProcessControls(gui, [&app](::gpk::cid_t iControl) { return one_if(iControl == app.IdExit); }))
		return 1;

	app.DialogMain.Update();

	//------------------------------------------------
	::gpk::SFrameInfo														& frameInfo									= framework.FrameInfo;
	::gme::SViewportScene													& scene										= app.Scene;
	::gpk::minmaxf2_t														& nearFar									= scene.Camera.NearFar;
	{
		::std::lock_guard														lockViewport								(app.LockViewport);
		scene.Projection.Identity();
		static	float															cameraRotation								= 0;
		scene.Camera																= {{10, 5, 0}, {}};
		scene.LightPos															= scene.Camera.Position;
		cameraRotation														+= (float)framework.RootWindow.Input->MouseCurrent.Deltas.x / 5.0f;
		//camera.Position	.RotateY(cameraRotation);
		scene.Camera.Position	.RotateY(frameInfo.Microseconds.Total / 1000000.0f);
		scene.LightPos		.RotateY(frameInfo.Microseconds.Total /  500000.0f);
		scene.ViewMatrix.LookAt(scene.Camera.Position, scene.Camera.Target, scene.CameraUp);
		::gpk::pobj<::gpk::SDialogViewport>									viewport									= {};
		app.DialogMain.Controls[app.Viewport].as(viewport);
		const ::gpk::n2u2_t											& offscreenMetrics							= gui.Controls.Placement[viewport->IdClient].Area.Size.Cast<uint32_t>();
		scene.Projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar);
		scene.Projection															= scene.ViewMatrix * scene.Projection;
		scene.LightPos.Normalize();

		::gpk::m4f32													mViewport									= {};
		mViewport._11														= 2.0f / offscreenMetrics.x;
		mViewport._22														= 2.0f / offscreenMetrics.y;
		mViewport._33														= 1.0f / (float)(nearFar.Max - nearFar.Min);
		mViewport._43														= (float)(-nearFar.Min * ( 1.0f / (nearFar.Max - nearFar.Min) ));
		mViewport._44														= 1.0f;
		scene.Projection													= scene.Projection * mViewport.GetInverse();
	}

	timer.Frame();
	snprintf(app.StringFrameRateUpdate, ::gpk::size(app.StringFrameRateUpdate) - 2, "Last frame time (update): %fs.", (float)timer.LastTimeSeconds);
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	gui.Controls.Text[app.IdFrameRateUpdate].Text							= app.StringFrameRateUpdate;
	::gpk::controlMetricsInvalidate(gui, app.IdFrameRateUpdate);
	return 0;
}


::gpk::err_t			draw			(::gme::SApplication & app)							{
	static ::gpk::STimer		timer;
	::gpk::SFramework			& framework		= app.Framework;
	::gpk::SGUI					& gui			= *framework.GUI;

	::gpk::pobj<::gpk::rtbgra8d32>	buffer3d	= app.Buffer3D;
	::gpk::pobj<::gpk::SDialogViewport>	viewport	= {};
	app.DialogMain.Controls[app.Viewport].as(viewport);
	const ::gpk::n2<uint16_t>											& offscreenMetrics							= gui.Controls.Placement[viewport->IdClient].Area.Size.Cast<uint16_t>();
	buffer3d->resize(offscreenMetrics.Cast<uint32_t>(), {0, 0, 0, 0}, (uint32_t)-1);

	::gpk::array_pod<::gpk::tri3<float>>								& triangle3dList							= app.VertexCache.Triangle3dTransformed	;
	::gpk::array_pod<::gpk::bgra>										& triangle3dColorList						= app.VertexCache.Triangle3dColorList	;
	const uint32_t															countTriangles								= app.ModelGeometry.Positions.Indices.size() / 3;
	triangle3dList		.resize(countTriangles);
	triangle3dColorList	.resize(countTriangles * 3);
	::gpk::m4<float>													projection									;
	::gme::SCamera															camera										;
	{
		::std::lock_guard														lockViewport								(app.LockViewport);
		projection															= app.Scene.Projection;
		camera																= app.Scene.Camera;
	}

	::gpk::n3f2_t													& lightPos									= app.Scene.LightPos;
	for(uint32_t iTriangle = 0; iTriangle < countTriangles; ++iTriangle) {
		::gpk::tri3<float>												& transformedTriangle						= triangle3dList[iTriangle];
		transformedTriangle													=
			{ app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 0]]
			, app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 1]]
			, app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 2]]
			};
		::gpk::transform(transformedTriangle, projection);
	}
	::gpk::array_pod<::gpk::tri2<int32_t>>							triangle2dList								= {};
	triangle2dList.resize(countTriangles);
	const ::gpk::n2<int32_t>											screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
	for(uint32_t iTriangle = 0; iTriangle < countTriangles; ++iTriangle) { // Maybe the scale
		::gpk::tri3<float>												& transformedTriangle3D						= triangle3dList[iTriangle];
		::gpk::tri2<int32_t>												& transformedTriangle2D						= triangle2dList[iTriangle];
		transformedTriangle2D.A												= {(int32_t)transformedTriangle3D.A.x, (int32_t)transformedTriangle3D.A.y};
		transformedTriangle2D.B												= {(int32_t)transformedTriangle3D.B.x, (int32_t)transformedTriangle3D.B.y};
		transformedTriangle2D.C												= {(int32_t)transformedTriangle3D.C.x, (int32_t)transformedTriangle3D.C.y};
		::gpk::translate(transformedTriangle2D, screenCenter);

	}

	::gpk::view<::gpk::n3f2_t>								normals										= {(::gpk::n3f2_t*)app.ModelGeometry.NormalsVertex.Values.begin(), app.ModelGeometry.NormalsVertex.Values.size() * 3};
	for(uint32_t iVertex = 0; iVertex < app.ModelGeometry.Positions.Indices.size(); ++iVertex) {
		double																	lightFactor									= normals[iVertex].Dot(lightPos);
		triangle3dColorList[iVertex]										= (::gpk::RED * lightFactor).Clamp();
	}
	::gpk::array_pod<::gpk::n2<int16_t>>								wireframePixelCoords;
	::gpk::array_pod<::gpk::n2<int16_t>>								trianglePixelCoords;
	::gpk::n3f2_t													cameraFront										= (camera.Target - camera.Position).Normalize();
	::gpk::array_pod<::gpk::tri<float>>								triangleWeights;

	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																	lightFactor									= normals[iTriangle * 3].Dot(cameraFront);
		if(lightFactor > 0)
			continue;
		//if_true_e(::gpk::failed(::gpk::drawTriangle(buffer3d->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])));
		trianglePixelCoords.clear();
		::gpk::tri3<float>												transformedTriangle3D						= {};
		transformedTriangle3D.A												= {(float)triangle2dList[iTriangle].A.x, (float)triangle2dList[iTriangle].A.y, triangle3dList[iTriangle].A.z};
		transformedTriangle3D.B												= {(float)triangle2dList[iTriangle].B.x, (float)triangle2dList[iTriangle].B.y, triangle3dList[iTriangle].B.z};
		transformedTriangle3D.C												= {(float)triangle2dList[iTriangle].C.x, (float)triangle2dList[iTriangle].C.y, triangle3dList[iTriangle].C.z};
		if_true_e(::gpk::failed(::gpk::drawTriangle(offscreenMetrics.Cast<uint32_t>(), transformedTriangle3D, trianglePixelCoords, triangleWeights, buffer3d->DepthStencil.View)));
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord)
			buffer3d->Color.View[trianglePixelCoords[iCoord].y][trianglePixelCoords[iCoord].x] = triangle3dColorList[iTriangle * 3];

		::gpk::drawLine(offscreenMetrics, ::gpk::line2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::line2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::line2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	}
	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		::gpk::drawPixelLight(buffer3d->Color.View, wireframePixelCoords[iCoord], (::gpk::bgra)::gpk::GREEN, 0.05f, 1.5);

	gui.Controls.Images[viewport->IdClient].Image						= buffer3d->Color.View;

	// ---
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::LIGHTGRAY, 0xFFFFFFFFU);
	{
		::std::lock_guard														lock										(app.Framework.LockGUI);
		::gpk::guiDraw(*app.DialogMain.GUI, target->Color.View);
	}
	{
		::std::lock_guard														lock										(app.LockRender);
		app.Offscreen														= target;
	}
	timer.Frame();
	snprintf(app.StringFrameRateRender, ::gpk::size(app.StringFrameRateRender), "Last frame time (render): %fs.", (float)timer.LastTimeSeconds);
	::gpk::controlTextSet(gui, app.IdFrameRateRender, app.StringFrameRateRender);
	return 0;
}
