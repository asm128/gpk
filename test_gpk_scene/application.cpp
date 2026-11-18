#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_encoding.h"
#include "gpk_json_expression.h"
#include "gpk_file.h"
#include "gpk_noise.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "PNG Test");

::gpk::err_t			cleanup		(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
::gpk::err_t			setup		(::gme::SApplication & app)						{
	::gpk::SFramework		& framework			= app.Framework;
	::gpk::SWindow					& mainWindow						= framework.RootWindow;
	mainWindow.Size								= {1280, 720};
	if_true_e(::gpk::failed(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	{ // Build the exit button
		::gpk::SGUI										& gui								= *framework.GUI;
		gui.ColorModeDefault						= ::gpk::GUI_COLOR_MODE_3D;
		gui.ThemeDefault							= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
		app.IdExit									= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement									& controlExit						= gui.Controls.Placement[app.IdExit];
		controlExit.Area							= {{}, {64, 20}};
		controlExit.Border							= {10, 10, 10, 10};
		controlExit.Margin							= {1, 1, 1, 1};
		controlExit.Align							= ::gpk::ALIGN_BOTTOM_RIGHT;
		::gpk::SControlText								& controlText						= gui.Controls.Text[app.IdExit];
		controlText.Text							= "Exit";
		controlText.Align							= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints						& controlConstraints				= gui.Controls.Constraints[app.IdExit];
		controlConstraints.AttachSizeToControl		= {app.IdExit, -1};
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}

	// Set up main camera
	app.Scene.Renderer.Cameras.push_back(::gpk::vcst_t("0"), {});
	app.Scene.Renderer.Cameras[0].Angle									= ::gpk::math_pi * .25;
	app.Scene.Renderer.Cameras[0].ClipPlanes							= {.01f, 1000.0f};
	app.Scene.Renderer.Cameras[0].Position								= {200, 75, 200};
	app.Scene.Renderer.Cameras[0].Target								= {-15, 0, 0};
	app.Scene.Renderer.Cameras[0].Up									= {0, 1, 0};

	app.Scene.Renderer.Lights.push_back(::gpk::vcst_t("0"), {});
	app.Scene.Renderer.Lights[0].Ambient								= ::gpk::DARKGRAY;
	app.Scene.Renderer.Lights[0].Diffuse								= ::gpk::GRAY;
	app.Scene.Renderer.Lights[0].Specular								= ::gpk::WHITE;
	app.Scene.Renderer.Lights[0].Position								= {};
	app.Scene.Renderer.Lights[0].Direction								= {.5, .5, -.5};
	app.Scene.Renderer.Lights[0].Direction.Normalize();
	app.Scene.Renderer.Lights[0].Angle									= ::gpk::math_pi * .25;
	app.Scene.Renderer.Lights[0].RangeSquared							= 10000;
	app.Scene.Renderer.Lights[0].Type									= ::gpk::GLIGHT_TYPE_DIRECTIONAL;
	app.Scene.Renderer.Lights[0].Disabled								= false;

	do { if_fail_fef(app.IdModel = (::gpk::cid_t)app.Scene.CreateFromFile("../gpk_data/scene/icon_home.stl"), "%s", ""); } while(0);
	memset
		( (void*)app.Scene.Renderer.VertexColors[app.Scene.Renderer.Nodes[app.IdModel].VertexColor].begin()
		, 0xC0
		, app.Scene.Renderer.VertexColors[app.Scene.Renderer.Nodes[app.IdModel].VertexColor].size() * sizeof(uint32_t)
		);
	do { if_fail_fef(app.IdModel = (::gpk::cid_t)app.Scene.CreateFromFile("../gpk_data/scene/icon_home/tinker.obj"), "%s", ""); } while(0);
	return 0;
}

::gpk::err_t			update			(::gme::SApplication & app, bool exitSignal)	{
	//::gpk::STimer				timer;
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

	app.Scene.Renderer.Cameras[0].Position.RotateY(app.Framework.FrameInfo.Seconds.LastFrame);
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

::gpk::err_t			draw		(::gme::SApplication & app)							{
	//::gpk::STimer															timer;
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target->resize(app.Framework.RootWindow.Size, 0x00102030U, 0xFFFFFFFFU);
	for(uint32_t y = 0; y < target->Color.View.metrics().y / 3; ++y)
	for(uint32_t x = 0; x < target->Color.View.metrics().x / 3; ++x) {
		//target->Color.View[y * 3][x * 3]									= uint32_t(::gpk::noise1DBase(y * target->Color.View.metrics().x + x + app.Framework.FrameInfo.Microseconds.Total) + app.Framework.FrameInfo.Seconds.Total) | 0xFF000000;
	}

	if_true_e(::gpk::failed(::gpk::nodeRendererDraw(app.Scene.Renderer, 0, target->Color, target->DepthStencil)));

	//::gpk::array_pod<ubyte_t>												bytesPNG				= 0;
	//::gpk::pngFileWrite(target->Color.View, bytesPNG);
	//FILE																	* fp					= 0;
	//fopen_s(&fp, "png_test_generated.png", "wb");
	//if(fp) {
	//	fwrite(bytesPNG.begin(), 1, bytesPNG.size(), fp);
	//	fclose(fp);
	//}

	//::gpk::clearTarget(*target);
	{
		::std::lock_guard														lock					(app.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::std::lock_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	app.Framework.RootWindow.Repaint									= true;
	return 0;
}
