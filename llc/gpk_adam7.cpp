#include "gpk_adam7.h"

::gpk::error_t			gpk::adam7ScaleIndex
	( int32_t					iImage
	, ::gpk::n2<uint32_t>	& offsetMultiplier
	, ::gpk::n2<uint32_t>	& offsetBase
	)
{
	ree_if(iImage >= 7 || iImage < 0, "Invalid Adam7 image: %i.", iImage);
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

::gpk::error_t			gpk::adam7Sizes			(::gpk::view<::gpk::n2<uint32_t>> & imageSizes, const ::gpk::n2<uint32_t> & imageSize)	{
	imageSizes	[0]								= // 1
		{ (imageSize.x <= 4)	? 0U : round_up(imageSize.x, 8)
		, (imageSize.y <= 4)	? 0U : round_up(imageSize.y, 8)
		};
	imageSizes	[1]								= // 2
		{ (imageSize.x <= 4)	? 0U : (uint32_t)::gpk::max(((int32_t)imageSize.x - 4) / 8, (int32_t)0) + one_if(((int32_t)imageSize.x - 4) % 8)
		, (imageSize.y <= 4)	? 0U : round_up(imageSize.y, 8)
		};
	imageSizes	[2]								= // 3
		{ (imageSize.x <= 4)	? 0U : round_up(imageSize.x, 4)
		, (imageSize.y <= 4)	? 0U : (uint32_t)::gpk::max(((int32_t)imageSize.y - 4) / 8, (int32_t)0) + one_if(((int32_t)imageSize.y - 4) % 8)
		};
	imageSizes	[3]								= // 4
		{ (imageSize.x <= 2)	? 0U : (uint32_t)::gpk::max(((int32_t)imageSize.x - 2) / 4, (int32_t)0) + one_if(((int32_t)imageSize.x - 2) % 4)
		, (imageSize.y <= 4)	? 0U : round_up(imageSize.y, 4)
		};
	imageSizes	[4]								= // 5
		{ (imageSize.x <  2)	? 0U : round_up(imageSize.x, 2)
		, (imageSize.y <= 2)	? 0U : (uint32_t)::gpk::max(((int32_t)imageSize.y - 2) / 4, (int32_t)0) + one_if(((int32_t)imageSize.y - 2) % 4)
		};
	imageSizes	[5]								= // 6
		{ (imageSize.x < 2)		? 0U : (uint32_t)::gpk::max(((int32_t)imageSize.x - 1) / 2, (int32_t)0) + one_if(((int32_t)imageSize.x - 1) % 2)
		, (imageSize.y < 2)		? 0U : round_up(imageSize.y, 2)
		};
	imageSizes	[6]								= // Adam 7
		{ imageSize.x
		, imageSize.y >> 1
		};
	return 0;
}
