#include "gpk_image.h"

#ifndef GPK_ADAM7_H_23627
#define GPK_ADAM7_H_23627

namespace gpk
{
	err_t			adam7Sizes			(view<n2u2_t> & imageSizes, const n2<u2_t> & imageSize);
	err_t			adam7ScaleIndex
		( s2_t					iImage
		, n2u2_t	&		offsetMultiplier
		, n2u2_t	&		offsetBase
		);
	tplt<tpnm _tTexel>
	sttc	err_t	adam7Interlace		(view<img<_tTexel>> images, grid<_tTexel> & out_View)				{
		for(u2_t iImage = 0; iImage < images.size(); ++iImage) {
			n2u2_t				offsetMultiplier	= {1, 1};
			n2u2_t				offsetBase			= {0, 0};
			gpk_necall(gpk::adam7ScaleIndex(iImage, offsetMultiplier, offsetBase), "Invalid Adam7 image? Image index: %i.", iImage);
			img<_tTexel>			& image				= images[iImage];
			for(u2_t y = 0; y < image.metrics().y; ++y)
			for(u2_t x = 0; x < image.metrics().x; ++x) {
				n2u2_t				targetCell			= {x * offsetMultiplier.x + offsetBase.x, y * offsetMultiplier.y + offsetBase.y};
				if(targetCell.y < out_View.metrics().y) {
					view<_tTexel>		scanline			= out_View[targetCell.y];
					if(targetCell.x < scanline.size())	// Maybe return an error if not?
						scanline[targetCell.x]	= image.View[y][x];
				}
			}
		}
		return 0;
	}
} // namespace

#endif // GPK_ADAM7_H_23627


