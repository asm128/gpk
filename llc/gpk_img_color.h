#include "gpk_image.h"
#include "gpk_color.h"
#include "gpk_array_obj.h"
#include "gpk_ptr_obj.h"

#ifndef GPK_IMG_COLOR_H_23627
#define GPK_IMG_COLOR_H_23627

nmsp gpk
{
	tydf	img<::gpk::bgr  >	img8bgr	;
	tydf	img<::gpk::rgb  >	img8rgb	;
	tydf	img<::gpk::bgra >	img8bgra;
	tydf	img<::gpk::rgba >	img8rgba;
	tydf	img<::gpk::rgbaf>	imgfrgba;

	tydf rt<::gpk::bgra, u2_t>	rtbgra8d32;
	tydf rt<::gpk::rgba, u2_t>	rtrgba8d32;
	tydf rt<::gpk::bgra, u0_t>	rtbgra8s8;
	tydf rt<::gpk::rgba, u0_t>	rtrgba8s8;

	tydf	pobj<::gpk::img8bgr		>		pimg8bgr ;
	tydf	pobj<::gpk::img8rgb		>		pimg8rgb ;
	tydf	pobj<::gpk::img8bgra	>		pimg8bgra;
	tydf	pobj<::gpk::img8rgba	>		pimg8rgba;
	tydf	pobj<::gpk::imgfrgba	>		pimgfrgba;

	tydf pobj<::gpk::rtbgra8d32		>	prtbgra8d32;
	tydf pobj<::gpk::rtrgba8d32		>	prtrgba8d32;
	tydf pobj<::gpk::rtbgra8s8		>	prtbgra8s8;
	tydf pobj<::gpk::rtrgba8s8		>	prtrgba8s8;

	tydf	aobj<::gpk::pimg8bgr		>	apimg8bgr ;
	tydf	aobj<::gpk::pimg8rgb		>	apimg8rgb ;
	tydf	aobj<::gpk::pimg8bgra		>	apimg8bgra;
	tydf	aobj<::gpk::pimg8rgba		>	apimg8rgba;
	tydf	aobj<::gpk::pimgfrgba		>	apimgfrgba;

	tydf aobj<::gpk::prtbgra8d32	>	aprtbgra8d32;
	tydf aobj<::gpk::prtrgba8d32	>	aprtrgba8d32;
	tydf aobj<::gpk::prtbgra8s8		>	aprtbgra8s8;
	tydf aobj<::gpk::prtrgba8s8		>	aprtrgba8s8;
} // nmsp

#endif // GPK_IMG_COLOR_H_23627
