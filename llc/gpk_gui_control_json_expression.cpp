#include "gpk_gui_control_json_expression.h"
#include "gpk_append_json.h"
#include "gpk_ctti_stct_json.h"
#include "gpk_json_expression.h"

sttc ::gpk::err_t controlTableScriptResolveNode(cnst ::gpk::SJSONReader & source, ::gpk::u2_t iSource, cnst ::gpk::SJSONReader & values, ::gpk::u2_t iValues, ::gpk::string & output);

sttc ::gpk::err_t controlTableFrameworkEventPost(::gpk::SControlTable & controls, cnst ::gpk::SJSONReader & source) {
	cnst ::gpk::err_t iFramework = ::gpk::jsonObjectValueGet(source, 0, "framework");
	if(0 > iFramework)
		rtrn 0;
	cnst ::gpk::err_t iEvent = ::gpk::jsonObjectValueGet(source, (::gpk::u2_t)iFramework, "event");
	if(0 > iEvent)
		rtrn 0;
	cnst ::gpk::err_t iType = ::gpk::jsonObjectValueGet(source, (::gpk::u2_t)iEvent, "Type");
	if_fail_fe(iType);
	::gpk::u0_t eventType = {};
	if_fail_fe(::gpk::cttiValueLoad(source, (::gpk::u2_t)iType, eventType));
	::gpk::au0_t eventData = {};
	cnst ::gpk::err_t iData = ::gpk::jsonObjectValueGet(source, (::gpk::u2_t)iEvent, "Data");
	if(0 <= iData)
		if_fail_fe(::gpk::cttiValueLoad(source, (::gpk::u2_t)iData, eventData));
	rtrn ::gpk::eventEnqueueChild(controls.EventQueue, ::gpk::SYSTEM_EVENT_Framework, eventType, ::gpk::vcu0_t{eventData});
}

gpk::err_t gpk::controlTableScriptUpdate(SControlTable & controls) {
	for(u2_t iControl = 0; iControl < controls.States.size(); ++iControl) {
		if(false == controls.States[iControl].IsAction())
			continue;
		cnst vcst_t identifier = controls.Identifier[iControl];
		cnst vcst_t script = controls.Script[iControl];
		if(script.size())
			if_fail_fef(controlTableScriptRun(controls, (cid_t)iControl), "iControl:%u, Identifier:%.*s, Script:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(script));
	}
	rtrn 0;
}

gpk::err_t gpk::controlTableScriptRun(SControlTable & controls, vcst_t jsonExpression) {
	SJSONReader source = {};
	if_fail_fef(jsonParse(source, jsonExpression), "Script:%.*s", GPK_VIEW_ARGS(jsonExpression));
	string valuesJSON = {};
	if_fail_fe(controlTableJSONSave(controls, valuesJSON));
	SJSONReader values = {};
	if_fail_fef(jsonParse(values, valuesJSON), "ValuesSize:%u", valuesJSON.size());
	string patchJSON = {};
	if_fail_fef(::controlTableScriptResolveNode(source, 0, values, 0, patchJSON), "Script:%.*s", GPK_VIEW_ARGS(jsonExpression));
	SJSONReader patch = {};
	if_fail_fef(jsonParse(patch, patchJSON), "Patch:%.*s", GPK_VIEW_ARGS(patchJSON));
	if_fail_fef(controlTableJSONLoad(controls, patch), "Patch:%.*s", GPK_VIEW_ARGS(patchJSON));
	if_fail_fef(::controlTableFrameworkEventPost(controls, patch), "Patch:%.*s", GPK_VIEW_ARGS(patchJSON));
	rtrn 0;
}

gpk::err_t gpk::controlTableScriptRun(SControlTable & controls, cid_t iControl) {
	if_true_fef(controlInvalid(controls, iControl), "iControl:%i", iControl);
	cnst vcst_t script = controls.Script[iControl];
	if(0 == script.size())
		rtrn 0;
	rtrn controlTableScriptRun(controls, script);
}

sttc ::gpk::err_t controlTableScriptResolveNode(cnst ::gpk::SJSONReader & source, ::gpk::u2_t iSource, cnst ::gpk::SJSONReader & values, ::gpk::u2_t iValues, ::gpk::string & output) {
	if_true_fe(iSource >= source.Token.size());
	cnst ::gpk::JSON_TYPE type = source.Token[iSource].Type;
	if(::gpk::JSON_TYPE_OBJECT == type) {
		if_fail_fe(output.push_back('{'));
		cnst ::gpk::SJSONNode & node = *source.Tree[iSource];
		for(::gpk::u2_t iChild = 0; iChild < node.Children.size(); iChild += 2) {
			::gpk::vcst_t key = {};
			if_fail_fe(::gpk::jsonObjectGetString(source, (::gpk::u2_t)node.Children[iChild]->ObjectIndex, key));
			if_fail_fe(::gpk::appendOpenKey(output, key, 0 != iChild));
			if_fail_fe(::controlTableScriptResolveNode(source, (::gpk::u2_t)node.Children[iChild + 1]->ObjectIndex, values, iValues, output));
		}
		rtrn output.push_back('}');
	}
	if(::gpk::JSON_TYPE_ARRAY == type) {
		if_fail_fe(output.push_back('['));
		cnst ::gpk::SJSONNode & node = *source.Tree[iSource];
		for(::gpk::u2_t iChild = 0; iChild < node.Children.size(); ++iChild) {
			if(iChild) if_fail_fe(output.push_back(','));
			if_fail_fe(::controlTableScriptResolveNode(source, (::gpk::u2_t)node.Children[iChild]->ObjectIndex, values, iValues, output));
		}
		rtrn output.push_back(']');
	}
	if(::gpk::JSON_TYPE_STRING == type) {
		::gpk::vcst_t format = {};
		if_fail_fe(::gpk::jsonObjectGetString(source, iSource, format));
		if(format.size() >= 2 && '{' == format[0] && '}' == format[format.size() - 1]) {
			::gpk::vcst_t expression = {format.begin() + 1, format.size() - 2};
			::gpk::vcst_t resolved = {};
			cnst ::gpk::err_t iResolved = ::gpk::jsonExpressionResolve(expression, values, iValues, resolved);
			if_true_fe(-1 == iResolved);
			if((::gpk::u2_t)iResolved < values.Token.size() && ::gpk::JSON_TYPE_STRING == values.Token[iResolved].Type)
				rtrn ::gpk::appendJSONString(output, resolved);
			rtrn output.append(resolved);
		}
		::gpk::string formatted = {};
		if_fail_fe(::gpk::jsonStringFormat(format, values, iValues, formatted));
		rtrn ::gpk::appendJSONString(output, formatted);
	}
	rtrn ::gpk::jsonWrite(source.Tree[iSource], source.View, output);
}
