#include "gpk_json_formatter.h"

#include "gpk_parse.h"

::gpk::error_t									jsonAccessSolverParse		(const ::gpk::view_const_string& format, const ::gpk::SJSONNode& input, ::gpk::array_pod<char_t>& )			{
	return 0;
}

::gpk::error_t									gpk::jsonStringFormat			(const ::gpk::view_const_string& format, const ::gpk::SJSONNode& input, ::gpk::array_pod<char_t>& )			{
	::gpk::SStripLiteralState							stateLiteralStripper;
	::gpk::array_pod<::gpk::SStripLiteralType>			literalTypes;
	::gpk::stripLiteralParse(stateLiteralStripper, literalTypes, format);
	::gpk::array_obj<::gpk::view_const_string>			viewsAll;
	::gpk::stripLiteralGetViews(viewsAll, literalTypes, format);
	{
		info_printf("-**- %s -**-", "Results of token search");
		for(uint32_t iAll = 0; iAll < viewsAll.size(); ++iAll) {
			static constexpr const char						bufferFormatIn	[]				= "%s (%%u): %%.%us";
			char											bufferFormatOut	[8192]			= {};
			bool											isLiteral							= (literalTypes[iAll].Type == ::gpk::STRIP_LITERAL_TYPE_LITERAL);
			if(isLiteral)
				continue;// we only have to solve tokens
			sprintf_s(bufferFormatOut, bufferFormatIn, isLiteral ? "Literal" : "Token", viewsAll[iAll].size());
			info_printf(bufferFormatOut, iAll, viewsAll[iAll].begin());
		}
	}
	input;
	return 0;
}