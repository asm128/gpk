#include "gpk_image.h"

#ifndef GPK_ADAM7_H_928734982734
#define GPK_ADAM7_H_928734982734

namespace gpk
{
					::gpk::error_t											adam7Sizes						(::gpk::view_array<::gpk::SCoord2<uint32_t>> & imageSizes, const ::gpk::SCoord2<uint32_t> & imageSize);

	template<typename _tTexel> 
	static			::gpk::error_t											adam7Interlace					(::gpk::view_array<::gpk::SImage<_tTexel>> images, ::gpk::view_grid<_tTexel> & out_View)				{
		for(uint32_t iImage = 0; iImage < images.size(); ++iImage) {
			::gpk::SCoord2<uint32_t>													offsetMultiplier								= {1, 1};
			::gpk::SCoord2<uint32_t>													offsetBase										= {0, 0};
			::gpk::SImage<_tTexel>														& image											= images[iImage];
			switch(iImage) {
			case 0: offsetMultiplier = {8, 8}; offsetBase = {0, 0}; break;
			case 1: offsetMultiplier = {8, 8}; offsetBase = {4, 0}; break;
			case 2: offsetMultiplier = {4, 8}; offsetBase = {0, 4}; break;
			case 3: offsetMultiplier = {4, 4}; offsetBase = {2, 0}; break;
			case 4: offsetMultiplier = {2, 4}; offsetBase = {0, 2}; break;
			case 5: offsetMultiplier = {2, 2}; offsetBase = {1, 0}; break;
			case 6: offsetMultiplier = {1, 2}; offsetBase = {0, 1}; break;
			}
			for(uint32_t y = 0; y < image.metrics().y; ++y)
			for(uint32_t x = 0; x < image.metrics().x; ++x) {
				/*if(iImage == 0) */{
					::gpk::SCoord2<uint32_t>												targetCell										= {x * offsetMultiplier.x + offsetBase.x, y * offsetMultiplier.y + offsetBase.y};
					_tTexel																	inputColor										= image.View[y][x];
					if(targetCell.y < out_View.metrics().y)
						if(targetCell.x < out_View[targetCell.y].size())
							out_View[targetCell.y][targetCell.x]								= inputColor;
				}
			}
		}
		return 0;
	}
} // namespace

#endif // GPK_ADAM7_H_928734982734
