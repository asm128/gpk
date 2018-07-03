#include "gpk_adam7.h"

					::gpk::error_t											gpk::adam7Sizes						(::gpk::view_array<::gpk::SCoord2<uint32_t>> & imageSizes, const ::gpk::SCoord2<uint32_t> & imageSize)	{
	imageSizes	[0]																= // 1
		{ (imageSize.x <= 4)	? 0 : imageSize.x / 8 + one_if(imageSize.x % 8)
		, (imageSize.y <= 4)	? 0 : imageSize.y / 8 + one_if(imageSize.y % 8)
		};
	imageSizes	[1]																= // 2
		{ (imageSize.x <= 4)	? 0 : (uint32_t)::gpk::max(((int32_t)imageSize.x - 4) / 8, 0) + one_if(((int32_t)imageSize.x - 4) % 8)
		, (imageSize.y <= 4)	? 0 : imageSize.y / 8 + one_if((imageSize.y) % 8)
		};
	imageSizes	[2]																= // 3
		{ (imageSize.x <= 4)	? 0 : imageSize.x / 4 + one_if(imageSize.x % 4)
		, (imageSize.y <= 4)	? 0 : (uint32_t)::gpk::max(((int32_t)imageSize.y - 4) / 8, 0) + one_if(((int32_t)imageSize.y - 4) % 8)
		};
	imageSizes	[3]																= // 4
		{ (imageSize.x <= 2)	? 0 : (uint32_t)::gpk::max(((int32_t)imageSize.x - 2) / 4, 0) + one_if(((int32_t)imageSize.x - 2) % 4)
		, (imageSize.y <= 4)	? 0 : imageSize.y / 4 + one_if(imageSize.y % 4)
		};
	imageSizes	[4]																= // 5
		{ (imageSize.x <  2)	? 0 : (imageSize.x) / 2 + one_if(imageSize.x % 2)
		, (imageSize.y <= 2)	? 0 : (uint32_t)::gpk::max(((int32_t)imageSize.y - 2) / 4, 0) + one_if(((int32_t)imageSize.y - 2) % 4)
		};
	imageSizes	[5]																= // 6
		{ (imageSize.x < 2)	? 0 : (uint32_t)::gpk::max(((int32_t)imageSize.x - 1) / 2, 0) + one_if(((int32_t)imageSize.x - 1) % 2)
		, (imageSize.y < 2)	? 0 : (imageSize.y) / 2 + one_if(imageSize.y % 2)
		};
	imageSizes	[6]																= // Adam 7
		{ imageSize.x
		, imageSize.y		/ 2 
		};
	return 0;
}
