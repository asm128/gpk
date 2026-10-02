#include "gpk_error.h"

#ifdef GPK_ATMEL
#	include <ustd_functional.h>
#else
#	include <functional>
#endif

#ifndef GPK_FUNCTIONAL_H_23627
#define GPK_FUNCTIONAL_H_23627

namespace gpk
{
#ifdef GPK_ATMEL
	tplt<tpnm _tFunction>	
	using	function	= ::ustd::function<_tFunction>;
#else
	tplt<tpnm _tFunction>	
	using	function	= ::std::function<_tFunction>;
#endif

	tplt<tpnm ..._tArgs>					using	FVoid				= ::gpk::function<void(_tArgs&&...)>;
	tplt<tpnm ..._tArgs>					using	FBool				= ::gpk::function<bool(_tArgs&&...)>;
	tplt<tpnm ..._tArgs>					using	FError				= ::gpk::function<::gpk::error_t(_tArgs&&...)>;
	tplt<tpnm T, tpnm ..._tArgs>			using	FTransform			= ::gpk::function<T(_tArgs&&...)>;

	tplt<tpnm T>							using	TFuncForEach		= FError<T&>;
	tplt<tpnm T>							using	TFuncForEachConst	= FError<const T&>;
	tplt<tpnm T, tpnm tCount = uint32_t>	using	TFuncEnumerate		= FError<tCount&, T&>;
	tplt<tpnm T, tpnm tCount = uint32_t>	using	TFuncEnumerateConst	= FError<tCount&, const T&>;

	tplt<tpnm TSource, tpnm TTarget>		using	TFuncAppend 		= function<::gpk::error_t(TTarget 	& output, const TSource & origin)>;
	tplt<tpnm TIO>							using	TFuncSize			= function<::gpk::error_t(const TIO & origin)>;
} // namespace

#endif // GPK_FUNCTIONAL_H_23627
