#include "gpk_image.h"

#ifndef GPK_ADAM7_H_928734982734
#define GPK_ADAM7_H_928734982734

namespace gpk
{
	::gpk::error_t			adam7Sizes			(::gpk::view<::gpk::n2u32> & imageSizes, const ::gpk::n2<uint32_t> & imageSize);
	::gpk::error_t			adam7ScaleIndex
		( int32_t		iImage
		, ::gpk::n2u32	& offsetMultiplier
		, ::gpk::n2u32	& offsetBase
		);

	template<typename _tTexel>
	static	::gpk::error_t	adam7Interlace		(::gpk::view<::gpk::img<_tTexel>> images, ::gpk::grid<_tTexel> & out_View)				{
		for(uint32_t iImage = 0; iImage < images.size(); ++iImage) {
			::gpk::n2u32				offsetMultiplier	= {1, 1};
			::gpk::n2u32				offsetBase			= {0, 0};
			gpk_necall(::gpk::adam7ScaleIndex(iImage, offsetMultiplier, offsetBase), "Invalid Adam7 image? Image index: %i.", iImage);
			::gpk::img<_tTexel>			& image				= images[iImage];
			for(uint32_t y = 0; y < image.metrics().y; ++y)
			for(uint32_t x = 0; x < image.metrics().x; ++x) {
				::gpk::n2u32				targetCell			= {x * offsetMultiplier.x + offsetBase.x, y * offsetMultiplier.y + offsetBase.y};
				if(targetCell.y < out_View.metrics().y) {
					::gpk::view<_tTexel>		scanline			= out_View[targetCell.y];
					if(targetCell.x < scanline.size())	// Maybe return an error if not?
						scanline[targetCell.x]	= image.View[y][x];
				}
			}
		}
		return 0;
	}
} // namespace

#endif // GPK_ADAM7_H_928734982734
