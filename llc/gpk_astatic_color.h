#include "gpk_array_static.h"
#include "gpk_color.h"

#ifndef GPK_ASTATIC_COLOR_H_23624
#define GPK_ASTATIC_COLOR_H_23624

namespace gpk
{
	template<size_t _pixelCount> using	astfrgba	= ::gpk::astatic<::gpk::frgba	, _pixelCount>;
	template<size_t _pixelCount> using	astu8bgra	= ::gpk::astatic<::gpk::u8bgra	, _pixelCount>;
	template<size_t _pixelCount> using	astu8bgr	= ::gpk::astatic<::gpk::u8bgr	, _pixelCount>;
	template<size_t _pixelCount> using	astu8rgba	= ::gpk::astatic<::gpk::u8rgba	, _pixelCount>;
	template<size_t _pixelCount> using	astu8rgb	= ::gpk::astatic<::gpk::u8rgb	, _pixelCount>;

} // namespace

#endif // GPK_ASTATIC_COLOR_H_23624