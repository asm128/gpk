#include "application.h"
#include "gpk_bitmap_file.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"
#include "gpk_tcpip.h"
#include "gpk_chrono.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t											cleanup					(::gme::SApplication & app)							{ 
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::gpk::clientDisconnect(app.Client);
	::gpk::tcpipShutdown();
	return 0; 
}
			::gpk::error_t											setup						(::gme::SApplication & app)						{ 
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SDisplay															& mainWindow				= framework.MainDisplay;
	framework.Input.create();
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	::gpk::SGUI																& gui						= framework.GUI;
	{
		app.IdExit															= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlExit				= gui.Controls.Controls[app.IdExit];
		controlExit.Area													= {{0, 0}, {64, 20}};
		controlExit.Border													= {1, 1, 1, 1};
		controlExit.Margin													= {};
		controlExit.Align													= ::gpk::ALIGN_TOP_LEFT;
		controlExit.ColorTheme												= 16;
		::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdExit];
		controlConstraints.AttachSizeToText.y								= false;
		controlConstraints.AttachSizeToText.x								= false;
		controlConstraints.AttachSizeToControl.y							= app.IdExit;
		controlConstraints.AttachSizeToControl.x							= app.IdExit;
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}
	{
		app.IdExit															= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlExit				= gui.Controls.Controls[app.IdExit];
		controlExit.Area													= {{0, 0}, {64, 20}};
		controlExit.Border													= {1, 1, 1, 1};
		controlExit.Margin													= {1, 1, 1, 1};
		controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
		controlExit.ColorTheme												= 64;
		::gpk::SControlText														& controlText				= gui.Controls.Text[app.IdExit];
		controlText.Text													= "Exit";
		controlText.Align													= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdExit];
		controlConstraints.AttachSizeToText.y								= false;
		controlConstraints.AttachSizeToText.x								= false;
		gui.Controls.Modes[app.IdExit].ColorMode							= ::gpk::GUI_COLOR_MODE_3D;
		::gpk::controlSetParent(gui, app.IdExit, 0);
	}
	{
		app.Console.IdControl												= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlInput				= gui.Controls.Controls[app.Console.IdControl];
		controlInput.Area													= {{0, 0}, {512, 256}};
		controlInput.Border													= {1, 1, 1, 1};
		controlInput.Margin													= {1, 1, 1, 1};
		controlInput.Align													= ::gpk::ALIGN_BOTTOM_LEFT;
		controlInput.ColorTheme												= 25;
		::gpk::SControlText														& controlText				= gui.Controls.Text[app.Console.IdControl];
		controlText.Text													= {app.StringTest.begin(), app.StringTest.size()};
		controlText.Align													= ::gpk::ALIGN_TOP_LEFT;
		::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.Console.IdControl];
		controlConstraints.AttachSizeToText.y								= false; //app.IdText;
		controlConstraints.AttachSizeToText.x								= false; //app.IdText;
		controlConstraints.DockToControl									= {-1, -1, -1, -1};
		gui.Controls.States[app.Console.IdControl].Design					= false;
		gui.Controls.States[app.Console.IdControl].Hidden					= false;
		::gpk::controlSetParent(gui, app.Console.IdControl, 0);
	}
	{
		app.Console.IdInput													= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlInput				= gui.Controls.Controls[app.Console.IdInput];
		controlInput.Area													= {{0, 0}, {64, 20}};
		controlInput.Border													= {1, 1, 1, 1};
		controlInput.Margin													= {1, 1, 1, 1};
		controlInput.Align													= ::gpk::ALIGN_BOTTOM_LEFT;
		controlInput.ColorTheme												= 25;
		::gpk::SControlText														& controlText				= gui.Controls.Text[app.Console.IdInput];
		controlText.Text													= {app.StringTest.begin(), app.StringTest.size()};
		controlText.Align													= ::gpk::ALIGN_TOP_LEFT;
		::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.Console.IdInput];
		controlConstraints.AttachSizeToText.y								= true; //app.IdText;
		controlConstraints.AttachSizeToText.x								= true; //app.IdText;
		controlConstraints.DockToControl									= {-1, -1, -1, -1};
		gui.Controls.States[app.Console.IdInput].Design						= false;
		gui.Controls.States[app.Console.IdInput].Hidden						= false;
		::gpk::controlSetParent(gui, app.Console.IdInput, app.Console.IdControl);
	}
	{
		app.IdConsole														= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlInput				= gui.Controls.Controls[app.IdConsole];
		controlInput.Area													= {{0, 0}, {64, 20}};
		controlInput.Border													= {1, 1, 1, 1};
		controlInput.Margin													= {1, 1, 1, 1};
		controlInput.Align													= ::gpk::ALIGN_BOTTOM_LEFT;
		controlInput.ColorTheme												= 25;
		::gpk::SControlText														& controlText				= gui.Controls.Text[app.IdConsole];
		controlText.Text													= {app.StringTest.begin(), app.StringTest.size()};
		controlText.Align													= ::gpk::ALIGN_TOP_LEFT;
		::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdConsole];
		controlConstraints.AttachSizeToText.y								= true; //app.IdText;
		controlConstraints.AttachSizeToText.x								= true; //app.IdText;
		controlConstraints.DockToControl									= {-1, -1, -1, app.Console.IdInput};
		gui.Controls.States[app.IdConsole].Design							= false;
		gui.Controls.States[app.IdConsole].Hidden							= false;
		::gpk::controlSetParent(gui, app.IdConsole, app.Console.IdControl);
	}
	{
		app.IdConsole1														= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlInput				= gui.Controls.Controls[app.IdConsole1];
		controlInput.Area													= {{0, 0}, {64, 20}};
		controlInput.Border													= {1, 1, 1, 1};
		controlInput.Margin													= {1, 1, 1, 1};
		controlInput.Align													= ::gpk::ALIGN_BOTTOM_LEFT;
		controlInput.ColorTheme												= 25;
		::gpk::SControlText														& controlText				= gui.Controls.Text[app.IdConsole1];
		controlText.Text													= {app.StringTest.begin(), app.StringTest.size()};
		controlText.Align													= ::gpk::ALIGN_TOP_LEFT;
		::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdConsole1];
		controlConstraints.AttachSizeToText.y								= true; //app.IdText;
		controlConstraints.AttachSizeToText.x								= true; //app.IdText;
		controlConstraints.DockToControl									= {-1, -1, -1, app.IdConsole};
		gui.Controls.States[app.IdConsole1].Design							= false;
		gui.Controls.States[app.IdConsole1].Hidden							= false;
		::gpk::controlSetParent(gui, app.IdConsole1, app.Console.IdControl);
	}
	{
		app.IdConsole2														= ::gpk::controlCreate(gui);
		::gpk::SControl															& controlInput				= gui.Controls.Controls[app.IdConsole2];
		controlInput.Area													= {{0, 0}, {64, 20}};
		controlInput.Border													= {1, 1, 1, 1};
		controlInput.Margin													= {1, 1, 1, 1};
		controlInput.Align													= ::gpk::ALIGN_BOTTOM_LEFT;
		controlInput.ColorTheme												= 25;
		::gpk::SControlText														& controlText				= gui.Controls.Text[app.IdConsole2];
		controlText.Text													= {app.StringTest.begin(), app.StringTest.size()};
		controlText.Align													= ::gpk::ALIGN_TOP_LEFT;
		::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdConsole2];
		controlConstraints.AttachSizeToText.y								= true; //app.IdText;
		controlConstraints.AttachSizeToText.x								= true; //app.IdText;
		controlConstraints.DockToControl									= {-1, -1, -1, app.IdConsole1};
		gui.Controls.States[app.IdConsole2].Design							= false;
		gui.Controls.States[app.IdConsole2].Hidden							= false;
		::gpk::controlSetParent(gui, app.IdConsole2, app.Console.IdControl);
	}
	::gpk::tcpipInitialize();
	app.Client.AddressConnect											= {{}, 9998};
	::gpk::tcpipAddress(0, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, app.Client.Address);
	::gpk::clientConnect(app.Client);
	return 0; 
}
			::gpk::error_t											update						(::gme::SApplication & app, bool exitSignal)	{ 
	::gpk::STimer															timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
	}
	::gpk::SFramework														& framework					= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");

	::gpk::SGUI																& gui						= framework.GUI;
	if(0 == framework.Input)
		framework.Input.create();
	::gpk::SInput															& input						= *framework.Input;
	{
		::gpk::mutex_guard														lock						(app.LockGUI);
		::gpk::guiProcessInput(gui, input);
	}
	if(input.MouseCurrent.Deltas.z) {
		gui.Zoom.ZoomLevel													+= input.MouseCurrent.Deltas.z * (1.0 / (120 * 4));
		::gpk::guiUpdateMetrics(gui, app.Offscreen->Color.metrics(), true);
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
	for(uint32_t iKey = 0; iKey < 256; ++iKey)
		if(input.KeyDown((uint8_t)iKey)) {
			if(iKey >= '0' && iKey <= '9' || iKey == 0x20) {
				if(false == input.KeyboardCurrent.KeyState[VK_CONTROL] && false == input.KeyboardCurrent.KeyState[VK_MENU]) {
					::gpk::mutex_guard															lock					(app.LockGUI);
					app.StringTest.push_back(iKey & 0xFF);
				}
			}
			else if(iKey >= 'A' && iKey <= 'Z') {
				if(false == input.KeyboardCurrent.KeyState[VK_CONTROL] && false == input.KeyboardCurrent.KeyState[VK_MENU]) {
					::gpk::mutex_guard															lock					(app.LockGUI);
					if(input.KeyboardCurrent.KeyState[VK_SHIFT])
						app.StringTest.push_back(iKey & 0xFF);
					else
						app.StringTest.push_back((iKey & 0xFF) + 32);
			}
			}
			else if(iKey == VK_RETURN) {
				::gpk::mutex_guard															lock					(app.LockGUI);
				app.Console.Buffer.PushLine({app.StringTest.begin(), app.StringTest.size()});
				app.StringTest.clear();
				for(uint32_t iLine = 0; iLine < app.Console.Buffer.Lines.size(); ++iLine) 
					::gpk::controlTextSet(gui, app.IdConsole + iLine, app.Console.Buffer.Lines[app.Console.Buffer.Lines.size() - 1 - iLine]);
			}
			else if(iKey == VK_BACK) { // Backspace 
				if(input.KeyboardCurrent.KeyState[VK_CONTROL])  
					app.StringTest.clear();
				else if(app.StringTest.size())
					app.StringTest.resize(app.StringTest.size() - 1);
			}
			else if(iKey != VK_SHIFT && iKey != VK_CONTROL && iKey != VK_MENU) {
				if(false == input.KeyboardCurrent.KeyState[VK_CONTROL] && false == input.KeyboardCurrent.KeyState[VK_MENU]) {
					::gpk::mutex_guard															lock					(app.LockGUI);
					char																		number[32]				= {};
					//sprintf_s(number, "0x%X", iKey);
					app.StringTest.append(number, sprintf_s(number, "0x%X", iKey));
				}
			}
		}

	{
		::gpk::mutex_guard															lock					(app.LockGUI);
		::gpk::controlTextSet(gui, app.Console.IdInput, {app.StringTest.begin(), app.StringTest.size()});
	}

	reterr_error_if(app.Client.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "Failed to connect to server.")
	else 
	{
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived!");
		::gpk::clientUpdate(app.Client);
		::gpk::sleep(10);
	}
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}


			::gpk::error_t												draw					(::gme::SApplication & app)						{ 
	::gpk::STimer																timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, {0xFF, 0x40, 0x7F, 0xFF}, (uint32_t)-1);
	//::gpk::clearTarget(*target);
	{
		::gpk::mutex_guard															lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard															lock					(app.LockRender);
		app.Offscreen															= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}