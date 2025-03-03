#include "gpk_array_pod.h"
#include "gpk_array_obj.h"

#ifndef GPK_ARRAY_H_23627
#define GPK_ARRAY_H_23627

namespace gpk
{
	tplt <tpnm T>	using aaobj		= ::gpk::aobj<::gpk::aobj<T>>; 
	tplt <tpnm T>	using aapod		= ::gpk::aobj<::gpk::apod<T>>; 

	typedef	::gpk::aapod<uc_t	>	aauchar		;
	typedef	::gpk::aapod<sc_t		>	aachar		;
	typedef	::gpk::aapod<float		>	aafloat		, aaf32;
	typedef	::gpk::aapod<double		>	aadouble	, aaf64;
	typedef	::gpk::aapod<uint8_t	>	aauint8		, aau8;
	typedef	::gpk::aapod<uint16_t	>	aauint16	, aau16;
	typedef	::gpk::aapod<uint32_t	>	aauint32	, aau32;
	typedef	::gpk::aapod<uint64_t	>	aauint64	, aau64;
	typedef	::gpk::aapod<int8_t		>	aaint8		, aai8;
	typedef	::gpk::aapod<int16_t	>	aaint16		, aai16;
	typedef	::gpk::aapod<int32_t	>	aaint32		, aai32;
	typedef	::gpk::aapod<int64_t	>	aaint64		, aai64;
} // namespace 

#endif // GPK_ARRAY_H_23627
