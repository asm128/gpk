#include "gpk_window.h"
#include "gpk_system_key.h"

#if defined(GPK_XCB)
#	include <xcb/xcb_image.h>
#	include <xkbcommon/xkbcommon-keysyms.h>
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
#elif defined(GPK_XCB)
	const u2_t				values[]					= {window.WindowedWindowRect.Left, window.WindowedWindowRect.Top, window.Size.x, window.Size.y};
	xcb_configure_window(window.PlatformDetail.Connection, window.PlatformDetail.IdDrawable, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
	xcb_flush(window.PlatformDetail.Connection);
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
#elif defined(GPK_XCB)
	xcb_get_geometry_reply_t	* geometry					= xcb_get_geometry_reply(window.PlatformDetail.Connection, xcb_get_geometry(window.PlatformDetail.Connection, window.PlatformDetail.IdDrawable), 0);
	if_null_fe(geometry);
	window.WindowedWindowRect.Left	= geometry->x;
	window.WindowedWindowRect.Top	= geometry->y;
	window.WindowedWindowRect.Right	= geometry->x + geometry->width;
	window.WindowedWindowRect.Bottom	= geometry->y + geometry->height;
	free(geometry);
	xcb_screen_t				* screen					= xcb_setup_roots_iterator(xcb_get_setup(window.PlatformDetail.Connection)).data;
	window.Size				= {screen->width_in_pixels, screen->height_in_pixels};
	const u2_t				values[]					= {0, 0, window.Size.x, window.Size.y};
	xcb_configure_window(window.PlatformDetail.Connection, window.PlatformDetail.IdDrawable, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
	xcb_flush(window.PlatformDetail.Connection);
#endif // GPK_WINDOWS
	if_true_e(failed(::gpk::eventEnqueueScreenResize(window.EventQueue, window.Size)));
	window.FullScreen		= true;
	return 0;
}

#if defined(GPK_XCB)
static	u0_t		xcbSystemKey			(xkb_keysym_t keySym) {
	if(keySym >= XKB_KEY_a && keySym <= XKB_KEY_z)
		return (u0_t)('A' + keySym - XKB_KEY_a);
	if(keySym >= XKB_KEY_A && keySym <= XKB_KEY_Z)
		return (u0_t)keySym;
	if(keySym >= XKB_KEY_0 && keySym <= XKB_KEY_9)
		return (u0_t)keySym;
	if(keySym >= XKB_KEY_KP_0 && keySym <= XKB_KEY_KP_9)
		return (u0_t)(::gpk::SYSTEM_KEY_Numpad0 + keySym - XKB_KEY_KP_0);
	if(keySym >= XKB_KEY_F1 && keySym <= XKB_KEY_F24)
		return (u0_t)(::gpk::SYSTEM_KEY_F1 + keySym - XKB_KEY_F1);

	switch(keySym) {
	default					: return 0;
	case XKB_KEY_BackSpace		: return ::gpk::SYSTEM_KEY_Backspace;
	case XKB_KEY_Tab			:
	case XKB_KEY_ISO_Left_Tab	: return ::gpk::SYSTEM_KEY_Tab;
	case XKB_KEY_Clear			: return ::gpk::SYSTEM_KEY_Clear;
	case XKB_KEY_Return			:
	case XKB_KEY_KP_Enter		: return ::gpk::SYSTEM_KEY_Enter;
	case XKB_KEY_Shift_L		: return ::gpk::SYSTEM_KEY_LShift;
	case XKB_KEY_Shift_R		: return ::gpk::SYSTEM_KEY_RShift;
	case XKB_KEY_Control_L		: return ::gpk::SYSTEM_KEY_LControl;
	case XKB_KEY_Control_R		: return ::gpk::SYSTEM_KEY_RControl;
	case XKB_KEY_Alt_L			:
	case XKB_KEY_Meta_L			: return ::gpk::SYSTEM_KEY_LMenu;
	case XKB_KEY_Alt_R			:
	case XKB_KEY_Meta_R			:
	case XKB_KEY_ISO_Level3_Shift
							: return ::gpk::SYSTEM_KEY_RMenu;
	case XKB_KEY_Pause			:
	case XKB_KEY_Break			: return ::gpk::SYSTEM_KEY_Pause;
	case XKB_KEY_Caps_Lock		: return ::gpk::SYSTEM_KEY_Capital;
	case XKB_KEY_Escape			: return ::gpk::SYSTEM_KEY_Escape;
	case XKB_KEY_space			:
	case XKB_KEY_KP_Space		: return ::gpk::SYSTEM_KEY_Space;
	case XKB_KEY_Page_Up		:
	case XKB_KEY_KP_Prior		: return ::gpk::SYSTEM_KEY_Prior;
	case XKB_KEY_Page_Down		:
	case XKB_KEY_KP_Next		: return ::gpk::SYSTEM_KEY_Next;
	case XKB_KEY_End			:
	case XKB_KEY_KP_End			: return ::gpk::SYSTEM_KEY_End;
	case XKB_KEY_Home			:
	case XKB_KEY_KP_Home		: return ::gpk::SYSTEM_KEY_Home;
	case XKB_KEY_Left			:
	case XKB_KEY_KP_Left		: return ::gpk::SYSTEM_KEY_Left;
	case XKB_KEY_Up				:
	case XKB_KEY_KP_Up			: return ::gpk::SYSTEM_KEY_Up;
	case XKB_KEY_Right			:
	case XKB_KEY_KP_Right		: return ::gpk::SYSTEM_KEY_Right;
	case XKB_KEY_Down			:
	case XKB_KEY_KP_Down		: return ::gpk::SYSTEM_KEY_Down;
	case XKB_KEY_Select			: return ::gpk::SYSTEM_KEY_Select;
	case XKB_KEY_Print			:
	case XKB_KEY_Sys_Req		: return ::gpk::SYSTEM_KEY_Snapshot;
	case XKB_KEY_Execute		: return ::gpk::SYSTEM_KEY_Execute;
	case XKB_KEY_Insert			:
	case XKB_KEY_KP_Insert		: return ::gpk::SYSTEM_KEY_Insert;
	case XKB_KEY_Delete			:
	case XKB_KEY_KP_Delete		: return ::gpk::SYSTEM_KEY_Delete;
	case XKB_KEY_Help			: return ::gpk::SYSTEM_KEY_Help;
	case XKB_KEY_Super_L		:
	case XKB_KEY_Hyper_L		: return ::gpk::SYSTEM_KEY_Lwin;
	case XKB_KEY_Super_R		:
	case XKB_KEY_Hyper_R		: return ::gpk::SYSTEM_KEY_Rwin;
	case XKB_KEY_Menu			: return ::gpk::SYSTEM_KEY_Apps;
	case XKB_KEY_KP_Multiply	: return ::gpk::SYSTEM_KEY_Multiply;
	case XKB_KEY_KP_Add			: return ::gpk::SYSTEM_KEY_Add;
	case XKB_KEY_KP_Separator	: return ::gpk::SYSTEM_KEY_Separator;
	case XKB_KEY_KP_Subtract	: return ::gpk::SYSTEM_KEY_Subtract;
	case XKB_KEY_KP_Decimal		: return ::gpk::SYSTEM_KEY_Decimal;
	case XKB_KEY_KP_Divide		: return ::gpk::SYSTEM_KEY_Divide;
	case XKB_KEY_Num_Lock		: return ::gpk::SYSTEM_KEY_Numlock;
	case XKB_KEY_Scroll_Lock	: return ::gpk::SYSTEM_KEY_Scroll;
	case XKB_KEY_semicolon		:
	case XKB_KEY_colon			: return ::gpk::SYSTEM_KEY_OEM_1;
	case XKB_KEY_equal			:
	case XKB_KEY_plus			: return ::gpk::SYSTEM_KEY_OEM_plus;
	case XKB_KEY_comma			:
	case XKB_KEY_less			: return ::gpk::SYSTEM_KEY_OEM_comma;
	case XKB_KEY_minus			:
	case XKB_KEY_underscore		: return ::gpk::SYSTEM_KEY_OEM_minus;
	case XKB_KEY_period			:
	case XKB_KEY_greater		: return ::gpk::SYSTEM_KEY_OEM_period;
	case XKB_KEY_slash			:
	case XKB_KEY_question		: return ::gpk::SYSTEM_KEY_OEM_2;
	case XKB_KEY_grave			:
	case XKB_KEY_asciitilde		: return ::gpk::SYSTEM_KEY_OEM_3;
	case XKB_KEY_bracketleft	:
	case XKB_KEY_braceleft		: return ::gpk::SYSTEM_KEY_OEM_4;
	case XKB_KEY_backslash		:
	case XKB_KEY_bar			: return ::gpk::SYSTEM_KEY_OEM_5;
	case XKB_KEY_bracketright	:
	case XKB_KEY_braceright		: return ::gpk::SYSTEM_KEY_OEM_6;
	case XKB_KEY_apostrophe		:
	case XKB_KEY_quotedbl		: return ::gpk::SYSTEM_KEY_OEM_7;
	}
}

static	xkb_keysym_t	xcbBaseKeySym			(const ::gpk::SWindowPlatformDetail & detail, xkb_keycode_t keyCode, xkb_keysym_t currentKeySym) {
	xkb_layout_index_t	layout					= xkb_state_key_get_layout(detail.XKBState, keyCode);
	if(XKB_LAYOUT_INVALID == layout)
		layout = 0;
	const xkb_keysym_t	* keySyms				= {};
	return 0 < xkb_keymap_key_get_syms_by_level(detail.XKBKeymap, keyCode, layout, 0, &keySyms) ? keySyms[0] : currentKeySym;
}

static	u2_t		xcbTextCodePoint		(const ::gpk::SWindowPlatformDetail & detail, xkb_keycode_t keyCode, xkb_keysym_t keySym) {
	const u2_t		codePoint					= xkb_state_key_get_utf32(detail.XKBState, keyCode);
	if(0 == detail.XKBComposeState)
		return codePoint;

	if(XKB_COMPOSE_FEED_ACCEPTED != xkb_compose_state_feed(detail.XKBComposeState, keySym))
		return codePoint;

	switch(xkb_compose_state_get_status(detail.XKBComposeState)) {
	default:
	case XKB_COMPOSE_NOTHING	: return codePoint;
	case XKB_COMPOSE_COMPOSING	: return 0;
	case XKB_COMPOSE_CANCELLED	:
		xkb_compose_state_reset(detail.XKBComposeState);
		return 0;
	case XKB_COMPOSE_COMPOSED	: {
		const u2_t	composed					= xkb_keysym_to_utf32(xkb_compose_state_get_one_sym(detail.XKBComposeState));
		xkb_compose_state_reset(detail.XKBComposeState);
		return composed;
	}	}
}

static	u2_t		xcbInputModifiers		(const ::gpk::SWindowPlatformDetail & detail, const ::gpk::SInputKeyboard & keyboard) {
	u2_t				modifiers					= {};
	if(0 < xkb_state_mod_name_is_active(detail.XKBState, XKB_MOD_NAME_SHIFT, XKB_STATE_MODS_EFFECTIVE)) modifiers |= ::gpk::INPUT_MODIFIER_Shift;
	if(0 < xkb_state_mod_name_is_active(detail.XKBState, XKB_MOD_NAME_CAPS , XKB_STATE_MODS_EFFECTIVE)) modifiers |= ::gpk::INPUT_MODIFIER_CapsLock;
	if(0 < xkb_state_mod_name_is_active(detail.XKBState, XKB_MOD_NAME_CTRL , XKB_STATE_MODS_EFFECTIVE)) modifiers |= ::gpk::INPUT_MODIFIER_Control;
	if(0 < xkb_state_mod_name_is_active(detail.XKBState, XKB_MOD_NAME_ALT  , XKB_STATE_MODS_EFFECTIVE)) modifiers |= ::gpk::INPUT_MODIFIER_Alt;
	if(0 < xkb_state_mod_name_is_active(detail.XKBState, XKB_MOD_NAME_NUM  , XKB_STATE_MODS_EFFECTIVE)) modifiers |= ::gpk::INPUT_MODIFIER_NumLock;
	if(0 < xkb_state_mod_name_is_active(detail.XKBState, XKB_MOD_NAME_LOGO , XKB_STATE_MODS_EFFECTIVE)) modifiers |= ::gpk::INPUT_MODIFIER_Super;
	if(keyboard.KeyState[::gpk::SYSTEM_KEY_RMenu]) modifiers |= ::gpk::INPUT_MODIFIER_AltGr;
	return modifiers;
}
#endif

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
		case XCB_KEY_PRESS		:
		case XCB_KEY_RELEASE	: {
			const bool				pressed					= XCB_KEY_PRESS == (event->response_type & ~0x80);
			const xcb_key_press_event_t & key					= *(xcb_key_press_event_t*)event;
			::gpk::SWindowPlatformDetail & detail				= displayInstance.PlatformDetail;
			const xkb_keysym_t	keySym						= xkb_state_key_get_one_sym(detail.XKBState, key.detail);
			const xkb_keysym_t	baseKeySym					= ::xcbBaseKeySym(detail, key.detail, keySym);
			const u0_t			keyCode						= ::xcbSystemKey(baseKeySym);
			const u2_t			codePoint					= pressed ? ::xcbTextCodePoint(detail, key.detail, keySym) : 0;

			xkb_state_update_key(detail.XKBState, key.detail, pressed ? XKB_KEY_DOWN : XKB_KEY_UP);
			if_fail_e(::gpk::inputKeyboardStateSet(input, keyCode, pressed, detail.XKBDeviceId, key.time * .001f));

			const ::gpk::SInputKeyEvent keyEvent = {keyCode, key.detail, detail.XKBDeviceId, keySym, ::xcbInputModifiers(detail, input.KeyboardCurrent), key.time};
			const bool			systemKey					= ::gpk::SYSTEM_KEY_Enter == keyCode && input.KeyboardCurrent.KeyState[::gpk::SYSTEM_KEY_Menu];
			if_fail_e(pressed
				? systemKey ? ::gpk::eventEnqueueKeyboardSysDown(displayInstance.EventQueue, keyEvent) : ::gpk::eventEnqueueKeyboardDown(displayInstance.EventQueue, keyEvent)
				: systemKey ? ::gpk::eventEnqueueKeyboardSysUp  (displayInstance.EventQueue, keyEvent) : ::gpk::eventEnqueueKeyboardUp  (displayInstance.EventQueue, keyEvent)
				);
			if(codePoint)
				if_fail_e(::gpk::eventEnqueueTextChar(displayInstance.EventQueue, ::gpk::SInputTextEvent{codePoint, keySym, detail.XKBDeviceId, key.detail}));
			displayInstance.Repaint = true;
			break;
		}
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
