#include "gpk_align.h"
#include "gpk_n2.h"

#ifndef GPK_GUI_IMAGE_H_23627
#define GPK_GUI_IMAGE_H_23627

namespace gpk
{
	struct SControlImage {
		::gpk::n2s1_t		ImageOffset	= {};
		::gpk::u0_t		ImageAlign	= ::gpk::ALIGN_CENTER;
		::gpk::u0_t		ImageInvertX	: 1;
		::gpk::u0_t		ImageInvertY	: 1;
		::gpk::u0_t		Rotation	: 2;
	};
}

#endif // GPK_GUI_IMAGE_H_23627
