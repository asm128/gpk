#include "blitter_output.h"
#include "llc_string_compose.h"

::llc::err_t			blt::outputModel			(::llc::string & output, const ::blt::STableDescription & tableDesc)				{
	if_fail_fef(output.push_back('{'), "%s", "Out of memory?");
	for(u2_t  iField = 0; iField < tableDesc.Fields.size(); ++iField) {
		::llc::appendQuoted(output, tableDesc.Fields[iField].Field);
		if_fail_fef(output.push_back(':'), "%s", "Out of memory?");
		const ::llc::JSON_TYPE							type							= tableDesc.Fields[iField].Type;
		switch(type) {
		case ::llc::JSON_TYPE_NULL			: output.append_string("null");		break;
		case ::llc::JSON_TYPE_INTEGER		: output.append_string("0");		break;
		case ::llc::JSON_TYPE_DECIMAL		: output.append_string("0.0");		break;
		case ::llc::JSON_TYPE_BOOLEAN			: output.append_string("false");	break;
		case ::llc::JSON_TYPE_ARRAY			: output.append_string("[]");		break;
		case ::llc::JSON_TYPE_OBJECT		: output.append_string("{}");		break;
		case ::llc::JSON_TYPE_STRING		:
			if(-1 == tableDesc.IndicesFieldToMap[iField])
				::llc::appendQuoted(output, {});
			else
				output.append_string("-1");
			break;
		default:
			info_printf("Unhandled type: %s.", ::llc::get_value_label(type).begin());
			output.append_string("null");
			break;
		}
		if(iField + 1 < tableDesc.Fields.size())
			output.push_back(',');
	}
	if_fail_fef(output.push_back('}'), "%s", "Out of memory?");
	return 0;
}
