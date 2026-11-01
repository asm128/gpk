#include "gpk_array_static.h"

#include "gpk_view_serialize.h"

#ifndef GPK_ASTATIC_SERIALIZE_H_23627
#define GPK_ASTATIC_SERIALIZE_H_23627

namespace gpk
{
	tplt<tpnm _tPOD, ::gpk::u2_t _nSize> 
	gpk::err_t 		loadView					(::gpk::vcu0_t & input, ::gpk::astatic<_tPOD, _nSize> & output) { 
		::gpk::view<const _tPOD>	readView					= {}; 
		::gpk::u2_t					bytesRead					= 0;
		if_fail_fe(bytesRead = ::gpk::viewRead(readView, input)); 
		if_fail_fe(input.slice(input, bytesRead));
		memcpy(output.begin(), readView.begin(), ::gpk::min(readView.byte_count(), ::gpk::view<_tPOD>{output}.byte_count()));
		return 0;
	}
} // namespace

#endif // GPK_ASTATIC_SERIALIZE_H_23627
