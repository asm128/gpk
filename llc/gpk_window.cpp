#include "gpk_window.h"

#if defined(GPK_XCB)
#	include <xcb/xcb_image.h>
#endif

GPK_USING_TYPEINT();

gpk::err_t 		gpk::fullScreenExit			(::gpk::SWindow & window) {
	if(false == window.FullScreen)
		return 0;

	window.Size				= window.WindowedWindowRect.Dimensions();
#if defined(GPK_WINDOWS)
	HWND						windowHandle				= window.PlatformDetail.WindowHandle;
	DWORD						style						= GetWindowLong(windowHandle, GWL_STYLE);
	SetWindowLong(windowHandle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
	SetWindowPos(windowHandle, NULL, window.WindowedWindowRect.Left, window.WindowedWindowRect.Top
		, window.WindowedWindowRect.Right  - window.WindowedWindowRect.Left
		, window.WindowedWindowRect.Bottom - window.WindowedWindowRect.Top
		, SWP_NOOWNERZORDER | SWP_FRAMECHANGED
	);
#endif // GPK_WINDOWS
	if_true_e(failed(::gpk::eventEnqueueScreenResize(window.EventQueue, window.Size)));
	window.FullScreen		= false;
	return 0;
}

gpk::err_t 		gpk::fullScreenEnter		(::gpk::SWindow & window) {
	if(window.FullScreen)
		return 1;

#if defined(GPK_WINDOWS)
	HWND						windowHandle				= window.PlatformDetail.WindowHandle;
	MONITORINFO					monitor_info				= {sizeof(monitor_info)};
	RECT						windowsRect					= {};
	if_true_vef(-1, FALSE == GetWindowRect(windowHandle, &windowsRect), "Cannot get window rect for hWnd(0x%x)", windowHandle);
	window.WindowedWindowRect.Top		= (u1_t)windowsRect.top		;
	window.WindowedWindowRect.Left		= (u1_t)windowsRect.left	;
	window.WindowedWindowRect.Right		= (u1_t)windowsRect.right	;
	window.WindowedWindowRect.Bottom	= (u1_t)windowsRect.bottom	;
	if_true_vef(-1, FALSE == GetMonitorInfoA(MonitorFromWindow(windowHandle, MONITOR_DEFAULTTOPRIMARY), &monitor_info), "Cannot get MONITORINFO for hWnd(0x%x)", windowHandle);

	const DWORD					style						= GetWindowLong(windowHandle, GWL_STYLE);
	SetWindowLong(windowHandle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
	SetWindowPos(windowHandle, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top
		, monitor_info.rcMonitor.right - monitor_info.rcMonitor.left
		, monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top
		, SWP_NOOWNERZORDER | SWP_FRAMECHANGED
	);

	window.Size				= 
		{ u1_t(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left)
		, u1_t(monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top)
		};
#endif // GPK_WINDOWS
	if_true_e(failed(::gpk::eventEnqueueScreenResize(window.EventQueue, window.Size)));
	window.FullScreen		= true;
	return 0;
}

gpk::err_t 		gpk::windowUpdateTick		(::gpk::SWindow& displayInstance)											{
	bool						quit						= false;
#if defined(GPK_WINDOWS)
	::MSG						msg							= {};
	int							counter						= 0;
	while(::PeekMessage(&msg, displayInstance.PlatformDetail.WindowHandle, 0, 0, PM_REMOVE) && (100 > counter++)) {
		::TranslateMessage	(&msg);
		::DispatchMessage	(&msg);
		if(msg.message == WM_QUIT)
			quit = true;
	}
#elif defined(GPK_XCB)
	::gpk::SInput				& input						= *displayInstance.Input;
	int							counter						= 0;
	while(xcb_generic_event_t * event = xcb_poll_for_event(displayInstance.PlatformDetail.Connection)) {
		switch(event->response_type & ~0x80) {
		default					: break;
		case XCB_EXPOSE			: displayInstance.Repaint = true; if_fail_e(::gpk::eventEnqueueScreenRedraw(displayInstance.EventQueue, 0)); break;
		case XCB_KEY_PRESS		: { const xcb_key_press_event_t   & key = *(xcb_key_press_event_t  *)event; input.KeyboardCurrent.KeyState[key.detail] = 1; displayInstance.Repaint = true; if_fail_e(::gpk::eventEnqueueKeyboardDown(displayInstance.EventQueue, key.detail)); break; }
		case XCB_KEY_RELEASE	: { const xcb_key_release_event_t & key = *(xcb_key_release_event_t*)event; input.KeyboardCurrent.KeyState[key.detail] = 0; displayInstance.Repaint = true; if_fail_e(::gpk::eventEnqueueKeyboardUp  (displayInstance.EventQueue, key.detail)); break; }
		case XCB_BUTTON_PRESS	: {
			const xcb_button_press_event_t & button = *(xcb_button_press_event_t*)event;
			displayInstance.Repaint = true;
			if(button.detail == 4 || button.detail == 5) {
				input.MouseCurrent.Deltas.z = button.detail == 4 ? 1 : -1;
				if_fail_e(::gpk::eventEnqueueMouseWheel(displayInstance.EventQueue, input.MouseCurrent.Deltas.z));
			}
			else if(button.detail && button.detail <= input.MouseCurrent.ButtonState.size()) {
				input.MouseCurrent.ButtonState[button.detail - 1] = 1;
				if_fail_e(::gpk::eventEnqueueMouseDown(displayInstance.EventQueue, button.detail - 1));
			}
			break;
		}
		case XCB_BUTTON_RELEASE	: {
			const xcb_button_release_event_t & button = *(xcb_button_release_event_t*)event;
			displayInstance.Repaint = true;
			if(button.detail && button.detail <= input.MouseCurrent.ButtonState.size() && button.detail != 4 && button.detail != 5) {
				input.MouseCurrent.ButtonState[button.detail - 1] = 0;
				if_fail_e(::gpk::eventEnqueueMouseUp(displayInstance.EventQueue, button.detail - 1));
			}
			break;
		}
		case XCB_MOTION_NOTIFY	: {
			const xcb_motion_notify_event_t & motion = *(xcb_motion_notify_event_t*)event;
			const s2_t x = motion.same_screen ? motion.event_x : motion.root_x;
			const s2_t y = motion.same_screen ? motion.event_y : motion.root_y;
			input.MouseCurrent.Position.x = ::gpk::clamped(x, 0, (s2_t)displayInstance.Size.x);
			input.MouseCurrent.Position.y = ::gpk::clamped(y, 0, (s2_t)displayInstance.Size.y);
			input.MouseCurrent.Deltas.x = input.MouseCurrent.Position.x - input.MousePrevious.Position.x;
			input.MouseCurrent.Deltas.y = input.MouseCurrent.Position.y - input.MousePrevious.Position.y;
			displayInstance.Repaint = true;
			if_fail_e(::gpk::eventEnqueueMousePosition(displayInstance.EventQueue, input.MouseCurrent.Position));
			break;
		}
		case XCB_CONFIGURE_NOTIFY: {
			const xcb_configure_notify_event_t & configure = *(xcb_configure_notify_event_t*)event;
			const ::gpk::n2u1_t size = {configure.width, configure.height};
			if(size != displayInstance.Size) {
				displayInstance.PreviousSize = displayInstance.Size;
				displayInstance.Size = size;
				displayInstance.Resized = displayInstance.Repaint = true;
				if_fail_e(::gpk::eventEnqueueScreenResize(displayInstance.EventQueue, size));
			}
			break;
		}
		case XCB_CLIENT_MESSAGE	: {
			const xcb_client_message_event_t & message = *(xcb_client_message_event_t*)event;
			if(message.data.data32[0] == displayInstance.PlatformDetail.WMDelete) {
				displayInstance.Closed = quit = true;
				if_fail_e(::gpk::eventEnqueueScreenClose(displayInstance.EventQueue));
			}
			break;
		}
		case XCB_DESTROY_NOTIFY	: displayInstance.Closed = quit = true; if_fail_e(::gpk::eventEnqueueScreenDestroy(displayInstance.EventQueue)); break;
		}
		free(event);
		if(100 <= ++counter)
			break;
	}
#else
	(void)displayInstance;
#endif // GPK_WINDOWS
	return quit ? 1 : 0;
}

gpk::err_t 		gpk::windowUpdate		(::gpk::SWindow& displayInstance)											{
	displayInstance.EventQueue.clear();
	displayInstance.Resized	= false;
	displayInstance.Repaint	= false;
	bool						quit					= false;
#if defined(GPK_WINDOWS)
	::MSG						msg						= {};
	while(::PeekMessage(&msg, displayInstance.PlatformDetail.WindowHandle, 0, 0, PM_NOREMOVE))
		if(1 == windowUpdateTick(displayInstance))
			quit					= true;
#elif defined(GPK_XCB)
	const ::gpk::err_t updateResult = ::gpk::windowUpdateTick(displayInstance);
	if_true_ve(updateResult, ::gpk::failed(updateResult));
	quit = 1 == updateResult;
#endif // GPK_WINDOWS
	return quit ? 1 : 0;
}

#if defined(GPK_WINDOWS)
gpk::err_t 		drawBuffer				(::HDC hdc, ::gpk::SWindowPlatformDetail & offscreenDetail, int width, int height, const ::gpk::g8bgra& colorArray)				{
	const u2_t				bytesToCopy				= sizeof(::RGBQUAD) * colorArray.size();
	::gpk::n2u1_c				metricsSource			= colorArray.metrics().u1();
	::gpk::n2u1_c				prevSize				= {(u1_t)offscreenDetail.BitmapInfo.bmiHeader.biWidth, (u1_t)offscreenDetail.BitmapInfo.bmiHeader.biHeight};
	if( metricsSource.x != offscreenDetail.BitmapInfo.bmiHeader.biWidth 
	 || metricsSource.y != offscreenDetail.BitmapInfo.bmiHeader.biHeight
	 || width  != offscreenDetail.BitmapInfo.bmiHeader.biWidth 
	 || height != offscreenDetail.BitmapInfo.bmiHeader.biHeight
	 || 0 == offscreenDetail.IntermediateDeviceContext
	 || 0 == offscreenDetail.IntermediateBitmap
	) {
		offscreenDetail.BitmapInfoSize			= sizeof(::BITMAPINFO);
		//memcpy(&offscreenDetail.BitmapInfo->bmiColors[1], colorArray[0].begin(), metricsSource.x * metricsSource.y * sizeof(::gpk::bgra));

		offscreenDetail.BitmapInfo.bmiHeader				= {sizeof(::BITMAPINFO)};
		offscreenDetail.BitmapInfo.bmiHeader.biWidth		= colorArray.metrics().x;
		offscreenDetail.BitmapInfo.bmiHeader.biHeight		= colorArray.metrics().y;
		offscreenDetail.BitmapInfo.bmiHeader.biPlanes		= 1;
		offscreenDetail.BitmapInfo.bmiHeader.biBitCount		= 32;
		offscreenDetail.BitmapInfo.bmiHeader.biCompression	= BI_RGB;
		offscreenDetail.BitmapInfo.bmiHeader.biSizeImage	= bytesToCopy;

		if(offscreenDetail.IntermediateBitmap)
			DeleteObject(offscreenDetail.IntermediateBitmap);

		if(offscreenDetail.IntermediateDeviceContext)
			DeleteDC(offscreenDetail.IntermediateDeviceContext);

		::gpk::SWindowPlatformDetail::TOSWindowNativeTexel		* pixelBits						= 0;
		offscreenDetail.IntermediateDeviceContext			= ::CreateCompatibleDC(hdc);    // <- note, we're creating, so it needs to be destroyed
		if_true_vef(-1, 0 == (offscreenDetail.IntermediateBitmap		= ::CreateDIBSection(offscreenDetail.IntermediateDeviceContext, &offscreenDetail.BitmapInfo, DIB_RGB_COLORS, (void**) &pixelBits, NULL, 0)), "%s", "Failed to create intermediate dib section.");
		offscreenDetail.PixelBits							= {pixelBits, colorArray.area()};
	}
	const u2_t lineWidth	= metricsSource.x * sizeof(::gpk::bgra);
	const u2_t lastLine		= metricsSource.y - 1;
	const ::gpk::bgra									* dest										= colorArray.begin();
	for(u2_t y = 0, yMax = metricsSource.y; y < yMax; ++y)
		memcpy(&offscreenDetail.PixelBits[y * metricsSource.x], &dest[(lastLine - y) * metricsSource.x], lineWidth);

	::HBITMAP									hBmpOld										= (::HBITMAP)::SelectObject(offscreenDetail.IntermediateDeviceContext, offscreenDetail.IntermediateBitmap);    // <- altering state
	if_true_ef(FALSE == ::BitBlt(hdc, 0, 0
		, ::gpk::min(metricsSource.x, (u1_t)offscreenDetail.BitmapInfo.bmiHeader.biWidth)
		, ::gpk::min(metricsSource.y, (u1_t)offscreenDetail.BitmapInfo.bmiHeader.biHeight)
		, offscreenDetail.IntermediateDeviceContext, 0, 0, SRCCOPY)
		, "%s", "Not sure why would this happen but probably due to mismanagement of the target size or the system resources. I've had it failing when I acquired the device too much and never released it."
	);
	//::SetStretchBltMode(hdc, COLORONCOLOR);
	//if_true_e(FALSE == ::StretchBlt(hdc, 0, 0, width, height, offscreenDetail.IntermediateDeviceContext, 0, 0, colorArray.metrics().x, colorArray.metrics().y, SRCCOPY), "%s", "Not sure why would this happen but probably due to mismanagement of the target size or the system resources. I've had it failing when I acquired the device too much and never released it.");
	::SelectObject(hdc, hBmpOld);	// put the old bitmap back in the DC (restore state)
	return 0;
}
#endif // GPK_WINDOWS

gpk::err_t 		gpk::windowPresentTarget					(::gpk::SWindow& displayInstance, const ::gpk::g8bgra& targetToPresent)		{
	(void)displayInstance; (void)targetToPresent;
#if defined(GPK_WINDOWS)
	::HWND					windowHandle								= displayInstance.PlatformDetail.WindowHandle;
	if_true_vwf(1, 0 == windowHandle, "%s", "presentTarget called without a valid window handle set for the main window.");
	::HDC					dc											= ::GetDC(windowHandle);
	if_true_vef(-1, 0 == dc, "%s", "Failed to retrieve device context from the provided window handle.");
	if_true_ef(failed(::drawBuffer(dc, displayInstance.PlatformDetail, displayInstance.Size.x, displayInstance.Size.y, targetToPresent)), "%s", "Not sure why this would happen.");
	::ReleaseDC(windowHandle, dc);
#elif defined(GPK_XCB)
	if(false == displayInstance.Repaint)
		return 0;
	xcb_screen_t				* screen							= xcb_setup_roots_iterator(xcb_get_setup(displayInstance.PlatformDetail.Connection)).data;
	const ::gpk::n2<u2_t>	& metrics						= targetToPresent.metrics();
	xcb_image_t				* image							= xcb_image_create_native
		( displayInstance.PlatformDetail.Connection
		, metrics.x
		, metrics.y
		, XCB_IMAGE_FORMAT_Z_PIXMAP
		, screen->root_depth
		, (u0_t*)targetToPresent.begin()
		, targetToPresent.size() * sizeof(::gpk::bgra)
		, (u0_t*)targetToPresent.begin()
		);
	if_null_fe(image);
	xcb_image_put(displayInstance.PlatformDetail.Connection, displayInstance.PlatformDetail.IdDrawable, displayInstance.PlatformDetail.GC, image, 0, 0, 0);
	image->base = 0;
	xcb_image_destroy(image);
	xcb_flush(displayInstance.PlatformDetail.Connection);
	displayInstance.Repaint = false;
#else // ??
	(void)displayInstance; (void)targetToPresent;
#endif // GPK_WINDOWS
	return 0;
}
