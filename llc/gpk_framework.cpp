// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "gpk_framework.h"
#include "gpk_safe.h"
#if defined(GPK_WINDOWS)
#	include <Windows.h>
#	include <ShellScalingApi.h>	// for GetDpiForMonitor()
#elif defined(GPK_XCB)
#	include <xcb/xcb_image.h>
#endif

static				::gpk::error_t														updateDPI									(::gpk::SFramework& framework)													{
#if defined(GPK_WINDOWS)
	if(0 != framework.MainDisplay.PlatformDetail.WindowHandle) {
		RECT																						rcWindow									= {};
		::GetWindowRect(framework.MainDisplay.PlatformDetail.WindowHandle, &rcWindow);
		POINT																						point										= {rcWindow.left + 8, rcWindow.top};
		::gpk::SCoord2<uint32_t>																	dpi											= {96, 96};
#define GPK_WINDOWS7_COMPAT
#if defined(GPK_WINDOWS7_COMPAT)
		if((framework.GUI->Zoom.DPI * 96).Cast<uint32_t>() != dpi) {
			framework.GUI->Zoom.DPI																	= {dpi.x / 96.0, dpi.y / 96.0};
			::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SFramework::TTexel, uint32_t>>					offscreen									= framework.MainDisplayOffscreen;
			::gpk::guiUpdateMetrics(*framework.GUI, offscreen->Color.View.metrics(), true);
		}
#else
		HMONITOR																					hMonitor									= ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
		HRESULT																						hr											= ::GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpi.x, &dpi.y);
		if(0 == hr && (framework.GUI->Zoom.DPI * 96).Cast<uint32_t>() != dpi) {
			framework.GUI->Zoom.DPI																	= {dpi.x / 96.0, dpi.y / 96.0};
			::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SFramework::TTexel, uint32_t>>					offscreen									= framework.MainDisplayOffscreen;
			::gpk::guiUpdateMetrics(*framework.GUI, offscreen->Color.View.metrics(), true);
		}
#endif
	}
#endif
	return 0;
}
					::gpk::error_t		gpk::updateFramework						(::gpk::SFramework& framework)													{
	if(0 == framework.Input.get_ref())
		framework.Input.create();
	if(0 == framework.GUI.get_ref())
		framework.GUI.create();
	::gpk::SInput								& input										= *framework.Input;
	input.KeyboardPrevious					= input.KeyboardCurrent;
	input.MousePrevious						= input.MouseCurrent;
	input.MouseCurrent.Deltas				= {};
	::gpk::SFrameInfo							& frameInfo									= framework.FrameInfo;
	::gpk::STimer								& timer										= framework.Timer;
	timer		.Frame();
	frameInfo	.Frame(::gpk::min((unsigned long long)timer.LastTimeMicroseconds, 200000ULL));
	::gpk::SWindow								& mainWindow								= framework.MainDisplay;
#if defined(GPK_XCB)
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>
												& guiRenderTarget			= framework.MainDisplayOffscreen;
	if(mainWindow.Repaint) {
		xcb_screen_t								* xcbScreen					= xcb_setup_roots_iterator(xcb_get_setup(framework.PlatformDetail.XCBConnection)).data;
		const xcb_image_format_t					format						= XCB_IMAGE_FORMAT_Z_PIXMAP;
		const ::gpk::SCoord2<uint32_t>				& guiRenderTargetMetrics	= guiRenderTarget->Color.View.metrics();
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
		framework.MainDisplay.Repaint	= false;
	}
	while (xcb_generic_event_t * ev = xcb_poll_for_event(mainWindow.PlatformDetail.Connection)) {
		switch (ev->response_type & ~0x80) {
		default					: break;
		case XCB_EXPOSE			: {
			//xcb_expose_event_t												* x					= (xcb_expose_event_t *)ev;
			mainWindow.Repaint	= true;
			break;
		}
		case XCB_KEY_PRESS		: { framework.MainDisplay.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Key %i Down"	, iev->detail); input.KeyboardCurrent.KeyState		[iev->detail]		= 1; break;	}
		case XCB_KEY_RELEASE	: { framework.MainDisplay.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Key %i Up"		, iev->detail); input.KeyboardCurrent.KeyState		[iev->detail]		= 0; break;	}
		case XCB_BUTTON_PRESS	: { framework.MainDisplay.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Button %i Down"	, iev->detail); input.MouseCurrent   .ButtonState	[iev->detail - 1]	= 1; break;	}
		case XCB_BUTTON_RELEASE	: { framework.MainDisplay.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Button %i Up"	, iev->detail); input.MouseCurrent   .ButtonState	[iev->detail - 1]	= 0; break;	}
		case XCB_MOTION_NOTIFY	: {
			xcb_motion_notify_event_t			* mev				= (xcb_motion_notify_event_t*)ev;
			int32_t								xPos				= mev->same_screen ? mev->event_x : mev->root_x;
			int32_t								yPos				= mev->same_screen ? mev->event_y : mev->root_y;
			input.MouseCurrent.Position.x	= ::gpk::clamp(xPos, 0, (int32_t)mainWindow.Size.x);
			input.MouseCurrent.Position.y	= ::gpk::clamp(yPos, 0, (int32_t)mainWindow.Size.y);
			input.MouseCurrent.Deltas.x		= input.MouseCurrent.Position.x - input.MousePrevious.Position.x;
			input.MouseCurrent.Deltas.y		= input.MouseCurrent.Position.y - input.MousePrevious.Position.y;
			//info_printf("Mouse position: %i, %i", input.MouseCurrent.Position.x, input.MouseCurrent.Position.y);
			framework.MainDisplay.Repaint	= true;
			break;
		}
		}
	}
	::gpk::error_t				updateResult		= 0;
#else
	::gpk::error_t				updateResult		= ::gpk::windowUpdate(mainWindow);
#endif
	ree_if(errored(updateResult), "%s", "Not sure why this would fail.");
	rvi_if(1, mainWindow.Closed, "%s", "Application exiting because the main window was closed.");
	rvi_if(1, 1 == updateResult, "%s", "Application exiting because the WM_QUIT message was processed.");
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SFramework::TTexel, uint32_t>>					offscreen									= framework.MainDisplayOffscreen;
#if defined(GPK_WINDOWS)
	if(mainWindow.PlatformDetail.WindowHandle) {
		if(offscreen && offscreen->Color.Texels.size())
			gerror_if(errored(::gpk::windowPresentTarget(mainWindow, offscreen->Color.View)), "%s", "Unknown error.");
	}
#endif
	::gpk::SGUI																					& gui										= *framework.GUI;
	{
		::gpk::mutex_guard																			lock										(framework.LockGUI);
		::gpk::guiProcessInput(gui, *framework.Input, mainWindow.EventQueue);
	}

	if(framework.Settings.GUIZoom) {
		if(framework.Input->MouseCurrent.Deltas.z) {
			::gpk::mutex_guard																			lock										(framework.LockGUI);
			if(framework.Input->MouseCurrent.Deltas.z > 0) {
				if(gui.Zoom.ZoomLevel > 1)
					++gui.Zoom.ZoomLevel;
				else
					gui.Zoom.ZoomLevel																		*= 2;

			}
			else {
				if(gui.Zoom.ZoomLevel > 1)
					--gui.Zoom.ZoomLevel;
				else
					gui.Zoom.ZoomLevel																		*= .5;
			}
			::gpk::guiUpdateMetrics(gui, framework.MainDisplay.Size, true);
		}
	}
	return ::updateDPI(framework);
}

#if defined(GPK_WINDOWS)
#	include <Windowsx.h>

static				::RECT									minClientRect								= {100, 100, 100 + 320, 100 + 200};

//extern				::SApplication							* g_ApplicationInstance						;
static				LRESULT WINAPI							mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	//::SApplication													& applicationInstance						= *g_ApplicationInstance;
	static	const int												adjustedMinRect								= ::AdjustWindowRectEx(&minClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	if(0 == ::GetWindowLongPtrA(hWnd, GWLP_USERDATA))
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	::gpk::SWindow													& mainDisplay								= *(::gpk::SWindow*)::GetWindowLongPtrA(hWnd, GWLP_USERDATA);
	
	if(!mainDisplay.Input)
		mainDisplay.Input.create();
	::gpk::SInput													& input										= *mainDisplay.Input;
	::gpk::SWindowPlatformDetail									& displayDetail								= mainDisplay.PlatformDetail;

	::gpk::SSysEvent												newEvent;
	switch(uMsg) {
	default: break;
	case WM_SHOWWINDOW		: newEvent.Type = ::gpk::SYSEVENT_SHOW			; newEvent.Data.resize(sizeof(WPARAM)); *(WPARAM*)&newEvent.Data[0] = wParam; mainDisplay.EventQueue.push_back(newEvent); mainDisplay.Repaint = true; info_printf("%s", "ShowWindow"); break;
	case WM_CLOSE			: newEvent.Type = ::gpk::SYSEVENT_CLOSE			; newEvent.Data.resize(sizeof(WPARAM)); *(WPARAM*)&newEvent.Data[0] = wParam; mainDisplay.EventQueue.push_back(newEvent); ::DestroyWindow(hWnd); return 0;
	case WM_KEYDOWN			: newEvent.Type = ::gpk::SYSEVENT_KEY_DOWN		; newEvent.Data.resize(sizeof(WPARAM)); *(WPARAM*)&newEvent.Data[0] = wParam; mainDisplay.EventQueue.push_back(newEvent); if(wParam > ::gpk::size(input.KeyboardPrevious.KeyState)) break; input.KeyboardCurrent.KeyState[wParam] = 1; mainDisplay.Repaint = true; return 0;
	case WM_KEYUP			: newEvent.Type = ::gpk::SYSEVENT_KEY_UP		; newEvent.Data.resize(sizeof(WPARAM)); *(WPARAM*)&newEvent.Data[0] = wParam; mainDisplay.EventQueue.push_back(newEvent); if(wParam > ::gpk::size(input.KeyboardPrevious.KeyState)) break; input.KeyboardCurrent.KeyState[wParam] = 0; mainDisplay.Repaint = true; return 0;
	case WM_SYSKEYDOWN		: newEvent.Type = ::gpk::SYSEVENT_SYSKEY_DOWN	; newEvent.Data.resize(sizeof(WPARAM)); *(WPARAM*)&newEvent.Data[0] = wParam; mainDisplay.EventQueue.push_back(newEvent); if(wParam > ::gpk::size(input.KeyboardPrevious.KeyState)) break; input.KeyboardCurrent.KeyState[wParam] = 1; mainDisplay.Repaint = true; return 0;
	case WM_SYSKEYUP		: newEvent.Type = ::gpk::SYSEVENT_SYSKEY_UP		; newEvent.Data.resize(sizeof(WPARAM)); *(WPARAM*)&newEvent.Data[0] = wParam; mainDisplay.EventQueue.push_back(newEvent); if(wParam > ::gpk::size(input.KeyboardPrevious.KeyState)) break; input.KeyboardCurrent.KeyState[wParam] = 0; mainDisplay.Repaint = true; return 0;
	case WM_LBUTTONDOWN		: newEvent.Type = ::gpk::SYSEVENT_MOUSE_DOWN	; newEvent.Data.push_back(0); mainDisplay.EventQueue.push_back(newEvent); info_printf("%s", "L Down"		); if(0 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[0] =  1; mainDisplay.Repaint = true; return 0;
	case WM_LBUTTONDBLCLK	: newEvent.Type = ::gpk::SYSEVENT_MOUSE_DBLCLK	; newEvent.Data.push_back(0); mainDisplay.EventQueue.push_back(newEvent); info_printf("%s", "L DClck"	); if(0 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[0] =  1; mainDisplay.Repaint = true; return 0;
	case WM_LBUTTONUP		: newEvent.Type = ::gpk::SYSEVENT_MOUSE_UP		; newEvent.Data.push_back(0); mainDisplay.EventQueue.push_back(newEvent); info_printf("%s", "L Up"		); if(0 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[0] =  0; mainDisplay.Repaint = true; return 0;
	case WM_RBUTTONDOWN		: newEvent.Type = ::gpk::SYSEVENT_MOUSE_DOWN	; newEvent.Data.push_back(1); mainDisplay.EventQueue.push_back(newEvent); info_printf("%s", "R Down"		); if(1 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[1] =  1; mainDisplay.Repaint = true; return 0;
	case WM_RBUTTONDBLCLK	: newEvent.Type = ::gpk::SYSEVENT_MOUSE_DBLCLK	; newEvent.Data.push_back(1); mainDisplay.EventQueue.push_back(newEvent); info_printf("%s", "R DClck"	); if(1 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[1] =  1; mainDisplay.Repaint = true; return 0;
	case WM_RBUTTONUP		: newEvent.Type = ::gpk::SYSEVENT_MOUSE_UP		; newEvent.Data.push_back(1); mainDisplay.EventQueue.push_back(newEvent); info_printf("%s", "R Up"		); if(1 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[1] =  0; mainDisplay.Repaint = true; return 0;
	case WM_MBUTTONDOWN		: newEvent.Type = ::gpk::SYSEVENT_MOUSE_DOWN	; newEvent.Data.push_back(2); mainDisplay.EventQueue.push_back(newEvent); info_printf("%s", "M Down"		); if(2 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[2] =  1; mainDisplay.Repaint = true; return 0;
	case WM_MBUTTONDBLCLK	: newEvent.Type = ::gpk::SYSEVENT_MOUSE_DBLCLK	; newEvent.Data.push_back(2); mainDisplay.EventQueue.push_back(newEvent); info_printf("%s", "M DClck"	); if(2 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[2] =  1; mainDisplay.Repaint = true; return 0;
	case WM_MBUTTONUP		: newEvent.Type = ::gpk::SYSEVENT_MOUSE_UP		; newEvent.Data.push_back(2); mainDisplay.EventQueue.push_back(newEvent); info_printf("%s", "M Up"		); if(2 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[2] =  0; mainDisplay.Repaint = true; return 0;
	case WM_MOUSEWHEEL		: {
		info_printf("%s", "WM_MOUSEWHEEL");
		int32_t														zDelta				= GET_WHEEL_DELTA_WPARAM(wParam);
		input.MouseCurrent.Deltas.z								= zDelta;
		mainDisplay.Repaint										= true;

		newEvent.Type											= ::gpk::SYSEVENT_MOUSE_WHEEL; 
		newEvent.Data.resize(sizeof(double)); 
		*(int32_t*)&newEvent.Data[0]								= zDelta;
		mainDisplay.EventQueue.push_back(newEvent); 
		return 0;
		}
	case WM_MOUSEMOVE		: {
		verbose_printf("%s", "WM_MOUSEMOVE");
		::gpk::SCoord2<int16_t>									mousePos		= {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		input.MouseCurrent.Position.x						= ::gpk::clamp(mousePos.x, (int16_t)0, (int16_t)mainDisplay.Size.x);
		input.MouseCurrent.Position.y						= ::gpk::clamp(mousePos.y, (int16_t)0, (int16_t)mainDisplay.Size.y);
		input.MouseCurrent.Deltas.x							= input.MouseCurrent.Position.x - input.MousePrevious.Position.x;
		input.MouseCurrent.Deltas.y							= input.MouseCurrent.Position.y - input.MousePrevious.Position.y;
		if(input.MouseCurrent.Deltas.x || input.MouseCurrent.Deltas.y)
			mainDisplay.Repaint = true;

		newEvent.Type										= ::gpk::SYSEVENT_MOUSE_POSITION; 
		newEvent.Data.resize(sizeof(::gpk::SCoord2<int16_t>)); 
		*(::gpk::SCoord2<int16_t>*)&newEvent.Data[0]		= mousePos;
		mainDisplay.EventQueue.push_back(newEvent); 
		return 0;
		}
	case WM_GETMINMAXINFO	:	// Catch this message so to prevent the window from becoming too small.
		((::MINMAXINFO*)lParam)->ptMinTrackSize													= {minClientRect.right - minClientRect.left, minClientRect.bottom - minClientRect.top};
		return 0;
	case WM_SIZE			:
		info_printf("%s", "WM_SIZE");
		if(lParam) {
			::gpk::SCoord2<uint16_t>																			newMetrics									= ::gpk::SCoord2<WORD>{LOWORD(lParam), HIWORD(lParam)}.Cast<uint16_t>();
			if(newMetrics != mainDisplay.Size.Cast<uint16_t>()) {
				mainDisplay.PreviousSize																= mainDisplay.Size;
				mainDisplay.Size																		= newMetrics.Cast<uint32_t>();
				mainDisplay.Resized																		= true;
				mainDisplay.Repaint																		= true;
				char																						buffer		[256]							= {};
				//snprintf(buffer, ::gpk::size(buffer) - 2, "[%u x %u]. Last frame seconds: %g. ", (uint32_t)newMetrics.x, (uint32_t)newMetrics.y, applicationInstance.Framework.Timer.LastTimeSeconds);
				snprintf(buffer, ::gpk::size(buffer) - 2, "[%u x %u].", (uint32_t)newMetrics.x, (uint32_t)newMetrics.y);
#if defined(UNICODE)
#else
				SetWindowText(mainDisplay.PlatformDetail.WindowHandle, buffer);
#endif
				newEvent.Type										= ::gpk::SYSEVENT_RESIZE; 
				newEvent.Data.resize(sizeof(::gpk::SCoord2<uint16_t>)); 
				*(::gpk::SCoord2<uint16_t>*)&newEvent.Data[0]		= newMetrics;
				mainDisplay.EventQueue.push_back(newEvent); 
			}
		}
		if( wParam == SIZE_MINIMIZED ) {
			mainDisplay.MinOrMaxed = mainDisplay.NoDraw												= true;
		}
		else if( wParam == SIZE_MAXIMIZED ) {
			mainDisplay.Resized = mainDisplay.MinOrMaxed											= true;
			mainDisplay.NoDraw																		= false;
		}
		else if( wParam == SIZE_RESTORED ) {
			mainDisplay.Resized																		= true;
			mainDisplay.MinOrMaxed																	= true;
			mainDisplay.NoDraw																		= false;
		}
		else {
			//State.Resized									= true;	?
			mainDisplay.MinOrMaxed = mainDisplay.NoDraw												= false;
		}
		break;
	case WM_PAINT			:
		info_printf("%s", "WM_PAINT");
		mainDisplay.Repaint																		= true;
		break;
	case WM_DESTROY			:
		displayDetail.WindowHandle																= 0;
		mainDisplay.Closed																		= true;
		::PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static				void																initWndClass								(::HINSTANCE hInstance, const TCHAR* className, WNDPROC wndProc, ::WNDCLASSEX& wndClassToInit)	{
	wndClassToInit																			= {sizeof(::WNDCLASSEX),};
	wndClassToInit.lpfnWndProc																= wndProc;
	wndClassToInit.hInstance																= hInstance;
	wndClassToInit.hIcon																	= LoadIcon	(NULL, IDC_ICON);
	wndClassToInit.hCursor																	= LoadCursor(NULL, IDC_ARROW);
	wndClassToInit.hbrBackground															= (::HBRUSH)(COLOR_3DFACE + 1);
	wndClassToInit.lpszClassName															= className;
	wndClassToInit.style																	= CS_DBLCLKS;
}
#elif defined(GPK_XCB)
static				::gpk::error_t														xcbWindowCreate								(::gpk::SWindow & window) {
	if(0 == window.PlatformDetail.Connection) {
		window.PlatformDetail.Connection														= xcb_connect(NULL, NULL);
	}
	xcb_screen_t																				* xcbScreen									= xcb_setup_roots_iterator(xcb_get_setup(window.PlatformDetail.Connection)).data;
	window.PlatformDetail.IdDrawableBackPixmap												= xcb_generate_id(window.PlatformDetail.Connection);
	xcb_create_pixmap(window.PlatformDetail.Connection, xcbScreen->root_depth, window.PlatformDetail.IdDrawableBackPixmap, xcbScreen->root, window.Size.x, window.Size.y);
	{	// create graphics context
		window.PlatformDetail.GC																= xcb_generate_id(window.PlatformDetail.Connection);
		const uint32_t																				mask		= XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
		const uint32_t																				values[3]	= {xcbScreen->black_pixel, xcbScreen->white_pixel, 0};
		xcb_create_gc(window.PlatformDetail.Connection, window.PlatformDetail.GC, xcbScreen->root, mask, values);
	}
	{	// create the window
		window.PlatformDetail.IdDrawable																		= xcb_generate_id(window.PlatformDetail.Connection);
		const uint32_t																				mask		= XCB_CW_BACK_PIXMAP | XCB_CW_EVENT_MASK;
		const uint32_t																				values[2]	= { window.PlatformDetail.IdDrawableBackPixmap
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

					::gpk::error_t														gpk::mainWindowDestroy						(::gpk::SWindow& mainWindow)				{
#if defined(GPK_WINDOWS)
	::DestroyWindow(mainWindow.PlatformDetail.WindowHandle);
	::gpk::windowUpdate(mainWindow);
#elif defined(GPK_XCB)

#else
	(void)mainWindow;
#endif
	return 0;
}

					::gpk::error_t														gpk::mainWindowCreate						(::gpk::SWindow& mainWindow, ::gpk::SRuntimeValuesDetail& runtimeValues, ::gpk::ptr_obj<SInput>& displayInput)	{
	if(0 == displayInput.get_ref())
		displayInput.create();
#if defined(GPK_WINDOWS)
	::gpk::SWindowPlatformDetail																& displayDetail								= mainWindow.PlatformDetail;
	HINSTANCE																					hInstance									= runtimeValues.EntryPointArgsWin.hInstance;
	::initWndClass(hInstance, displayDetail.WindowClassName, ::mainWndProc, displayDetail.WindowClass);
	::RegisterClassEx(&displayDetail.WindowClass);
	::RECT																						finalClientRect								= {100, 100, 100 + (LONG)mainWindow.Size.x, 100 + (LONG)mainWindow.Size.y};
	DWORD																						windowStyle									= WS_OVERLAPPEDWINDOW; //WS_POPUP;
	::AdjustWindowRectEx(&finalClientRect, windowStyle, FALSE, 0);
	mainWindow.PlatformDetail.WindowHandle													= ::CreateWindowEx(0, displayDetail.WindowClassName, TEXT("Window"), windowStyle | CS_DBLCLKS
		, finalClientRect.left
		, finalClientRect.top
		, finalClientRect.right		- finalClientRect.left
		, finalClientRect.bottom	- finalClientRect.top
		, 0, 0, displayDetail.WindowClass.hInstance, 0
		);
	::SetWindowLongPtrA(mainWindow.PlatformDetail.WindowHandle, GWLP_USERDATA, (LONG_PTR)&mainWindow);
	::SetWindowTextA(displayDetail.WindowHandle, runtimeValues.EntryPointArgsStd.ArgsCommandLine[0]);
	::ShowWindow	(displayDetail.WindowHandle, SW_SHOWMAXIMIZED);
	//::UpdateWindow	(displayDetail.WindowHandle);
#elif defined(GPK_XCB)
	//if(0 == mainWindow.PlatformDetail.Connection) {
	//	if(0 == framework.PlatformDetail.XCBConnection)
	//		framework.PlatformDetail.XCBConnection													= xcb_connect(0, 0);
	//	mainWindow.PlatformDetail.Connection													= framework.PlatformDetail.XCBConnection;
	//}
	if(0 == mainWindow.PlatformDetail.Connection) {
		mainWindow.PlatformDetail.Connection													= xcb_connect(0, 0);
	}
	xcb_screen_t																				* xcbScreen									= xcb_setup_roots_iterator(xcb_get_setup(mainWindow.PlatformDetail.Connection)).data;
	::std::shared_ptr<xcb_get_geometry_reply_t>													geometry									(xcb_get_geometry_reply(mainWindow.PlatformDetail.Connection, xcb_get_geometry(mainWindow.PlatformDetail.Connection, xcbScreen->root), nullptr), free);
	mainWindow.Size																			= {geometry->width, geometry->height};
	gpk_necall(xcbWindowCreate(mainWindow), "%s", "Failed to create main window.");
	(void)runtimeValues;
#else
	(void)runtimeValues;
#endif
	mainWindow.Resized																		= true;
	return 0;
}
