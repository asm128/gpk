#include "gpk_typeint.h"

#include <functional>

#ifndef GPK_FUNCTIONAL_H_23627
#define GPK_FUNCTIONAL_H_23627

namespace gpk
{
	tplt<tpnm ..._tArgs>					using	FVoid				= ::std::function<void(_tArgs&&...)>;
	tplt<tpnm ..._tArgs>					using	FBool				= ::std::function<bool(_tArgs&&...)>;

	tplt<tpnm T>							using	TFuncForEach		= FVoid<T&>;
	tplt<tpnm T>							using	TFuncForEachConst	= FVoid<const T&>;
	tplt<tpnm T, tpnm tCount = uint32_t>	using	TFuncEnumerate		= FVoid<tCount&, T&>;
	tplt<tpnm T, tpnm tCount = uint32_t>	using	TFuncEnumerateConst	= FVoid<tCount&, const T&>	;

	tplt<tpnm T, tpnm ..._tArgs>			using	FTransform			= ::std::function<T(_tArgs&&...)>;
} // namespace

#endif // GPK_FUNCTIONAL_H_23627
