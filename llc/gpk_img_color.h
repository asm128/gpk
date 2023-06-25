#include "gpk_image.h"
#include "gpk_color.h"
#include "gpk_array_obj.h"
#include "gpk_ptr_obj.h"

#ifndef GPK_IMG_COLOR_H_90238749823611
#define GPK_IMG_COLOR_H_90238749823611

namespace gpk
{
	typedef	img<::gpk::bgr  >	img8bgr	;
	typedef	img<::gpk::rgb  >	img8rgb	;
	typedef	img<::gpk::bgra >	img8bgra;
	typedef	img<::gpk::rgba >	img8rgba;
	typedef	img<::gpk::rgbaf>	imgfrgba;

	typedef rt<::gpk::bgra, uint32_t>	rtbgra8d32;
	typedef rt<::gpk::rgba, uint32_t>	rtrgba8d32;
	typedef rt<::gpk::bgra, uint8_t>	rtbgra8s8;
	typedef rt<::gpk::rgba, uint8_t>	rtrgba8s8;

	typedef	pobj<::gpk::img8bgr		>		pimg8bgr ;
	typedef	pobj<::gpk::img8rgb		>		pimg8rgb ;
	typedef	pobj<::gpk::img8bgra	>		pimg8bgra;
	typedef	pobj<::gpk::img8rgba	>		pimg8rgba;
	typedef	pobj<::gpk::imgfrgba	>		pimgfrgba;

	typedef pobj<::gpk::rtbgra8d32		>	prtbgra8d32;
	typedef pobj<::gpk::rtrgba8d32		>	prtrgba8d32;
	typedef pobj<::gpk::rtbgra8s8		>	prtbgra8s8;
	typedef pobj<::gpk::rtrgba8s8		>	prtrgba8s8;

	typedef	aobj<::gpk::pimg8bgr		>	apimg8bgr ;
	typedef	aobj<::gpk::pimg8rgb		>	apimg8rgb ;
	typedef	aobj<::gpk::pimg8bgra		>	apimg8bgra;
	typedef	aobj<::gpk::pimg8rgba		>	apimg8rgba;
	typedef	aobj<::gpk::pimgfrgba		>	apimgfrgba;

	typedef aobj<::gpk::prtbgra8d32	>	aprtbgra8d32;
	typedef aobj<::gpk::prtrgba8d32	>	aprtrgba8d32;
	typedef aobj<::gpk::prtbgra8s8		>	aprtbgra8s8;
	typedef aobj<::gpk::prtrgba8s8		>	aprtrgba8s8;
} // namespace

#endif // GPK_IMG_COLOR_H_90238749823611
