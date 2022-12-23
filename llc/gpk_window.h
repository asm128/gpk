#include "gpk_sysevent.h"

#include "gpk_array_static.h"
#include "gpk_ptr.h"
#include "gpk_input.h"
#include "gpk_image.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#elif defined(GPK_XCB)
#	include <xcb/xcb.h>
#endif

#include "gpk_color.h"
#ifndef GPK_DISPLAY_H_293874928374
#define GPK_DISPLAY_H_293874928374

namespace gpk
{

	struct SWindowPlatformDetail {
#if defined(GPK_WINDOWS)
		typedef				::gpk::SColorBGRA						TOSWindowNativeTexel;
		static constexpr	const TCHAR								DefaultRootWindowClassName	[256]			= TEXT("GPK_WINDOW");

							::HWND									WindowHandle								= {};
							::WNDCLASSEX							WindowClass									= {sizeof(::WNDCLASSEX), CS_DBLCLKS, 0, 0, 0, 0, 0, 0, 0, 0, DefaultRootWindowClassName, 0};
							uint32_t								BitmapInfoSize								= 0;
							::BITMAPINFO							BitmapInfo									= {sizeof(::BITMAPINFO)};
							::HDC									IntermediateDeviceContext					= 0;    // <- note, we're creating, so it needs to be destroyed
							::HBITMAP								IntermediateBitmap							= 0;
							::gpk::view_array<TOSWindowNativeTexel>	PixelBits									= {};

																	~SWindowPlatformDetail						()							{
			if(IntermediateBitmap			) ::DeleteObject	(IntermediateBitmap			);
			if(IntermediateDeviceContext	) ::DeleteDC		(IntermediateDeviceContext	);
			if(WindowHandle					) ::DestroyWindow	(WindowHandle);
		}
#elif defined(GPK_XCB)
		typedef				::gpk::SColorBGRA						TOSWindowNativeTexel;
							xcb_connection_t						* Connection								= {};
							xcb_gcontext_t							GC											= {};
							xcb_drawable_t							IdDrawable									= {};
							xcb_drawable_t							IdDrawableBackPixmap						= {};

																	~SWindowPlatformDetail						() {
			if(uint32_t(-1) != GC) {
				xcb_free_gc(Connection, GC);
				info_printf("%s.", "Window graphics context freed");
			}
		}
#endif
	};

	struct SWindow {
		typedef				::gpk::SColorBGRA						TTexel;
		typedef				::gpk::SRenderTarget<TTexel, uint32_t>	TOffscreen;

							::gpk::SWindowPlatformDetail			PlatformDetail		= {};
							::gpk::array_obj<::gpk::SSysEvent>		EventQueue			= {};
							::gpk::pobj<TOffscreen>				BackBuffer			= {};
							::gpk::pobj<::gpk::SInput>			Input				= {};
							::gpk::SCoord2<uint32_t>				PreviousSize		= {};
							::gpk::SCoord2<uint32_t>				Size				= {320, 200};
							::gpk::SRectLimits<int32_t>				WindowedWindowRect	= {0, 0, 320, 200};

							bool									Resized				: 1;
							bool									Repaint				: 1;
							bool									NoDraw				: 1;
							bool									MinOrMaxed			: 1;
							bool									Closed				: 1;
							bool									FullScreen			: 1;
	};

	::gpk::error_t												windowPresentTarget						(::gpk::SWindow & windowInstance, const ::gpk::view_grid<::gpk::SColorBGRA>& targetToPresent);
	::gpk::error_t												windowUpdate							(::gpk::SWindow & windowInstance);
	::gpk::error_t												windowUpdateTick						(::gpk::SWindow & windowInstance);
	::gpk::error_t												fullScreenExit							(::gpk::SWindow & windowInstance);
	::gpk::error_t												fullScreenEnter							(::gpk::SWindow & windowInstance);
	static inline ::gpk::error_t								fullScreenToggle						(::gpk::SWindow & windowInstance) { return windowInstance.FullScreen ? ::gpk::fullScreenExit(windowInstance) : ::gpk::fullScreenEnter(windowInstance); }

} // namespace

#endif // GPK_DISPLAY_H_293874928374
