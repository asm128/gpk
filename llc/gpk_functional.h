#include "gpk_platform.h"

#include <functional>

#ifndef GPK_FUNCTIONAL_H_230518
#define GPK_FUNCTIONAL_H_230518

namespace gpk
{
	template<typename ..._tArgs>	using	FVoid	= ::std::function<void(_tArgs&&...)>;
} // namespace

#endif // GPK_FUNCTIONAL_H_230518
