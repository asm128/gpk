#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_encoding.h"
#include "gpk_json_expression.h"
#include "gpk_file.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Solar System Test");

::gpk::err_t			cleanup			(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }

::gpk::err_t			setup			(::gme::SApplication & app)						{
	::gpk::SFramework			& framework		= app.Framework;
	::gpk::SWindow				& mainWindow	= framework.RootWindow;
	mainWindow.Size			= {1280, 720};
	if_true_e(::gpk::failed(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	{ // Build the exit button
		::gpk::SGUI					& gui			= *framework.GUI;
		gui.ColorModeDefault	= ::gpk::GUI_COLOR_MODE_3D;
		gui.ThemeDefault		= ::gpk::ASCII_COLOR_Dark_green * 16 + 7;
		app.IdExit				= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement& controlExit		= gui.Controls.Placement[app.IdExit];
		controlExit.Area		= {{}, {64, 20}};
		controlExit.Border		= {10, 10, 10, 10};
		controlExit.Margin		= {1, 1, 1, 1};
		controlExit.Align		= ::gpk::ALIGN_BOTTOM_RIGHT;
		::gpk::SControlText		& controlText		= gui.Controls.Text[app.IdExit];
		controlText.Text		= "Exit";
		controlText.Align		= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints[app.IdExit];
		controlConstraints.AttachSizeToControl	= {app.IdExit, -1};
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}
	do { if_fail_fe(::ssg::solarSystemSetup(app.SolarSystemGame)); } while(0);
	return 0;
}

::gpk::err_t			update			(::gme::SApplication & app, bool exitSignal)	{
	//::gpk::STimer									timer;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard			lock			(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	::gpk::SFramework			& framework		= app.Framework;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui			= *framework.GUI;
	::gpk::acid					toProcess		= {};
	if(1 == ::gpk::guiProcessControls(gui, [&app](::gpk::cid_t iControl) { return iControl == app.IdExit ? 1 : 0; }))
		return 1;

	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

::gpk::err_t			draw		(::gme::SApplication & app)							{
	//::gpk::STimer															timer;
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target->resize(app.Framework.RootWindow.Size, {}, (uint32_t)-1);

	const ::gpk::n3f2_t	cameraPosition	= app.SolarSystemGame.Scene.Camera.Position;
	const ::gpk::n3f2_t	cameraTarget	= app.SolarSystemGame.Scene.Camera.Target;
	const ::gpk::n3f2_t	cameraUp		= {0, 1};

	const ::gpk::n2u1_t			offscreenMetrics	= target->Color.View.metrics16();

	::gpk::n3f2_t				cameraFront			= (cameraTarget - cameraPosition).Normalized();

	//::gpk::SEngineSceneConstants	constants		= {};
	//constants.CameraPosition	= cameraPosition;
	//constants.CameraFront		= cameraFront;
	//constants.LightPosition		= {0, 10, 0};
	//constants.LightDirection	= {0, -1, 0};

	::gpk::minmax				nearFar 			= {.0001f, 10000.0f}; 

	//constants.View.LookAt(cameraPosition, cameraTarget, cameraUp);
	//constants.Perspective.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Min, nearFar.Max);
	//constants.Screen.ViewportLH(offscreenMetrics);
	//constants.VP			= constants.View * constants.Perspective;
	//constants.VPS			= constants.VP * constants.Screen;

	::gpk::drawScene(target->Color, target->DepthStencil, app.SolarSystemGame.Engine.Scene->RenderCache, *app.SolarSystemGame.Engine.Scene, cameraPosition, cameraTarget, cameraUp, nearFar);

	::ssg::solarSystemUpdate(app.SolarSystemGame, app.Framework.Timer.LastTimeSeconds, target);

	//::gpk::grid_scale(target->Color.View, targetGame->Color.View);

	{
		::std::lock_guard			lock					(app.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::std::lock_guard			lock					(app.LockRender);
		app.Offscreen			= target;
	}
	app.Framework.RootWindow.Repaint	= true;
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
