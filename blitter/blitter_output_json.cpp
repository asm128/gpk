#include "blitter_output.h"
#include "gpk_string_compose.h"

GPK_USING_TYPEINT();

::gpk::err_t			blt::outputModel			(::gpk::string & output, const ::blt::STableDescription & tableDesc)				{
	if_fail_fef(output.push_back('{'), "%s", "Out of memory?");
	for(u2_t  iField = 0; iField < tableDesc.Fields.size(); ++iField) {
		::gpk::appendQuoted(output, tableDesc.Fields[iField].Field);
		if_fail_fef(output.push_back(':'), "%s", "Out of memory?");
		const ::gpk::JSON_TYPE							type							= tableDesc.Fields[iField].Type;
		switch(type) {
		case ::gpk::JSON_TYPE_NULL			: output.append(::gpk::vcst_t{"null"});		break;
		case ::gpk::JSON_TYPE_INTEGER		: output.append(::gpk::vcst_t{"0"});		break;
		case ::gpk::JSON_TYPE_DECIMAL		: output.append(::gpk::vcst_t{"0.0"});		break;
		case ::gpk::JSON_TYPE_BOOLEAN			: output.append(::gpk::vcst_t{"false"});	break;
		case ::gpk::JSON_TYPE_ARRAY			: output.append(::gpk::vcst_t{"[]"});		break;
		case ::gpk::JSON_TYPE_OBJECT		: output.append(::gpk::vcst_t{"{}"});		break;
		case ::gpk::JSON_TYPE_STRING		:
			if(-1 == tableDesc.IndicesFieldToMap[iField])
				::gpk::appendQuoted(output, {});
			else
				output.append(::gpk::vcst_t{"-1"});
			break;
		default:
			info_printf("Unhandled type: %s.", ::gpk::get_value_label(type).begin());
			output.append(::gpk::vcst_t{"null"});
			break;
		}
		if(iField + 1 < tableDesc.Fields.size())
			output.push_back(',');
	}
	if_fail_fef(output.push_back('}'), "%s", "Out of memory?");
	return 0;
}
