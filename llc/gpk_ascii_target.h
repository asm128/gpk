#include "gpk_ascii_color.h"
#include "gpk_image.h"

#ifndef GPK_ASCII_TARGET_H_23627
#define GPK_ASCII_TARGET_H_23627

namespace gpk
{
#pragma pack(push, 1)
	stct SASCIICell {
		u0_t	Character	= {};
		u1_t	Color		= {};
	};
#pragma pack(pop)

	stct SASCIITarget {
		imgu0_t	Characters	= {};
		imgu1_t	Colors		= {};

		inxp	n2u2_c&	metrics	() cnst nxpt	{ rtrn Characters.metrics(); }
		inxp	u2_c&		size	() cnst nxpt	{ rtrn Characters.size(); }
		inxp	bool		valid	() cnst nxpt {
			n2u2_c& characterMetrics = Characters.metrics();
			n2u2_c& colorMetrics = Colors.metrics();
			u3_c characterArea = u3_t(characterMetrics.x) * characterMetrics.y;
			u3_c colorArea = u3_t(colorMetrics.x) * colorMetrics.y;
			rtrn characterMetrics.x == colorMetrics.x
				&& characterMetrics.y == colorMetrics.y
				&& characterArea == Characters.size()
				&& colorArea == Colors.size()
				&& Characters.begin() == Characters.Texels.begin()
				&& Colors.begin() == Colors.Texels.begin()
				;
		}

		err_t			resize	(n2u2_c& newSize) {
			u3_c cellCountWide = u3_t(newSize.x) * newSize.y;
			if_true_fef(cellCountWide > 0xFFFFFFFFULL, "ASCII target dimensions exceed the supported cell count: %u x %u.", newSize.x, newSize.y);
			u2_c cellCount = u2_t(cellCountWide);
			if_fail_fef(gpk::resize(cellCount, Characters.Texels, Colors.Texels), "Cannot resize ASCII target to %u x %u.", newSize.x, newSize.y);
			Characters.View = {Characters.Texels.begin(), newSize};
			Colors.View = {Colors.Texels.begin(), newSize};
			rtrn 0;
		}
	};

	err_t	asciiTargetClear	(SASCIITarget& target, u0_t character = ' ', u1_t color = ASCII_COLOR_WHITE);
}

#endif // GPK_ASCII_TARGET_H_23627


