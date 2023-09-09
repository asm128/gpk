#include "gpk_input.h"
#include "gpk_event_input.h"
#include "gpk_event_screen.h"

#include "gpk_array_static.h"
#include "gpk_ptr_obj.h"
#include "gpk_image.h"
#include "gpk_rect.h"
#include "gpk_grid_color.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#elif defined(GPK_XCB)
#	include <xcb/xcb.h>
#endif

#ifndef GPK_WINDOW_H_23627
#define GPK_WINDOW_H_23627

namespace gpk
{

	struct SWindowPlatformDetail {
#if defined(GPK_WINDOWS)
		typedef	::gpk::bgra			TOSWindowNativeTexel;
		stacxpr	::gpk::astatic<TCHAR, 256>	DefaultRootWindowClassName	= {TEXT("GPK_WINDOW")};

		::HWND								WindowHandle				= {};
		::WNDCLASSEX						WindowClass					= {sizeof(::WNDCLASSEX), CS_DBLCLKS, 0, 0, 0, 0, 0, 0, 0, 0, DefaultRootWindowClassName.begin(), 0};
		uint32_t							BitmapInfoSize				= 0;
		::BITMAPINFO						BitmapInfo					= {sizeof(::BITMAPINFO)};
		::HDC								IntermediateDeviceContext	= 0;    // <- note, we're creating, so it needs to be destroyed
		::HBITMAP							IntermediateBitmap			= 0;
		::gpk::view<TOSWindowNativeTexel>	PixelBits					= {};

											~SWindowPlatformDetail		()	{
			if(IntermediateBitmap			) ::DeleteObject	(IntermediateBitmap			);
			if(IntermediateDeviceContext	) ::DeleteDC		(IntermediateDeviceContext	);
			if(WindowHandle					) ::DestroyWindow	(WindowHandle);
		}
#elif defined(GPK_XCB)
		typedef	::gpk::bgra					TOSWindowNativeTexel;
		xcb_connection_t					* Connection				= {};
		xcb_gcontext_t						GC							= {};
		xcb_drawable_t						IdDrawable					= {};
		xcb_drawable_t						IdDrawableBackPixmap		= {};

											~SWindowPlatformDetail		()	{
			if(uint32_t(-1) != GC) {
				xcb_free_gc(Connection, GC);
				info_printf("%s.", "Window graphics context freed");
			}
		}
#endif
	};

	struct SWindow {
		typedef	::gpk::bgra					TTexel;
		typedef	::gpk::rt<TTexel, uint32_t>	TOffscreen;

		::gpk::SWindowPlatformDetail		PlatformDetail			= {};
		::gpk::apobj<::gpk::SEventSystem>	EventQueue				= {};
		::gpk::pobj<TOffscreen>				BackBuffer				= {};
		::gpk::pobj<::gpk::SInput>			Input					= {};
		::gpk::n2u16						PreviousSize			= {};
		::gpk::n2u16						Size					= {320, 200};
		::gpk::rectu16						WindowedWindowRect		= {0, 0, 320, 200};

		bool								Resized					: 1;
		bool								Repaint					: 1;
		bool								NoDraw					: 1;
		bool								MinOrMaxed				: 1;
		bool								Closed					: 1;
		bool								FullScreen				: 1;
	};

	::gpk::error_t						windowPresentTarget		(::gpk::SWindow & windowInstance, const ::gpk::g8bgra & targetToPresent);
	::gpk::error_t						windowUpdate			(::gpk::SWindow & windowInstance);
	::gpk::error_t						windowUpdateTick		(::gpk::SWindow & windowInstance);
	::gpk::error_t						fullScreenExit			(::gpk::SWindow & windowInstance);
	::gpk::error_t						fullScreenEnter			(::gpk::SWindow & windowInstance);

	stainli ::gpk::error_t				fullScreenToggle		(::gpk::SWindow & windowInstance) { 
		return windowInstance.FullScreen ? ::gpk::fullScreenExit(windowInstance) : ::gpk::fullScreenEnter(windowInstance); 
	}

} // namespace

#endif // GPK_WINDOW_H_23627
