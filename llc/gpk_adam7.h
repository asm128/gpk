#include "gpk_image.h"

#ifndef GPK_ADAM7_H_928734982734
#define GPK_ADAM7_H_928734982734

namespace gpk
{
					::gpk::error_t							adam7Sizes						(::gpk::view_array<::gpk::SCoord2<uint32_t>> & imageSizes, const ::gpk::SCoord2<uint32_t> & imageSize);
					::gpk::error_t							adam7ScaleIndex
		( int32_t					iImage
		, ::gpk::SCoord2<uint32_t>	& offsetMultiplier
		, ::gpk::SCoord2<uint32_t>	& offsetBase
		);

	template<typename _tTexel>
	static			::gpk::error_t							adam7Interlace					(::gpk::view_array<::gpk::SImage<_tTexel>> images, ::gpk::view2d<_tTexel> & out_View)				{
		for(uint32_t iImage = 0; iImage < images.size(); ++iImage) {
			::gpk::SCoord2<uint32_t>									offsetMultiplier				= {1, 1};
			::gpk::SCoord2<uint32_t>									offsetBase						= {0, 0};
			gpk_necall(::gpk::adam7ScaleIndex(iImage, offsetMultiplier, offsetBase), "Invalid Adam7 image? Image index: %i.", iImage);
			::gpk::SImage<_tTexel>										& image							= images[iImage];
			for(uint32_t y = 0; y < image.metrics().y; ++y)
			for(uint32_t x = 0; x < image.metrics().x; ++x) {
				::gpk::SCoord2<uint32_t>									targetCell						= {x * offsetMultiplier.x + offsetBase.x, y * offsetMultiplier.y + offsetBase.y};
				if(targetCell.y < out_View.metrics().y) {
					::gpk::view_array<_tTexel>									scanline						= out_View[targetCell.y];
					if(targetCell.x < scanline.size())	// Maybe return an error if not?
						scanline[targetCell.x]									= image.View[y][x];
				}
			}
		}
		return 0;
	}
} // namespace

#endif // GPK_ADAM7_H_928734982734
