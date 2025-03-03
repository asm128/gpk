#ifndef GPK_ATMEL

// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "gpk_framework.h"
//#include "gpk_safe.h"
#include "gpk_apod_serialize.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#	include <ShellScalingApi.h>	// for GetDpiForMonitor()
#	include <joystickapi.h>
#	include <xinput.h>
#	pragma warning(disable: 4995)
#elif defined(GPK_XCB)
#	include <xcb/xcb_image.h>
#endif

using ::gpk::sc_t;

static	::gpk::error_t	updateDPI									(::gpk::SFramework & framework)													{
#if defined(GPK_WINDOWS)
	if(0 != framework.RootWindow.PlatformDetail.WindowHandle) {
		RECT			rcWindow									= {};
		::GetWindowRect(framework.RootWindow.PlatformDetail.WindowHandle, &rcWindow);
		POINT			point										= {rcWindow.left + 8, rcWindow.top};
		::gpk::n2<uint32_t>																	dpi											= {96, 96};
#define GPK_WINDOWS7_COMPAT
#if defined(GPK_WINDOWS7_COMPAT)
		if((framework.GUI->Zoom.DPI * 96).u2_t() != dpi) {
			framework.GUI->Zoom.DPI																	= {dpi.x / 96.0, dpi.y / 96.0};
			::gpk::pobj<::gpk::SWindow::TOffscreen>												offscreen									= framework.RootWindow.BackBuffer;
			::gpk::guiUpdateMetrics(*framework.GUI, offscreen->Color.View.metrics().u1_t(), true);
		}
#else
		HMONITOR				hMonitor									= ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
		HRESULT			hr											= ::GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpi.x, &dpi.y);
		if(0 == hr && (framework.GUI->Zoom.DPI * 96).u2_t() != dpi) {
			framework.GUI->Zoom.DPI																	= {dpi.x / 96.0, dpi.y / 96.0};
			::gpk::pobj<::gpk::rt<::gpk::SFramework::TTexel, uint32_t>>					offscreen									= framework.RootWindow.BackBuffer;
			::gpk::guiUpdateMetrics(*framework.GUI, offscreen->Color.View.metrics(), true);
		}
#endif
	}
#endif
	return 0;
}
::gpk::error_t			gpk::updateFramework						(::gpk::SFramework& framework)													{
	if(0 == framework.GUI.get_ref())
		framework.GUI.create();
	::gpk::SWindow				& mainWindow								= framework.RootWindow;
	::gpk::SInput				& input										= *mainWindow.Input;
	input.KeyboardPrevious	= input.KeyboardCurrent;
	input.MousePrevious		= input.MouseCurrent;
	input.MouseCurrent.Deltas	= {};
	for(uint32_t iJoy = 0; iJoy < input.JoystickCurrent.size(); ++iJoy) {
		input.JoystickCurrent[iJoy].Deltas	= {};
		//XINPUT_STATE xistate = {};
		//HRESULT err = XInputGetState(iJoy, &xistate);
		//if(0 == err) {
		//	if(xistate.Gamepad.sThumbLX) { 
		//		input.JoystickCurrent[iJoy].Deltas.x = xistate.Gamepad.sThumbLX; }
		//	if(xistate.Gamepad.sThumbLY) { 
		//		input.JoystickCurrent[iJoy].Deltas.y = xistate.Gamepad.sThumbLY; }
		//	if(xistate.Gamepad.sThumbRX) { 
		//		input.JoystickCurrent[iJoy].Deltas.x = xistate.Gamepad.sThumbRX; }
		//	if(xistate.Gamepad.sThumbRX) { 
		//		input.JoystickCurrent[iJoy].Deltas.y = xistate.Gamepad.sThumbRX; }
		//}
	}

	::gpk::SFrameInfo							& frameInfo									= framework.FrameInfo;
	::gpk::STimer								& timer										= framework.Timer;
	timer		.Frame();
	frameInfo	.Frame(::gpk::min(timer.LastTimeMicroseconds, (uint64_t)200000));
#if defined(GPK_XCB)
	::gpk::pobj<::gpk::rt<::gpk::bgra, uint32_t>>
												& guiRenderTarget			= framework.RootWindow.BackBuffer;
	if(mainWindow.Repaint) {
		xcb_screen_t								* xcbScreen					= xcb_setup_roots_iterator(xcb_get_setup(framework.PlatformDetail.XCBConnection)).data;
		const xcb_image_format_t					format						= XCB_IMAGE_FORMAT_Z_PIXMAP;
		const ::gpk::n2<uint32_t>				& guiRenderTargetMetrics	= guiRenderTarget->Color.View.metrics();
		xcb_image_t									* image						= xcb_image_create_native
			( mainWindow.PlatformDetail.Connection
			, guiRenderTargetMetrics.x
			, guiRenderTargetMetrics.y
			, format
			, xcbScreen->root_depth
			, (uint8_t*)guiRenderTarget->Color.View.begin()
			, guiRenderTarget->Color.View.size() * 4
			, (uint8_t*)guiRenderTarget->Color.View.begin()
			);
		xcb_image_put(mainWindow.PlatformDetail.Connection, mainWindow.PlatformDetail.IdDrawable, mainWindow.PlatformDetail.GC, image, 0, 0, 0);
		image->base						= 0;
		xcb_image_destroy(image);
		xcb_flush(mainWindow.PlatformDetail.Connection);
		framework.RootWindow.Repaint	= false;
	}
	while (xcb_generic_event_t * ev = xcb_poll_for_event(mainWindow.PlatformDetail.Connection)) {
		switch (ev->response_type & ~0x80) {
		default					: break;
		case XCB_EXPOSE			: {
			//xcb_expose_event_t												* x					= (xcb_expose_event_t *)ev;
			mainWindow.Repaint	= true;
			break;
		}
		case XCB_KEY_PRESS		: { framework.RootWindow.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Key %i Down"	, iev->detail); input.KeyboardCurrent.KeyState		[iev->detail]		= 1; break;	}
		case XCB_KEY_RELEASE	: { framework.RootWindow.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Key %i Up"		, iev->detail); input.KeyboardCurrent.KeyState		[iev->detail]		= 0; break;	}
		case XCB_BUTTON_PRESS	: { framework.RootWindow.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Button %i Down"	, iev->detail); input.MouseCurrent   .ButtonState	[iev->detail - 1]	= 1; break;	}
		case XCB_BUTTON_RELEASE	: { framework.RootWindow.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Button %i Up"	, iev->detail); input.MouseCurrent   .ButtonState	[iev->detail - 1]	= 0; break;	}
		case XCB_MOTION_NOTIFY	: {
			xcb_motion_notify_event_t			* mev				= (xcb_motion_notify_event_t*)ev;
			int32_t								xPos				= mev->same_screen ? mev->event_x : mev->root_x;
			int32_t								yPos				= mev->same_screen ? mev->event_y : mev->root_y;
			input.MouseCurrent.Position.x	= ::gpk::clamped(xPos, 0, (int32_t)mainWindow.Size.x);
			input.MouseCurrent.Position.y	= ::gpk::clamped(yPos, 0, (int32_t)mainWindow.Size.y);
			input.MouseCurrent.Deltas.x		= input.MouseCurrent.Position.x - input.MousePrevious.Position.x;
			input.MouseCurrent.Deltas.y		= input.MouseCurrent.Position.y - input.MousePrevious.Position.y;
			//info_printf("Mouse position: %i, %i", input.MouseCurrent.Position.x, input.MouseCurrent.Position.y);
			framework.RootWindow.Repaint	= true;
			break;
		}
		}
	}
	::gpk::error_t				updateResult		= 0;
#else
	::gpk::error_t				updateResult		= ::gpk::windowUpdate(mainWindow);
#endif
	rees_if(errored(updateResult));
	rvis_if(1, mainWindow.Closed);
	rvis_if(1, 1 == updateResult);
	::gpk::pobj<::gpk::SWindow::TOffscreen>	offscreen	= framework.RootWindow.BackBuffer;
#if defined(GPK_WINDOWS)
	if(mainWindow.PlatformDetail.WindowHandle) {
		if(offscreen && offscreen->Color.Texels.size())
			e_if(errored(::gpk::windowPresentTarget(mainWindow, offscreen->Color.View)));
	}
#endif
	::gpk::SGUI						& gui				= *framework.GUI;
	{
		::std::lock_guard				lock										(framework.LockGUI);
		::gpk::guiProcessInput(gui, *mainWindow.Input, mainWindow.EventQueue);
	}

	if(framework.Settings.GUIZoom) {
		if(mainWindow.Input->MouseCurrent.Deltas.z) {
			::std::lock_guard				lock										(framework.LockGUI);
			if(mainWindow.Input->MouseCurrent.Deltas.z > 0) {
				if(gui.Zoom.ZoomLevel > 1)
					++gui.Zoom.ZoomLevel;
				else
					gui.Zoom.ZoomLevel		*= 2;
			}
			else {
				if(gui.Zoom.ZoomLevel > 1)
					--gui.Zoom.ZoomLevel;
				else
					gui.Zoom.ZoomLevel		*= .5;
			}
			::gpk::guiUpdateMetrics(gui, framework.RootWindow.Size, true);
		}
	}
	return ::updateDPI(framework);
}

#if defined(GPK_WINDOWS)
#	include <Windowsx.h>

static	::RECT			minClientRect			= {100, 100, 100 + 320, 100 + 200};

//extern				::SApplication				* g_ApplicationInstance						;
static	LRESULT WINAPI	mainWndProc				(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	switch(uMsg) {
	default					: break;
	case WM_GETMINMAXINFO	:	// Catch this message so to prevent the window from becoming too small.
		((::MINMAXINFO*)lParam)->ptMinTrackSize	= {minClientRect.right - minClientRect.left, minClientRect.bottom - minClientRect.top};
		return 0;
	case WM_NCCREATE			: {
		const CREATESTRUCTA						& createStruct			= *(const CREATESTRUCTA*)lParam;
		::SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR)createStruct.lpCreateParams);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	}
	//::SApplication						& app						= *g_ApplicationInstance;
	static	const int						adjustedMinRect								= ::AdjustWindowRectEx(&minClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	if(0 == ::GetWindowLongPtrA(hWnd, GWLP_USERDATA))
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	::gpk::SWindow				* pWindow		= (::gpk::SWindow*)::GetWindowLongPtrA(hWnd, GWLP_USERDATA);
	if(0 == pWindow)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	::gpk::SWindow				& mainDisplay	= *pWindow;

	//if(!mainDisplay.Input)
		//mainDisplay.Input.create();

	::gpk::SInput						& input										= *mainDisplay.Input;
	::gpk::SWindowPlatformDetail		& displayDetail								= mainDisplay.PlatformDetail;

	::gpk::pobj<::gpk::SEventSystem>	newSystemEvent;
	switch(uMsg) {
	default: break;
	case WM_CLOSE			: e_if(errored(::gpk::eventEnqueueScreenClose  (mainDisplay.EventQueue))); ::DestroyWindow(hWnd); return 0;
	case WM_DESTROY			: e_if(errored(::gpk::eventEnqueueScreenDestroy(mainDisplay.EventQueue))); 
		displayDetail.WindowHandle									= 0;
		mainDisplay.Closed											= true;
		::PostQuitMessage(0);
		return 0;
	case WM_CREATE			: {
		RECT								rect					= {};
		GetClientRect(hWnd, &rect);
		::gpk::n2u16						newMetrics				= {(uint16_t)(rect.right - rect.left), (uint16_t)(rect.bottom - rect.top)};
		e_if(errored(::gpk::eventEnqueueScreenCreate(mainDisplay.EventQueue, {newMetrics, *(const CREATESTRUCTA*)lParam})));
		break;
	}
	case WM_SHOWWINDOW		: { 
		if(0 == wParam) {
			e_if(errored(::gpk::eventEnqueueScreenHide(mainDisplay.EventQueue, (uint8_t)lParam))); 
		} 
		else {
			e_if(errored(::gpk::eventEnqueueScreenShow(mainDisplay.EventQueue, (uint8_t)lParam)));
			mainDisplay.Repaint = true; 
		}
		break;
	}
	case WM_CHAR			: verbose_printf("WM_CHAR          '%u'.", wParam); if_fail_e(gpk::eventEnqueueTextChar				(mainDisplay.EventQueue, (uint32_t)wParam)); mainDisplay.Repaint = true; break;
	case WM_KEYDOWN			: verbose_printf("WM_KEYDOWN       '%u'.", wParam); if_fail_e(gpk::eventEnqueueKeyboardDown			(mainDisplay.EventQueue, (uint8_t )wParam)); if(wParam < ::gpk::size(input.KeyboardPrevious.KeyState)) input.KeyboardCurrent.KeyState[(uint32_t)wParam] = 1; mainDisplay.Repaint = true; break;
	case WM_KEYUP			: verbose_printf("WM_KEYUP         '%u'.", wParam); if_fail_e(gpk::eventEnqueueKeyboardUp			(mainDisplay.EventQueue, (uint8_t )wParam)); if(wParam < ::gpk::size(input.KeyboardPrevious.KeyState)) input.KeyboardCurrent.KeyState[(uint32_t)wParam] = 0; mainDisplay.Repaint = true; break;
	case WM_SYSKEYDOWN		: verbose_printf("WM_SYSKEYDOWN    '%u'.", wParam); if_fail_e(gpk::eventEnqueueKeyboardSysDown		(mainDisplay.EventQueue, (uint8_t )wParam)); if(wParam < ::gpk::size(input.KeyboardPrevious.KeyState)) input.KeyboardCurrent.KeyState[(uint32_t)wParam] = 1; mainDisplay.Repaint = true; break;
	case WM_SYSKEYUP		: verbose_printf("WM_SYSKEYUP      '%u'.", wParam); if_fail_e(gpk::eventEnqueueKeyboardSysUp		(mainDisplay.EventQueue, (uint8_t )wParam)); if(wParam < ::gpk::size(input.KeyboardPrevious.KeyState)) input.KeyboardCurrent.KeyState[(uint32_t)wParam] = 0; mainDisplay.Repaint = true; break;
	case WM_LBUTTONDOWN		: verbose_printf("WM_LBUTTONDOWN   '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseDown			(mainDisplay.EventQueue, 0)); input.MouseCurrent.ButtonState[0] = 1; mainDisplay.Repaint = true; break;
	case WM_RBUTTONDOWN		: verbose_printf("WM_RBUTTONDOWN   '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseDown			(mainDisplay.EventQueue, 1)); input.MouseCurrent.ButtonState[1] = 1; mainDisplay.Repaint = true; break;
	case WM_MBUTTONDOWN		: verbose_printf("WM_MBUTTONDOWN   '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseDown			(mainDisplay.EventQueue, 2)); input.MouseCurrent.ButtonState[2] = 1; mainDisplay.Repaint = true; break;
	case WM_LBUTTONUP		: verbose_printf("WM_LBUTTONUP     '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseUp				(mainDisplay.EventQueue, 0)); input.MouseCurrent.ButtonState[0] = 0; mainDisplay.Repaint = true; break;
	case WM_RBUTTONUP		: verbose_printf("WM_RBUTTONUP     '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseUp				(mainDisplay.EventQueue, 1)); input.MouseCurrent.ButtonState[1] = 0; mainDisplay.Repaint = true; break;
	case WM_MBUTTONUP		: verbose_printf("WM_MBUTTONUP     '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseUp				(mainDisplay.EventQueue, 2)); input.MouseCurrent.ButtonState[2] = 0; mainDisplay.Repaint = true; break;
	case WM_LBUTTONDBLCLK	: verbose_printf("WM_LBUTTONDBLCLK '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseDoubleClick		(mainDisplay.EventQueue, 0)); input.MouseCurrent.ButtonState[0] = 1; mainDisplay.Repaint = true; break;
	case WM_RBUTTONDBLCLK	: verbose_printf("WM_RBUTTONDBLCLK '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseDoubleClick		(mainDisplay.EventQueue, 1)); input.MouseCurrent.ButtonState[1] = 1; mainDisplay.Repaint = true; break;
	case WM_MBUTTONDBLCLK	: verbose_printf("WM_MBUTTONDBLCLK '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseDoubleClick		(mainDisplay.EventQueue, 2)); input.MouseCurrent.ButtonState[2] = 1; mainDisplay.Repaint = true; break;
	case WM_MOUSEWHEEL		: verbose_printf("WM_MOUSEWHEEL    '%u'.", wParam); if_fail_e(gpk::eventEnqueueMouseWheel			(mainDisplay.EventQueue, input.MouseCurrent.Deltas.z = GET_WHEEL_DELTA_WPARAM(wParam))); mainDisplay.Repaint = true; break;
	case WM_MOUSEMOVE		: {
		::gpk::n2i16						mousePos		= {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		input.MouseCurrent.Position.x	= (int16_t)::gpk::clamped(mousePos.x, (int16_t)0, (int16_t)(mainDisplay.Size.x - 1));
		input.MouseCurrent.Position.y	= (int16_t)::gpk::clamped(mousePos.y, (int16_t)0, (int16_t)(mainDisplay.Size.y - 1));
		input.MouseCurrent.Deltas.x		= input.MouseCurrent.Position.x - input.MousePrevious.Position.x;
		input.MouseCurrent.Deltas.y		= input.MouseCurrent.Position.y - input.MousePrevious.Position.y;
		if(input.MouseCurrent.Deltas.x || input.MouseCurrent.Deltas.y)
			mainDisplay.Repaint				= true;

		e_if(errored(::gpk::eventEnqueueMousePosition(mainDisplay.EventQueue, input.MouseCurrent.Position))); 

		break;
		}
	case WM_ACTIVATE: {
		switch(wParam) {
		case WA_ACTIVE		: e_if(errored(::gpk::eventEnqueueScreenActivate  (mainDisplay.EventQueue, 0))); /*XInputEnable(1); */break; // Activated by some method other than a mouse click (for example, by a call to the SetActiveWindow function or by use of the keyboard interface to select the window).
		case WA_CLICKACTIVE	: e_if(errored(::gpk::eventEnqueueScreenActivate  (mainDisplay.EventQueue, 1))); /*XInputEnable(1); */break; // Activated by a mouse click.
		case WA_INACTIVE	: e_if(errored(::gpk::eventEnqueueScreenDeactivate(mainDisplay.EventQueue, 0))); /*XInputEnable(0); */break; 
		}
		break;
	}
	case WM_SIZE			:
		info_printf("%s", "WM_SIZE");
		switch(wParam) {
		default				: mainDisplay.MinOrMaxed = mainDisplay.NoDraw = false; break;
		case SIZE_MINIMIZED	: mainDisplay.MinOrMaxed = mainDisplay.NoDraw = true ; break;
		case SIZE_MAXIMIZED	:
		case SIZE_RESTORED	:
			mainDisplay.Resized	= mainDisplay.MinOrMaxed	= true;
			mainDisplay.NoDraw								= false;
			break;
		}
		if(lParam) {
			::gpk::n2<uint16_t>					newMetrics					= ::gpk::n2<WORD>{LOWORD(lParam), HIWORD(lParam)}.u1_t();
			RECT								windowrect					= {};
			GetClientRect(hWnd, &windowrect);
			newMetrics = {uint16_t(windowrect.right - windowrect.left), uint16_t(windowrect.bottom - windowrect.top)};
			if(newMetrics != mainDisplay.Size.u1_t()) {
				mainDisplay.PreviousSize		= mainDisplay.Size;
				mainDisplay.Size				= newMetrics;
				mainDisplay.Resized				= true;
				mainDisplay.Repaint				= true;
				sc_t								buffer		[256]							= {};
				//snprintf(buffer, ::gpk::size(buffer) - 2, "[%u x %u]. Last frame seconds: %g. ", (uint32_t)newMetrics.x, (uint32_t)newMetrics.y, app.Framework.Timer.LastTimeSeconds);
				snprintf(buffer, ::gpk::size(buffer) - 2, "[%u x %u].", (uint32_t)newMetrics.x, (uint32_t)newMetrics.y);
#if defined(UNICODE)
#else
				SetWindowText(mainDisplay.PlatformDetail.WindowHandle, buffer);
#endif
				e_if(errored(::gpk::eventEnqueueScreenResize(mainDisplay.EventQueue, newMetrics)));
			}
		}
		break;
	case WM_PAINT			:
		info_printf("%s", "WM_PAINT");
		mainDisplay.Repaint		= true;
		break;
	//case MM_JOY1BUTTONDOWN	: e_if(errored(::gpk::eventEnqueueJoyPadDown		(mainDisplay.EventQueue, 0))); input.MouseCurrent.ButtonState[0] = 1; *(LPARAM*)&newEvent.Data[1] = lParam; mainDisplay.EventQueueOld.push_back(newEvent); mainDisplay.Repaint = true; break;// A button on joystick JOYSTICKID1 has been pressed.
	//case MM_JOY1BUTTONUP		: e_if(errored(::gpk::eventEnqueueJoyPadUp			(mainDisplay.EventQueue, 0))); input.MouseCurrent.ButtonState[0] = 1; *(LPARAM*)&newEvent.Data[1] = lParam; mainDisplay.EventQueueOld.push_back(newEvent); mainDisplay.Repaint = true; break;// A button on joystick JOYSTICKID1 has been released.
	//case MM_JOY1ZMOVE			: e_if(errored(::gpk::eventEnqueueJoyPadZMove		(mainDisplay.EventQueue, 0))); input.MouseCurrent.ButtonState[0] = 1; *(LPARAM*)&newEvent.Data[1] = lParam; mainDisplay.EventQueueOld.push_back(newEvent); mainDisplay.Repaint = true; break;// Joystick JOYSTICKID1 changed position in the z-direction.
	//case MM_JOY2BUTTONDOWN	: e_if(errored(::gpk::eventEnqueueJoyPadDown		(mainDisplay.EventQueue, 0))); input.MouseCurrent.ButtonState[0] = 1; *(LPARAM*)&newEvent.Data[1] = lParam; mainDisplay.EventQueueOld.push_back(newEvent); mainDisplay.Repaint = true; break;// A button on joystick JOYSTICKID2 has been pressed.
	//case MM_JOY2BUTTONUP		: e_if(errored(::gpk::eventEnqueueJoyPadUp			(mainDisplay.EventQueue, 0))); input.MouseCurrent.ButtonState[0] = 1; *(LPARAM*)&newEvent.Data[1] = lParam; mainDisplay.EventQueueOld.push_back(newEvent); mainDisplay.Repaint = true; break;// A button on joystick JOYSTICKID2 has been released.
	//case MM_JOY2ZMOVE			: e_if(errored(::gpk::eventEnqueueJoyPadZMove		(mainDisplay.EventQueue, 0))); input.MouseCurrent.ButtonState[0] = 1; *(LPARAM*)&newEvent.Data[1] = lParam; mainDisplay.EventQueueOld.push_back(newEvent); mainDisplay.Repaint = true; break;// Joystick JOYSTICKID2 changed position in the x- or y-direction
	//case MM_JOY1MOVE		: 
	//case MM_JOY2MOVE		: { 
	//	const sc_t							iJoystick					= (MM_JOY2MOVE == uMsg) ? 1 : 0;
	//	newEvent.Type					= ::gpk::SYSEVENT_JOY_MOVE; 
	//	newEvent.Data.resize(1 + sizeof(LPARAM)); 
	//	newEvent.Data[0]				= iJoystick;
	//	*(LPARAM*)&newEvent.Data[1]		= lParam; 
	//	mainDisplay.EventQueueOld.push_back(newEvent); 
	//
	//	::gpk::n2<uint16_t>										joyPos										= {LOWORD(lParam), HIWORD(lParam)};
	//	if(input.JoystickCurrent.size() < 4) {
	//		input.JoystickCurrent.resize(4, {});
	//		input.JoystickPrevious.resize(4, {});
	//	}
	//	::gpk::SInputJoystick											& joystickCurrent							= input.JoystickCurrent	[iJoystick];
	//	::gpk::SInputJoystick											& joystickPrevious							= input.JoystickPrevious[iJoystick];
	//	joystickCurrent.Position.x								= ::gpk::clamped(joyPos.x, (uint16_t)0, (uint16_t)mainDisplay.Size.x);
	//	joystickCurrent.Position.y								= ::gpk::clamped(joyPos.y, (uint16_t)0, (uint16_t)mainDisplay.Size.y);
	//	joystickCurrent.Deltas.x								= joystickCurrent.Position.x - joystickPrevious.Position.x;
	//	joystickCurrent.Deltas.y								= joystickCurrent.Position.y - joystickPrevious.Position.y;
	//	if(joystickCurrent.Deltas.x || joystickCurrent.Deltas.y)
	//		mainDisplay.Repaint = true;
	//
	//	break;// Joystick JOYSTICKID2 changed position in the z-direction.
	//}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static	void					initWndClass								(::HINSTANCE hInstance, const TCHAR* className, WNDPROC wndProc, ::WNDCLASSEX& wndClassToInit)	{
	wndClassToInit					= {sizeof(::WNDCLASSEX),};
	wndClassToInit.lpfnWndProc		= wndProc;
	wndClassToInit.hInstance		= hInstance;
	wndClassToInit.hIcon			= LoadIcon	(NULL, IDC_ICON);
	wndClassToInit.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wndClassToInit.hbrBackground	= (::HBRUSH)(COLOR_3DFACE + 1);
	wndClassToInit.lpszClassName	= className;
	wndClassToInit.style			= CS_DBLCLKS;
}
#elif defined(GPK_XCB)
static	::gpk::error_t	xcbWindowCreate								(::gpk::SWindow & window) {
	if(0 == window.PlatformDetail.Connection) {
		window.PlatformDetail.Connection														= xcb_connect(NULL, NULL);
	}
	xcb_screen_t			* xcbScreen									= xcb_setup_roots_iterator(xcb_get_setup(window.PlatformDetail.Connection)).data;
	window.PlatformDetail.IdDrawableBackPixmap												= xcb_generate_id(window.PlatformDetail.Connection);
	xcb_create_pixmap(window.PlatformDetail.Connection, xcbScreen->root_depth, window.PlatformDetail.IdDrawableBackPixmap, xcbScreen->root, window.Size.x, window.Size.y);
	{	// create graphics context
		window.PlatformDetail.GC																= xcb_generate_id(window.PlatformDetail.Connection);
		const uint32_t			mask		= XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
		const uint32_t			values[3]	= {xcbScreen->black_pixel, xcbScreen->white_pixel, 0};
		xcb_create_gc(window.PlatformDetail.Connection, window.PlatformDetail.GC, xcbScreen->root, mask, values);
	}
	{	// create the window
		window.PlatformDetail.IdDrawable																		= xcb_generate_id(window.PlatformDetail.Connection);
		const uint32_t			mask		= XCB_CW_BACK_PIXMAP | XCB_CW_EVENT_MASK;
		const uint32_t			values[2]	= { window.PlatformDetail.IdDrawableBackPixmap
			, XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_STRUCTURE_NOTIFY
			| XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS  | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION
		};
		xcb_create_window
			( window.PlatformDetail.Connection			// connection
			, xcbScreen->root_depth	// depth
			, window.PlatformDetail.IdDrawable			// window Id
			, xcbScreen->root		// parent window
			, 0, 0										// x, y
			, window.Size.x, window.Size.y				// width, height
			, 0											// border_width
			, XCB_WINDOW_CLASS_INPUT_OUTPUT				// class
			, xcbScreen->root_visual	// visual
			, mask, values								// masks
			);
		xcb_map_window(window.PlatformDetail.Connection, window.PlatformDetail.IdDrawable);
	}
	return 0;
}
#endif

::gpk::error_t			gpk::mainWindowDestroy						(::gpk::SWindow& mainWindow)				{
#if defined(GPK_WINDOWS)
	::DestroyWindow(mainWindow.PlatformDetail.WindowHandle);
	::gpk::windowUpdate(mainWindow);
#elif defined(GPK_XCB)

#else
	(void)mainWindow;
#endif
	return 0;
}

::gpk::error_t			gpk::mainWindowCreate	(::gpk::SWindow& mainWindow, ::gpk::SRuntimeValuesDetail& runtimeValues, ::gpk::pobj<SInput>& displayInput)	{
	if(0 == displayInput.get_ref())
		displayInput.create();

#if defined(GPK_WINDOWS)
	::gpk::SWindowPlatformDetail	& displayDetail			= mainWindow.PlatformDetail;
	HINSTANCE						hInstance				= runtimeValues.EntryPointArgsWin.hInstance;
	::initWndClass(hInstance, ::gpk::SWindowPlatformDetail::DefaultRootWindowClassName.begin(), ::mainWndProc, displayDetail.WindowClass);
	::RegisterClassEx(&displayDetail.WindowClass);
	::RECT							finalClientRect			= {100, 100, 100 + (LONG)mainWindow.Size.x, 100 + (LONG)mainWindow.Size.y};
	DWORD							windowStyle				= WS_OVERLAPPEDWINDOW; //WS_POPUP;
	::AdjustWindowRectEx(&finalClientRect, windowStyle, FALSE, 0);
	displayDetail.WindowHandle	= ::CreateWindowEx(0, displayDetail.WindowClass.lpszClassName, TEXT("Window"), windowStyle | CS_DBLCLKS
		, finalClientRect.left
		, finalClientRect.top
		, finalClientRect.right		- finalClientRect.left
		, finalClientRect.bottom	- finalClientRect.top
		, 0, 0, displayDetail.WindowClass.hInstance, &mainWindow
		);

	::SetWindowTextA(displayDetail.WindowHandle, runtimeValues.EntryPointArgsStd.ArgsCommandLine[0]);
	::ShowWindow	(displayDetail.WindowHandle, SW_SHOW);
	//::UpdateWindow	(displayDetail.WindowHandle);
#elif defined(GPK_XCB)
	//if(0 == mainWindow.PlatformDetail.Connection) {
	//	if(0 == framework.PlatformDetail.XCBConnection)
	//		framework.PlatformDetail.XCBConnection													= xcb_connect(0, 0);
	//	mainWindow.PlatformDetail.Connection													= framework.PlatformDetail.XCBConnection;
	//}
	if(0 == displayDetail.Connection) {
		displayDetail.Connection	= xcb_connect(0, 0);
	}
	xcb_screen_t				* xcbScreen			= xcb_setup_roots_iterator(xcb_get_setup(mainWindow.PlatformDetail.Connection)).data;
	::std::shared_ptr<xcb_get_geometry_reply_t>	geometry									(xcb_get_geometry_reply(mainWindow.PlatformDetail.Connection, xcb_get_geometry(mainWindow.PlatformDetail.Connection, xcbScreen->root), nullptr), free);
	mainWindow.Size						= {geometry->width, geometry->height};
	gpk_necall(xcbWindowCreate(mainWindow), "%s", "Failed to create main window.");
	(void)runtimeValues;
#else
	(void)runtimeValues;
#endif
	mainWindow.Resized		= true;
	return 0;
}

#endif