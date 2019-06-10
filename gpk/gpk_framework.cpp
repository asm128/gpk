// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "gpk_framework.h"
#include "gpk_safe.h"
#if defined(GPK_WINDOWS)
#	include <Windows.h>
#	include <ShellScalingApi.h>	// for GetDpiForMonitor()
#endif

struct SDisplayInput {
						::gpk::SDisplay														& Display;
						::gpk::ptr_obj<::gpk::SInput>										Input;
};

static				::gpk::error_t														updateDPI									(::gpk::SFramework& framework)													{
#if defined(GPK_WINDOWS)
	if(0 != framework.MainDisplay.PlatformDetail.WindowHandle) {
		RECT																						rcWindow									= {};
		::GetWindowRect(framework.MainDisplay.PlatformDetail.WindowHandle, &rcWindow);
		POINT																						point										= {rcWindow.left + 8, rcWindow.top};
		::gpk::SCoord2<uint32_t>																	dpi											= {96, 96};
		HMONITOR																					hMonitor									= ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
		HRESULT																						hr											= ::GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpi.x, &dpi.y);
		if(0 == hr && (framework.GUI->Zoom.DPI * 96).Cast<uint32_t>() != dpi) {
			framework.GUI->Zoom.DPI																	= {dpi.x / 96.0, dpi.y / 96.0};
			::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SFramework::TTexel, uint32_t>>					offscreen									= framework.MainDisplayOffscreen;
			::gpk::guiUpdateMetrics(*framework.GUI, offscreen->Color.View.metrics(), true);
		}
	}
#endif
	return 0;
}
					::gpk::error_t														gpk::updateFramework						(::gpk::SFramework& framework)													{
	if(0 == framework.Input)
		framework.Input.create();

	if(0 == framework.GUI)
		framework.GUI.create();

	SInput																						& input										= *framework.Input;
	input.KeyboardPrevious																	= input.KeyboardCurrent;
	input.MousePrevious																		= input.MouseCurrent;
	input.MouseCurrent.Deltas																= {};
	::gpk::SFrameInfo																			& frameInfo									= framework.FrameInfo;
	::gpk::STimer																				& timer										= framework.Timer;
	timer		.Frame();
	frameInfo	.Frame(::gpk::min((unsigned long long)timer.LastTimeMicroseconds, 200000ULL));
	::gpk::SDisplay																				& mainWindow								= framework.MainDisplay;
	::gpk::error_t																				updateResult								= ::gpk::displayUpdateTick(mainWindow);
	ree_if(errored(updateResult), "%s", "Not sure why this would fail.");
	rvi_if(1, mainWindow.Closed, "%s", "Application exiting because the main window was closed.");
	rvi_if(1, 1 == updateResult, "%s", "Application exiting because the WM_QUIT message was processed.");
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SFramework::TTexel, uint32_t>>					offscreen									= framework.MainDisplayOffscreen;
#if defined(GPK_WINDOWS)
	if(mainWindow.PlatformDetail.WindowHandle) {
		if(offscreen && offscreen->Color.Texels.size())
			error_if(errored(::gpk::displayPresentTarget(mainWindow, offscreen->Color.View)), "%s", "Unknown error.");
	}
#endif
	::gpk::SGUI																					& gui										= *framework.GUI;
	{
		::gpk::mutex_guard																			lock										(framework.LockGUI);
		::gpk::guiProcessInput(gui, *framework.Input);
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

static constexpr	const uint32_t														BMP_SCREEN_WIDTH							= 1280;
static constexpr	const uint32_t														BMP_SCREEN_HEIGHT							= uint32_t(::BMP_SCREEN_WIDTH * (9.0 / 16.0));
static				::RECT																minClientRect								= {100, 100, 100 + 320, 100 + 200};

//extern				::SApplication														* g_ApplicationInstance						;
#if defined(GPK_WINDOWS)
static				LRESULT WINAPI														mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)							{
	//::SApplication																				& applicationInstance						= *g_ApplicationInstance;
	static	const int																			adjustedMinRect								= ::AdjustWindowRectEx(&minClientRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	::SDisplayInput																				* actualMainDisplay							= (::SDisplayInput*)::GetWindowLongPtrA(hWnd, GWLP_USERDATA);
	::gpk::SDisplay																				dummyDisplay								= {};	// we need this to create the reference in case the display pointer isn't there.
	::gpk::SInput																				dummyInput									= {};
	::gpk::SDisplay																				& mainDisplay								= (actualMainDisplay) ? actualMainDisplay->Display : dummyDisplay;
	::gpk::SInput																				& input										= (actualMainDisplay && actualMainDisplay->Input) ? *actualMainDisplay->Input : dummyInput;
	::gpk::SDisplayPlatformDetail																& displayDetail								= mainDisplay.PlatformDetail;

	int32_t																						zDelta										= {};
	switch(uMsg) {
	default: break;
	case WM_CLOSE			: ::DestroyWindow(hWnd); return 0;
	case WM_KEYDOWN			: if(wParam > ::gpk::size(input.KeyboardPrevious.KeyState)) break; input.KeyboardCurrent.KeyState[wParam] = 1; return 0;
	case WM_KEYUP			: if(wParam > ::gpk::size(input.KeyboardPrevious.KeyState)) break; input.KeyboardCurrent.KeyState[wParam] = 0; return 0;
	case WM_LBUTTONDOWN		: info_printf("L Down"	); if(0 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[0] =  1; return 0;
	case WM_LBUTTONDBLCLK	: info_printf("L DClck"	); if(0 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[0] =  1; return 0;
	case WM_LBUTTONUP		: info_printf("L Up"	); if(0 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[0] =  0; return 0;
	case WM_RBUTTONDOWN		: info_printf("R Down"	); if(1 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[1] =  1; return 0;
	case WM_RBUTTONDBLCLK	: info_printf("R DClck"	); if(1 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[1] =  1; return 0;
	case WM_RBUTTONUP		: info_printf("R Up"	); if(1 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[1] =  0; return 0;
	case WM_MBUTTONDOWN		: info_printf("M Down"	); if(2 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[2] =  1; return 0;
	case WM_MBUTTONDBLCLK	: info_printf("M DClck"	); if(2 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[2] =  1; return 0;
	case WM_MBUTTONUP		: info_printf("M Up"	); if(2 > ::gpk::size(input.MouseCurrent.ButtonState)) break; input.MouseCurrent.ButtonState[2] =  0; return 0;
	case WM_MOUSEWHEEL		:
		zDelta																					= GET_WHEEL_DELTA_WPARAM(wParam);
		input.MouseCurrent.Deltas.z																= zDelta;
		return 0;
	case WM_MOUSEMOVE		: {
		int32_t																						xPos										= GET_X_LPARAM(lParam);
		int32_t																						yPos										= GET_Y_LPARAM(lParam);
		input.MouseCurrent.Position.x															= ::gpk::clamp(xPos, 0, (int32_t)mainDisplay.Size.x);
		input.MouseCurrent.Position.y															= ::gpk::clamp(yPos, 0, (int32_t)mainDisplay.Size.y);
		input.MouseCurrent.Deltas.x																= input.MouseCurrent.Position.x - input.MousePrevious.Position.x;
		input.MouseCurrent.Deltas.y																= input.MouseCurrent.Position.y - input.MousePrevious.Position.y;
		return 0;
	}
	case WM_GETMINMAXINFO	:	// Catch this message so to prevent the window from becoming too small.
		((::MINMAXINFO*)lParam)->ptMinTrackSize													= {minClientRect.right - minClientRect.left, minClientRect.bottom - minClientRect.top};
		return 0;
	case WM_SIZE			:
		if(lParam) {
			::gpk::SCoord2<uint32_t>																	newMetrics									= ::gpk::SCoord2<WORD>{LOWORD(lParam), HIWORD(lParam)}.Cast<uint32_t>();
			if(newMetrics != mainDisplay.Size) {
				mainDisplay.PreviousSize																= mainDisplay.Size;
				mainDisplay.Size																		= newMetrics;
				mainDisplay.Resized																		= true;
				mainDisplay.Repaint																		= true;
				char																						buffer		[256]							= {};
				//sprintf_s(buffer, "[%u x %u]. Last frame seconds: %g. ", (uint32_t)newMetrics.x, (uint32_t)newMetrics.y, applicationInstance.Framework.Timer.LastTimeSeconds);
				sprintf_s(buffer, "[%u x %u].", (uint32_t)newMetrics.x, (uint32_t)newMetrics.y);
#if defined(UNICODE)
#else
				SetWindowText(mainDisplay.PlatformDetail.WindowHandle, buffer);
#endif
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
	case WM_PAINT			: break;
	case WM_DESTROY			:
		::SDisplayInput																				* oldInput									= (::SDisplayInput*)::SetWindowLongPtrA(displayDetail.WindowHandle, GWLP_USERDATA, 0);
		displayDetail.WindowHandle																= 0;
		mainDisplay.Closed																		= true;
		safe_delete(oldInput);
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
#endif

					::gpk::error_t														gpk::mainWindowDestroy						(::gpk::SDisplay& mainWindow)				{
	::DestroyWindow(mainWindow.PlatformDetail.WindowHandle);
	::gpk::displayUpdate(mainWindow);
	return 0;
}

					::gpk::error_t														gpk::mainWindowCreate						(::gpk::SDisplay& mainWindow, ::gpk::SRuntimeValuesDetail& runtimeValues, ::gpk::ptr_obj<SInput>& displayInput)				{
	if(0 == displayInput)
		displayInput.create();
	::gpk::SDisplayPlatformDetail																& displayDetail								= mainWindow.PlatformDetail;
	HINSTANCE																					hInstance									= runtimeValues.EntryPointArgsWin.hInstance;
	::initWndClass(hInstance, displayDetail.WindowClassName, ::mainWndProc, displayDetail.WindowClass);
	::RegisterClassEx(&displayDetail.WindowClass);
	mainWindow.Size																			= {::BMP_SCREEN_WIDTH, ::BMP_SCREEN_HEIGHT};
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
	::SetWindowLongPtrA(mainWindow.PlatformDetail.WindowHandle, GWLP_USERDATA, (LONG_PTR)new SDisplayInput{mainWindow, displayInput});
	::ShowWindow	(displayDetail.WindowHandle, SW_SHOW);
	::UpdateWindow	(displayDetail.WindowHandle);
	::SetWindowTextA(displayDetail.WindowHandle, runtimeValues.EntryPointArgsStd.ArgsCommandLine[0]);
	return 0;
}
#endif
