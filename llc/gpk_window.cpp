#include "gpk_window.h"

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
		::gpk::error_t					drawBuffer									(::HDC hdc, int width, int height, const ::gpk::view_grid<::gpk::SColorBGRA>& colorArray)				{
	struct SOffscreenDetail { // raii destruction of resources
		uint32_t									BitmapInfoSize								= 0;
		::BITMAPINFO								BitmapInfo									= {sizeof(::BITMAPINFO)};
		::HDC										IntermediateDeviceContext					= 0;    // <- note, we're creating, so it needs to be destroyed
		::HBITMAP									IntermediateBitmap							= 0;

													~SOffscreenDetail							()							{
			if(IntermediateBitmap			) ::DeleteObject	(IntermediateBitmap			);
			if(IntermediateDeviceContext	) ::DeleteDC		(IntermediateDeviceContext	);
		}
	}											offscreenDetail								= {};

	const uint32_t								bytesToCopy									= sizeof(::RGBQUAD) * colorArray.size();
	offscreenDetail.BitmapInfoSize			= sizeof(::BITMAPINFO) + bytesToCopy;
	const ::gpk::SCoord2<uint32_t>				metricsSource								= colorArray.metrics();
	//memcpy(&offscreenDetail.BitmapInfo->bmiColors[1], colorArray[0].begin(), metricsSource.x * metricsSource.y * sizeof(::gpk::SColorBGRA));

	offscreenDetail.BitmapInfo.bmiHeader				= {sizeof(::BITMAPINFO)};
	offscreenDetail.BitmapInfo.bmiHeader.biWidth		= colorArray.metrics().x;
	offscreenDetail.BitmapInfo.bmiHeader.biHeight		= colorArray.metrics().y;
	offscreenDetail.BitmapInfo.bmiHeader.biPlanes		= 1;
	offscreenDetail.BitmapInfo.bmiHeader.biBitCount		= 32;
	offscreenDetail.BitmapInfo.bmiHeader.biCompression	= BI_RGB;
	offscreenDetail.BitmapInfo.bmiHeader.biSizeImage	= bytesToCopy;

	offscreenDetail.IntermediateDeviceContext			= ::CreateCompatibleDC(hdc);    // <- note, we're creating, so it needs to be destroyed
	uint32_t												* ppvBits									= 0;
	ree_if(0 == (offscreenDetail.IntermediateBitmap = ::CreateDIBSection(offscreenDetail.IntermediateDeviceContext, &offscreenDetail.BitmapInfo, DIB_RGB_COLORS, (void**) &ppvBits, NULL, 0)), "%s", "Failed to create intermediate dib section.");
	for(uint32_t y = 0, yMax = metricsSource.y; y < yMax; ++y)
		memcpy(&ppvBits[y * metricsSource.x], colorArray[metricsSource.y - 1 - y].begin(), metricsSource.x * sizeof(::gpk::SColorBGRA));

	::HBITMAP									hBmpOld										= (::HBITMAP)::SelectObject(offscreenDetail.IntermediateDeviceContext, offscreenDetail.IntermediateBitmap);    // <- altering state
	//gerror_if(FALSE == ::BitBlt(hdc, 0, 0, width, height, offscreenDetail.IntermediateDeviceContext, 0, 0, SRCCOPY), "%s", "Not sure why would this happen but probably due to mismanagement of the target size or the system resources. I've had it failing when I acquired the device too much and never released it.");
	::SetStretchBltMode(hdc, COLORONCOLOR);
	e_if(FALSE == ::StretchBlt(hdc, 0, 0, width, height, offscreenDetail.IntermediateDeviceContext, 0, 0, colorArray.metrics().x, colorArray.metrics().y, SRCCOPY), "%s", "Not sure why would this happen but probably due to mismanagement of the target size or the system resources. I've had it failing when I acquired the device too much and never released it.");
	::SelectObject(hdc, hBmpOld);	// put the old bitmap back in the DC (restore state)
	return 0;
}
#endif

		::gpk::error_t																			gpk::windowPresentTarget					(::gpk::SWindow& displayInstance, const ::gpk::view_grid<::gpk::SColorBGRA>& targetToPresent)		{
	(void)displayInstance; (void)targetToPresent;
#if defined(GPK_WINDOWS)
	::HWND																								windowHandle								= displayInstance.PlatformDetail.WindowHandle;
	retwarn_gwarn_if(0 == windowHandle, "%s", "presentTarget called without a valid window handle set for the main window.");
	::HDC																								dc											= ::GetDC(windowHandle);
	ree_if(0 == dc, "%s", "Failed to retrieve device context from the provided window handle.");
	e_if(errored(::drawBuffer(dc, displayInstance.Size.x, displayInstance.Size.y, targetToPresent)), "%s", "Not sure why this would happen.");
	::ReleaseDC(windowHandle, dc);
#elif defined(GPK_XCB)
	(void)displayInstance; (void)targetToPresent;
#else
	(void)displayInstance; (void)targetToPresent;
#endif
	return 0;
}
