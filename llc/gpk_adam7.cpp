#include "gpk_adam7.h"

gpk::err_t			gpk::adam7ScaleIndex
	( ::gpk::s2_t					iImage
	, ::gpk::n2<::gpk::u2_t>	& offsetMultiplier
	, ::gpk::n2<::gpk::u2_t>	& offsetBase
	)
{
	if_true_fef(iImage >= 7 || iImage < 0, "Invalid Adam7 image: %i.", iImage);
	switch(iImage) {
	case 0: offsetMultiplier = {8, 8}; offsetBase = {0, 0}; break;
	case 1: offsetMultiplier = {8, 8}; offsetBase = {4, 0}; break;
	case 2: offsetMultiplier = {4, 8}; offsetBase = {0, 4}; break;
	case 3: offsetMultiplier = {4, 4}; offsetBase = {2, 0}; break;
	case 4: offsetMultiplier = {2, 4}; offsetBase = {0, 2}; break;
	case 5: offsetMultiplier = {2, 2}; offsetBase = {1, 0}; break;
	case 6: offsetMultiplier = {1, 2}; offsetBase = {0, 1}; break;
	}
	return 0;
}

gpk::err_t			gpk::adam7Sizes			(::gpk::view<::gpk::n2<::gpk::u2_t>> & imageSizes, const ::gpk::n2<::gpk::u2_t> & imageSize)	{
	imageSizes	[0]								= // 1
		{ (imageSize.x <= 4)	? 0U : round_up(imageSize.x, ::gpk::u2_t(8))
		, (imageSize.y <= 4)	? 0U : round_up(imageSize.y, ::gpk::u2_t(8))
		};
	imageSizes	[1]								= // 2
		{ (imageSize.x <= 4)	? 0U : (::gpk::u2_t)::gpk::max(((::gpk::s2_t)imageSize.x - 4) / 8, (::gpk::s2_t)0) + one_if(((::gpk::s2_t)imageSize.x - 4) % 8)
		, (imageSize.y <= 4)	? 0U : round_up(imageSize.y, ::gpk::u2_t(8))
		};
	imageSizes	[2]								= // 3
		{ (imageSize.x <= 4)	? 0U : round_up(imageSize.x, ::gpk::u2_t(4))
		, (imageSize.y <= 4)	? 0U : (::gpk::u2_t)::gpk::max(((::gpk::s2_t)imageSize.y - 4) / 8, (::gpk::s2_t)0) + one_if(((::gpk::s2_t)imageSize.y - 4) % 8)
		};
	imageSizes	[3]								= // 4
		{ (imageSize.x <= 2)	? 0U : (::gpk::u2_t)::gpk::max(((::gpk::s2_t)imageSize.x - 2) / 4, (::gpk::s2_t)0) + one_if(((::gpk::s2_t)imageSize.x - 2) % 4)
		, (imageSize.y <= 4)	? 0U : round_up(imageSize.y, ::gpk::u2_t(4))
		};
	imageSizes	[4]								= // 5
		{ (imageSize.x <  2)	? 0U : round_up(imageSize.x, ::gpk::u2_t(2))
		, (imageSize.y <= 2)	? 0U : (::gpk::u2_t)::gpk::max(((::gpk::s2_t)imageSize.y - 2) / 4, (::gpk::s2_t)0) + one_if(((::gpk::s2_t)imageSize.y - 2) % 4)
		};
	imageSizes	[5]								= // 6
		{ (imageSize.x < 2)		? 0U : (::gpk::u2_t)::gpk::max(((::gpk::s2_t)imageSize.x - 1) / 2, (::gpk::s2_t)0) + one_if(((::gpk::s2_t)imageSize.x - 1) % 2)
		, (imageSize.y < 2)		? 0U : round_up(imageSize.y, ::gpk::u2_t(2))
		};
	imageSizes	[6]								= // Adam 7
		{ imageSize.x
		, imageSize.y >> 1
		};
	return 0;
}
