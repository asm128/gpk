#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"

#include "gpk_dialog_controls.h"
#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

					::gpk::error_t										generateModelFromHeights					(const ::gpk::view_grid<::STileHeights<float>> & tileHeights, ::gpk::SModelGeometry<float> & generated)																											{
	for(uint32_t y = 0, yMax = tileHeights.metrics().y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = tileHeights.metrics().x; x < xMax; ++x) {
		float																		fx											= (float)x;
		float																		fy											= (float)y;
		const ::STileHeights<float>													& curTile									= tileHeights[y][x];
		const ::gpk::STriangle3D<float>												tri1										= {{fx, curTile.Heights[0], fy}, {fx, curTile.Heights[2], fy + 1}, {fx + 1, curTile.Heights[3], fy + 1}};
		const ::gpk::STriangle3D<float>												tri2										= {{fx, curTile.Heights[0], fy}, {fx + 1, curTile.Heights[3], fy + 1}, {fx + 1, curTile.Heights[1], fy}};
		generated.Positions.push_back(tri1);
		generated.Positions.push_back(tri2);
		const ::gpk::SCoord3<float>													normal1										= (tri1.B - tri1.A).Cross(tri1.C - tri1.A);
		const ::gpk::SCoord3<float>													normal2										= (tri1.B - tri1.A).Cross(tri1.C - tri1.A);
		generated.NormalsVertex		.push_back({normal1, normal1, normal1});
		generated.NormalsVertex		.push_back({normal2, normal2, normal2});
		generated.NormalsTriangle	.push_back(normal1);
		generated.NormalsTriangle	.push_back(normal2);
	}

	const ::gpk::SCoord2<double>											gridUnit										= {1.0 / tileHeights.metrics().x, 1.0 / tileHeights.metrics().y};
	const ::gpk::SCoord2<double>											gridMetricsF									= tileHeights.metrics().Cast<double>();
	for(uint32_t z = 0; z < tileHeights.metrics().y; ++z) 
	for(uint32_t x = 0; x < tileHeights.metrics().x; ++x) { 
		const ::gpk::SCoord2<double>											gridCell										= {x / gridMetricsF.x, z / gridMetricsF.y};
		const ::gpk::SCoord2<double>											gridCellFar										= gridCell + gridUnit;
		generated.UVs.push_back({{(float)gridCell.x, (float)gridCell.y}, {(float)gridCell.x			, (float)gridCellFar.y}	, gridCellFar.Cast<float>()					}); 
		generated.UVs.push_back({{(float)gridCell.x, (float)gridCell.y}, gridCellFar.Cast<float>()	, {(float)gridCellFar.x, (float)gridCell.y}	}); 
	}

	generated;
	return 0;
}

			::gpk::error_t											updateProjections			(const ::gpk::SCoord2<uint32_t>& newSize, const ::gpk::SSceneCamera& camera, ::gpk::SSceneTransforms& transforms)												{
	::gpk::SMatrix4<float>													& finalProjection							= transforms.FinalProjection	;
	::gpk::SMatrix4<float>													& fieldOfView								= transforms.FieldOfView		;
	::gpk::SMatrix4<float>													& mviewport									= transforms.Viewport			;
	::gpk::SMatrix4<float>													& viewportInverse							= transforms.ViewportInverse	;
	::gpk::SMatrix4<float>													& viewportInverseCentered					= transforms.ViewportInverse	;
	fieldOfView.FieldOfView(camera.Angle * ::gpk::math_pi, newSize.x / (double)newSize.y, camera.NearFar.Near, camera.NearFar.Far);
	mviewport.Viewport(newSize, camera.NearFar.Far, camera.NearFar.Near);
	viewportInverse														= mviewport.GetInverse();
	const ::gpk::SCoord2<int32_t>											screenCenter								= {(int32_t)newSize.x / 2, (int32_t)newSize.y / 2};
	viewportInverseCentered												= viewportInverse;
	viewportInverseCentered._41											+= screenCenter.x;
	viewportInverseCentered._42											+= screenCenter.y;
	finalProjection														= fieldOfView * viewportInverseCentered;
	transforms.FinalProjectionInverse									= finalProjection.GetInverse();
	return 0;
}

			::gpk::error_t											cleanup						(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
			::gpk::error_t											setup						(::gme::SApplication & app)						{
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SDisplay															& mainWindow				= framework.MainDisplay;
	app.Framework.GUI													= app.DialogMain.GUI;
	app.DialogMain.Input												= framework.Input;
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?????!?!?!?!?");
	::gpk::SGUI																& gui						= *framework.GUI;
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

	::gpk::ptr_obj<::gpk::SDialogTuner>										tuner						= {};
	app.NumericTuner													= ::gpk::tunerCreate(app.DialogMain, tuner);
	tuner->ValueLimits.Min												= 100;
	tuner->ValueLimits.Max												= 200;
	::gpk::SGUIControlTable    & controlTable = gui.Controls;
	controlTable.Controls[tuner->IdGUIControl].Area.Offset	= {128, 192};
	controlTable.Controls[tuner->IdGUIControl].Area.Size.x	= 128;
	controlTable.Controls[tuner->IdGUIControl].Area.Size.y	=  20;
	::gpk::tunerSetValue(*tuner, 0);

	::gpk::ptr_obj<::gpk::SDialogSlider>									slider						= {};
	app.Slider															= ::gpk::sliderCreate(app.DialogMain, slider);
	slider->ValueLimits.Min												= 0;
	slider->ValueLimits.Max												= 255;
	controlTable.Controls[slider->IdGUIControl].Area.Offset	= {128, 128};
	controlTable.Controls[slider->IdGUIControl].Area.Size.x	= 128;
	controlTable.Controls[slider->IdGUIControl].Area.Size.y	= 8;

	::gpk::ptr_obj<::gpk::SDialogCheckBox>									checkbox					= {};
	app.CheckBox														= ::gpk::checkBoxCreate(app.DialogMain, checkbox);
	controlTable.Controls[checkbox->IdGUIControl].Area.Offset	= {128, 256};


	::gpk::ptr_obj<::gpk::SDialogViewport>									viewport					= {};
	app.Viewport														= ::gpk::viewportCreate(app.DialogMain, viewport);
	controlTable.Controls[viewport->IdGUIControl].Area.Offset	= {320, 128};
	controlTable.Controls[viewport->IdGUIControl].Area.Size		= {800, 600};
	controlTable.States[viewport->IdClient].ImageInvertY		= true;

	char																		bmpFileName1	[]							= "..\\gpk_data\\images\\test.png";//"pow_core_0.bmp";
	error_if(errored(::gpk::pngFileLoad(bmpFileName1, app.TextureGrid)), "");

	// Load and pretransform our cube geometry.
	const ::gpk::view_grid<::gpk::SColorBGRA>									& textureGridView								= app.TextureGrid.View;
	const ::gpk::SCoord2<uint32_t>												& textureGridMetrics							= textureGridView.metrics();
	app.TileHeights.resize(textureGridView.metrics());
	for(uint32_t y = 0, yMax = textureGridMetrics.y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = textureGridMetrics.x; x < xMax; ++x) {
		::STileHeights<float>														& curTile									= app.TileHeights[y][x];
		float																		curHeight									= textureGridView[y][x].g / 255.0f;
		curTile.Heights[0]														= curHeight;
		curTile.Heights[1]														= curHeight;
		curTile.Heights[2]														= curHeight;
		curTile.Heights[3]														= curHeight;
	}
	for(uint32_t y = 0, yMax = (textureGridMetrics.y - 1); y < yMax; ++y)
	for(uint32_t x = 0, xMax = (textureGridMetrics.x - 1); x < xMax; ++x) {
		::STileHeights<float>														& curTile									= app.TileHeights[y		][x		];
		::STileHeights<float>														& frontTile									= app.TileHeights[y		][x + 1	];
		::STileHeights<float>														& leftTile									= app.TileHeights[y + 1	][x		];
		::STileHeights<float>														& flTile									= app.TileHeights[y + 1	][x + 1	];
		float																		averageHeight								= 
			( curTile	.Heights[3]
			+ frontTile	.Heights[2]
			+ leftTile	.Heights[1]
			+ flTile	.Heights[0]
			) / 4;
		curTile									.Heights[3]						= averageHeight;
		frontTile								.Heights[2]						= averageHeight;	
		leftTile								.Heights[1]						= averageHeight;	
		flTile									.Heights[0]						= averageHeight;
	}
	
	::gpk::SModelGeometry<float>												& geometryGrid								= app.Grid;
	::generateModelFromHeights(app.TileHeights.View, geometryGrid);
	//::gpk::generateGridGeometry(textureGridMetrics, geometryGrid);
	
	const ::gpk::SCoord3<float>													gridCenter									= {textureGridMetrics.x / 2.0f, 0, textureGridMetrics.y / 2.0f};
	
	for(uint32_t iTriangle = 0; iTriangle < geometryGrid.Positions.size(); ++iTriangle) {
		::gpk::STriangle3D<float>													& transformedTriangle						= geometryGrid.Positions	[iTriangle];
		transformedTriangle.A													-= gridCenter;
		transformedTriangle.B													-= gridCenter;
		transformedTriangle.C													-= gridCenter;
	}

	app.GridPivot.Orientation								= {0,0,0,1}; 

	::gpk::view_array<::gpk::SCoord3<float>>										positions									= {(::gpk::SCoord3<float>*)geometryGrid.Positions		.begin(), geometryGrid.Positions		.size() * 3};
	::gpk::view_array<::gpk::SCoord3<float>>										normals										= {(::gpk::SCoord3<float>*)geometryGrid.NormalsVertex	.begin(), geometryGrid.NormalsVertex	.size() * 3};
	//
	for(uint32_t iBlend = 0; iBlend < 10; ++iBlend) 
	for(uint32_t iPosition = 0; iPosition < positions.size(); ++iPosition) {
		const ::gpk::SCoord3<float>														& curPos									= positions	[iPosition];
		::gpk::SCoord3<float>															& curNormal									= normals	[iPosition];
		for(uint32_t iPosOther = iPosition + 1; iPosOther < positions.size(); ++iPosOther) {
			const ::gpk::SCoord3<float>														& otherPos									= positions	[iPosOther];
			if((curPos - otherPos).LengthSquared() < (0.001 * .001)) {
				::gpk::SCoord3<float>															& otherNormal								= normals	[iPosOther];
				otherNormal = curNormal														= ((otherNormal + curNormal) / 2).Normalize();
				break;
			}
		}
	}

	framework.Settings.GUIZoom											= false;
	app.Scene.Camera.Points.Position									= {30, 40, 0};
	app.Scene.Camera.NearFar.Far										= 1000;
	app.Scene.Camera.NearFar.Near										= 0.001;
	return 0;
}

			::gpk::error_t											update						(::gme::SApplication & app, bool exitSignal)	{
	::gpk::STimer															timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
	}
	::gpk::SFramework														& framework					= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");

	::gpk::SGUI																& gui						= *framework.GUI;
	::gpk::array_pod<uint32_t>												controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	for(uint32_t iProcessable = 0, countControls = controlsToProcess.size(); iProcessable < countControls; ++iProcessable) {
		uint32_t																iControl					= controlsToProcess[iProcessable];
		const ::gpk::SControlState												& controlState				= gui.Controls.States[iControl];
		if(controlState.Execute) {
			info_printf("Executed %u.", iControl);
			if(iControl == (uint32_t)app.IdExit)
				return 1;
		}
	}

	app.DialogMain.Update();

	//----------------------------------------------
	bool																		updateProjection							= false;
	::gpk::SFrameInfo															& frameInfo									= framework.FrameInfo;
	if(framework.Input->KeyboardCurrent.KeyState[VK_ADD			])	{ updateProjection = true; app.Scene.Camera.Angle += frameInfo.Seconds.LastFrame * .05f; }
	if(framework.Input->KeyboardCurrent.KeyState[VK_SUBTRACT	])	{ updateProjection = true; app.Scene.Camera.Angle -= frameInfo.Seconds.LastFrame * .05f; }
	//if(updateProjection) {
		::gpk::ptr_obj<::gpk::SDialogViewport>										viewport									= {};
		app.DialogMain.Controls[app.Viewport].as(viewport);
		const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= gui.Controls.Controls[viewport->IdClient].Area.Size.Cast<uint32_t>();
		::updateProjections(offscreenMetrics, app.Scene.Camera, app.Scene.Transforms);
	//}

	//------------------------------------------------ Camera
	::gpk::SCameraPoints														& camera									= app.Scene.Camera.Points;
	camera.Position.RotateY(framework.Input->MouseCurrent.Deltas.x / 20.0f / (framework.Input->KeyboardCurrent.KeyState[VK_CONTROL] ? 2.0 : 1));
	if(framework.Input->MouseCurrent.Deltas.z) {
		::gpk::SCoord3<float>														zoomVector									= camera.Position;
		zoomVector.Normalize();
		const double																zoomWeight									= framework.Input->MouseCurrent.Deltas.z * (framework.Input->KeyboardCurrent.KeyState[VK_SHIFT] ? 10 : 1) / 240.;
		camera.Position															+= zoomVector * zoomWeight * .5;
	}
	::gpk::SMatrix4<float>														& viewMatrix								= app.Scene.Transforms.View;
	::gpk::SCameraVectors														& cameraVectors								= app.Scene.Camera.Vectors;
	cameraVectors.Up														= {0, 1, 0};
	cameraVectors.Front														= (camera.Target - camera.Position).Normalize();
	cameraVectors.Right														= cameraVectors.Up		.Cross(cameraVectors.Front).Normalize();
	cameraVectors.Up														= cameraVectors.Front	.Cross(cameraVectors.Right).Normalize();
	viewMatrix.View3D(camera.Position, cameraVectors.Right, cameraVectors.Up, cameraVectors.Front);

	//------------------------------------------------ Lights
	::gpk::SCoord3<float>														& lightDir									= app.LightDirection;
	lightDir.RotateY(frameInfo.Microseconds.LastFrame / 1000000.0f);
	lightDir.Normalize();

	//------------------------------------------------ 
	app.GridPivot.Scale										= {2.f, 4.f, 2.f};
	app.GridPivot.Position									= {};

	timer.Frame();
	sprintf_s(app.StringFrameRateUpdate, "Last frame time (update): %fs.", (float)timer.LastTimeSeconds);
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	gui.Controls.Text[app.IdFrameRateUpdate].Text							= app.StringFrameRateUpdate;
	::gpk::controlMetricsInvalidate(gui, app.IdFrameRateUpdate);

	return 0;
}

			::gpk::error_t											drawGrids									(::gme::SApplication& applicationInstance, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>& target);
			::gpk::error_t											draw										(::gme::SApplication & app)							{
	::gpk::STimer															timer;
	::gpk::SFramework														& framework									= app.Framework;
	::gpk::SGUI																& gui										= *framework.GUI;
	static ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>				buffer3D;
	::gpk::ptr_obj<::gpk::SDialogViewport>									viewport									= {};
	app.DialogMain.Controls[app.Viewport].as(viewport);
	const ::gpk::SCoord2<uint32_t>											& offscreenMetrics							= gui.Controls.Controls[viewport->IdClient].Area.Size.Cast<uint32_t>();

	buffer3D.resize(offscreenMetrics, {0, 0, 0, 0}, 0xFFFFFFFF);
	gui.Controls.Controls[viewport->IdClient].Image						= buffer3D.Color.View;
	int32_t 																pixelsDrawn0								= drawGrids(app, buffer3D); 
	error_if(errored(pixelsDrawn0), "??");
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target->resize(app.Framework.MainDisplay.Size, ::gpk::LIGHTGRAY, 0xFFFFFFFFU);
	{
		::gpk::mutex_guard														lock					(app.Framework.LockGUI);
		::gpk::guiDraw(*app.DialogMain.GUI, target->Color.View);
	}
	{
		::gpk::mutex_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}
	timer.Frame();
	sprintf_s(app.StringFrameRateRender, "Last frame time (render): %fs.", (float)timer.LastTimeSeconds);
	gui.Controls.Text[app.IdFrameRateRender].Text							= app.StringFrameRateRender;
	::gpk::controlMetricsInvalidate(gui, app.IdFrameRateRender);
	return 0;
}
