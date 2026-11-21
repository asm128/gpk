#include "gpk_gui_example.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
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
					gpk::err_t									indexValues									(const ::gpk::view<const _tValue> & in_values, ::gpk::array_pod<_tIndex> & out_indices, ::gpk::array_pod<_tValue> & out_values, ::gpk::array_pod<_tIndex> & out_remap){
	for(::gpk::u2_t iValue = 0; iValue < in_values.size(); ++iValue) {
		bool																	bFound										= false;
		const _tValue															& in_value									= in_values[iValue];
		for(::gpk::u2_t iValueIndexed = 0; iValueIndexed < out_values.size(); ++iValueIndexed) {
			const _tValue															& out_value									= out_values[iValueIndexed];
			if(in_value == out_value) {
				bFound																= true;
				out_indices	.push_back((_tIndex)iValueIndexed);
				out_remap	.push_back((_tIndex)iValue);
				break;
			}
		}
		if(false == bFound) {
			::gpk::s2_t																	newIndex;
			if_fail_fef(newIndex = out_values.push_back(in_value), "%s", "Out of memory?");
			out_indices	.push_back((_tIndex)newIndex);
			out_remap	.push_back((_tIndex)iValue);
		}
	}
	return 0;
}

static gpk::err_t gaugeBuildRadial(::gme::SUIRadialGauge & gauge, const ::gpk::circlef2_t & gaugeMetrics, ::gpk::s1_t resolution, ::gpk::s1_t width) {
	const double				stepUnit						= (1.0 / resolution) * ::gpk::math_2pi;
	const ::gpk::spheref2_t		sphereMetrics					= {gaugeMetrics.Radius, {gaugeMetrics.Center.x, gaugeMetrics.Center.y, .5f}};
	const double				finalRadius						= gaugeMetrics.Radius;
	const double				outerRadius						= finalRadius;
	const double				innerRadius						= outerRadius - width;
	for(::gpk::s1_t iStep = 0, stepCount = resolution; iStep < stepCount; ++iStep) {
		::gpk::SSinCos				sinCos							= {sin(iStep * stepUnit), -cos(iStep * stepUnit)};
		const ::gpk::n3f3_t			relativePosSmall				= {sinCos.Sin * innerRadius, sinCos.Cos * innerRadius};
		const ::gpk::n3f3_t			relativePos						= {sinCos.Sin * outerRadius, sinCos.Cos * outerRadius};
		gauge.Vertices.push_back(sphereMetrics.Center + relativePosSmall.f2());
		gauge.Vertices.push_back(sphereMetrics.Center + relativePos.f2());
	}
	for(::gpk::s1_t iStep = 0, stepCount = (::gpk::s1_t)(resolution * 2) - 3; iStep < stepCount; ++iStep) {
		gauge.Indices.push_back({::gpk::s1_t(0 + iStep), ::gpk::s1_t(1 + iStep), ::gpk::s1_t(2 + iStep)});
		gauge.Indices.push_back({::gpk::s1_t(1 + iStep), ::gpk::s1_t(3 + iStep), ::gpk::s1_t(2 + iStep)});
	}
	const ::gpk::s1_t				iStep							= ::gpk::s1_t(gauge.Vertices.size() - 2);
	gauge.Indices.push_back({::gpk::s1_t(0 + iStep), ::gpk::s1_t(1 + iStep), 0});
	gauge.Indices.push_back({::gpk::s1_t(1 + iStep), 1, 0});
	gauge.MaxValue			= resolution;
	return 0;
}

static gpk::err_t gaugeImageUpdate(::gme::SUIRadialGauge & gauge, ::gpk::g8bgra target, ::gpk::rgbaf colorMin, ::gpk::rgbaf colorMid, ::gpk::rgbaf colorMax, ::gpk::bgra colorEmpty, bool radialColor) {
	static ::gpk::imgu2_t		dummyDepth;
	const ::gpk::n3f2_t			center3							= (gauge.Vertices[1] - gauge.Vertices[gauge.Vertices.size() / 2 + 1]) / 2 + gauge.Vertices[gauge.Vertices.size() / 2 + 1];
	const ::gpk::n2f2_t			center2							= {center3.x, center3.y};
	const double				radiusLarge						= (center3 - gauge.Vertices[1]).Length();
	const double				radiusSmall						= (center3 - gauge.Vertices[0]).Length();
	const double				radiusHalfWidth					= (radiusLarge - radiusSmall) / 2;
	const double				radiusCenter						= radiusHalfWidth + radiusSmall;
	dummyDepth.resize(target.metrics(), 0xFFFFFFFFU);
	::gpk::apod<::gpk::n2s1_t>	pixelCoords;
	::gpk::apod<::gpk::trif2_t>	triangleWeights;
	for(::gpk::u2_t iTriangle = 0, triangleCount = gauge.Indices.size(); iTriangle < triangleCount; ++iTriangle) {
		const ::gpk::tri3f2_t			triangleCoords				= {gauge.Vertices[gauge.Indices[iTriangle].A], gauge.Vertices[gauge.Indices[iTriangle].B], gauge.Vertices[gauge.Indices[iTriangle].C]};
		const double					colorFactor					= radialColor ? ::gpk::min(1.0, iTriangle / (double)triangleCount) : gauge.CurrentValue / (float)gauge.MaxValue;
		::gpk::rgbaf					finalColor;
		const bool						isEmptyGauge				= ((iTriangle + 2) >> 1) >= (::gpk::u2_t)gauge.CurrentValue * 2;
		finalColor					= isEmptyGauge
			? ::gpk::rgbaf(colorEmpty)
			: (colorFactor < .5)
				? ::gpk::interpolate_linear(colorMin, colorMid, (::gpk::min(1.0, iTriangle * 2.25 / (double)triangleCount)))
				: ::gpk::interpolate_linear(colorMid, colorMax, (colorFactor - .5) * 2);
		finalColor.Clamp();
		pixelCoords.clear();
		::gpk::drawTriangle(target.metrics(), triangleCoords, pixelCoords, triangleWeights, dummyDepth);
		for(::gpk::u2_t iPixelCoords = 0; iPixelCoords < pixelCoords.size(); ++iPixelCoords) {
			const ::gpk::n2s1_t		pixelCoord					= pixelCoords[iPixelCoords];
			const ::gpk::n2f2_t		floatCoord					= pixelCoord.f2();
			const double				distanceFromCenter			= (floatCoord - center2).Length();
			const double				distanceFromRadiusCenter	= fabs(distanceFromCenter - radiusCenter) / radiusHalfWidth;
			finalColor.a				= (float)(1.f - distanceFromRadiusCenter);
			::gpk::bgra					& targetPixel				= target[pixelCoord.y][pixelCoord.x];
			targetPixel					= ::gpk::interpolate_linear(::gpk::rgbaf{targetPixel}, finalColor, finalColor.a);
			targetPixel.a				= ::gpk::u0_t(finalColor.a * 255);
		}
	}
	return 0;
}

static ::gpk::cid_t gaugeCreate(::gpk::SGUI & gui, ::gme::SUIRadialGauge & gauge, ::gpk::cid_t parent, const ::gpk::n2u2_t & imageMetrics, const ::gpk::circlef2_t & gaugeMetrics, ::gpk::s1_t resolution, ::gpk::s1_t width) {
	const ::gpk::cid_t idControl = ::gpk::controlCreate(gui);
	if_fail_fe(idControl);
	if_fail_fe(::gpk::controlSetParent(gui, idControl, parent));
	if_fail_fe(gauge.Image.resize(imageMetrics));
	if_fail_fe(::gaugeBuildRadial(gauge, gaugeMetrics, resolution, width));
	gui.Controls.ImageViews[idControl] = gauge.Image.View;
	gui.Controls.Images[idControl].ImageAlign = ::gpk::ALIGN_CENTER;
	gui.Controls.Draw[idControl].NoClient = true;
	gui.Controls.Draw[idControl].NoBorder = true;
	return idControl;
}

template<typename _tId>
struct SControlParams {
	_tId							Id;
	::gpk::cid_t					Parent;
	::gpk::rect2s1_t					Area;
	::gpk::rectu1_t					Border;
	::gpk::rectu1_t					Margin;
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
	, {::gme::GUI_EXAMPLE_CONTROL_Dryer_panel      , -1, {{0, 32}, {448, 88}}, { 2,  2,  2,  2}, {2, 2, 2, 2}, ::gpk::ALIGN_TOP_RIGHT    , "DRYER DATA", ::gpk::ALIGN_CENTER_TOP , ::gpk::GUI_COLOR_MODE_Flat}
	, {::gme::GUI_EXAMPLE_CONTROL_Program_label    ,  2, {{8,  8}, {428, 32}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_TOP_LEFT     , "Program:"   , ::gpk::ALIGN_CENTER_LEFT, ::gpk::GUI_COLOR_MODE_Flat}
	, {::gme::GUI_EXAMPLE_CONTROL_Program_value    ,  3, {{     }, {160, 28}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_CENTER_RIGHT , "default"    , ::gpk::ALIGN_CENTER     , ::gpk::GUI_COLOR_MODE_3D  }
	, {::gme::GUI_EXAMPLE_CONTROL_Status_label     ,  2, {{8, 44}, {428, 32}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_TOP_LEFT     , "Status:"    , ::gpk::ALIGN_CENTER_LEFT, ::gpk::GUI_COLOR_MODE_Flat}
	, {::gme::GUI_EXAMPLE_CONTROL_Status_value     ,  5, {{     }, {160, 28}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_CENTER_RIGHT , "READY"      , ::gpk::ALIGN_CENTER     , ::gpk::GUI_COLOR_MODE_3D  }
	, {::gme::GUI_EXAMPLE_CONTROL_Klib_panel       , -1, {{16, 16}, {224, 140}}, { 2,  2,  2,  2}, {2, 2, 2, 2}, ::gpk::ALIGN_BOTTOM_LEFT  , "Order Menu" , ::gpk::ALIGN_CENTER_TOP , ::gpk::GUI_COLOR_MODE_Flat}
	, {::gme::GUI_EXAMPLE_CONTROL_Klib_selection   ,  7, {{ 8,108}, {208,  24}}, { 1,  1,  1,  1}, {2, 2, 2, 2}, ::gpk::ALIGN_TOP_LEFT     , "No selection", ::gpk::ALIGN_CENTER     , ::gpk::GUI_COLOR_MODE_3D  }
	};

template<typename _tId>
static gpk::err_t buildControlTree(::gpk::SGUI & gui, ::gpk::acid & ids, ::gpk::view<const SControlParams<_tId>> params, ::gpk::u2_t idCount) {
	if_fail_fe(ids.resize(idCount));
	for(::gpk::u2_t iControl = 0; iControl < params.size(); ++iControl) {
		const SControlParams<_tId>	& param	= params[iControl];
		const ::gpk::cid_t	idControl	= ::gpk::controlCreate(gui);
		if_fail_fe(idControl);
		ids[param.Id]						= idControl;
		::gpk::SControlPlacement	& placement	= gui.Controls.Placement[idControl];
		placement.Area						= param.Area;
		placement.Border					= param.Border;
		placement.Margin					= param.Margin;
		placement.Align						= param.Align;
		gui.Controls.Draw[idControl].ColorMode	= param.ColorMode;
		gui.Controls.Text[idControl].Align		= param.TextAlign;
		if_fail_fe(::gpk::controlTextSet(gui, idControl, param.Text));
		if_fail_fe(::gpk::controlSetParent(gui, idControl, -1 == param.Parent ? -1 : ids[param.Parent]));
	}
	return 0;
}

template<typename _tValue>
static gpk::err_t guiCreateControlList(::gpk::SGUI & gui, ::gpk::view<const SMenuItem<_tValue>> items, ::gpk::cid_t parent, const ::gpk::n2u1_t & buttonSize, const ::gpk::n2s1_t & offset, ::gpk::ALIGN align, ::gpk::acid & ids) {
	::gpk::aobj<::gpk::vcsc_t>	labels;
	if_fail_fe(labels.resize(items.size()));
	for(::gpk::u2_t iItem = 0; iItem < labels.size(); ++iItem)
		labels[iItem] = items[iItem].Text;
	return ::gpk::guiCreateControlList(gui, labels, parent, buttonSize, offset, align, ::gpk::ALIGN_CENTER, ids);
}

gpk::err_t			cleanup		(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
gpk::err_t			setup		(::gme::SApplication & app)						{
	app.Buffer3D.create();
	::gpk::SFramework				& framework									= app.Framework;
	::gpk::SWindow					& mainWindow								= framework.RootWindow;
	framework.RootWindow.Size												= {1280, 720};
	if_true_e(::gpk::failed(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	::gpk::SGUI						& gui										= *framework.GUI;
	gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ::gpk::ASCII_COLOR_LIGHTGREY * 16;
	if_fail_fe(::gpk::guiCreateControlList<::gme::GUI_EXAMPLE_ACTION>(gui, -1, {160, 24}, {16, -24}, ::gpk::ALIGN_CENTER_LEFT, ::gpk::ALIGN_CENTER, app.IdActions));
	if_fail_fe(::gpk::inputBoxCreate(app.InputBox, gui));
	gui.Controls.Placement[app.InputBox.IdRoot].Align						= ::gpk::ALIGN_CENTER_BOTTOM;
	if_fail_fe(app.InputBox.SetText(gui, "Viewport"));
	if_fail_fe(app.InputBox.Edit(gui, false));

	if_fail_fe(::buildControlTree<::gme::GUI_EXAMPLE_CONTROL>(gui, app.IdControls, {controlList, ::gpk::size(controlList)}, ::gme::GUI_EXAMPLE_CONTROL_COUNT));
	if_fail_fe(::guiCreateControlList<::gme::GUI_EXAMPLE_KLIB_ACTION>(gui, {klibMenuItems, ::gpk::size(klibMenuItems)}, app.IdControls[::gme::GUI_EXAMPLE_CONTROL_Klib_panel], {208, 20}, {8, 24}, ::gpk::ALIGN_TOP_LEFT, app.IdKlibActions));

	::gpk::STuner<uint8_t> & tuner = app.NumericTuner;
	if_fail_fe(::gpk::tunerCreate(gui, tuner));
	tuner.ValueLimits.Min										= 100;
	tuner.ValueLimits.Max										= 200;
	::gpk::SControlTable											 & controlTable								= gui.Controls;
	controlTable.Placement[tuner.IdControl].Area.Offset		= {128, 192};
	controlTable.Placement[tuner.IdControl].Area.Size.x		= 128;
	controlTable.Placement[tuner.IdControl].Area.Size.y		=  20;
	tuner.SetValue(gui, 0);

	::gpk::SSlider & slider = app.Slider;
	if_fail_fe(::gpk::sliderCreate(gui, slider));
	slider.ValueLimits.Min										= 0;
	slider.ValueLimits.Max										= 255;
	controlTable.Placement[slider.IdControl].Area.Offset	= {128, 128};
	controlTable.Placement[slider.IdControl].Area.Size.x	= 128;
	controlTable.Placement[slider.IdControl].Area.Size.y	= 8;
	slider.ValueCurrent										= 192;

	app.GaugeControl											= ::gaugeCreate(gui, app.Gauge, -1, {112, 112}, {48, {56, 56}}, 32, 12);
	if_fail_fe(app.GaugeControl);
	controlTable.Placement[app.GaugeControl].Area				= {{16, 64}, {112, 112}};

	::gpk::SCheckBox & checkbox = app.CheckBox;
	if_fail_fe(::gpk::checkBoxCreate(gui, checkbox));
	controlTable.Placement[checkbox.IdControl].Area.Offset			= {128, 256};

	::gpk::SViewport & viewport = app.Viewport;
	if_fail_fe(::gpk::viewportCreate(gui, viewport));
	const ::gpk::cid_t idLayer = ::gpk::controlCreate(gui);
	if_fail_fe(idLayer);
	controlTable.Draw[idLayer].NoClient = controlTable.Draw[idLayer].NoBorder = true;
	controlTable.Placement[idLayer].Margin = controlTable.Placement[idLayer].Border = {};
	controlTable.Constraints[idLayer].AttachSizeToControl = {idLayer, idLayer};
	controlTable.States[idLayer].Mask &= ~::gpk::GUI_CONTROL_FLAG_Hovered;
	if_fail_fe(::gpk::controlSetParent(gui, viewport.IdControl, idLayer));
	if_fail_fe(::gpk::controlSetParent(gui, app.InputBox.IdRoot, idLayer));
	controlTable.Placement	[viewport.IdControl	].Area.Offset			= {320, 128};
	controlTable.Placement	[viewport.IdControl	].Area.Size				= {640, 480};
	controlTable.Images		[viewport.IdControls[::gpk::VIEWPORT_CONTROL_TARGET]		].ImageInvertY			= true;

	stxp const ::gpk::n3f2_t							cubeCenter									= {0.5f, 0.5f, 0.5f};
	::gpk::array_pod<::gpk::u0_t>												& remap										= app.ModelGeometry.PositionRemap;
	::indexValues({&modelPositionsVertices[0].A, ::gpk::size(modelPositionsVertices) * 3}, app.ModelGeometry.Positions.Indices, app.ModelGeometry.Positions.Values, remap);
	for(::gpk::u2_t iVertex = 0; iVertex < app.ModelGeometry.Positions.Values.size(); ++iVertex) {
		::gpk::n3f2_t													& vertexToTransform					= app.ModelGeometry.Positions.Values[iVertex];
		vertexToTransform													-= cubeCenter;
	}
	app.ModelGeometry.NormalsVertex.Values									= modelNormalVectors;
	return 0;
}

gpk::err_t			update				(::gme::SApplication & app, bool exitSignal)	{
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
	if_fail_fe(::gpk::guiGetProcessableControls(gui, toProcess));
	const gpk::err_t		inputBoxResult	= app.InputBox.Update(gui, app.InputBox.VirtualKeyboard, framework.RootWindow.EventQueue, toProcess);
	if_fail_fe(inputBoxResult);
	if(INT_MAX == inputBoxResult) {
		::gpk::SViewport & viewport = app.Viewport;
		if_fail_fe(::gpk::controlTextSet(gui, viewport.IdControls[::gpk::VIEWPORT_CONTROL_TITLE], ::gpk::vcst_t{app.InputBox.Text}));
		if_fail_fe(app.InputBox.Edit(gui, false));
	}
	else if(0 == inputBoxResult) {
		bool					exitRequested	= false;
		if_fail_fe(::gpk::guiProcessControls(gui, toProcess, [&app, &gui, &exitRequested](::gpk::cid_t iControl) {
			if(iControl == app.IdActions[::gme::GUI_EXAMPLE_ACTION_Exit])
				exitRequested	= true;
			else if(iControl == app.IdActions[::gme::GUI_EXAMPLE_ACTION_Edit_label])
				return app.InputBox.Edit(gui, true);
			else if(iControl == app.IdActions[::gme::GUI_EXAMPLE_ACTION_Fold_viewport]) {
				::gpk::SViewport & viewport = app.Viewport;
				return ::gpk::viewportFold(gui, viewport, viewport.Settings.Unfolded);
			}
			for(::gpk::u2_t iItem = 0; iItem < app.IdKlibActions.size(); ++iItem)
				if(iControl == app.IdKlibActions[iItem]) {
					const SMenuItem<::gme::GUI_EXAMPLE_KLIB_ACTION> & item = klibMenuItems[iItem];
					return ::gpk::controlTextSet(gui, app.IdControls[::gme::GUI_EXAMPLE_CONTROL_Klib_selection], ::gme::GUI_EXAMPLE_KLIB_ACTION_Exit == item.ReturnValue ? ::gpk::vcst_t{"WELCOME COMMANDER"} : ::gpk::vcst_t{item.Text});
				}
			return 0;
		}));
		if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitRequested);
	}

	if_fail_fe(app.NumericTuner.Update(gui, gui.Controls.EventQueue));
	if_fail_fe(::gpk::sliderUpdate(gui, *framework.RootWindow.Input, app.Slider));
	if_fail_fe(::gpk::checkBoxUpdate(gui, app.CheckBox));
	if_fail_fe(::gpk::viewportUpdate(gui, *framework.RootWindow.Input, app.Viewport));
	::gpk::SSlider & slider = app.Slider;
	app.Gauge.SetValue((float)slider.ValueCurrent, (uint32_t)slider.ValueLimits.Max);
	memset(app.Gauge.Image.View.begin(), 0, app.Gauge.Image.View.byte_count());
	if_fail_fe(::gaugeImageUpdate(app.Gauge, app.Gauge.Image.View, ::gpk::RED, ::gpk::YELLOW, ::gpk::LIGHTGREEN, ::gme::SUIRadialGauge::COLOR_BKG_DEFAULT, false));

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
		::gpk::SViewport & viewport = app.Viewport;
		const ::gpk::n2u2_t											& offscreenMetrics							= gui.Controls.Placement[viewport.IdControls[::gpk::VIEWPORT_CONTROL_TARGET]].Area.Size.Cast<uint32_t>();
		scene.Projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar);
		scene.Projection															= scene.ViewMatrix * scene.Projection;
		scene.LightPos.Normalize();

		::gpk::m4f2_t													mViewport									= {};
		mViewport(0, 0)														= 2.0f / offscreenMetrics.x;
		mViewport(1, 1)														= 2.0f / offscreenMetrics.y;
		mViewport(2, 2)														= 1.0f / (float)(nearFar.Max - nearFar.Min);
		mViewport(3, 2)														= (float)(-nearFar.Min * ( 1.0f / (nearFar.Max - nearFar.Min) ));
		mViewport(3, 3)														= 1.0f;
		scene.Projection													= scene.Projection * mViewport.GetInverse();
	}

	timer.Frame();
	snprintf(app.StringFrameRateUpdate, ::gpk::size(app.StringFrameRateUpdate) - 2, "Last frame time (update): %fs.", (float)timer.LastTimeSeconds);
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	gui.Controls.Text[app.IdControls[::gme::GUI_EXAMPLE_CONTROL_Frame_rate_update]].Text	= app.StringFrameRateUpdate;
	::gpk::controlMetricsInvalidate(gui, app.IdControls[::gme::GUI_EXAMPLE_CONTROL_Frame_rate_update]);
	return 0;
}


gpk::err_t			draw			(::gme::SApplication & app)							{
	static ::gpk::STimer		timer;
	::gpk::SFramework			& framework		= app.Framework;
	::gpk::SGUI					& gui			= *framework.GUI;

	::gpk::pobj<::gpk::rtbgra8d32>	buffer3d	= app.Buffer3D;
	::gpk::SViewport & viewport = app.Viewport;
	const ::gpk::n2<uint16_t>											& offscreenMetrics							= gui.Controls.Placement[viewport.IdControls[::gpk::VIEWPORT_CONTROL_TARGET]].Area.Size.Cast<uint16_t>();
	buffer3d->resize(offscreenMetrics.Cast<uint32_t>(), {0, 0, 0, 0}, (uint32_t)-1);

	::gpk::array_pod<::gpk::tri3<float>>								& triangle3dList							= app.VertexCache.Triangle3dTransformed	;
	::gpk::array_pod<::gpk::bgra>										& triangle3dColorList						= app.VertexCache.Triangle3dColorList	;
	const ::gpk::u2_t															countTriangles								= app.ModelGeometry.Positions.Indices.size() / 3;
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
	for(::gpk::u2_t iTriangle = 0; iTriangle < countTriangles; ++iTriangle) {
		::gpk::tri3<float>												& transformedTriangle						= triangle3dList[iTriangle];
		transformedTriangle													=
			{ app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 0]]
			, app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 1]]
			, app.ModelGeometry.Positions.Values[app.ModelGeometry.Positions.Indices[iTriangle * 3 + 2]]
			};
		::gpk::transform(transformedTriangle, projection);
	}
	::gpk::array_pod<::gpk::tri2<::gpk::s2_t>>							triangle2dList								= {};
	triangle2dList.resize(countTriangles);
	const ::gpk::n2<::gpk::s2_t>											screenCenter								= {(::gpk::s2_t)offscreenMetrics.x / 2, (::gpk::s2_t)offscreenMetrics.y / 2};
	for(::gpk::u2_t iTriangle = 0; iTriangle < countTriangles; ++iTriangle) { // Maybe the scale
		::gpk::tri3<float>												& transformedTriangle3D						= triangle3dList[iTriangle];
		::gpk::tri2<::gpk::s2_t>												& transformedTriangle2D						= triangle2dList[iTriangle];
		transformedTriangle2D.A												= {(::gpk::s2_t)transformedTriangle3D.A.x, (::gpk::s2_t)transformedTriangle3D.A.y};
		transformedTriangle2D.B												= {(::gpk::s2_t)transformedTriangle3D.B.x, (::gpk::s2_t)transformedTriangle3D.B.y};
		transformedTriangle2D.C												= {(::gpk::s2_t)transformedTriangle3D.C.x, (::gpk::s2_t)transformedTriangle3D.C.y};
		::gpk::translate(transformedTriangle2D, screenCenter);

	}

	::gpk::view<::gpk::n3f2_t>								normals										= {(::gpk::n3f2_t*)app.ModelGeometry.NormalsVertex.Values.begin(), app.ModelGeometry.NormalsVertex.Values.size() * 3};
	for(::gpk::u2_t iVertex = 0; iVertex < app.ModelGeometry.Positions.Indices.size(); ++iVertex) {
		double																	lightFactor									= normals[iVertex].Dot(lightPos);
		triangle3dColorList[iVertex]										= (::gpk::RED * lightFactor).Clamp();
	}
	::gpk::array_pod<::gpk::n2<::gpk::s1_t>>								wireframePixelCoords;
	::gpk::array_pod<::gpk::n2<::gpk::s1_t>>								trianglePixelCoords;
	::gpk::n3f2_t													cameraFront										= (camera.Target - camera.Position).Normalize();
	::gpk::array_pod<::gpk::tri<float>>								triangleWeights;

	for(::gpk::u2_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																	lightFactor									= normals[iTriangle * 3].Dot(cameraFront);
		if(lightFactor > 0)
			continue;
		//if_true_e(::gpk::failed(::gpk::drawTriangle(buffer3d->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])));
		trianglePixelCoords.clear();
		::gpk::tri3<float>												transformedTriangle3D						= {};
		transformedTriangle3D.A												= {(float)triangle2dList[iTriangle].A.x, (float)triangle2dList[iTriangle].A.y, triangle3dList[iTriangle].A.z};
		transformedTriangle3D.B												= {(float)triangle2dList[iTriangle].B.x, (float)triangle2dList[iTriangle].B.y, triangle3dList[iTriangle].B.z};
		transformedTriangle3D.C												= {(float)triangle2dList[iTriangle].C.x, (float)triangle2dList[iTriangle].C.y, triangle3dList[iTriangle].C.z};
		if_true_e(::gpk::failed(::gpk::drawTriangle(offscreenMetrics.Cast<::gpk::u2_t>(), transformedTriangle3D, trianglePixelCoords, triangleWeights, buffer3d->DepthStencil.View)));
		for(::gpk::u2_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord)
			buffer3d->Color.View[trianglePixelCoords[iCoord].y][trianglePixelCoords[iCoord].x] = triangle3dColorList[iTriangle * 3];

		::gpk::drawLine(offscreenMetrics, ::gpk::line2<::gpk::s2_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::line2<::gpk::s2_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::line2<::gpk::s2_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	}
	for(::gpk::u2_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		::gpk::drawPixelLight(buffer3d->Color.View, wireframePixelCoords[iCoord], (::gpk::bgra)::gpk::GREEN, 0.05f, 1.5);

	gui.Controls.ImageViews[viewport.IdControls[::gpk::VIEWPORT_CONTROL_TARGET]]						= buffer3d->Color.View;

	// ---
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::LIGHTGRAY, 0xFFFFFFFFU);
	{
		::std::lock_guard														lock										(app.Framework.LockGUI);
		::gpk::guiDraw(gui, target->Color.View);
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
