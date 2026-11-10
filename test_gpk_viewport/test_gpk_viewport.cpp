#include "test_gpk_viewport.h"
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

usns gpk;

// Vertex coordinates for cube faces
stxp	const tri3<float>					modelPositionsVertices	[12]						=
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

stxp	const tri3<float>						modelNormalVectors		[12]						=
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
					err_t									indexValues									(const view<const _tValue> & in_values, apod<_tIndex> & out_indices, apod<_tValue> & out_values, apod<_tIndex> & out_remap){
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
			if_fail_fe(newIndex = out_values.push_back(in_value));
			out_indices	.push_back((_tIndex)newIndex);
			out_remap	.push_back((_tIndex)iValue);
		}
	}
	return 0;
}

err_t			cleanup		(::gme::SApplication & app)						{ return mainWindowDestroy(app.Framework.RootWindow); }
err_t			setup		(::gme::SApplication & app)						{
	app.Buffer3D.create();
	SFramework				& framework									= app.Framework;
	SWindow					& mainWindow								= framework.RootWindow;
	app.Framework.GUI													= app.DialogMain.GUI;
	app.DialogMain.Input												= mainWindow.Input;
	framework.RootWindow.Size												= {1280, 720};
	if_true_e(failed(mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	SGUI						& gui										= *framework.GUI;
	gui.ColorModeDefault												= GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ASCII_COLOR_DARKGREEN * 16 + 7;
	{
		app.IdExit															= controlCreate(gui);
		SControlPlacement															& controlExit								= gui.Controls.Placement[app.IdExit];
		controlExit.Area													= {{}, {64, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ALIGN_CENTER_BOTTOM;
		SControlText														& controlText								= gui.Controls.Text[app.IdExit];
		controlText.Text													= "Exit";
		controlText.Align													= ALIGN_CENTER;
		controlSetParent(gui, app.IdExit, -1);
	}

	{
		app.IdFrameRateUpdate												= controlCreate(gui);
		SControlPlacement															& controlExit								= gui.Controls.Placement[app.IdFrameRateUpdate];
		controlExit.Area													= {{}, {384, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ALIGN_TOP_LEFT;
		SControlText														& controlText								= gui.Controls.Text[app.IdFrameRateUpdate];
		controlText.Align													= ALIGN_CENTER;
		controlSetParent(gui, app.IdFrameRateUpdate, -1);
	}

	{
		app.IdFrameRateRender												= controlCreate(gui);
		SControlPlacement															& controlExit								= gui.Controls.Placement[app.IdFrameRateRender];
		controlExit.Area													= {{}, {384, 20}};
		controlExit.Border													= {10, 10, 10, 10};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ALIGN_TOP_RIGHT;
		SControlText														& controlText								= gui.Controls.Text[app.IdFrameRateRender];
		controlText.Align													= ALIGN_CENTER;
		controlSetParent(gui, app.IdFrameRateRender, -1);
	}

	pobj<SDialogTuner<uint8_t>>						tuner						= {};
	app.NumericTuner											= tunerCreate(app.DialogMain, tuner);
	tuner->ValueLimits.Min										= 100;
	tuner->ValueLimits.Max										= 200;
	SControlTable											 & controlTable								= gui.Controls;
	controlTable.Placement[tuner->IdGUIControl].Area.Offset		= {128, 192};
	controlTable.Placement[tuner->IdGUIControl].Area.Size.x		= 128;
	controlTable.Placement[tuner->IdGUIControl].Area.Size.y		=  20;
	tuner->SetValue(0);

	pobj<SDialogSlider>							slider										= {};
	app.Slider													= sliderCreate(app.DialogMain, slider);
	slider->ValueLimits.Min										= 0;
	slider->ValueLimits.Max										= 255;
	controlTable.Placement[slider->IdGUIControl].Area.Offset	= {128, 128};
	controlTable.Placement[slider->IdGUIControl].Area.Size.x	= 128;
	controlTable.Placement[slider->IdGUIControl].Area.Size.y	= 8;

	pobj<SDialogCheckBox>									checkbox									= {};
	app.CheckBox														= checkBoxCreate(app.DialogMain, checkbox);
	controlTable.Placement[checkbox->IdGUIControl].Area.Offset			= {128, 256};

	pobj<SDialogViewport>									viewport									= {};
	app.Viewport														= viewportCreate(app.DialogMain, viewport);
	controlTable.Placement	[viewport->IdGUIControl	].Area.Offset			= {320, 128};
	controlTable.Placement	[viewport->IdGUIControl	].Area.Size				= {640, 480};
	controlTable.Images		[viewport->IdClient		].ImageInvertY			= true;

	stxp n3f2_c							cubeCenter									= {0.5f, 0.5f, 0.5f};
	au0_t								& remap										= app.ModelGeometry.PositionRemap;
	::indexValues({&modelPositionsVertices[0].A, ::gpk::size(modelPositionsVertices) * 3}, app.ModelGeometry.Positions.Indices, app.ModelGeometry.Positions.Values, remap);
	for(uint32_t iVertex = 0; iVertex < app.ModelGeometry.Positions.Values.size(); ++iVertex) {
		n3f2_t								& vertexToTransform					= app.ModelGeometry.Positions.Values[iVertex];
		vertexToTransform				-= cubeCenter;
	}
	app.ModelGeometry.NormalsVertex.Values	= modelNormalVectors;
	return 0;
}

err_t			update				(::gme::SApplication & app, bool exitSignal)	{
	static STimer		timer;
	if_true_vi(APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard			lock			(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	SFramework			& framework		= app.Framework;
	if_true_vi(APPLICATION_STATE_EXIT, APPLICATION_STATE_EXIT == updateFramework(app.Framework));

	SGUI					& gui			= *framework.GUI;
	acid					toProcess		= {};
	if(1 == guiProcessControls(gui, [&app](cid_t iControl) { return one_if(iControl == app.IdExit); }))
		return 1;

	app.DialogMain.Update();

	//------------------------------------------------
	SFrameInfo														& frameInfo									= framework.FrameInfo;
	::gme::SViewportScene													& scene										= app.Scene;
	minmaxf2_t														& nearFar									= scene.Camera.NearFar;
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
		pobj<SDialogViewport>									viewport									= {};
		app.DialogMain.Controls[app.Viewport].as(viewport);
		const n2u2_t											& offscreenMetrics							= gui.Controls.Placement[viewport->IdClient].Area.Size.Cast<uint32_t>();
		scene.Projection.FieldOfView(.25 * math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar);
		scene.Projection															= scene.ViewMatrix * scene.Projection;
		scene.LightPos.Normalize();

		m4f2_t													mViewport									= {};
		mViewport._11														= 2.0f / offscreenMetrics.x;
		mViewport._22														= 2.0f / offscreenMetrics.y;
		mViewport._33														= 1.0f / (float)(nearFar.Max - nearFar.Min);
		mViewport._43														= (float)(-nearFar.Min * ( 1.0f / (nearFar.Max - nearFar.Min) ));
		mViewport._44														= 1.0f;
		scene.Projection													= scene.Projection * mViewport.GetInverse();
	}

	timer.Frame();
	snprintf(app.StringFrameRateUpdate, size(app.StringFrameRateUpdate) - 2, "Last frame time (update): %fs.", (float)timer.LastTimeSeconds);
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	gui.Controls.Text[app.IdFrameRateUpdate].Text							= app.StringFrameRateUpdate;
	controlMetricsInvalidate(gui, app.IdFrameRateUpdate);
	return 0;
}


err_t			draw			(::gme::SApplication & app)							{
	static STimer		timer;
	SFramework			& framework		= app.Framework;
	SGUI					& gui			= *framework.GUI;

	pobj<rtbgra8d32>	buffer3d	= app.Buffer3D;
	pobj<SDialogViewport>	viewport	= {};
	app.DialogMain.Controls[app.Viewport].as(viewport);
	const n2u1_t											& offscreenMetrics							= gui.Controls.Placement[viewport->IdClient].Area.Size.Cast<uint16_t>();
	buffer3d->resize(offscreenMetrics.Cast<uint32_t>(), {0, 0, 0, 0}, (uint32_t)-1);

	apod<tri3f2_t>				& triangle3dList							= app.VertexCache.Triangle3dTransformed	;
	apod<bgra>					& triangle3dColorList						= app.VertexCache.Triangle3dColorList	;
	u2_c						countTriangles								= app.ModelGeometry.Positions.Indices.size() / 3;
	triangle3dList		.resize(countTriangles);
	triangle3dColorList	.resize(countTriangles * 3);
	m4<float>													projection									;
	::gme::SCamera															camera										;
	{
		::gpk::lock_guard														lockViewport								(app.LockViewport);
		projection															= app.Scene.Projection;
		camera																= app.Scene.Camera;
	}

	n3f2_t													& lightPos									= app.Scene.LightPos;
	for(uint32_t iTriangle = 0; iTriangle < countTriangles; ++iTriangle) {
		tri3<float>												& transformedTriangle						= triangle3dList[iTriangle];
		transformedTriangle													=
			{ app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 0]]
			, app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 1]]
			, app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 2]]
			};
		transform(transformedTriangle, projection);
	}
	apod<tri2s2_t>							triangle2dList								= {};
	triangle2dList.resize(countTriangles);
	const n2<int32_t>											screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
	for(uint32_t iTriangle = 0; iTriangle < countTriangles; ++iTriangle) { // Maybe the scale
		tri3<float>												& transformedTriangle3D						= triangle3dList[iTriangle];
		tri2<int32_t>												& transformedTriangle2D						= triangle2dList[iTriangle];
		transformedTriangle2D.A												= {(int32_t)transformedTriangle3D.A.x, (int32_t)transformedTriangle3D.A.y};
		transformedTriangle2D.B												= {(int32_t)transformedTriangle3D.B.x, (int32_t)transformedTriangle3D.B.y};
		transformedTriangle2D.C												= {(int32_t)transformedTriangle3D.C.x, (int32_t)transformedTriangle3D.C.y};
		translate(transformedTriangle2D, screenCenter);

	}

	view<n3f2_t>								normals										= {(n3f2_t*)app.ModelGeometry.NormalsVertex.Values.begin(), app.ModelGeometry.NormalsVertex.Values.size() * 3};
	for(uint32_t iVertex = 0; iVertex < app.ModelGeometry.Positions.Indices.size(); ++iVertex) {
		double																	lightFactor									= normals[iVertex].Dot(lightPos);
		triangle3dColorList[iVertex]										= (RED * lightFactor).Clamp();
	}
	apod<n2s1_t>								wireframePixelCoords;
	apod<n2s1_t>								trianglePixelCoords;
	n3f2_t													cameraFront										= (camera.Target - camera.Position).Normalize();
	apod<trif2_t>								triangleWeights;

	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																	lightFactor									= normals[iTriangle * 3].Dot(cameraFront);
		if(lightFactor > 0)
			continue;
		//if_true_e(failed(drawTriangle(buffer3d->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])));
		trianglePixelCoords.clear();
		tri3<float>												transformedTriangle3D						= {};
		transformedTriangle3D.A												= {(float)triangle2dList[iTriangle].A.x, (float)triangle2dList[iTriangle].A.y, triangle3dList[iTriangle].A.z};
		transformedTriangle3D.B												= {(float)triangle2dList[iTriangle].B.x, (float)triangle2dList[iTriangle].B.y, triangle3dList[iTriangle].B.z};
		transformedTriangle3D.C												= {(float)triangle2dList[iTriangle].C.x, (float)triangle2dList[iTriangle].C.y, triangle3dList[iTriangle].C.z};
		if_true_e(failed(drawTriangle(offscreenMetrics.Cast<uint32_t>(), transformedTriangle3D, trianglePixelCoords, triangleWeights, buffer3d->DepthStencil.View)));
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord)
			buffer3d->Color.View[trianglePixelCoords[iCoord].y][trianglePixelCoords[iCoord].x] = triangle3dColorList[iTriangle * 3];

		drawLine(offscreenMetrics, line2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		drawLine(offscreenMetrics, line2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		drawLine(offscreenMetrics, line2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	}
	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		drawPixelLight(buffer3d->Color.View, wireframePixelCoords[iCoord], (bgra)GREEN, 0.05f, 1.5);

	gui.Controls.Images[viewport->IdClient].Image						= buffer3d->Color.View;

	// ---
	pobj<rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, LIGHTGRAY, 0xFFFFFFFFU);
	{
		::std::lock_guard														lock										(app.Framework.LockGUI);
		guiDraw(*app.DialogMain.GUI, target->Color.View);
	}
	{
		::std::lock_guard														lock										(app.LockRender);
		app.Offscreen														= target;
	}
	timer.Frame();
	snprintf(app.StringFrameRateRender, size(app.StringFrameRateRender), "Last frame time (render): %fs.", (float)timer.LastTimeSeconds);
	controlTextSet(gui, app.IdFrameRateRender, app.StringFrameRateRender);
	return 0;
}
