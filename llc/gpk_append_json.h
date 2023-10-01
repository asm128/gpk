#include "gpk_string_compose.h"

#ifndef GPK_APPEND_JSON_H
#define GPK_APPEND_JSON_H

namespace gpk
{
	gpk::err_t			appendKeyValue			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value);
	gpk::err_t			appendKeyString			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value);
	template<size_t nSize>
	stainli	gpk::err_t	appendKeyString			(::gpk::achar & output, ::gpk::vcs key, const char(&value)[nSize])	{ return appendKeyString(output, key, ::gpk::vcs{value}); }
	stainli	gpk::err_t	appendKeyString			(::gpk::achar & output, ::gpk::vcs key, const ::gpk::vcc & value)	{ return appendKeyString(output, key, ::gpk::vcs{value}); }
	gpk::err_t			appendKeyObject			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs itemsNotEnclosed);
	gpk::err_t			appendKeyList			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs itemsNotEnclosed);
	gpk::err_t			appendOpenKey			(::gpk::achar & output, ::gpk::vcs key);

} // namespace 

#endif // GPK_APPEND_JSON_H