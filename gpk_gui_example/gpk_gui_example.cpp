#include "gpk_gui_example.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_dialog_controls.h"
#include "gpk_matrix.h"
#include "gpk_bitmap_target.h"
#include "gpk_raster_lh.h"
#include "gpk_string.h"
#include "gpk_circle.h"
#include "gpk_sphere.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

// Vertex coordinates for cube faces
stacxpr	const ::gpk::tri3<float>					modelPositionsVertices	[12]						=
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

stacxpr	const ::gpk::tri3<float>						modelNormalVectors		[12]						=
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
					::gpk::error_t									indexValues									(const ::gpk::view<const _tValue> & in_values, ::gpk::array_pod<_tIndex> & out_indices, ::gpk::array_pod<_tValue> & out_values, ::gpk::array_pod<_tIndex> & out_remap){
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

static ::gpk::error_t gaugeBuildRadial(::gme::SUIRadialGauge & gauge, const ::gpk::circlef32 & gaugeMetrics, int16_t resolution, int16_t width) {
	const double				stepUnit						= (1.0 / resolution) * ::gpk::math_2pi;
	const ::gpk::spheref32		sphereMetrics					= {gaugeMetrics.Radius, {gaugeMetrics.Center.x, gaugeMetrics.Center.y, .5f}};
	const double				finalRadius						= gaugeMetrics.Radius;
	const double				outerRadius						= finalRadius;
	const double				innerRadius						= outerRadius - width;
	for(int16_t iStep = 0, stepCount = resolution; iStep < stepCount; ++iStep) {
		::gpk::SSinCos				sinCos							= {sin(iStep * stepUnit), -cos(iStep * stepUnit)};
		const ::gpk::n3f3_t			relativePosSmall				= {sinCos.Sin * innerRadius, sinCos.Cos * innerRadius};
		const ::gpk::n3f3_t			relativePos						= {sinCos.Sin * outerRadius, sinCos.Cos * outerRadius};
		gauge.Vertices.push_back(sphereMetrics.Center + relativePosSmall.f2_t());
		gauge.Vertices.push_back(sphereMetrics.Center + relativePos.f2_t());
	}
	for(int16_t iStep = 0, stepCount = (int16_t)(resolution * 2) - 3; iStep < stepCount; ++iStep) {
		gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), int16_t(2 + iStep)});
		gauge.Indices.push_back({int16_t(1 + iStep), int16_t(3 + iStep), int16_t(2 + iStep)});
	}
	const int16_t				iStep							= int16_t(gauge.Vertices.size() - 2);
	gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), 0});
	gauge.Indices.push_back({int16_t(1 + iStep), 1, 0});
	gauge.MaxValue			= resolution;
	return 0;
}

static ::gpk::error_t gaugeImageUpdate(::gme::SUIRadialGauge & gauge, ::gpk::g8bgra target, ::gpk::rgbaf colorMin, ::gpk::rgbaf colorMid, ::gpk::rgbaf colorMax, ::gpk::bgra colorEmpty, bool radialColor) {
	static ::gpk::imgu32		dummyDepth;
	const ::gpk::n3f2_t			center3							= (gauge.Vertices[1] - gauge.Vertices[gauge.Vertices.size() / 2 + 1]) / 2 + gauge.Vertices[gauge.Vertices.size() / 2 + 1];
	const ::gpk::n2f2_t			center2							= {center3.x, center3.y};
	const double				radiusLarge						= (center3 - gauge.Vertices[1]).Length();
	const double				radiusSmall						= (center3 - gauge.Vertices[0]).Length();
	const double				radiusHalfWidth					= (radiusLarge - radiusSmall) / 2;
	const double				radiusCenter						= radiusHalfWidth + radiusSmall;
	dummyDepth.resize(target.metrics(), 0xFFFFFFFFU);
	::gpk::apod<::gpk::n2s1_t>	pixelCoords;
	::gpk::apod<::gpk::trif32>	triangleWeights;
	for(uint32_t iTriangle = 0, triangleCount = gauge.Indices.size(); iTriangle < triangleCount; ++iTriangle) {
		const ::gpk::tri3f32			triangleCoords				= {gauge.Vertices[gauge.Indices[iTriangle].A], gauge.Vertices[gauge.Indices[iTriangle].B], gauge.Vertices[gauge.Indices[iTriangle].C]};
		const double					colorFactor					= radialColor ? ::gpk::min(1.0, iTriangle / (double)triangleCount) : gauge.CurrentValue / (float)gauge.MaxValue;
		::gpk::rgbaf					finalColor;
		const bool						isEmptyGauge				= ((iTriangle + 2) >> 1) >= (uint32_t)gauge.CurrentValue * 2;
		finalColor					= isEmptyGauge
			? ::gpk::rgbaf(colorEmpty)
			: (colorFactor < .5)
				? ::gpk::interpolate_linear(colorMin, colorMid, (::gpk::min(1.0, iTriangle * 2.25 / (double)triangleCount)))
				: ::gpk::interpolate_linear(colorMid, colorMax, (colorFactor - .5) * 2);
		finalColor.Clamp();
		pixelCoords.clear();
		::gpk::drawTriangle(target.metrics(), triangleCoords, pixelCoords, triangleWeights, dummyDepth);
		for(uint32_t iPixelCoords = 0; iPixelCoords < pixelCoords.size(); ++iPixelCoords) {
			const ::gpk::n2s1_t		pixelCoord					= pixelCoords[iPixelCoords];
			const ::gpk::n2f2_t		floatCoord					= pixelCoord.f2_t();
			const double				distanceFromCenter			= (floatCoord - center2).Length();
			const double				distanceFromRadiusCenter	= fabs(distanceFromCenter - radiusCenter) / radiusHalfWidth;
			finalColor.a				= (float)(1.f - distanceFromRadiusCenter);
			::gpk::bgra					& targetPixel				= target[pixelCoord.y][pixelCoord.x];
			targetPixel					= ::gpk::interpolate_linear(::gpk::rgbaf{targetPixel}, finalColor, finalColor.a);
			targetPixel.a				= uint8_t(finalColor.a * 255);
		}
	}
	return 0;
}

static ::gpk::cid_t gaugeCreate(::gpk::SGUI & gui, ::gme::SUIRadialGauge & gauge, ::gpk::cid_t parent, const ::gpk::n2u2_t & imageMetrics, const ::gpk::circlef32 & gaugeMetrics, int16_t resolution, int16_t width) {
	const ::gpk::cid_t idControl = ::gpk::controlCreate(gui);
	gpk_necs(idControl);
	gpk_necs(::gpk::controlSetParent(gui, idControl, parent));
	gpk_necs(gauge.Image.resize(imageMetrics));
	gpk_necs(::gaugeBuildRadial(gauge, gaugeMetrics, resolution, width));
	gui.Controls.Images[idControl].Image = gauge.Image.View;
	gui.Controls.Images[idControl].ImageAlign = ::gpk::ALIGN_CENTER;
	gui.Controls.Draw[idControl].NoClient = true;
	gui.Controls.Draw[idControl].NoBorder = true;
	return idControl;
}

template<typename _tId>
struct SControlParams {
	_tId							Id;
	::gpk::cid_t					Parent;
	::gpk::rect2i16					Area;
	::gpk::rectu16					Border;
	::gpk::rectu16					Margin;
	::gpk::ALIGN					Align;
	::gpk::vcst_t					Text;
	::gpk::ALIGN					TextAlign;
	::gpk::GUI_COLOR_MODE			ColorMode;
};

template<typename _tValue>
struct SMenuItem {
	_tValue							ReturnValue;
	::gpk::string					Text;
};

static const SMenuItem<::gme::GUI_EXAMPLE_KLIB_ACTION> klibMenuItems[] =
	{ {::gme::GUI_EXAMPLE_KLIB_ACTION_Production, "Available Production"}
	, {::gme::GUI_EXAMPLE_KLIB_ACTION_Research  , "Available Research"  }
	, {::gme::GUI_EXAMPLE_KLIB_ACTION_Upgrade   , "Upgrade"             }
	, {::gme::GUI_EXAMPLE_KLIB_ACTION_Exit      , "Exit this menu"      }
	};

static const SControlParams<::gme::GUI_EXAMPLE_CONTROL> controlList[] =
	{ {::gme::GUI_EXAMPLE_CONTROL_Frame_rate_update, -1, {{     }, {384, 20}}, {10, 10, 10, 10}, {1, 1, 1, 1}, ::gpk::ALIGN_TOP_LEFT     , ""          , ::gpk::ALIGN_CENTER     , ::gpk::GUI_COLOR_MODE_3D  }
	, {::gme::GUI_EXAMPLE_CONTROL_Frame_rate_render, -1, {{     }, {384, 20}}, {10, 10, 10, 10}, {1, 1, 1, 1}, ::gpk::ALIGN_TOP_RIGHT    , ""          , ::gpk::ALIGN_CENTER     , ::gpk::GUI_COLOR_MODE_3D  }
	, {::gme::GUI_EXAMPLE_CONTROL_Dryer_panel      , -1, {{0, 16}, {448, 88}}, { 2,  2,  2,  2}, {2, 2, 2, 2}, ::gpk::ALIGN_CENTER_BOTTOM, "DRYER DATA", ::gpk::ALIGN_CENTER_TOP , ::gpk::GUI_COLOR_MODE_Flat}
	, {::gme::GUI_EXAMPLE_CONTROL_Program_label    ,  2, {{8,  8}, {428, 32}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_TOP_LEFT     , "Program:"   , ::gpk::ALIGN_CENTER_LEFT, ::gpk::GUI_COLOR_MODE_Flat}
	, {::gme::GUI_EXAMPLE_CONTROL_Program_value    ,  3, {{     }, {160, 28}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_CENTER_RIGHT , "default"    , ::gpk::ALIGN_CENTER     , ::gpk::GUI_COLOR_MODE_3D  }
	, {::gme::GUI_EXAMPLE_CONTROL_Status_label     ,  2, {{8, 44}, {428, 32}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_TOP_LEFT     , "Status:"    , ::gpk::ALIGN_CENTER_LEFT, ::gpk::GUI_COLOR_MODE_Flat}
	, {::gme::GUI_EXAMPLE_CONTROL_Status_value     ,  5, {{     }, {160, 28}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_CENTER_RIGHT , "READY"      , ::gpk::ALIGN_CENTER     , ::gpk::GUI_COLOR_MODE_3D  }
	, {::gme::GUI_EXAMPLE_CONTROL_Klib_panel       , -1, {{16, 16}, {224, 140}}, { 2,  2,  2,  2}, {2, 2, 2, 2}, ::gpk::ALIGN_BOTTOM_LEFT  , "Order Menu" , ::gpk::ALIGN_CENTER_TOP , ::gpk::GUI_COLOR_MODE_Flat}
	, {::gme::GUI_EXAMPLE_CONTROL_Klib_selection   ,  7, {{ 8,108}, {208,  24}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_TOP_LEFT     , "No selection", ::gpk::ALIGN_CENTER     , ::gpk::GUI_COLOR_MODE_3D  }
	};

template<typename _tId>
static ::gpk::error_t buildControlTree(::gpk::SGUI & gui, ::gpk::acid & ids, ::gpk::view<const SControlParams<_tId>> params, uint32_t idCount) {
	gpk_necs(ids.resize(idCount));
	for(uint32_t iControl = 0; iControl < params.size(); ++iControl) {
		const SControlParams<_tId>	& param	= params[iControl];
		const ::gpk::cid_t	idControl	= ::gpk::controlCreate(gui);
		gpk_necs(idControl);
		ids[param.Id]						= idControl;
		::gpk::SControlPlacement	& placement	= gui.Controls.Placement[idControl];
		placement.Area						= param.Area;
		placement.Border					= param.Border;
		placement.Margin					= param.Margin;
		placement.Align						= param.Align;
		gui.Controls.Draw[idControl].ColorMode	= param.ColorMode;
		gui.Controls.Text[idControl].Align		= param.TextAlign;
		gpk_necs(::gpk::controlTextSet(gui, idControl, param.Text));
		gpk_necs(::gpk::controlSetParent(gui, idControl, -1 == param.Parent ? -1 : ids[param.Parent]));
	}
	return 0;
}

template<typename _tValue>
static ::gpk::error_t guiCreateControlList(::gpk::SGUI & gui, ::gpk::view<const SMenuItem<_tValue>> items, ::gpk::cid_t parent, const ::gpk::n2u1_t & buttonSize, const ::gpk::n2s1_t & offset, ::gpk::ALIGN align, ::gpk::acid & ids) {
	::gpk::aobj<::gpk::vcsc_t>	labels;
	gpk_necs(labels.resize(items.size()));
	for(uint32_t iItem = 0; iItem < labels.size(); ++iItem)
		labels[iItem] = items[iItem].Text;
	return ::gpk::guiCreateControlList(gui, labels, parent, buttonSize, offset, align, ::gpk::ALIGN_CENTER, ids);
}

::gpk::error_t			cleanup		(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
::gpk::error_t			setup		(::gme::SApplication & app)						{
	app.Buffer3D.create();
	::gpk::SFramework				& framework									= app.Framework;
	::gpk::SWindow					& mainWindow								= framework.RootWindow;
	app.Framework.GUI													= app.DialogMain.GUI;
	app.DialogMain.Input												= mainWindow.Input;
	framework.RootWindow.Size												= {1280, 720};
	es_if(::gpk::failed(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	::gpk::SGUI						& gui										= *framework.GUI;
	gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
	gpk_necs(::gpk::guiCreateControlList<::gme::GUI_EXAMPLE_ACTION>(gui, -1, {160, 24}, {16, -24}, ::gpk::ALIGN_CENTER_LEFT, ::gpk::ALIGN_CENTER, app.IdActions));
	gpk_necs(::gpk::inputBoxCreate(app.InputBox, gui));
	gui.Controls.Placement[app.InputBox.IdRoot].Align						= ::gpk::ALIGN_CENTER_BOTTOM;
	gpk_necs(app.InputBox.SetText(gui, "Viewport"));
	gpk_necs(app.InputBox.Edit(gui, false));

	gpk_necs(::buildControlTree<::gme::GUI_EXAMPLE_CONTROL>(gui, app.IdControls, {controlList, ::gpk::size(controlList)}, ::gme::GUI_EXAMPLE_CONTROL_COUNT));
	gpk_necs(::guiCreateControlList<::gme::GUI_EXAMPLE_KLIB_ACTION>(gui, {klibMenuItems, ::gpk::size(klibMenuItems)}, app.IdControls[::gme::GUI_EXAMPLE_CONTROL_Klib_panel], {208, 20}, {8, 24}, ::gpk::ALIGN_TOP_LEFT, app.IdKlibActions));

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
	slider->ValueCurrent										= 192;

	app.GaugeControl											= ::gaugeCreate(gui, app.Gauge, -1, {112, 112}, {48, {56, 56}}, 32, 12);
	gpk_necs(app.GaugeControl);
	controlTable.Placement[app.GaugeControl].Area				= {{16, 64}, {112, 112}};

	::gpk::pobj<::gpk::SDialogCheckBox>									checkbox									= {};
	app.CheckBox														= ::gpk::checkBoxCreate(app.DialogMain, checkbox);
	controlTable.Placement[checkbox->IdGUIControl].Area.Offset			= {128, 256};

	::gpk::pobj<::gpk::SDialogViewport>									viewport									= {};
	app.Viewport														= ::gpk::viewportCreate(app.DialogMain, viewport);
	controlTable.Placement	[viewport->IdGUIControl	].Area.Offset			= {320, 128};
	controlTable.Placement	[viewport->IdGUIControl	].Area.Size				= {640, 480};
	controlTable.Images		[viewport->IdClient		].ImageInvertY			= true;

	stacxpr const ::gpk::n3f2_t							cubeCenter									= {0.5f, 0.5f, 0.5f};
	::gpk::array_pod<uint8_t>												& remap										= app.ModelGeometry.PositionRemap;
	::indexValues({&modelPositionsVertices[0].A, ::gpk::size(modelPositionsVertices) * 3}, app.ModelGeometry.Positions.Indices, app.ModelGeometry.Positions.Values, remap);
	for(uint32_t iVertex = 0; iVertex < app.ModelGeometry.Positions.Values.size(); ++iVertex) {
		::gpk::n3f2_t													& vertexToTransform					= app.ModelGeometry.Positions.Values[iVertex];
		vertexToTransform													-= cubeCenter;
	}
	app.ModelGeometry.NormalsVertex.Values									= modelNormalVectors;
	return 0;
}

::gpk::error_t			update				(::gme::SApplication & app, bool exitSignal)	{
	static ::gpk::STimer		timer;
	rvis_if(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard			lock			(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	::gpk::SFramework			& framework		= app.Framework;
	rvis_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui			= *framework.GUI;
	::gpk::acid					toProcess		= {};
	gpk_necs(::gpk::guiGetProcessableControls(gui, toProcess));
	const ::gpk::error_t		inputBoxResult	= app.InputBox.Update(gui, app.InputBox.VirtualKeyboard, framework.RootWindow.EventQueue, toProcess);
	gpk_necs(inputBoxResult);
	if(INT_MAX == inputBoxResult) {
		::gpk::pobj<::gpk::SDialogViewport>	viewport;
		app.DialogMain.Controls[app.Viewport].as(viewport);
		gpk_necs(::gpk::controlTextSet(gui, viewport->IdTitle, ::gpk::vcs{app.InputBox.Text}));
		gpk_necs(app.InputBox.Edit(gui, false));
	}
	else if(0 == inputBoxResult) {
		bool					exitRequested	= false;
		gpk_necs(::gpk::guiProcessControls(gui, toProcess, [&app, &gui, &exitRequested](::gpk::cid_t iControl) {
			if(iControl == app.IdActions[::gme::GUI_EXAMPLE_ACTION_Exit])
				exitRequested	= true;
			else if(iControl == app.IdActions[::gme::GUI_EXAMPLE_ACTION_Edit_label])
				return app.InputBox.Edit(gui, true);
			else if(iControl == app.IdActions[::gme::GUI_EXAMPLE_ACTION_Fold_viewport]) {
				::gpk::pobj<::gpk::SDialogViewport>	viewport;
				app.DialogMain.Controls[app.Viewport].as(viewport);
				return ::gpk::viewportFold(*viewport, viewport->Settings.Unfolded);
			}
			for(uint32_t iItem = 0; iItem < app.IdKlibActions.size(); ++iItem)
				if(iControl == app.IdKlibActions[iItem]) {
					const SMenuItem<::gme::GUI_EXAMPLE_KLIB_ACTION> & item = klibMenuItems[iItem];
					return ::gpk::controlTextSet(gui, app.IdControls[::gme::GUI_EXAMPLE_CONTROL_Klib_selection], ::gme::GUI_EXAMPLE_KLIB_ACTION_Exit == item.ReturnValue ? ::gpk::vcst_t{"WELCOME COMMANDER"} : ::gpk::vcst_t{item.Text});
				}
			return 0;
		}));
		rvis_if(::gpk::APPLICATION_STATE_EXIT, exitRequested);
	}

	app.DialogMain.Update();
	::gpk::pobj<::gpk::SDialogSlider>	slider;
	app.DialogMain.Controls[app.Slider].as(slider);
	app.Gauge.SetValue((float)slider->ValueCurrent, (uint32_t)slider->ValueLimits.Max);
	memset(app.Gauge.Image.View.begin(), 0, app.Gauge.Image.View.byte_count());
	gpk_necs(::gaugeImageUpdate(app.Gauge, app.Gauge.Image.View, ::gpk::RED, ::gpk::YELLOW, ::gpk::LIGHTGREEN, ::gme::SUIRadialGauge::COLOR_BKG_DEFAULT, false));

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
	gui.Controls.Text[app.IdControls[::gme::GUI_EXAMPLE_CONTROL_Frame_rate_update]].Text	= app.StringFrameRateUpdate;
	::gpk::controlMetricsInvalidate(gui, app.IdControls[::gme::GUI_EXAMPLE_CONTROL_Frame_rate_update]);
	return 0;
}


::gpk::error_t			draw			(::gme::SApplication & app)							{
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
		//es_if(::gpk::failed(::gpk::drawTriangle(buffer3d->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])));
		trianglePixelCoords.clear();
		::gpk::tri3<float>												transformedTriangle3D						= {};
		transformedTriangle3D.A												= {(float)triangle2dList[iTriangle].A.x, (float)triangle2dList[iTriangle].A.y, triangle3dList[iTriangle].A.z};
		transformedTriangle3D.B												= {(float)triangle2dList[iTriangle].B.x, (float)triangle2dList[iTriangle].B.y, triangle3dList[iTriangle].B.z};
		transformedTriangle3D.C												= {(float)triangle2dList[iTriangle].C.x, (float)triangle2dList[iTriangle].C.y, triangle3dList[iTriangle].C.z};
		es_if(::gpk::failed(::gpk::drawTriangle(offscreenMetrics.Cast<uint32_t>(), transformedTriangle3D, trianglePixelCoords, triangleWeights, buffer3d->DepthStencil.View)));
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
	::gpk::controlTextSet(gui, app.IdControls[::gme::GUI_EXAMPLE_CONTROL_Frame_rate_render], app.StringFrameRateRender);
	return 0;
}
