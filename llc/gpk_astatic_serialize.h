#include "gpk_array_static.h"

#include "gpk_view_serialize.h"

#ifndef GPK_ASTATIC_SERIALIZE_H_230520
#define GPK_ASTATIC_SERIALIZE_H_230520

namespace gpk
{
	template<typename _tPOD, uint32_t _nSize> 
	::gpk::error_t			loadView					(::gpk::vcu8 & input, ::gpk::astatic<_tPOD, _nSize> & output) { 
		::gpk::view<const _tPOD>	readView					= {}; 
		uint32_t					bytesRead					= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(readView, input)); 
		input					= {input.begin() + bytesRead, input.size() - bytesRead}; 
		memcpy(output.begin(), readView.begin(), ::gpk::min(readView.byte_count(), ::gpk::view<_tPOD>{output}.byte_count()));
		return 0;
	}
} // namespace

#endif // GPK_ASTATIC_SERIALIZE_H_230520
