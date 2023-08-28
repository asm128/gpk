#include "gpk_typeint.h"

#ifdef GPK_ARDUINO
#	include <ustd_functional.h>
#else
#	include <functional>
#endif

#ifndef GPK_FUNCTIONAL_H_23627
#define GPK_FUNCTIONAL_H_23627

namespace gpk
{
#ifdef GPK_ARDUINO
	tplt<tpnm _tFunction>	
	using	function	= ::ustd::function<_tFunction>;
#else
	tplt<tpnm _tFunction>	
	using	function	= ::std::function<_tFunction>;
#endif

	tplt<tpnm ..._tArgs>					using	FVoid				= ::gpk::function<void(_tArgs&&...)>;
	tplt<tpnm ..._tArgs>					using	FBool				= ::gpk::function<bool(_tArgs&&...)>;
	tplt<tpnm T, tpnm ..._tArgs>			using	FTransform			= ::gpk::function<T(_tArgs&&...)>;

	tplt<tpnm T>							using	TFuncForEach		= FVoid<T&>;
	tplt<tpnm T>							using	TFuncForEachConst	= FVoid<const T&>;
	tplt<tpnm T, tpnm tCount = uint32_t>	using	TFuncEnumerate		= FVoid<tCount&, T&>;
	tplt<tpnm T, tpnm tCount = uint32_t>	using	TFuncEnumerateConst	= FVoid<tCount&, const T&>;

} // namespace

#endif // GPK_FUNCTIONAL_H_23627
