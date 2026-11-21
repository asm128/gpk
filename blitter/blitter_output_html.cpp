//#include "gpk_storage.h"
//#include "gpk_json.h"
//#include "blitter.h"
//#include "blitter_process.h"
//
//::gpk::err_t			outputComposeFormById		(::gpk::array_pod<char_t> & output, ::blt::SBlitter & instance, u2_t  iTable, u3_t  idDetail, const ::gpk::vcst_t uriAction)				{
//	::blt::TNamedBlitterDB								& table							= instance.Databases[iTable];
//	const ::blt::STableDescription						& tableDesc						= table.Val.Description;
//	::gpk::array_obj<::gpk::array_pod<char_t>>			valuesFromDB					= {};
//	::gpk::vcst_t								outputRecord					= {};
//	::blt::recordGet(instance.LoadCache, table, idDetail, outputRecord, instance.Folder);
//	output											= ::gpk::vcst_t{"Content-type: text/html\r\n\r\n"};
//	::gpk::array_obj<::gpk::SHTTPResponse>				responseFromAPIs				= {};
//	::gpk::array_obj<::gpk::SJSONReader>				jsonFromAPIs					= {};
//	::blt::retrieve_all_apis(responseFromAPIs, jsonFromAPIs, instance.APIs, tableDesc.FieldMaps);
//	::gpk::array_pod<byte_t>							jsScriptFile					= {};
//	::gpk::fileToMemory(::gpk::vcst_t{"bltapi.js"}, jsScriptFile);
//	if_fail_fef(output.append_string("<script type=\"text/javascript\">")	, "%s", "Out of memory?");
//	if_fail_fef(output.append(jsScriptFile)									, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("</script>")							, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("<form method=\"POST\" action=\"")		, "%s", "Out of memory?");
//	if_fail_fef(output.append(uriAction)										, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("\" onsubmit=\"formSubmit('")			, "%s", "Out of memory?");
//	if_fail_fef(output.append(tableDesc.TableName)							, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("', 'en-US'); return false;\" ")		, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("\" id=\"")								, "%s", "Out of memory?");
//	if_fail_fef(output.append(tableDesc.TableName)							, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("\">")									, "%s", "Out of memory?");
//	// -- Generate input fields
//	if_fail_fef(output.append_string("<table>"), "%s", "Out of memory?");
//	for(u2_t  iField = 0; iField < tableDesc.Fields.size(); ++iField) {
//		const ::gpk::vcst_t						fieldName					= tableDesc.Fields[iField].Field;
//		output.append_string("\n<tr><td>");
//		output.append(fieldName);
//		output.append_string("\n</td>");
//		output.append_string("\n<td>");
//		const ::gpk::JSON_TYPE								type							= tableDesc.Fields[iField].Type;
//		const u2_t 										indexOfJSONTable				= tableDesc.IndicesFieldToMap[iField];
//		switch(type) {
//		case ::gpk::JSON_TYPE_NULL			: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"\"			value=\""); output.append(valuesFromDB[iField]); output.append_string("\"/>"); break;
//		case ::gpk::JSON_TYPE_INTEGER		: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"number\"	value=\""); output.append(valuesFromDB[iField]); output.append_string("\"/>"); break;
//		case ::gpk::JSON_TYPE_DECIMAL		: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"number\"	value=\""); output.append(valuesFromDB[iField]); output.append_string("\"/>"); break;
//		case ::gpk::JSON_TYPE_BOOLEAN			: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"checkbox\"	value=\""); output.append(valuesFromDB[iField]); output.append_string("\"/>"); break;
//		case ::gpk::JSON_TYPE_ARRAY			: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"\"			value=\""); output.append(valuesFromDB[iField]); output.append_string("\"/>"); break;
//		case ::gpk::JSON_TYPE_OBJECT		:
//			output.append_string("<select id=\"");
//			output.append(fieldName);
//			output.append_string("\" name=\"");
//			output.append(fieldName);
//			output.append_string("\" >");
//			{
//				if(-1 == indexOfJSONTable) {
//					output.append_string("</select>");
//					continue;
//				}
//				const ::gpk::SJSONReader						& fieldReader					= jsonFromAPIs[indexOfJSONTable];
//				if(-1 == indexOfJSONTable || 0 == fieldReader.Tree.size()) {
//					output.append_string("</select>");
//					continue;
//				}
//				for(u2_t  iRecord = 0, countRecords = ::gpk::jsonArraySize(fieldReader, 0); iRecord < countRecords; ++iRecord) {
//					const s2_t								indexOfItemNode					= ::gpk::jsonArrayValueGet(fieldReader, 0, iRecord);
//					if(0 > indexOfItemNode)
//						continue;
//					const s2_t								indexOfNameValueNode			= ::gpk::jsonObjectValueGet(fieldReader, indexOfItemNode, ::gpk::vcst_t{"name"	});
//					const s2_t								indexOfIdNode					= ::gpk::jsonObjectValueGet(fieldReader, indexOfItemNode, ::gpk::vcst_t{"id"		});
//					if(0 > indexOfNameValueNode || 0 > indexOfIdNode)
//						continue;
//					output.append_string("<option value=\"");
//					const ::gpk::vcst_t				idCurrentItem					= fieldReader.View[indexOfIdNode];
//					output.append(idCurrentItem);
//					output.append_string("\" ");
//					if(idCurrentItem == valuesFromDB[iField])
//						output.append_string("selected");
//					output.append_string(" >");
//					output.append(fieldReader.View[indexOfNameValueNode]);
//					output.append_string("</option>");
//				}
//			}
//			output.append_string("</select>");
//			break;
//		case ::gpk::JSON_TYPE_STRING		:
//			if((-1 == tableDesc.IndicesFieldToMap[iField])) {
//				output.append_string("<input  id=\"");
//				output.append(tableDesc.Fields[iField].Field);
//				output.append_string("\" name=\"");
//				output.append(fieldName);
//				output.append_string("\" type=\"text\" value=\"");
//				output.append(valuesFromDB[iField]);
//				output.append_string("\" />");
//			}
//			else {
//				output.append_string("<select id=\"");
//				output.append(fieldName);
//				output.append_string("\" name=\"");
//				output.append(fieldName);
//				output.append_string("\" >");
//				{
//					if(-1 == indexOfJSONTable) {
//						output.append_string("</select>");
//						continue;
//					}
//					const ::gpk::SJSONReader						& fieldReader					= jsonFromAPIs[indexOfJSONTable];
//					if(-1 == indexOfJSONTable || 0 == fieldReader.Tree.size()) {
//						output.append_string("</select>");
//						continue;
//					}
//					for(u2_t  iRecord = 0, countRecords = ::gpk::jsonArraySize(fieldReader, 0); iRecord < countRecords; ++iRecord) {
//						const s2_t								indexOfItemNode					= ::gpk::jsonArrayValueGet(fieldReader, 0, iRecord);
//						if(0 > indexOfItemNode)
//							continue;
//						const s2_t								indexOfNameValueNode			= ::gpk::jsonObjectValueGet(fieldReader, indexOfItemNode, ::gpk::vcst_t{"name"	});
//						const s2_t								indexOfIdNode					= ::gpk::jsonObjectValueGet(fieldReader, indexOfItemNode, ::gpk::vcst_t{"id"		});
//						if(0 > indexOfNameValueNode || 0 > indexOfIdNode)
//							continue;
//						output.append_string("<option value=\"");
//						const ::gpk::vcst_t				idCurrentItem					= fieldReader.View[indexOfIdNode];
//						output.append(idCurrentItem);
//						output.append_string("\" ");
//						if(idCurrentItem == valuesFromDB[iField])
//							output.append_string("selected");
//						output.append_string(" >");
//						output.append(fieldReader.View[indexOfNameValueNode]);
//						output.append_string("</option>");
//					}
//				}
//				output.append_string("</select>");
//			}
//			break;
//		default:
//			info_printf("Unhandled type: %s.", ::gpk::get_value_label(type).begin());
//			output.append_string("null");
//			break;
//		}
//		output.append_string("\n</td></tr>");
//	}
//	if_fail_fef(output.append_string("</table><input type=\"submit\" value=\"Save\"/></form>"), "%s", "Out of memory?");
//	return 0;
//}
//
//::gpk::err_t			outputComposeForm			(::gpk::array_pod<char_t> & output, ::blt::SBlitter & instance, u2_t  iTable, const ::gpk::vcst_t uriAction)				{
//	const ::blt::STableDescription						& tableDesc						= instance.Databases[iTable].Val.Description;
//	output											= ::gpk::vcst_t{"Content-type: text/html\r\n\r\n"};
//	::gpk::array_pod<byte_t>							jsScriptFile					= {};
//	::gpk::fileToMemory(::gpk::vcst_t{"bltapi.js"}, jsScriptFile);
//	if_fail_fef(output.append_string("<script type=\"text/javascript\">")	, "%s", "Out of memory?");
//	if_fail_fef(output.append(jsScriptFile)									, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("</script>")							, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("<form method=\"POST\" action=\"")		, "%s", "Out of memory?");
//	if_fail_fef(output.append(uriAction)										, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("\" onsubmit=\"formSubmit('")			, "%s", "Out of memory?");
//	if_fail_fef(output.append(tableDesc.TableName)							, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("', 'en-US'); return false;\" ")		, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("\" id=\"")								, "%s", "Out of memory?");
//	if_fail_fef(output.append(tableDesc.TableName)							, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("\">")									, "%s", "Out of memory?");
//	if_fail_fef(output.append_string("<table>")								, "%s", "Out of memory?");
//	::gpk::array_obj<::gpk::SHTTPResponse>				responseFromAPIs				= {};
//	::gpk::array_obj<::gpk::SJSONReader>				jsonFromAPIs					= {};
//	::blt::retrieve_all_apis(responseFromAPIs, jsonFromAPIs, instance.APIs, tableDesc.FieldMaps);
//	for(u2_t  iField = 0; iField < tableDesc.Fields.size(); ++iField) {
//		const ::gpk::vcst_t						fieldName					= tableDesc.Fields[iField].Field;
//		output.append_string("\n<tr><td>");
//		output.append(fieldName);
//		output.append_string("\n</td>");
//		output.append_string("\n<td>");
//		if(fieldName == ::gpk::vcst_t{"id"})
//			output.append_string("<p id=\"id\">?</p>");
//		else {
//			const ::gpk::JSON_TYPE								type							= tableDesc.Fields[iField].Type;
//			const u2_t 										indexOfJSONTable				= tableDesc.IndicesFieldToMap[iField];
//			switch(type) {
//			case ::gpk::JSON_TYPE_NULL			: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"\"			value=\"\"		/>"); break;
//			case ::gpk::JSON_TYPE_INTEGER		: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"number\"	value=\"0\"		/>"); break;
//			case ::gpk::JSON_TYPE_DECIMAL		: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"number\"	value=\"0.0\"	/>"); break;
//			case ::gpk::JSON_TYPE_BOOLEAN			: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"checkbox\"	value=\"\"		/>"); break;
//			case ::gpk::JSON_TYPE_ARRAY			: output.append_string("<input id=\""); output.append(fieldName); output.append_string("\" name=\""); output.append(fieldName); output.append_string("\" type=\"\"			value=\"\"		/>"); break;
//			case ::gpk::JSON_TYPE_OBJECT		:
//				output.append_string("<select id=\"");
//				output.append(fieldName);
//				output.append_string("\" name=\"");
//				output.append(fieldName);
//				output.append_string("\" >");
//				{
//					if(-1 == indexOfJSONTable) {
//						output.append_string("</select>");
//						continue;
//					}
//					const ::gpk::SJSONReader						& fieldReader					= jsonFromAPIs[indexOfJSONTable];
//					if(-1 == indexOfJSONTable || 0 == fieldReader.Tree.size()) {
//						output.append_string("</select>");
//						continue;
//					}
//					for(u2_t  iRecord = 0, countRecords = ::gpk::jsonArraySize(fieldReader, 0); iRecord < countRecords; ++iRecord) {
//						s2_t										indexOfItemNode					= ::gpk::jsonArrayValueGet(fieldReader, 0, iRecord);
//						if(0 > indexOfItemNode)
//							continue;
//						::gpk::vcst_t						viewOfItem						= fieldReader.View[indexOfItemNode];
//						s2_t										indexOfNameValueNode			= ::gpk::jsonObjectValueGet(fieldReader, indexOfItemNode, ::gpk::vcst_t{"name"	});
//						s2_t										indexOfIdNode					= ::gpk::jsonObjectValueGet(fieldReader, indexOfItemNode, ::gpk::vcst_t{"id"		});
//						if(0 > indexOfNameValueNode || 0 > indexOfIdNode)
//							continue;
//						output.append_string("<option value=\"");
//						output.append(jsonFromAPIs[indexOfJSONTable].View[indexOfIdNode]);
//						output.append_string("\" >");
//						output.append(jsonFromAPIs[indexOfJSONTable].View[indexOfNameValueNode]);
//						output.append_string("</option>");
//					}
//				}
//				output.append_string("</select>");
//				break;
//			case ::gpk::JSON_TYPE_STRING		:
//				if((-1 == tableDesc.IndicesFieldToMap[iField])) {
//					output.append_string("<input  id=\"");
//					output.append(fieldName);
//					output.append_string("\" name=\"");
//					output.append(fieldName);
//					output.append_string("\" type=\"text\">");
//				}
//				else {
//					output.append_string("<select id=\"");
//					output.append(fieldName);
//					output.append_string("\" name=\"");
//					output.append(fieldName);
//					output.append_string("\" >");
//					{
//						if(-1 == indexOfJSONTable) {
//							output.append_string("</select>");
//							continue;
//						}
//						const ::gpk::SJSONReader						& fieldReader					= jsonFromAPIs[indexOfJSONTable];
//						if(-1 == indexOfJSONTable || 0 == fieldReader.Tree.size()) {
//							output.append_string("</select>");
//							continue;
//						}
//						for(u2_t  iRecord = 0, countRecords = ::gpk::jsonArraySize(fieldReader, 0); iRecord < countRecords; ++iRecord) {
//							s2_t										indexOfItemNode					= ::gpk::jsonArrayValueGet(fieldReader, 0, iRecord);
//							if(0 > indexOfItemNode)
//								continue;
//							::gpk::vcst_t						viewOfItem						= fieldReader.View[indexOfItemNode];
//							s2_t										indexOfNameValueNode			= ::gpk::jsonObjectValueGet(fieldReader, indexOfItemNode, ::gpk::vcst_t{"name"	});
//							s2_t										indexOfIdNode					= ::gpk::jsonObjectValueGet(fieldReader, indexOfItemNode, ::gpk::vcst_t{"id"		});
//							if(0 > indexOfNameValueNode || 0 > indexOfIdNode)
//								continue;
//							output.append_string("<option value=\"");
//							output.append(jsonFromAPIs[indexOfJSONTable].View[indexOfIdNode]);
//							output.append_string("\" >");
//							output.append(jsonFromAPIs[indexOfJSONTable].View[indexOfNameValueNode]);
//							output.append_string("</option>");
//						}
//					}
//					output.append_string("</select>");
//				}
//				break;
//			default:
//				info_printf("Unhandled type: %s.", ::gpk::get_value_label(type).begin());
//				output.append_string("null");
//				break;
//			}
//		}
//		output.append_string("\n</td></tr>");
//	}
//	if_fail_fef(output.append_string("</table><input type=\"submit\" value=\"Add\"/></form>"), "%s", "Out of memory?");
//	return 0;
//}
