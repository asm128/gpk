#include "gpk_coord.h"
#include "gpk_color.h"
#include "gpk_grid_view.h"

#if defined(GPK_WINDOWS)
#	include <windows.h>
#endif 

#ifndef GPK_DISPLAY_H_293874928374
#define GPK_DISPLAY_H_293874928374

namespace gpk 
{
	struct SDisplayPlatformDetail {
#if defined(GPK_WINDOWS)
							::HWND													WindowHandle								= {};
							::WNDCLASSEX											WindowClass									= {};

		static constexpr	const TCHAR												WindowClassName	[256]						= TEXT("SUGAR_WINDOW");
#endif
	};

#if defined(GPK_WINDOWS)
	static inline		void													initWndClass								(::HINSTANCE hInstance, const TCHAR* className, ::WNDCLASSEX& wndClassToInit, WNDPROC wndProc)	{
		wndClassToInit																= {sizeof(::WNDCLASSEX),};
		wndClassToInit.lpfnWndProc													= wndProc;
		wndClassToInit.hInstance													= hInstance;
		wndClassToInit.hCursor														= LoadCursor(NULL, IDC_ARROW);
		wndClassToInit.hbrBackground												= (::HBRUSH)(COLOR_3DFACE + 1);
		wndClassToInit.lpszClassName												= className;
	}
#endif

	struct SDisplay {
							::gpk::SDisplayPlatformDetail							PlatformDetail								= {};
							::gpk::SCoord2<uint32_t>								PreviousSize								= {};
							::gpk::SCoord2<uint32_t>								Size										= {};
							bool													Resized										: 1;
							bool													Repaint										: 1;
							bool													NoDraw										: 1;
							bool													MinOrMaxed									: 1;
							bool													Closed										: 1;

																					SDisplay									()		noexcept
			: Resized		(false)
			, Repaint		(false)
			, NoDraw		(false)
			, MinOrMaxed	(false)
			, Closed		(false)
		{}
	};

		::gpk::error_t																displayPresentTarget						(::gpk::SDisplay& displayInstance, const ::gpk::grid_view<::gpk::SColorBGRA>& targetToPresent);
		::gpk::error_t																displayUpdate								(::gpk::SDisplay& applicationInstance);
		::gpk::error_t																displayUpdateTick							(::gpk::SDisplay& displayInstance);

} // namespace

#endif // GPK_DISPLAY_H_293874928374
