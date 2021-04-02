#include "gpk_coord.h"
#include "gpk_color.h"
#include "gpk_view_grid.h"
#include "gpk_array_static.h"
#include "gpk_array.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#elif defined(GPK_XCB)
#	include <xcb/xcb.h>
#endif

#ifndef GPK_DISPLAY_H_293874928374
#define GPK_DISPLAY_H_293874928374

namespace gpk
{
#pragma pack(push, 1)
	struct SDisplayPlatformDetail {
#if defined(GPK_WINDOWS)
							::HWND								WindowHandle								= {};
							::WNDCLASSEX						WindowClass									= {};

		static constexpr	const TCHAR							WindowClassName	[256]						= TEXT("GPK_WINDOW");
#elif defined(GPK_XCB)
							xcb_connection_t					* Connection								= {};
							xcb_gcontext_t						GC											= {};
							xcb_drawable_t						IdDrawable									= {};
							xcb_drawable_t						IdDrawableBackPixmap						= {};

																~SDisplayPlatformDetail						() {
			if(uint32_t(-1) != GC) {
				xcb_free_gc(Connection, GC);
				info_printf("%s.", "Window graphics context freed");
			}
		}
#endif
	};

#if defined(GPK_WINDOWS)
	static inline		void								initWndClass								(::HINSTANCE hInstance, const TCHAR* className, ::WNDCLASSEX& wndClassToInit, WNDPROC wndProc)	{
		wndClassToInit											= {sizeof(::WNDCLASSEX),};
		wndClassToInit.lpfnWndProc								= wndProc;
		wndClassToInit.hInstance								= hInstance;
		wndClassToInit.hCursor									= LoadCursor(NULL, IDC_ARROW);
		wndClassToInit.hbrBackground							= (::HBRUSH)(COLOR_3DFACE + 1);
		wndClassToInit.lpszClassName							= className;
	}
#endif

	enum DISPLAY_EVENT : uint16_t
		{	DISPLAY_EVENT_DESTROY
		,	DISPLAY_EVENT_CLOSE
		,	DISPLAY_EVENT_SIZE
		,	DISPLAY_EVENT_DRAW
		,	DISPLAY_EVENT_KEY_UP
		,	DISPLAY_EVENT_KEY_DOWN
		,	DISPLAY_EVENT_MOUSE_UP
		,	DISPLAY_EVENT_MOUSE_DOWN
		,	DISPLAY_EVENT_MOUSE_WHEEL
		,	DISPLAY_EVENT_MOUSE_POSITION
		,	DISPLAY_EVENT_TOUCH_BEGIN
		,	DISPLAY_EVENT_TOUCH_END
		,	DISPLAY_EVENT_TOUCH_UPDATE
		,	DISPLAY_EVENT_TOUCH_OWNERSHIP
		,	DISPLAY_EVENT_MINMAXINFO
		};

	struct SDisplayEvent {
							::gpk::DISPLAY_EVENT					Type;
							::gpk::array_static<uint8_t, 128>		Data;
	};

	struct SDisplay {
							::gpk::SDisplayPlatformDetail			PlatformDetail								= {};
							::gpk::array_pod<::gpk::SDisplayEvent>	Events										= {};
							::gpk::SCoord2<uint32_t>				PreviousSize								= {};
							::gpk::SCoord2<uint32_t>				Size										= {320, 200};
							bool									Resized										: 1;
							bool									Repaint										: 1;
							bool									NoDraw										: 1;
							bool									MinOrMaxed									: 1;
							bool									Closed										: 1;

																	SDisplay									()		noexcept
			: Resized		(true)
			, Repaint		(true)
			, NoDraw		(false)
			, MinOrMaxed	(false)
			, Closed		(false)
		{}
	};
#pragma pack(pop)

	::gpk::error_t												displayPresentTarget						(::gpk::SDisplay& displayInstance, const ::gpk::view_grid<::gpk::SColorBGRA>& targetToPresent);
	::gpk::error_t												displayUpdate								(::gpk::SDisplay& applicationInstance);
	::gpk::error_t												displayUpdateTick							(::gpk::SDisplay& displayInstance);
} // namespace

#endif // GPK_DISPLAY_H_293874928374
