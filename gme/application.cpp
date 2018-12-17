#include "application.h"
#include "gpk_bitmap_file.h"

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t											cleanup						(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
			::gpk::error_t											setup						(::gme::SApplication & app)						{
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SDisplay															& mainWindow				= framework.MainDisplay;
	framework.Input.create();
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	::gpk::SGUI																& gui						= *framework.GUI;
	gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
	app.IdExit															= ::gpk::controlCreate(gui);
	::gpk::SControl															& controlExit				= gui.Controls.Controls[app.IdExit];
	controlExit.Area													= {{0, 0}, {64, 20}};
	controlExit.Border													= {10, 10, 10, 10};
	controlExit.Margin													= {1, 1, 1, 1};
	controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
	::gpk::SControlText														& controlText				= gui.Controls.Text[app.IdExit];
	controlText.Text													= "Exit";
	controlText.Align													= ::gpk::ALIGN_CENTER;
	::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdExit];
	controlConstraints.AttachSizeToControl								= {app.IdExit, -1};
	::gpk::controlSetParent(gui, app.IdExit, -1);

	//{
	//	unsigned char		test	[]		= "unixunix";
	//	unsigned __int8		* c1			= (unsigned __int8		*)&test[0];
	//	unsigned short		* s1			= (unsigned short		*)&test[0];
	//	unsigned short		* s2			= (unsigned short		*)&test[2];
	//	unsigned short		* s3			= (unsigned short		*)&test[4];
	//	unsigned short		* s4			= (unsigned short		*)&test[6];
	//	unsigned int		* i1			= (unsigned int			*)&test[0];
	//	unsigned int		* i2			= (unsigned int			*)&test[4];
	//	unsigned long long	* ll1			= (unsigned long long	*)&test[0];
	//	c1	;
	//	s1	;
	//	s2	;
	//	s3	;
	//	s4	;
	//	i1	;
	//	i2	;
	//	ll1	;
	//	Sleep(1);
	//}
	//{
	//	unsigned int		test			= 'unix';
	//	unsigned __int8		* c1			= (unsigned __int8		*)&((char*)(&test))[0];
	//	unsigned short		* s1			= (unsigned short		*)&((char*)(&test))[0];
	//	unsigned short		* s2			= (unsigned short		*)&((char*)(&test))[2];
	//	unsigned short		* s3			= (unsigned short		*)&((char*)(&test))[4];
	//	unsigned short		* s4			= (unsigned short		*)&((char*)(&test))[6];
	//	unsigned int		* i1			= (unsigned int			*)&((char*)(&test))[0];
	//	unsigned int		* i2			= (unsigned int			*)&((char*)(&test))[4];
	//	unsigned long long	* ll1			= (unsigned long long	*)&((char*)(&test))[0];
	//	c1	;
	//	s1	;
	//	s2	;
	//	s3	;
	//	s4	;
	//	i1	;
	//	i2	;
	//	ll1	;
	//	Sleep(1);
	//}

	return 0;
}

			::gpk::error_t											draw					(::gme::SApplication & app)						{
	//::gpk::STimer															timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, {}, 0xFFFFFFFF);
	{
		::gpk::mutex_guard														lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

			::gpk::error_t											update						(::gme::SApplication & app, bool exitSignal)	{
	//::gpk::STimer															timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
	}
	::gpk::SFramework														& framework					= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");

	::gpk::SGUI																& gui						= *framework.GUI;
	{
		::gpk::mutex_guard														lock						(app.LockGUI);
		::gpk::guiProcessInput(gui, *app.Framework.Input);
	}
	if(app.Framework.Input->MouseCurrent.Deltas.z) {
		gui.Zoom.ZoomLevel													+= app.Framework.Input->MouseCurrent.Deltas.z * (1.0 / (120 * 4ULL));
		::gpk::guiUpdateMetrics(gui, framework.MainDisplay.Size, true);
	}

	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {
		const ::gpk::SControlState												& controlState				= gui.Controls.States[iControl];
		if(controlState.Unused || controlState.Disabled)
			continue;
		if(controlState.Execute) {
			info_printf("Executed %u.", iControl);
			if(iControl == (uint32_t)app.IdExit)
				return 1;
		}
	}

	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
