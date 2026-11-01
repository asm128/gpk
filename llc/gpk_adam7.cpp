#include "gpk_adam7.h"

gpk::err_t			gpk::adam7ScaleIndex
	( s2_t			iImage
	, n2u2_t		& offsetMultiplier
	, n2u2_t		& offsetBase
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
gpk::err_t	gpk::adam7Sizes	(view<n2u2_t> & imageSizes, cnst n2u2_t & imageSize) {
	if_true_fef(imageSizes.size() < 7, "Pass storage:%u, required:7.", imageSizes.size());
	for(u2_t iPass = 0; iPass < 7; ++iPass) {
		n2u2_t multiplier = {}, base = {};
		if_fail_fe(adam7ScaleIndex(iPass, multiplier, base));
		imageSizes[iPass] =
			{ imageSize.x <= base.x ? 0U : round_up(imageSize.x - base.x, multiplier.x)
			, imageSize.y <= base.y ? 0U : round_up(imageSize.y - base.y, multiplier.y)
			};
	}
	rtrn 0;
}
