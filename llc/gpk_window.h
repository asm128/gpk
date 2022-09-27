#include "gpk_sysevent.h"

#include "gpk_coord.h"
#include "gpk_color.h"
#include "gpk_view_grid.h"
#include "gpk_array_static.h"
#include "gpk_array.h"
#include "gpk_ptr.h"
#include "gpk_input.h"

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

	struct SWindowPlatformDetail {
#if defined(GPK_WINDOWS)
							::HWND										WindowHandle								= {};
							::WNDCLASSEX								WindowClass									= {};
							uint32_t									BitmapInfoSize								= 0;
							::BITMAPINFO								BitmapInfo									= {sizeof(::BITMAPINFO)};
							::HDC										IntermediateDeviceContext					= 0;    // <- note, we're creating, so it needs to be destroyed
							::HBITMAP									IntermediateBitmap							= 0;
							uint32_t									* PixelBits									= 0;

																		~SWindowPlatformDetail						()							{
								if(IntermediateBitmap			) ::DeleteObject	(IntermediateBitmap			);
								if(IntermediateDeviceContext	) ::DeleteDC		(IntermediateDeviceContext	);
								if(WindowHandle					) ::DestroyWindow	(WindowHandle);
							}

		static constexpr	const TCHAR									WindowClassName	[256]						= TEXT("GPK_WINDOW");
#elif defined(GPK_XCB)
							xcb_connection_t							* Connection								= {};
							xcb_gcontext_t								GC											= {};
							xcb_drawable_t								IdDrawable									= {};
							xcb_drawable_t								IdDrawableBackPixmap						= {};

																		~SWindowPlatformDetail						() {
			if(uint32_t(-1) != GC) {
				xcb_free_gc(Connection, GC);
				info_printf("%s.", "Window graphics context freed");
			}
		}
#endif
	};

	struct SWindow {
							::gpk::SWindowPlatformDetail			PlatformDetail	= {};
							::gpk::array_obj<::gpk::SSysEvent>		EventQueue		= {};
							::gpk::ptr_obj<::gpk::SInput>			Input			= {};
							::gpk::SCoord2<uint32_t>				PreviousSize	= {};
							::gpk::SCoord2<uint32_t>				Size			= {320, 200};
							bool									Resized			: 1;
							bool									Repaint			: 1;
							bool									NoDraw			: 1;
							bool									MinOrMaxed		: 1;
							bool									Closed			: 1;

																	SWindow			()		noexcept
			: Resized		(true)
			, Repaint		(true)
			, NoDraw		(false)
			, MinOrMaxed	(false)
			, Closed		(false)
		{}
	};
#pragma pack(pop)

	::gpk::error_t												windowPresentTarget						(::gpk::SWindow& displayInstance, const ::gpk::view_grid<::gpk::SColorBGRA>& targetToPresent);
	::gpk::error_t												windowUpdate							(::gpk::SWindow& applicationInstance);
	::gpk::error_t												windowUpdateTick						(::gpk::SWindow& displayInstance);
} // namespace

#endif // GPK_DISPLAY_H_293874928374
