#include "gpk_string_compose.h"

#ifndef GPK_APPEND_HTML_H
#define GPK_APPEND_HTML_H

namespace gpk
{
	gpk::err_t			appendHtmlHead			(::gpk::achar & output, ::gpk::vcs tagAttributes = {}, ::gpk::vcs innerHtml = {});
	gpk::err_t			appendHtmlBody			(::gpk::achar & output, ::gpk::vcs tagAttributes = {}, ::gpk::vcs innerHtml = {});
	gpk::err_t			appendHtmlScript		(::gpk::achar & output, ::gpk::vcs tagAttributes = {}, ::gpk::vcs innerHtml = {});
	gpk::err_t			appendHtmlTable			(::gpk::achar & output, ::gpk::vcs tagAttributes = {}, ::gpk::vcs innerHtml = {});
	gpk::err_t			appendHtmlTableRow		(::gpk::achar & output, ::gpk::vcs tagAttributes = {}, ::gpk::vcs innerHtml = {});
	gpk::err_t			appendHtmlTableCol		(::gpk::achar & output, ::gpk::vcs tagAttributes = {}, ::gpk::vcs innerHtml = {});
	gpk::err_t			appendHtmlPage			(::gpk::achar & output, const ::gpk::FAppend & funcAppendCSS, const ::gpk::FAppend & funcAppendJS, const ::gpk::FAppend & funcAppendBody);
	gpk::err_t			appendHtmlPage			(::gpk::achar & output, const ::gpk::FAppend & funcAppendBody);
} // namespace 

#endif // GPK_APPEND_HTML_H