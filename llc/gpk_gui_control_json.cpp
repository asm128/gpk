#include "gpk_gui_control_json.h"
#include "gpk_ctti_stct_json.h"

#include "gpk_array.h"
#include "gpk_append_json.h"

GPK_USING_TYPEINT();
GPK_USING_ARRAY();

usns gpk;

sttc ::gpk::err_t controlJSONSave(::gpk::string & output, cnst ::gpk::SControlTable & controls, ::gpk::cid_t iControl);

sttc ::gpk::err_t controlEventJSONSave(::gpk::string & output, cnst ::gpk::SEventSystem & event) {
	if_fail_fe(output.push_back('{'));
	if_fail_fe(::gpk::appendOpenKey(output, "Type"));
	if_fail_fe(::gpk::cttiValueSave(output, (::gpk::u0_t)event.Type));
	if_fail_fe(::gpk::appendOpenKey(output, "Data", true));
	if_fail_fe(::gpk::cttiValueSave(output, event.Data));
	rtrn output.push_back('}');
}

sttc ::gpk::err_t controlEventQueueJSONSave(::gpk::string & output, cnst ::gpk::apobj<::gpk::SEventSystem> & events, ::gpk::b8_t & separator) {
	if_fail_fe(::gpk::appendOpenKey(output, "EventQueue", separator));
	if_fail_fe(output.push_back('['));
	for(::gpk::u2_t iEvent = 0; iEvent < events.size(); ++iEvent) {
		if(iEvent)
			if_fail_fe(output.push_back(','));
		cnst ::gpk::SEventSystem empty = {};
		if_fail_fe(::controlEventJSONSave(output, events[iEvent] ? *events[iEvent] : empty));
	}
	if_fail_fe(output.push_back(']'));
	separator = true;
	rtrn 0;
}

sttc ::gpk::err_t controlChildrenJSONSave(::gpk::string & output, cnst ::gpk::SControlTable & controls, ::gpk::cid_t iControl, b8_t & separator) {
	cnst ::gpk::acid & children = controls.Children[iControl];
	for(::gpk::u2_t iChild = 0; iChild < children.size(); ++iChild) {
		cnst ::gpk::cid_t idChild = children[iChild];
		if(controls.States.size() <= (::gpk::u2_t)idChild)
			continue;
		cnst ::gpk::vcst_t identifier = controls.Identifier[idChild];
		if(identifier.size()) {
			if_fail_fe(::gpk::appendOpenKey(output, identifier, separator));
			if_fail_fe(::controlJSONSave(output, controls, idChild));
			separator = true;
		}
		else
			if_fail_fe(::controlChildrenJSONSave(output, controls, idChild, separator));
	}
	rtrn 0;
}

tplt<tpnm T>
sttc gpk::err_t guiComponentJSONSave(::gpk::string & output, ::gpk::vcst_t name, cnst T & component, b8_t & separator) {
	if_fail_fe(::gpk::appendOpenKey(output, name, separator));
	if_fail_fe(::gpk::cttiValueSave(output, component));
	separator = true;
	rtrn 0;
}

tplt<tpnm T>
sttc gpk::err_t guiComponentJSONSaveDelta(::gpk::string & output, ::gpk::vcst_t name, cnst T & component, cnst T & defaults, b8_t & separator) {
	if constexpr(::gpk::ctti_struct<T>) {
		::gpk::string members = {};
		::gpk::b8_t memberSeparator = false;
		cnst auto & metadata = ::gpk::cttiStruct<T>();
		for(::gpk::u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
			cnst ::gpk::vcst_t memberName = metadata.View(metadata.Members[iMember].Name);
			::gpk::string valueJSON = {};
			::gpk::string defaultJSON = {};
			cnst ::gpk::SCTTIFieldJSONOverride * fieldOverride = ::gpk::cttiFieldJSONOverride<T>(memberName);
			if(fieldOverride && fieldOverride->Save) {
				if_fail_fe(fieldOverride->Save(valueJSON, &component));
				if_fail_fe(fieldOverride->Save(defaultJSON, &defaults));
			}
			else {
				if_fail_fe(::gpk::cttiVisit(component, iMember, [&](cnst auto & member) { rtrn ::gpk::cttiValueSave(valueJSON, member); }));
				if_fail_fe(::gpk::cttiVisit(defaults, iMember, [&](cnst auto & member) { rtrn ::gpk::cttiValueSave(defaultJSON, member); }));
			}
			if(valueJSON == defaultJSON)
				continue;
			if_fail_fe(::gpk::appendOpenKey(members, memberName, memberSeparator));
			if_fail_fe(members.append(valueJSON));
			memberSeparator = true;
		}
		if(false == memberSeparator)
			rtrn 0;
		if_fail_fe(::gpk::appendOpenKey(output, name, separator));
		if_fail_fe(output.push_back('{'));
		if_fail_fe(output.append(members));
		if_fail_fe(output.push_back('}'));
	}
	else {
		::gpk::string valueJSON = {};
		::gpk::string defaultJSON = {};
		if_fail_fe(::gpk::cttiValueSave(valueJSON, component));
		if_fail_fe(::gpk::cttiValueSave(defaultJSON, defaults));
		if(valueJSON == defaultJSON)
			rtrn 0;
		if_fail_fe(::gpk::appendOpenKey(output, name, separator));
		if_fail_fe(output.append(valueJSON));
	}
	separator = true;
	rtrn 1;
}

sttc ::gpk::err_t controlJSONSave(::gpk::string & output, cnst ::gpk::SControlTable & controls, ::gpk::cid_t iControl) {
	if_fail_fe(output.push_back('{'));
	b8_t separator = false;
	if_fail_fe(::gpk::appendOpenKey(output, "Id"));
	if_fail_fe(::gpk::cttiValueSave(output, iControl));
	separator = true;
	if_fail_fe(::guiComponentJSONSave(output, "State", controls.States[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "TextState", controls.TextState[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Constraints", controls.Constraints[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Placement", controls.Placement[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Area", controls.Area[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Draw", controls.Draw[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Text", controls.Text[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Script", controls.Script[iControl], separator));
	if_fail_fe(::controlChildrenJSONSave(output, controls, iControl, separator));
	rtrn output.push_back('}');
}

sttc ::gpk::err_t controlTreeJSONSave(::gpk::string & output, cnst ::gpk::SControlTable & controls, ::gpk::cid_t iControl, ::gpk::b8_t & separator) {
	if_true_fef(controls.States.size() <= (::gpk::u2_t)iControl, "iControl:%i", iControl);
	cnst ::gpk::vcst_t identifier = controls.Identifier[iControl];
	if_true_fef(0 == identifier.size(), "iControl:%i", iControl);
	::gpk::u2_c offset = output.size();
	if_fail_fe(::gpk::appendOpenKey(output, identifier, separator));
	if_fail_fe(::controlJSONSave(output, controls, iControl));
	separator = true;
	rtrn output.size() - offset;
}

sttc b8_t controlHasIdentifiedAncestor(cnst ::gpk::SControlTable & controls, ::gpk::cid_t iControl) {
	for(::gpk::cid_t iParent = controls.States[iControl].Parent; (::gpk::u2_t)iParent < controls.States.size(); iParent = controls.States[iParent].Parent)
		if(controls.Identifier[iParent].size())
			rtrn true;
	rtrn false;
}

gpk::err_t	gpk::controlTableJSONSave	(cnst SControlTable & controls, string & outputJsonString) {
	u2_c offset = outputJsonString.size();
	if_fail_fe(outputJsonString.push_back('{'));
	b8_t separator = false;
	if_fail_fe(::controlEventQueueJSONSave(outputJsonString, controls.EventQueue, separator));
	for(u2_t iControl = 0; iControl < controls.States.size(); ++iControl) {
		if(0 == controls.Identifier[iControl].size() || ::controlHasIdentifiedAncestor(controls, (cid_t)iControl))
			continue;
		if_fail_fe(::controlTreeJSONSave(outputJsonString, controls, (cid_t)iControl, separator));
	}
	if_fail_fe(outputJsonString.push_back('}'));
	rtrn outputJsonString.size() - offset;
}

tplt<tpnm T>
sttc ::gpk::err_t guiComponentJSONLoad(cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode, ::gpk::vcst_t name, T & component) {
	cnst ::gpk::err_t iComponent = ::gpk::jsonObjectValueGet(input, iNode, name);
	if(0 > iComponent)
		rtrn 0;
	if_fail_fe(::gpk::cttiValueLoad(input, (::gpk::u2_t)iComponent, component));
	rtrn 1;
}

sttc ::gpk::err_t controlEventQueueJSONLoad(cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode, ::gpk::apobj<::gpk::SEventSystem> & events) {
	cnst ::gpk::err_t iEvents = ::gpk::jsonObjectValueGet(input, iNode, "EventQueue");
	if(0 > iEvents)
		rtrn 0;
	if_true_fe(input.Token[iEvents].Type != ::gpk::JSON_TYPE_ARRAY);
	cnst ::gpk::SJSONNode & node = *input.Tree[iEvents];
	events.clear();
	if_fail_fe(events.resize(node.Children.size()));
	for(::gpk::u2_t iEvent = 0; iEvent < events.size(); ++iEvent) {
		cnst ::gpk::u2_t iEventNode = (::gpk::u2_t)node.Children[iEvent]->ObjectIndex;
		if_true_fe(input.Token[iEventNode].Type != ::gpk::JSON_TYPE_OBJECT);
		::gpk::u0_t eventType = {};
		if_fail_fe(::guiComponentJSONLoad(input, iEventNode, "Type", eventType));
		events[iEvent]->Type = (::gpk::SYSTEM_EVENT)eventType;
		if_fail_fe(::guiComponentJSONLoad(input, iEventNode, "Data", events[iEvent]->Data));
	}
	rtrn 1;
}

sttc ::gpk::err_t controlJSONLoad(::gpk::SControlTable & controls, ::gpk::cid_t iControl, cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode);

sttc ::gpk::err_t controlChildrenJSONLoad(::gpk::SControlTable & controls, ::gpk::cid_t iControl, cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode) {
	cnst ::gpk::acid & children = controls.Children[iControl];
	for(::gpk::u2_t iChild = 0; iChild < children.size(); ++iChild) {
		cnst ::gpk::cid_t idChild = children[iChild];
		if(controls.States.size() <= (::gpk::u2_t)idChild)
			continue;
		cnst ::gpk::vcst_t identifier = controls.Identifier[idChild];
		if(identifier.size()) {
			cnst ::gpk::err_t iChildNode = ::gpk::jsonObjectValueGet(input, iNode, identifier);
			if(0 <= iChildNode)
				if_fail_fe(::controlJSONLoad(controls, idChild, input, (::gpk::u2_t)iChildNode));
		}
		else
			if_fail_fe(::controlChildrenJSONLoad(controls, idChild, input, iNode));
	}
	rtrn 0;
}

sttc ::gpk::err_t controlJSONLoad(::gpk::SControlTable & controls, ::gpk::cid_t iControl, cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode) {
	::gpk::err_t loaded = 0;
	::gpk::SControlState state = controls.States[iControl];
	::gpk::err_t loadedState = ::guiComponentJSONLoad(input, iNode, "State", state);
	if_fail_fe(loadedState);
	if(loadedState) {
		cnst ::gpk::cid_t oldParent = controls.States[iControl].Parent;
		if(state.Parent != oldParent)
			if_fail_fe(::gpk::controlSetParent(controls, iControl, state.Parent));
		controls.States[iControl] = state;
		++loaded;
	}
	::gpk::err_t result = ::guiComponentJSONLoad(input, iNode, "TextState", controls.TextState[iControl]);
	if_fail_fe(result); loaded += result;
	result = ::guiComponentJSONLoad(input, iNode, "Constraints", controls.Constraints[iControl]);
	if_fail_fe(result); loaded += result;
	result = ::guiComponentJSONLoad(input, iNode, "Placement", controls.Placement[iControl]);
	if_fail_fe(result); loaded += result;
	result = ::guiComponentJSONLoad(input, iNode, "Area", controls.Area[iControl]);
	if_fail_fe(result); loaded += result;
	result = ::guiComponentJSONLoad(input, iNode, "Draw", controls.Draw[iControl]);
	if_fail_fe(result); loaded += result;
	result = ::guiComponentJSONLoad(input, iNode, "Script", controls.Script[iControl]);
	if_fail_fe(result); loaded += result;
	::gpk::SControlText text = controls.Text[iControl];
	result = ::guiComponentJSONLoad(input, iNode, "Text", text);
	if_fail_fe(result);
	if(result) {
		controls.Text[iControl] = text;
		loaded += result;
	}
	if(loaded)
		if_fail_fe(::gpk::controlMetricsInvalidate(controls, iControl));
	if(loadedState)
		controls.States[iControl] = state;
	if_fail_fe(::controlChildrenJSONLoad(controls, iControl, input, iNode));
	rtrn loaded;
}

sttc ::gpk::err_t controlCTTKey(::gpk::string & key, cnst ::gpk::SControlTable & controls, ::gpk::cid_t iControl) {
	cnst ::gpk::vcst_t identifier = controls.Identifier[iControl];
	rtrn identifier.size() ? ::gpk::append_string(key, identifier) : ::gpk::append_strings(key, "control_", ::gpk::str(iControl));
}

sttc ::gpk::err_t controlCTTJSONSave(::gpk::string & output, cnst ::gpk::SControlTable & controls, ::gpk::cid_t iControl, cnst ::gpk::SControlTable & defaults, ::gpk::cid_t iDefault) {
	if_fail_fe(output.push_back('{'));
	::gpk::b8_t separator = false;
	::gpk::SControlState state = controls.States[iControl];
	state.Parent = ::gpk::CID_INVALID;
	if_fail_fe(::guiComponentJSONSaveDelta(output, "State", state, defaults.States[iDefault], separator));
	if_fail_fe(::guiComponentJSONSaveDelta(output, "TextState", controls.TextState[iControl], defaults.TextState[iDefault], separator));
	if_fail_fe(::guiComponentJSONSaveDelta(output, "Constraints", controls.Constraints[iControl], defaults.Constraints[iDefault], separator));
	if_fail_fe(::guiComponentJSONSaveDelta(output, "Placement", controls.Placement[iControl], defaults.Placement[iDefault], separator));
	if_fail_fe(::guiComponentJSONSaveDelta(output, "Area", controls.Area[iControl], defaults.Area[iDefault], separator));
	if_fail_fe(::guiComponentJSONSaveDelta(output, "Draw", controls.Draw[iControl], defaults.Draw[iDefault], separator));
	if_fail_fe(::guiComponentJSONSaveDelta(output, "Text", controls.Text[iControl], defaults.Text[iDefault], separator));
	if_fail_fe(::guiComponentJSONSaveDelta(output, "Script", controls.Script[iControl], defaults.Script[iDefault], separator));
	cnst ::gpk::acid & children = controls.Children[iControl];
	for(::gpk::u2_t iChild = 0; iChild < children.size(); ++iChild) {
		cnst ::gpk::cid_t idChild = children[iChild];
		if(::gpk::controlInvalid(controls, idChild))
			continue;
		::gpk::string key = {};
		if_fail_fe(::controlCTTKey(key, controls, idChild));
		if_fail_fe(::gpk::appendOpenKey(output, key, separator));
		if_fail_fe(::controlCTTJSONSave(output, controls, idChild, defaults, iDefault));
		separator = true;
	}
	rtrn output.push_back('}');
}

gpk::err_t gpk::controlTableJSONSave(string & output, cnst SControlTable & controls, vs1_t defaultColors) {
	SControlTable defaults = {};
	cnst cid_t iDefault = controlCreate(defaults, defaultColors);
	if_fail_fe(iDefault);
	if_fail_fe(output.push_back('{'));
	b8_t separator = false;
	if_fail_fe(::controlEventQueueJSONSave(output, controls.EventQueue, separator));
	for(u2_t iControl = 0; iControl < controls.States.size(); ++iControl) {
		if(controlInvalid(controls, (cid_t)iControl) || false == controlInvalid(controls, controls.States[iControl].Parent))
			continue;
		string key = {};
		if_fail_fe(::controlCTTKey(key, controls, (cid_t)iControl));
		if_fail_fe(appendOpenKey(output, key, separator));
		if_fail_fe(::controlCTTJSONSave(output, controls, (cid_t)iControl, defaults, iDefault));
		separator = true;
	}
	rtrn output.push_back('}');
}

sttc ::gpk::b8_t controlCTTComponent(::gpk::vcst_t name) {
	rtrn name == "State" || name == "TextState" || name == "Constraints" || name == "Placement" || name == "Area" || name == "Draw" || name == "Text" || name == "Script";
}

sttc ::gpk::err_t guiJSONTemplateDynamic(cnst ::gpk::SJSONReader & source, ::gpk::u2_t iSource, ::gpk::vcst_t rootIdentifier, ::gpk::b8_t & dynamic, ::gpk::b8_t & scoped) {
	if_true_fef(iSource >= source.Token.size(), "iSource:%u, TokenCount:%u", iSource, source.Token.size());
	cnst ::gpk::JSON_TYPE type = source.Token[iSource].Type;
	if(::gpk::JSON_TYPE_STRING == type) {
		::gpk::vcst_t value = {};
		if_fail_fef(::gpk::jsonObjectGetString(source, iSource, value), "iSource:%u", iSource);
		for(::gpk::u2_t iCharacter = 0; iCharacter < value.size(); ++iCharacter)
			if('{' == value[iCharacter]) {
				dynamic = true;
				if(0 == rootIdentifier.size() || value.size() < iCharacter + 1 + rootIdentifier.size())
					scoped = false;
				else {
					for(::gpk::u2_t iRootCharacter = 0; iRootCharacter < rootIdentifier.size(); ++iRootCharacter)
						if(value[iCharacter + 1 + iRootCharacter] != rootIdentifier[iRootCharacter]) {
							scoped = false;
							break;
						}
					cnst ::gpk::u2_t iAfterRoot = iCharacter + 1 + rootIdentifier.size();
					if(scoped && iAfterRoot < value.size() && '.' != value[iAfterRoot] && '[' != value[iAfterRoot] && '}' != value[iAfterRoot])
						scoped = false;
				}
			}
		rtrn 0;
	}
	if(::gpk::JSON_TYPE_OBJECT != type && ::gpk::JSON_TYPE_ARRAY != type)
		rtrn 0;
	cnst ::gpk::SJSONNode & node = *source.Tree[iSource];
	cnst ::gpk::u2_t first = ::gpk::JSON_TYPE_OBJECT == type ? 1U : 0U;
	cnst ::gpk::u2_t step = ::gpk::JSON_TYPE_OBJECT == type ? 2U : 1U;
	for(::gpk::u2_t iChild = first; iChild < node.Children.size(); iChild += step) {
		cnst ::gpk::u2_t iChildNode = (::gpk::u2_t)node.Children[iChild]->ObjectIndex;
		if_fail_fef(::guiJSONTemplateDynamic(source, iChildNode, rootIdentifier, dynamic, scoped), "iSource:%u, iChild:%u, iChildNode:%u", iSource, iChild, iChildNode);
	}
	rtrn 0;
}

sttc ::gpk::err_t controlCTTJSONLoad(::gpk::SControlTable & controls, ::gpk::vs1_t defaultColors, cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode, ::gpk::vcst_t identifier, ::gpk::cid_t iParent) {
	cnst ::gpk::cid_t iControl = ::gpk::controlCreate(controls, defaultColors);
	if_fail_fe(iControl);
	controls.Identifier[iControl] = identifier;
	if_fail_fe(::gpk::controlSetParent(controls, iControl, iParent));
	::gpk::SControlState state = controls.States[iControl];
	if_fail_fe(::guiComponentJSONLoad(input, iNode, "State", state));
	state.Parent = iParent;
	controls.States[iControl] = state;
	if_fail_fe(::guiComponentJSONLoad(input, iNode, "TextState", controls.TextState[iControl]));
	if_fail_fe(::guiComponentJSONLoad(input, iNode, "Constraints", controls.Constraints[iControl]));
	if_fail_fe(::guiComponentJSONLoad(input, iNode, "Placement", controls.Placement[iControl]));
	if_fail_fe(::guiComponentJSONLoad(input, iNode, "Area", controls.Area[iControl]));
	if_fail_fe(::guiComponentJSONLoad(input, iNode, "Draw", controls.Draw[iControl]));
	::gpk::SControlText text = controls.Text[iControl];
	if_fail_fe(::guiComponentJSONLoad(input, iNode, "Text", text));
	controls.Text[iControl] = text;
	if_fail_fe(::guiComponentJSONLoad(input, iNode, "Script", controls.Script[iControl]));
	cnst ::gpk::SJSONNode & node = *input.Tree[iNode];
	for(::gpk::u2_t iChild = 0; iChild + 1 < node.Children.size(); iChild += 2) {
		::gpk::vcst_t key = {};
		if_fail_fe(::gpk::jsonObjectGetString(input, (::gpk::u2_t)node.Children[iChild]->ObjectIndex, key));
		if(::controlCTTComponent(key))
			continue;
		cnst ::gpk::u2_t iChildNode = (::gpk::u2_t)node.Children[iChild + 1]->ObjectIndex;
		if_true_fef(input.Token[iChildNode].Type != ::gpk::JSON_TYPE_OBJECT, "Control '%.*s' child '%.*s' is not an object.", GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(key));
		if_fail_fe(::controlCTTJSONLoad(controls, defaultColors, input, iChildNode, key, iControl));
	}
	rtrn iControl;
}

gpk::err_t gpk::controlTableJSONLoad(SControlTable & controls, vs1_t defaultColors, cnst SJSONReader & input, u2_t iNode) {
	if_true_fe(iNode >= input.Token.size() || input.Token[iNode].Type != JSON_TYPE_OBJECT);
	SControlTable loaded = {};
	loaded.EventQueue = controls.EventQueue;
	cnst SJSONNode & node = *input.Tree[iNode];
	for(u2_t iChild = 0; iChild + 1 < node.Children.size(); iChild += 2) {
		vcst_t identifier = {};
		if_fail_fe(jsonObjectGetString(input, (u2_t)node.Children[iChild]->ObjectIndex, identifier));
		if(identifier == "EventQueue")
			continue;
		cnst u2_t iControlNode = (u2_t)node.Children[iChild + 1]->ObjectIndex;
		if_true_fef(input.Token[iControlNode].Type != JSON_TYPE_OBJECT, "Root control '%.*s' is not an object.", GPK_VIEW_ARGS(identifier));
		if_fail_fe(::controlCTTJSONLoad(loaded, defaultColors, input, iControlNode, identifier, CID_INVALID));
	}
	if_fail_fe(::controlEventQueueJSONLoad(input, iNode, loaded.EventQueue));
	controls = loaded;
	rtrn controls.States.size();
}
//
//gpk::err_t gpk::controlTableJSONSave(cnst SControlTable & controls, string & outputJsonString) {
//	string output = {};
//	asts1_t<GUI_CONTROL_PALETTE_COUNT> defaultColors = {};
//	if(controls.Colors)
//		defaultColors = controls.Colors->DefaultColors;
//	if_fail_fe(controlTableJSONSave(output, controls, defaultColors));
//	rtrn fileFromMemory(fileName, output);
//}

gpk::err_t gpk::controlTableJSONLoad(SControlTable & controls, vcst_t & inputJsonDocumentAndCursor) {
	SJSONReader input = {};
	if_fail_fe(jsonParse(input, inputJsonDocumentAndCursor));
	cnst err_t result = controlTableJSONLoad(controls, input);
	rtrn result;
}

gpk::err_t gpk::controlTableJSONLoad(SControlTable & controls, cnst SJSONReader & input, u2_t iNode) {
	if_true_fe(iNode >= input.Token.size() || input.Token[iNode].Type != JSON_TYPE_OBJECT);
	for(u2_t iControl = 0; iControl < controls.States.size(); ++iControl) {
		if(0 == controls.Identifier[iControl].size() || ::controlHasIdentifiedAncestor(controls, (cid_t)iControl))
			continue;
		cnst err_t iControlNode = jsonObjectValueGet(input, iNode, controls.Identifier[iControl]);
		if(0 <= iControlNode)
			if_fail_fe(::controlJSONLoad(controls, (cid_t)iControl, input, (u2_t)iControlNode));
	}
	if_fail_fe(::controlEventQueueJSONLoad(input, iNode, controls.EventQueue));
	rtrn 0;
}
