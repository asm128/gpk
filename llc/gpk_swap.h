#include "gpk_platform_globals.h"

#ifndef GPK_ATMEL
#	include <memory> // this is required for ::std::swap()
#endif

#ifndef GPK_SWAP_H
#define GPK_SWAP_H

namespace gpk
{
#ifndef GPK_ATMEL
	using ::std::swap;
#else
	template <class _Ty>
	inline constexpr void swap(_Ty& _Left, _Ty& _Right) noexcept {
		_Ty _Tmp = _Left;
		_Left    = _Right;
		_Right   = _Tmp;
	}
#endif
} // namespace

#endif // GPK_SWAP_H
