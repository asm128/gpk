#include "gpk_typeint.h"

#include <functional>

#ifndef GPK_FUNCTIONAL_H_23627
#define GPK_FUNCTIONAL_H_23627

namespace gpk
{
	template<typename ..._tArgs>	using	FVoid	= ::std::function<void(_tArgs&&...)>;
	template<typename ..._tArgs>	using	FBool	= ::std::function<bool(_tArgs&&...)>;

	template<typename T, typename ..._tArgs>	
	using	FTransform	= ::std::function<T(_tArgs&&...)>;

} // namespace

#endif // GPK_FUNCTIONAL_H_23627
