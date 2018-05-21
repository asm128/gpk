// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "gpk_framework.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#endif

					::gpk::error_t										gpk::updateFramework						(::gpk::SFramework& framework)													{
	framework.Input.KeyboardPrevious											= framework.Input.KeyboardCurrent;
	framework.Input.MousePrevious												= framework.Input.MouseCurrent;
	framework.Input.MouseCurrent.Deltas											= {};
	::gpk::SFrameInfo															& frameInfo									= framework.FrameInfo;
	::gpk::STimer																& timer										= framework.Timer;
	timer		.Frame();
	frameInfo	.Frame(::gpk::min(timer.LastTimeMicroseconds, 200000ULL));
	::gpk::SDisplay																& mainWindow								= framework.MainDisplay;
	ree_if(errored(::gpk::displayUpdate(mainWindow)), "Not sure why this would fail.");
	rvi_if(1, 0 == mainWindow.PlatformDetail.WindowHandle, "Application exiting because the main window was closed.");
	::gpk::SFramework::TOffscreen												& offscreen									= framework.MainDisplayOffscreen.Color;
	error_if(errored(::gpk::displayPresentTarget(mainWindow, offscreen.View)), "Unknown error.");
#if defined(GPK_WINDOWS)
	Sleep(1);
#elif defined(GPK_ANDROID)
	Sleep_Para_Android(1);
#endif
	return 0;
}

					::gpk::error_t										gpk::clearTarget							(::gpk::SRenderTarget& targetToClear)	{ 
	::gpk::SFramework::TOffscreen												& offscreen									= targetToClear.Color;
	::gpk::STexture<uint32_t>													& offscreenDepth							= targetToClear.DepthStencil;
	::memset(offscreenDepth	.Texels.begin(), -1, sizeof(uint32_t)								* offscreenDepth	.Texels.size());	// Clear target.
	::memset(offscreen		.Texels.begin(), 0, sizeof(::gpk::SFramework::TOffscreen::TTexel)	* offscreen			.Texels.size());	// Clear target.
	return 0;					
}