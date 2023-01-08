#include "gpk_window.h"

::gpk::error_t							gpk::fullScreenExit				(::gpk::SWindow & window) {
	if(false == window.FullScreen)
		return 0;

	window.Size								= window.WindowedWindowRect.Dimensions();

	HWND										windowHandle					= window.PlatformDetail.WindowHandle;
	DWORD										style							= GetWindowLong(windowHandle, GWL_STYLE);
	SetWindowLong(windowHandle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
	SetWindowPos(windowHandle, NULL, window.WindowedWindowRect.Left, window.WindowedWindowRect.Top
		, window.WindowedWindowRect.Right  - window.WindowedWindowRect.Left
		, window.WindowedWindowRect.Bottom - window.WindowedWindowRect.Top
		, SWP_NOOWNERZORDER | SWP_FRAMECHANGED
	);

	::gpk::SSysEvent							newEvent						= {::gpk::SYSEVENT_WINDOW_RESIZE, };
	newEvent.Data.resize(sizeof(::gpk::SCoord2<uint16_t>));
	(*(::gpk::SCoord2<uint16_t>*)newEvent.Data.begin())	= window.Size.Cast<uint16_t>();
	window.EventQueue[window.EventQueue.push_back({})] = newEvent;
	window.FullScreen						= false;
	return 0;
}

::gpk::error_t							gpk::fullScreenEnter			(::gpk::SWindow & framework) {
	HWND										windowHandle					= framework.PlatformDetail.WindowHandle;
	if(framework.FullScreen)
		return 1;

	MONITORINFO									monitor_info					= {sizeof(monitor_info)};
	ree_if(FALSE == GetWindowRect(windowHandle, (LPRECT)&framework.WindowedWindowRect), "Cannot get window rect for hWnd(0x%x)", windowHandle);
	ree_if(FALSE == GetMonitorInfoA(MonitorFromWindow(windowHandle, MONITOR_DEFAULTTOPRIMARY), &monitor_info), "Cannot get MONITORINFO for hWnd(0x%x)", windowHandle);

	DWORD										style							= GetWindowLong(windowHandle, GWL_STYLE);
	SetWindowLong(windowHandle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
	SetWindowPos(windowHandle, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top
		, monitor_info.rcMonitor.right - monitor_info.rcMonitor.left
		, monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top
		, SWP_NOOWNERZORDER | SWP_FRAMECHANGED
	);

	framework.FullScreen					= true;
	framework.Size							= 
		{ uint16_t(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left)
		, uint16_t(monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top)
		};

	::gpk::SSysEvent							newEvent						= {::gpk::SYSEVENT_WINDOW_RESIZE, };
	newEvent.Data.resize(sizeof(::gpk::SCoord2<uint16_t>));
	(*(::gpk::SCoord2<uint16_t>*)newEvent.Data.begin())	= framework.Size.Cast<uint16_t>();
	framework.EventQueue.push_back(newEvent);
	return 0;
}

		::gpk::error_t					gpk::windowUpdateTick						(::gpk::SWindow& displayInstance)											{
	bool										quit	= false;
#if defined(GPK_WINDOWS)
	::MSG										msg											= {};
	int											counter										= 0;
	while(::PeekMessage(&msg, displayInstance.PlatformDetail.WindowHandle, 0, 0, PM_REMOVE) && (100 > counter++)) {
		::TranslateMessage	(&msg);
		::DispatchMessage	(&msg);
		if(msg.message == WM_QUIT)
			quit = true;
	}
#elif defined(GPK_XCB)
	(void)displayInstance;
#else
	(void)displayInstance;
#endif
	return quit ? 1 : 0;
}

		::gpk::error_t					gpk::windowUpdate							(::gpk::SWindow& displayInstance)											{
	displayInstance.EventQueue.clear();
	displayInstance.Resized					= false;
	displayInstance.Repaint					= false;
	bool										quit	= false;
#if defined(GPK_WINDOWS)
	::MSG										msg											= {};
	while(::PeekMessage(&msg, displayInstance.PlatformDetail.WindowHandle, 0, 0, PM_NOREMOVE))
		if(1 == windowUpdateTick(displayInstance))
			quit									= true;
#elif defined(GPK_XCB)

#endif
	return quit ? 1 : 0;
}

#if defined(GPK_WINDOWS)
		::gpk::error_t					drawBuffer									(::HDC hdc, ::gpk::SWindowPlatformDetail & offscreenDetail, int width, int height, const ::gpk::view2d<::gpk::SColorBGRA>& colorArray)				{
	const uint32_t								bytesToCopy									= sizeof(::RGBQUAD) * colorArray.size();
	const ::gpk::SCoord2<uint16_t>				metricsSource								= colorArray.metrics().Cast<uint16_t>();
	const ::gpk::SCoord2<uint16_t>				prevSize									= {(uint16_t)offscreenDetail.BitmapInfo.bmiHeader.biWidth, (uint16_t)offscreenDetail.BitmapInfo.bmiHeader.biHeight};
	if( metricsSource.x != offscreenDetail.BitmapInfo.bmiHeader.biWidth 
	 || metricsSource.y != offscreenDetail.BitmapInfo.bmiHeader.biHeight
	 || width  != offscreenDetail.BitmapInfo.bmiHeader.biWidth 
	 || height != offscreenDetail.BitmapInfo.bmiHeader.biHeight
	 || 0 == offscreenDetail.IntermediateDeviceContext
	 || 0 == offscreenDetail.IntermediateBitmap
	) {
		offscreenDetail.BitmapInfoSize			= sizeof(::BITMAPINFO);
		//memcpy(&offscreenDetail.BitmapInfo->bmiColors[1], colorArray[0].begin(), metricsSource.x * metricsSource.y * sizeof(::gpk::SColorBGRA));

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
		ree_if(0 == (offscreenDetail.IntermediateBitmap		= ::CreateDIBSection(offscreenDetail.IntermediateDeviceContext, &offscreenDetail.BitmapInfo, DIB_RGB_COLORS, (void**) &pixelBits, NULL, 0)), "%s", "Failed to create intermediate dib section.");
		offscreenDetail.PixelBits							= {pixelBits, colorArray.area()};
	}
	const uint32_t lineWidth	= metricsSource.x * sizeof(::gpk::SColorBGRA);
	const uint32_t lastLine		= metricsSource.y - 1;
	const ::gpk::SColorBGRA									* dest										= colorArray.begin();
	for(uint32_t y = 0, yMax = metricsSource.y; y < yMax; ++y)
		memcpy(&offscreenDetail.PixelBits[y * metricsSource.x], &dest[(lastLine - y) * metricsSource.x], lineWidth);

	::HBITMAP									hBmpOld										= (::HBITMAP)::SelectObject(offscreenDetail.IntermediateDeviceContext, offscreenDetail.IntermediateBitmap);    // <- altering state
	e_if(FALSE == ::BitBlt(hdc, 0, 0
		, ::gpk::min(metricsSource.x, (uint16_t)offscreenDetail.BitmapInfo.bmiHeader.biWidth)
		, ::gpk::min(metricsSource.y, (uint16_t)offscreenDetail.BitmapInfo.bmiHeader.biHeight)
		, offscreenDetail.IntermediateDeviceContext, 0, 0, SRCCOPY)
		, "%s", "Not sure why would this happen but probably due to mismanagement of the target size or the system resources. I've had it failing when I acquired the device too much and never released it."
	);
	//::SetStretchBltMode(hdc, COLORONCOLOR);
	//e_if(FALSE == ::StretchBlt(hdc, 0, 0, width, height, offscreenDetail.IntermediateDeviceContext, 0, 0, colorArray.metrics().x, colorArray.metrics().y, SRCCOPY), "%s", "Not sure why would this happen but probably due to mismanagement of the target size or the system resources. I've had it failing when I acquired the device too much and never released it.");
	::SelectObject(hdc, hBmpOld);	// put the old bitmap back in the DC (restore state)
	return 0;
}
#endif

		::gpk::error_t																			gpk::windowPresentTarget					(::gpk::SWindow& displayInstance, const ::gpk::view2d<::gpk::SColorBGRA>& targetToPresent)		{
	(void)displayInstance; (void)targetToPresent;
#if defined(GPK_WINDOWS)
	::HWND																								windowHandle								= displayInstance.PlatformDetail.WindowHandle;
	retwarn_gwarn_if(0 == windowHandle, "%s", "presentTarget called without a valid window handle set for the main window.");
	::HDC																								dc											= ::GetDC(windowHandle);
	ree_if(0 == dc, "%s", "Failed to retrieve device context from the provided window handle.");
	e_if(errored(::drawBuffer(dc, displayInstance.PlatformDetail, displayInstance.Size.x, displayInstance.Size.y, targetToPresent)), "%s", "Not sure why this would happen.");
	::ReleaseDC(windowHandle, dc);
#elif defined(GPK_XCB)
	(void)displayInstance; (void)targetToPresent;
#else
	(void)displayInstance; (void)targetToPresent;
#endif
	return 0;
}
