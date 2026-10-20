#include "gpk_ascii_target.h"

::gpk::err_t gpk::asciiTargetClear(::gpk::SASCIITarget& target, ::gpk::u0_t character, ::gpk::u1_t color) {
	if_true_fef(false == target.valid(), "Cannot clear mismatched ASCII target planes. Character metrics:%u x %u, cells:%u. Color metrics:%u x %u, cells:%u."
		, target.Characters.metrics().x, target.Characters.metrics().y, target.Characters.size()
		, target.Colors.metrics().x, target.Colors.metrics().y, target.Colors.size()
		);
	if_fail_fe(target.Characters.View.fill(character));
	if_fail_fe(target.Colors.View.fill(color));
	rtrn 0;
}


