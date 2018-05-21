#include "gpk_grid_view.h"		// for grid_view<>
#include "gpk_ascii_target.h"	// for SASCIITarget

#ifndef GPK_ASCII_DISPLAY_H_34234234
#define GPK_ASCII_DISPLAY_H_34234234

namespace gpk
{
					::gpk::error_t									asciiDisplayCreate					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::gpk::error_t									asciiDisplayDestroy					();
					::gpk::error_t									asciiDisplayPresent					(const ::gpk::view_const_uint8& characters, const ::gpk::view_const_uint16& colorRefs);
					::gpk::error_t									asciiDisplayClear					(uint8_t character = ' ', uint16_t colorRef = ::gpk::ASCII_COLOR_WHITE);
					::gpk::error_t									asciiDisplaySize					(::gpk::SCoord2<uint32_t>& size);

					::gpk::error_t									asciiDisplayResize					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::gpk::error_t									asciiDisplayPaletteSet				(const ::gpk::array_view<::gpk::SColorRGBA>& palette);
					::gpk::error_t									asciiDisplayPaletteReset			();
					::gpk::error_t									asciiDisplayTitleSet				(const ::gpk::view_const_string& title);

	template <typename _tUnit>
	static inline	::gpk::error_t									asciiDisplayCreate					(const ::gpk::SCoord2<_tUnit>& displaySize)				{ return ::gpk::asciiDisplayCreate(displaySize.x, displaySize.y); }
	static inline	::gpk::error_t									asciiDisplayPresent					(const ::gpk::SASCIITarget& renderTarget)				{
		return ::gpk::asciiDisplayPresent
			( ::gpk::view_const_uint8 {renderTarget.Characters	.begin(), renderTarget.Characters	.size()}
			, ::gpk::view_const_uint16{renderTarget.Colors		.begin(), renderTarget.Colors		.size()}
			);
	}
}

#endif // GPK_ASCII_DISPLAY_H_34234234
