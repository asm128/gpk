#include "gpk_append_json.h"

gpk::err_t	gpk::appendOpenKey		(::gpk::achar & output, ::gpk::vcs key)							{ return ::gpk::append_strings(output, '"', key, "\":"); }
gpk::err_t	gpk::appendKeyValue		(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value)		{ return ::gpk::append_strings(output, '"', key, "\":", value); }
gpk::err_t	gpk::appendKeyString	(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value)		{ return ::gpk::append_strings(output, '"', key, "\":", '{',  value, '}'); }
gpk::err_t	gpk::appendKeyList		(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs listItems)	{ return ::gpk::append_strings(output, '"', key, "\":", '[',  listItems, ']'); }
gpk::err_t	gpk::appendKeyObject	(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs listItems)	{ return ::gpk::append_strings(output, '"', key, "\":", '{',  listItems, '}'); }
