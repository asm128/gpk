#include "gpk_gui.h"
#include "gpk_ctti_stct_json.h"
#include "gpk_gui_theme.h"
#include "gpk_gui_text.h"
#include "gpk_base64.h"
#include "gpk_json_expression.h"
#include "gpk_label.h"
#include "gpk_rect_align.h"
#include "gpk_framework_module.h"
#include "gpk_gui_control_theme.h"
#include "gpk_event_input.h"

usns gpk;

sttc ::gpk::err_t guiEventChildType(::gpk::SYSTEM_EVENT parent, ::gpk::vcst_t name, ::gpk::u0_t & child) {
	switch(parent) {
	default: rtrn 0;
	case ::gpk::SYSTEM_EVENT_Framework	: { ::gpk::APPLICATION_STATE value = {}; if_fail_fe(::gpk::get_enum<::gpk::APPLICATION_STATE>().get_value(name, value)); child = (::gpk::u0_t)value; rtrn 1; }
	case ::gpk::SYSTEM_EVENT_GUI		: { ::gpk::EVENT_GUI_CONTROL value = {}; if_fail_fe(::gpk::get_enum<::gpk::EVENT_GUI_CONTROL>().get_value(name, value)); child = (::gpk::u0_t)value; rtrn 1; }
	case ::gpk::SYSTEM_EVENT_Text		: { ::gpk::EVENT_TEXT value = {}; if_fail_fe(::gpk::get_enum<::gpk::EVENT_TEXT>().get_value(name, value)); child = (::gpk::u0_t)value; rtrn 1; }
	case ::gpk::SYSTEM_EVENT_Keyboard	: { ::gpk::EVENT_KEYBOARD value = {}; if_fail_fe(::gpk::get_enum<::gpk::EVENT_KEYBOARD>().get_value(name, value)); child = (::gpk::u0_t)value; rtrn 1; }
	case ::gpk::SYSTEM_EVENT_Mouse		: { ::gpk::EVENT_MOUSE value = {}; if_fail_fe(::gpk::get_enum<::gpk::EVENT_MOUSE>().get_value(name, value)); child = (::gpk::u0_t)value; rtrn 1; }
	case ::gpk::SYSTEM_EVENT_Touch		: { ::gpk::EVENT_TOUCH value = {}; if_fail_fe(::gpk::get_enum<::gpk::EVENT_TOUCH>().get_value(name, value)); child = (::gpk::u0_t)value; rtrn 1; }
	case ::gpk::SYSTEM_EVENT_Joypad		: { ::gpk::EVENT_JOYPAD value = {}; if_fail_fe(::gpk::get_enum<::gpk::EVENT_JOYPAD>().get_value(name, value)); child = (::gpk::u0_t)value; rtrn 1; }
	}
}

sttc ::gpk::vcst_t guiEventChildName(::gpk::SYSTEM_EVENT parent, ::gpk::u0_t child) {
	switch(parent) {
	default: rtrn {};
	case ::gpk::SYSTEM_EVENT_Framework	: rtrn ::gpk::get_value_name((::gpk::APPLICATION_STATE)child);
	case ::gpk::SYSTEM_EVENT_GUI		: rtrn ::gpk::get_value_name((::gpk::EVENT_GUI_CONTROL)child);
	case ::gpk::SYSTEM_EVENT_Text		: rtrn ::gpk::get_value_name((::gpk::EVENT_TEXT)child);
	case ::gpk::SYSTEM_EVENT_Keyboard	: rtrn ::gpk::get_value_name((::gpk::EVENT_KEYBOARD)child);
	case ::gpk::SYSTEM_EVENT_Mouse		: rtrn ::gpk::get_value_name((::gpk::EVENT_MOUSE)child);
	case ::gpk::SYSTEM_EVENT_Touch		: rtrn ::gpk::get_value_name((::gpk::EVENT_TOUCH)child);
	case ::gpk::SYSTEM_EVENT_Joypad		: rtrn ::gpk::get_value_name((::gpk::EVENT_JOYPAD)child);
	}
}

sttc ::gpk::err_t guiControlFlag(::gpk::vcst_t name, ::gpk::GUI_CONTROL_FLAG & flag) {
	stxp ::gpk::GUI_CONTROL_FLAG flags[] =
		{ ::gpk::GUI_CONTROL_FLAG_Hidden, ::gpk::GUI_CONTROL_FLAG_Disabled, ::gpk::GUI_CONTROL_FLAG_Hovered
		, ::gpk::GUI_CONTROL_FLAG_Focused, ::gpk::GUI_CONTROL_FLAG_Pressed, ::gpk::GUI_CONTROL_FLAG_Dragging
		, ::gpk::GUI_CONTROL_FLAG_Selected, ::gpk::GUI_CONTROL_FLAG_Updated, ::gpk::GUI_CONTROL_FLAG_Action
		};
	for(::gpk::u2_t iFlag = 0; iFlag < szof(flags) / szof(flags[0]); ++iFlag)
		if(name == ::gpk::cttiEnumName(flags[iFlag])) {
			flag = flags[iFlag];
			rtrn 1;
		}
	rtrn 0;
}

sttc ::gpk::err_t guiScriptEventPattern(::gpk::vcst_t key, ::gpk::cid_t iControl, ::gpk::SEventSystem & pattern) {
	::gpk::u2_t dot0 = key.size(), dot1 = key.size();
	for(::gpk::u2_t iCharacter = 0; iCharacter < key.size(); ++iCharacter)
		if('.' == key[iCharacter]) {
			if(dot0 == key.size()) dot0 = iCharacter;
			else { dot1 = iCharacter; break; }
		}
	::gpk::vcst_t part0 = {key.begin(), dot0};
	::gpk::vcst_t part1 = dot0 < key.size() ? ::gpk::vcst_t{key.begin() + dot0 + 1, dot1 - dot0 - 1} : ::gpk::vcst_t{};
	::gpk::vcst_t part2 = dot1 < key.size() ? ::gpk::vcst_t{key.begin() + dot1 + 1, key.size() - dot1 - 1} : ::gpk::vcst_t{};
	::gpk::SYSTEM_EVENT parent = {};
	if(0 <= ::gpk::get_enum<::gpk::SYSTEM_EVENT>().get_value(part0, parent)) {
		if(::gpk::SYSTEM_EVENT_GUI != parent) {
			pattern.Type = parent;
			if(0 == part1.size()) rtrn 0;
			::gpk::u0_t child = {};
			if(0 >= ::guiEventChildType(parent, part1, child)) rtrn 0;
			if_fail_fe(::gpk::eventWrapChild(pattern, child, ::gpk::vcu0_t{}));
			rtrn 1;
		}
		part0 = part1;
		part1 = part2;
	}
	::gpk::GUI_CONTROL_FLAG flag = {};
	if(0 >= ::guiControlFlag(part0, flag)) rtrn 0;
	::gpk::SChangeControlState change = {iControl};
	if(part1 == "Clear") change.Clear = flag;
	else if(0 == part1.size() || part1 == "Set") change.Set = flag;
	else rtrn 0;
	pattern.Type = ::gpk::SYSTEM_EVENT_GUI;
	if_fail_fe(::gpk::eventWrapChild(pattern, ::gpk::EVENT_GUI_CONTROL_StateChange, ::gpk::vcu0_t{(cnst ::gpk::u0_t*)&change, szof(change)}));
	rtrn 1;
}

GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlPlacement, Area);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlPlacement, Margin);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlPlacement, Border);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlPlacement, Align);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlPlacement, RelativeBorderSize);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlPlacement, RelativeMarginSize);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlPlacement, RelativeMetrics);

GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlDraw, ColorMode);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlDraw, FrameOut);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlDraw, NoBorder);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlDraw, NoClient);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlDraw, UseNewPalettes);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlDraw, ColorTheme);
GDEFINE_STCT_CTTI_JSON_MEMBER(::gpk::SControlDraw, Palettes);

gpk::cid_t		gpk::controlCreateChild	(SGUI & gui, cid_t iParent)						{
	cid_t				idControl				= controlCreate(gui);
	controlSetParent(gui.Controls, idControl, iParent);
	return idControl;
}

gpk::err_t		gpk::guiSetupDefaults	(SGUI & gui)							{
	if_fail_fe(controlColorsDefault(gui.Colors));

	if(0 == gui.Fonts.size())
		if_fail_fe(controlFontsDefault(gui.Fonts));

	if(gui.Fonts.size() <= gui.SelectedFont)
		gui.SelectedFont = u1_t(gui.Fonts.size() - 1);
	return 0;
}

gpk::cid_t		gpk::controlCreate		(SGUI & gui)							{
	if_fail_fe(guiSetupDefaults(gui));
	return controlCreate(gui.Controls, gui.Colors->DefaultColors);
}

tplt<tpnm T>
sttc gpk::err_t guiComponentJSONSave(::gpk::string & output, ::gpk::vcst_t name, cnst T & component, b8_t & separator) {
	if_fail_fe(::gpk::appendOpenKey(output, name, separator));
	if_fail_fe(::gpk::cttiValueSave(output, component));
	separator = true;
	rtrn 0;
}

sttc b8_t guiControlHasIdentifiedAncestor(cnst ::gpk::SGUI & gui, ::gpk::cid_t iControl) {
	for(::gpk::cid_t iParent = gui.Controls.States[iControl].Parent; false == ::gpk::controlInvalid(gui, iParent); iParent = gui.Controls.States[iParent].Parent)
		if(gui.Controls.Identifier[iParent].size())
			rtrn true;
	rtrn false;
}

sttc ::gpk::err_t guiControlScriptJSONSave(::gpk::string & output, cnst ::gpk::SGUI & gui, ::gpk::cid_t iControl, ::gpk::b8_t & separator) {
	if_fail_fe(::gpk::appendOpenKey(output, "Script", separator));
	cnst ::gpk::vcst_t script = gui.Controls.Script[iControl];
	::gpk::SJSONReader scriptJSON = {};
	if(script.size() && 0 <= ::gpk::jsonParse(scriptJSON, script) && ::gpk::JSON_TYPE_OBJECT == scriptJSON.Token[0].Type) {
		::gpk::aobj<::gpk::vcst_t> keys = {};
		if_fail_fe(::gpk::jsonObjectKeyList(scriptJSON, 0, keys));
		for(::gpk::u2_t iKey = 0; iKey < keys.size(); ++iKey) {
			::gpk::SEventSystem pattern = {};
			if(0 < ::guiScriptEventPattern(keys[iKey], iControl, pattern)) {
				if_fail_fe(output.append(script));
				separator = true;
				rtrn 1;
			}
		}
	}
	if_fail_fe(::gpk::cttiValueSave(output, gui.Controls.Script[iControl]));
	separator = true;
	rtrn 0;
}

sttc ::gpk::cid_t guiControlIdentifiedRoot(cnst ::gpk::SGUI & gui, ::gpk::cid_t iControl) {
	::gpk::cid_t iRoot = gui.Controls.Identifier[iControl].size() ? iControl : ::gpk::CID_INVALID;
	for(::gpk::cid_t iParent = gui.Controls.States[iControl].Parent; false == ::gpk::controlInvalid(gui, iParent); iParent = gui.Controls.States[iParent].Parent)
		if(gui.Controls.Identifier[iParent].size())
			iRoot = iParent;
	rtrn iRoot;
}

sttc ::gpk::err_t guiControlJSONSave(::gpk::string & output, cnst ::gpk::SGUI & gui, ::gpk::cid_t iControl);

sttc ::gpk::err_t guiControlChildrenJSONSave(::gpk::string & output, cnst ::gpk::SGUI & gui, ::gpk::cid_t iControl, b8_t & separator) {
	cnst ::gpk::acid & children = gui.Controls.Children[iControl];
	for(::gpk::u2_t iChild = 0; iChild < children.size(); ++iChild) {
		cnst ::gpk::cid_t idChild = children[iChild];
		//if(::gpk::controlInvalid(gui, idChild))	WHY IS THIS CHECK NEEDED? IS THE RELATIONSHIP MANAGEMENT BROKEN?
		//	continue;								WHY IS THIS CHECK NEEDED? IS THE RELATIONSHIP MANAGEMENT BROKEN?
		::gpk::vcst_c identifier = gui.Controls.Identifier[idChild];
		if(0 == identifier.size()) {
			if_fail_fe(::guiControlChildrenJSONSave(output, gui, idChild, separator));
		} else {
			if_fail_fe(::gpk::appendOpenKey(output, identifier, separator));
			if_fail_fe(::guiControlJSONSave(output, gui, idChild));
			separator = true;
		}
	}
	rtrn 0;
}

sttc ::gpk::err_t guiControlJSONSave(::gpk::string & output, cnst ::gpk::SGUI & gui, ::gpk::cid_t iControl) {
	if_fail_fe(output.push_back('{'));
	b8_t separator = false;
	if_fail_fe(::gpk::appendOpenKey(output, "Id"));
	if_fail_fe(::gpk::cttiValueSave(output, iControl));
	separator = true;
	if_fail_fe(::guiComponentJSONSave(output, "State", gui.Controls.States[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "TextState", gui.Controls.TextState[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Constraints", gui.Controls.Constraints[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Placement", gui.Controls.Placement[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Draw", gui.Controls.Draw[iControl], separator));
	if_fail_fe(::guiComponentJSONSave(output, "Text", gui.Controls.Text[iControl], separator));
	if_fail_fe(::guiControlScriptJSONSave(output, gui, iControl, separator));
	if_fail_fe(::guiControlChildrenJSONSave(output, gui, iControl, separator));
	rtrn output.push_back('}');
}

sttc ::gpk::err_t guiControlTreeJSONSave(::gpk::string & output, cnst ::gpk::SGUI & gui, ::gpk::cid_t iControl) {
	if_true_fef(::gpk::controlInvalid(gui, iControl), "iControl:%i", iControl);
	cnst ::gpk::vcst_t identifier = gui.Controls.Identifier[iControl];
	if_true_fef(0 == identifier.size(), "iControl:%i", iControl);
	::gpk::u2_c offset = output.size();
	if_fail_fe(output.push_back('{'));
	if_fail_fe(::gpk::appendOpenKey(output, identifier));
	if_fail_fe(::guiControlJSONSave(output, gui, iControl));
	if_fail_fe(output.push_back('}'));
	rtrn output.size() - offset;
}

gpk::err_t gpk::guiJSONSave(::gpk::string & output, cnst ::gpk::SGUI & gui) {
	u2_c offset = output.size();
	if_fail_fe(output.push_back('{'));
	b8_t separator = false;
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		if(controlInvalid(gui, (cid_t)iControl) || 0 == gui.Controls.Identifier[iControl].size() || ::guiControlHasIdentifiedAncestor(gui, (cid_t)iControl))
			continue;
		if_fail_fe(appendOpenKey(output, gui.Controls.Identifier[iControl], separator));
		if_fail_fe(::guiControlJSONSave(output, gui, (cid_t)iControl));
		separator = true;
	}
	if_fail_fe(output.push_back('}'));
	rtrn output.size() - offset;
}

tplt<tpnm T>
sttc ::gpk::err_t guiComponentJSONLoad(cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode, ::gpk::vcst_t name, T & component) {
	cnst ::gpk::err_t iComponent = ::gpk::jsonObjectValueGet(input, iNode, name);
	if(0 > iComponent)
		rtrn 0;
	if_fail_fe(::gpk::cttiValueLoad(input, (::gpk::u2_t)iComponent, component));
	rtrn 1;
}

sttc ::gpk::err_t guiControlJSONLoad(::gpk::SGUI & gui, ::gpk::cid_t iControl, cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode);

sttc ::gpk::err_t guiControlScriptJSONLoad(::gpk::SGUI & gui, ::gpk::cid_t iControl, cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode) {
	cnst ::gpk::err_t iScript = ::gpk::jsonObjectValueGet(input, iNode, "Script");
	if(0 > iScript)
		rtrn 0;
	if(::gpk::JSON_TYPE_STRING == input.Token[iScript].Type) {
		if_fail_fe(::gpk::cttiValueLoad(input, (::gpk::u2_t)iScript, gui.Controls.Script[iControl]));
		rtrn 1;
	}
	if_true_fe(::gpk::JSON_TYPE_OBJECT != input.Token[iScript].Type);
	gui.Controls.Script[iControl].clear();
	if_fail_fe(::gpk::jsonWrite(input.Tree[iScript], input.View, gui.Controls.Script[iControl]));
	rtrn 1;
}

sttc ::gpk::err_t guiControlChildrenJSONLoad(::gpk::SGUI & gui, ::gpk::cid_t iControl, cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode) {
	cnst ::gpk::acid & children = gui.Controls.Children[iControl];
	for(::gpk::u2_t iChild = 0; iChild < children.size(); ++iChild) {
		cnst ::gpk::cid_t idChild = children[iChild];
		if(::gpk::controlInvalid(gui, idChild))
			continue;
		cnst ::gpk::vcst_t identifier = gui.Controls.Identifier[idChild];
		if(identifier.size()) {
			cnst ::gpk::err_t iChildNode = ::gpk::jsonObjectValueGet(input, iNode, identifier);
			if(0 <= iChildNode)
				if_fail_fe(::guiControlJSONLoad(gui, idChild, input, (::gpk::u2_t)iChildNode));
		}
		else
			if_fail_fe(::guiControlChildrenJSONLoad(gui, idChild, input, iNode));
	}
	rtrn 0;
}

sttc ::gpk::err_t guiControlJSONLoad(::gpk::SGUI & gui, ::gpk::cid_t iControl, cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode) {
	::gpk::err_t loaded = 0;
	::gpk::SControlState state = gui.Controls.States[iControl];
	::gpk::err_t loadedState = ::guiComponentJSONLoad(input, iNode, "State", state);
	if_fail_fe(loadedState);
	if(loadedState) {
		cnst ::gpk::cid_t oldParent = gui.Controls.States[iControl].Parent;
		if(state.Parent != oldParent)
			if_fail_fe(::gpk::controlSetParent(gui, iControl, state.Parent));
		gui.Controls.States[iControl] = state;
		++loaded;
	}
	::gpk::err_t result = ::guiComponentJSONLoad(input, iNode, "TextState", gui.Controls.TextState[iControl]);
	if_fail_fe(result); loaded += result;
	result = ::guiComponentJSONLoad(input, iNode, "Constraints", gui.Controls.Constraints[iControl]);
	if_fail_fe(result); loaded += result;
	result = ::guiComponentJSONLoad(input, iNode, "Placement", gui.Controls.Placement[iControl]);
	if_fail_fe(result); loaded += result;
	result = ::guiComponentJSONLoad(input, iNode, "Draw", gui.Controls.Draw[iControl]);
	if_fail_fe(result); loaded += result;
	result = ::guiControlScriptJSONLoad(gui, iControl, input, iNode);
	if_fail_fe(result); loaded += result;
	::gpk::SControlText text = gui.Controls.Text[iControl];
	result = ::guiComponentJSONLoad(input, iNode, "Text", text);
	if_fail_fe(result);
	if(result) {
		cnst ::gpk::err_t iText = ::gpk::jsonObjectValueGet(input, iNode, "Text");
		if(0 <= iText && 0 <= ::gpk::jsonObjectValueGet(input, (::gpk::u2_t)iText, "Text"))
			text.Text = ::gpk::label(text.Text);
		gui.Controls.Text[iControl] = text;
		loaded += result;
	}
	if(loaded)
		if_fail_fe(::gpk::controlMetricsInvalidate(gui, iControl));
	if_fail_fe(::guiControlChildrenJSONLoad(gui, iControl, input, iNode));
	rtrn loaded;
}

gpk::err_t gpk::guiJSONLoad(::gpk::SGUI & gui, cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode) {
	if_true_fe(iNode >= input.Token.size());
	if(::gpk::JSON_TYPE_ARRAY == input.Token[iNode].Type) {
		cnst ::gpk::err_t count = ::gpk::jsonArraySize(input, iNode);
		if_fail_fe(count);
		for(::gpk::u2_t iElement = 0; iElement < (::gpk::u2_t)count; ++iElement) {
			cnst ::gpk::err_t iValue = ::gpk::jsonArrayValueGet(input, iNode, iElement);
			if_fail_fe(iValue);
			if_fail_fe(::gpk::guiJSONLoad(gui, input, (::gpk::u2_t)iValue));
		}
		rtrn 0;
	}
	if_true_fe(::gpk::JSON_TYPE_OBJECT != input.Token[iNode].Type);
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		if(controlInvalid(gui, (cid_t)iControl) || 0 == gui.Controls.Identifier[iControl].size() || ::guiControlHasIdentifiedAncestor(gui, (cid_t)iControl))
			continue;
		cnst err_t iControlNode = jsonObjectValueGet(input, iNode, gui.Controls.Identifier[iControl]);
		if(0 <= iControlNode)
			if_fail_fe(::guiControlJSONLoad(gui, (cid_t)iControl, input, (u2_t)iControlNode));
	}
	rtrn 0;
}

gpk::err_t gpk::guiLoad(::gpk::SGUI & gui, cnst ::gpk::SJSONReader & input) {
	cnst ::gpk::err_t iGUI = ::gpk::jsonExpressionResolve("GUI", input, 0);
	if_fail_fe(iGUI);
	::gpk::aobj<::gpk::vcst_t> identifiers = {};
	if_fail_fe(::gpk::jsonObjectKeyList(input, (::gpk::u2_t)iGUI, identifiers));
	for(::gpk::u2_t iIdentifier = 0; iIdentifier < identifiers.size(); ++iIdentifier) {
		if(identifiers[iIdentifier] == "Events")
			continue;
		cnst ::gpk::cid_t iControl = ::gpk::controlCreate(gui);
		if_fail_fe(iControl);
		gui.Controls.Identifier[iControl] = ::gpk::label(identifiers[iIdentifier]);
	}
	rtrn ::gpk::guiJSONLoad(gui, input, (::gpk::u2_t)iGUI);
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

sttc ::gpk::err_t guiJSONTemplateResolveNode(cnst ::gpk::SJSONReader & source, ::gpk::u2_t iSource, cnst ::gpk::SJSONReader & values, ::gpk::u2_t iValues, ::gpk::string & output) {
	if_true_fe(iSource >= source.Token.size());
	cnst ::gpk::JSON_TYPE type = source.Token[iSource].Type;
	if(::gpk::JSON_TYPE_OBJECT == type) {
		if_fail_fe(output.push_back('{'));
		cnst ::gpk::SJSONNode & node = *source.Tree[iSource];
		for(::gpk::u2_t iChild = 0; iChild < node.Children.size(); iChild += 2) {
			::gpk::vcst_t key = {};
			if_fail_fe(::gpk::jsonObjectGetString(source, (::gpk::u2_t)node.Children[iChild]->ObjectIndex, key));
			if_fail_fe(::gpk::appendOpenKey(output, key, 0 != iChild));
			if_fail_fe(::guiJSONTemplateResolveNode(source, (::gpk::u2_t)node.Children[iChild + 1]->ObjectIndex, values, iValues, output));
		}
		rtrn output.push_back('}');
	}
	if(::gpk::JSON_TYPE_ARRAY == type) {
		if_fail_fe(output.push_back('['));
		cnst ::gpk::SJSONNode & node = *source.Tree[iSource];
		for(::gpk::u2_t iChild = 0; iChild < node.Children.size(); ++iChild) {
			if(iChild) if_fail_fe(output.push_back(','));
			if_fail_fe(::guiJSONTemplateResolveNode(source, (::gpk::u2_t)node.Children[iChild]->ObjectIndex, values, iValues, output));
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

sttc ::gpk::err_t guiEventJSONSave(::gpk::string & output, cnst ::gpk::SEventSystem & event) {
	if_fail_fe(output.push_back('{'));
	if_fail_fe(::gpk::appendOpenKey(output, "Type"));
	if_fail_fe(::gpk::appendJSONString(output, ::gpk::get_value_name(event.Type)));
	if(event.Data.size()) {
		::gpk::vcu0_t childInput = event.Data;
		::gpk::SEView<::gpk::u0_t> child = {};
		if_fail_fe(child.Load(childInput));
		if_fail_fe(::gpk::appendOpenKey(output, "Data", true));
		if_fail_fe(output.push_back('{'));
		if_fail_fe(::gpk::appendOpenKey(output, "Type"));
		cnst ::gpk::vcst_t childName = ::guiEventChildName(event.Type, child.Type);
		if(childName.size()) {
			if_fail_fe(::gpk::appendJSONString(output, childName));
		}
		else {
			if_fail_fe(::gpk::cttiValueSave(output, child.Type));
		}
		if(child.Data.size()) {
			if_fail_fe(::gpk::appendOpenKey(output, "Data", true));
			if(::gpk::SYSTEM_EVENT_GUI == event.Type && ::gpk::EVENT_GUI_CONTROL_StateChange == child.Type) {
				::gpk::vcu0_t data = child.Data;
				::gpk::SChangeControlState change = {};
				if_fail_fe(::gpk::loadPOD(data, change));
				if_fail_fe(::gpk::cttiValueSave(output, change));
			}
			else if(::gpk::SYSTEM_EVENT_Text == event.Type && ::gpk::EVENT_TEXT_Char == child.Type) {
				::gpk::vcu0_t data = child.Data;
				::gpk::u2_t codePoint = {};
				if_fail_fe(::gpk::loadPOD(data, codePoint));
				if(codePoint <= 0x7F) {
					cnst ::gpk::sc_t character = (::gpk::sc_t)codePoint;
					if_fail_fe(::gpk::appendJSONString(output, ::gpk::vcst_t{&character, 1}));
				}
				else if_fail_fe(::gpk::cttiValueSave(output, codePoint));
			}
			else if(1 == child.Data.size()) {
				::gpk::vcu0_t data = child.Data;
				::gpk::u0_t value = {};
				if_fail_fe(::gpk::loadPOD(data, value));
				if_fail_fe(::gpk::cttiValueSave(output, value));
			}
			else if_fail_fe(::gpk::cttiValueSave(output, child.Data.size()));
		}
		if_fail_fe(output.push_back('}'));
	}
	rtrn output.push_back('}');
}

sttc ::gpk::err_t guiEventPostNode(::gpk::SGUI & gui, cnst ::gpk::SJSONReader & source, ::gpk::u2_t iEvent) {
	if(::gpk::JSON_TYPE_ARRAY == source.Token[iEvent].Type) {
		cnst ::gpk::err_t count = ::gpk::jsonArraySize(source, iEvent);
		if_fail_fe(count);
		for(::gpk::u2_t iElement = 0; iElement < (::gpk::u2_t)count; ++iElement) {
			cnst ::gpk::err_t iValue = ::gpk::jsonArrayValueGet(source, iEvent, iElement);
			if_fail_fe(iValue);
			if_fail_fe(::guiEventPostNode(gui, source, (::gpk::u2_t)iValue));
		}
		rtrn count;
	}
	if_true_fe(::gpk::JSON_TYPE_OBJECT != source.Token[iEvent].Type);
	cnst ::gpk::err_t iType = ::gpk::jsonObjectValueGet(source, iEvent, "Type");
	if_fail_fe(iType);
	::gpk::vcst_t typeName = {};
	if_fail_fe(::gpk::jsonObjectGetString(source, (::gpk::u2_t)iType, typeName));
	::gpk::SYSTEM_EVENT parent = {};
	if_fail_fe(::gpk::get_enum<::gpk::SYSTEM_EVENT>().get_value(typeName, parent));
	cnst ::gpk::err_t iData = ::gpk::jsonObjectValueGet(source, iEvent, "Data");
	if(0 > iData) {
		::gpk::pobj<::gpk::SEventSystem> event = {};
		event->Type = parent;
		rtrn gui.Controls.EventQueue.push_back(event);
	}
	cnst ::gpk::err_t iChildType = ::gpk::jsonObjectValueGet(source, (::gpk::u2_t)iData, "Type");
	if_fail_fe(iChildType);
	::gpk::vcst_t childName = {};
	if_fail_fe(::gpk::jsonObjectGetString(source, (::gpk::u2_t)iChildType, childName));
	::gpk::u0_t child = {};
	if_true_fe(0 >= ::guiEventChildType(parent, childName, child));
	cnst ::gpk::err_t iChildData = ::gpk::jsonObjectValueGet(source, (::gpk::u2_t)iData, "Data");
	if(::gpk::SYSTEM_EVENT_Text == parent && ::gpk::EVENT_TEXT_Char == child) {
		if_fail_fe(iChildData);
		::gpk::vcst_t text = {};
		if_fail_fe(::gpk::jsonObjectGetString(source, (::gpk::u2_t)iChildData, text));
		if_true_fe(0 == text.size());
		rtrn ::gpk::eventEnqueueTextChar(gui.Controls.EventQueue, (::gpk::u0_t)text[0]);
	}
	if(::gpk::SYSTEM_EVENT_GUI == parent && ::gpk::EVENT_GUI_CONTROL_StateChange == child) {
		if_fail_fe(iChildData);
		::gpk::SChangeControlState change = {};
		if_fail_fe(::gpk::cttiValueLoad(source, (::gpk::u2_t)iChildData, change));
		rtrn ::gpk::eventEnqueueChild(gui.Controls.EventQueue, parent, (::gpk::EVENT_GUI_CONTROL)child, change);
	}
	if(0 > iChildData)
		rtrn ::gpk::eventEnqueueChild(gui.Controls.EventQueue, parent, child, ::gpk::vcu0_t{});
	::gpk::u0_t value = {};
	if_fail_fe(::gpk::cttiValueLoad(source, (::gpk::u2_t)iChildData, value));
	rtrn ::gpk::eventEnqueueChild(gui.Controls.EventQueue, parent, child, value);
}

sttc ::gpk::err_t guiEventsPost(::gpk::SGUI & gui, cnst ::gpk::SJSONReader & source) {
	::gpk::err_t posted = 0;
	cnst ::gpk::vcst_t eventPaths[] = {"Runtime.Events", "GUI.Events"};
	for(::gpk::u2_t iPath = 0; iPath < szof(eventPaths) / szof(eventPaths[0]); ++iPath) {
		cnst ::gpk::err_t iEvents = ::gpk::jsonExpressionResolve(eventPaths[iPath], source, 0);
		if(0 <= iEvents) {
			if_fail_fe(::guiEventPostNode(gui, source, (::gpk::u2_t)iEvents));
			++posted;
		}
	}
	cnst ::gpk::err_t iLegacyType = ::gpk::jsonExpressionResolve("Runtime.Event.Type", source, 0);
	if(0 <= iLegacyType) {
		::gpk::vcst_t type = {};
		if_fail_fe(::gpk::jsonObjectGetString(source, (::gpk::u2_t)iLegacyType, type));
		::gpk::APPLICATION_STATE state = {};
		if_fail_fe(::gpk::get_enum<::gpk::APPLICATION_STATE>().get_value(type, state));
		if_fail_fe(::gpk::eventEnqueueChild(gui.Controls.EventQueue, ::gpk::SYSTEM_EVENT_Framework, state, ::gpk::vcu0_t{}));
		++posted;
	}
	rtrn posted;
}

sttc ::gpk::err_t guiScriptValuesJSONSave
	( ::gpk::string & output, cnst ::gpk::SGUI & gui, ::gpk::cid_t iRoot, ::gpk::b8_t scoped
	, cnst ::gpk::SEventSystem * runtimeEvent, cnst ::gpk::SEventSystem * guiEvent
	) {
	::gpk::string controls = {};
	if(scoped) {
		if_fail_fe(::guiControlTreeJSONSave(controls, gui, iRoot));
	}
	else {
		if_fail_fe(::gpk::guiJSONSave(controls, gui));
	}
	if_fail_fe(output.push_back('{'));
	if_fail_fe(::gpk::appendOpenKey(output, "Runtime"));
	if_fail_fe(output.push_back('{'));
	if_fail_fe(::gpk::appendOpenKey(output, "Events"));
	if(runtimeEvent) {
		if_fail_fe(::guiEventJSONSave(output, *runtimeEvent));
	}
	else {
		if_fail_fe(output.append("null"));
	}
	if_fail_fe(output.push_back('}'));
	if_fail_fe(::gpk::appendOpenKey(output, "GUI", true));
	if_fail_fe(output.push_back('{'));
	if_fail_fe(::gpk::appendOpenKey(output, "Events"));
	if(guiEvent) {
		if_fail_fe(::guiEventJSONSave(output, *guiEvent));
	}
	else {
		if_fail_fe(output.append("null"));
	}
	if_fail_fe(output.push_back('}'));
	if(controls.size() > 2) {
		if_fail_fe(output.push_back(','));
		if_fail_fe(output.append(::gpk::vcst_t{controls.begin() + 1, controls.size() - 2}));
	}
	rtrn output.push_back('}');
}

sttc ::gpk::err_t guiScriptRunSource
	( ::gpk::SGUI & gui, ::gpk::cid_t iControl, ::gpk::vcst_t script
	, cnst ::gpk::SEventSystem * runtimeEvent, cnst ::gpk::SEventSystem * guiEvent
	) {
	if_true_fef(controlInvalid(gui, iControl), "iControl:%i", iControl);
	cnst vcst_t identifier = gui.Controls.Identifier[iControl];
	if(0 == script.size())
		rtrn 0;
	SJSONReader source = {};
	if_fail_fef(jsonParse(source, script), "iControl:%i, Identifier:%.*s, Script:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(script));
	cnst cid_t iRoot = ::guiControlIdentifiedRoot(gui, iControl);
	cnst vcst_t rootIdentifier = controlInvalid(gui, iRoot) ? vcst_t{} : gui.Controls.Identifier[iRoot];
	b8_t dynamic = false;
	b8_t scoped = true;
	if_fail_fef(::guiJSONTemplateDynamic(source, 0, rootIdentifier, dynamic, scoped), "iControl:%i, Identifier:%.*s, Script:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(script));
	if(false == dynamic) {
		if_fail_fef(::guiEventsPost(gui, source), "iControl:%i, Identifier:%.*s, Script:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(script));
		if_fail_fef(guiJSONLoad(gui, source), "iControl:%i, Identifier:%.*s, Patch:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(script));
		rtrn 0;
	}
	string valuesJSON = {};
	if_fail_fef(::guiScriptValuesJSONSave(valuesJSON, gui, iRoot, scoped, runtimeEvent, guiEvent), "iControl:%i, Identifier:%.*s", iControl, GPK_VIEW_ARGS(identifier));
	SJSONReader values = {};
	if_fail_fef(jsonParse(values, valuesJSON), "iControl:%i, Identifier:%.*s, ValuesSize:%u", iControl, GPK_VIEW_ARGS(identifier), valuesJSON.size());
	string patchJSON = {};
	if_fail_fef(::guiJSONTemplateResolveNode(source, 0, values, 0, patchJSON), "iControl:%i, Identifier:%.*s, Script:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(script));
	SJSONReader patch = {};
	if_fail_fef(jsonParse(patch, patchJSON), "iControl:%i, Identifier:%.*s, Patch:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(patchJSON));
	if_fail_fef(::guiEventsPost(gui, patch), "iControl:%i, Identifier:%.*s, Patch:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(patchJSON));
	if_fail_fef(guiJSONLoad(gui, patch), "iControl:%i, Identifier:%.*s, Patch:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(patchJSON));
	rtrn 0;
}

gpk::err_t gpk::guiScriptRun(::gpk::SGUI & gui, ::gpk::cid_t iControl) {
	rtrn ::guiScriptRunSource(gui, iControl, gui.Controls.Script[iControl], {}, {});
}

sttc	gpk::err_t	actualControlUpdate			(SGUI & gui, cid_t iControl) {
	SControlImage			& controlImage				= gui.Controls.Images[iControl];
	n2u2_c					imageMetrics				= controlImage.Image.metrics();
	if(0 == (imageMetrics.x | imageMetrics.y))
		rtrn 0;
	if(0 == controlImage.ImageInvertY)
		rtrn 0;
	img8bgra				& inverted					= controlImage.Temp.Color;
	if_fail_fe(inverted.resize(imageMetrics));
	rtrn 0;
}
gpk::err_t			gpk::controlUpdateHierarchy	(SGUI & gui, cid_t iControl, n2u1_c & targetMetrics) {
	if_fail_fef(controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	if(gui.LastSize != targetMetrics) {
		for(u2_t iOutdated = 0; iOutdated < gui.Controls.States.size(); ++iOutdated)
			gui.Controls.SetUpdated((cid_t)iOutdated, false);
		gui.LastSize			= targetMetrics;
	}
	if(false == controlHidden(gui, iControl)) {
		if_fail_fe(::actualControlUpdate(gui, iControl));
		vccid				children						= gui.Controls.Children[iControl];
		for(u2_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			if_fail_fe(gpk::controlUpdateHierarchy(gui, children[iChild], targetMetrics));
	}
	rtrn 0;
}
gpk::err_t			gpk::guiUpdate				(SGUI & gui, n2u1_c & targetMetrics) {// , vpobj<SEventSystem> in_Events
//	guiProcessInput(gui, input, in_Events);
	if_true_block_logf(info_printf, gui.LastSize != targetMetrics || gui.LastZoom != gui.Zoom, {
			info_printf("GUI Render target size or dpi changed changed.");
			for(u2_t iOutdated = 0; iOutdated < gui.Controls.States.size(); ++iOutdated)
				gui.Controls.SetUpdated((cid_t)iOutdated, false);
			gui.LastSize		= targetMetrics;
			gui.LastZoom		= gui.Zoom;
		}, "%ux%u != %ux%u || %f != %f"
		, gui.LastSize.x, gui.LastSize.y
		, targetMetrics.x, targetMetrics.y
		, gui.LastZoom, gui.Zoom
		);
	if_fail_fe(guiUpdateMetrics(gui, targetMetrics, false));
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
		if(false == controlInvalid(gui, (cid_t)iControl) && controlInvalid(gui, gui.Controls.States[iControl].Parent))
			if_fail_e(controlUpdateHierarchy(gui, (cid_t)iControl, targetMetrics));
	rtrn 0;
}
sttc gpk::err_t	buildControlGeometry	(cnst SControlPlacement & control, cnst SControlArea & controlMetrics, cnst SGUIZoom & zoom, view<rect2s1_t> finalRects, view<tri2s1_t> triangles)					{
	rects1_t			scaledBorders			= {};
	n2f3_c				scaleFinal				= zoom.DPI * zoom.ZoomLevel;
	scaledBorders.Left		= (s1_t)(control.Border.Left		* scaleFinal.x);
	scaledBorders.Top		= (s1_t)(control.Border.Top		* scaleFinal.y);
	scaledBorders.Right		= (s1_t)(control.Border.Right	* scaleFinal.x);
	scaledBorders.Bottom	= (s1_t)(control.Border.Bottom	* scaleFinal.y);
	(void)scaledBorders;

	cnst rect2s1_t		& rectTotal				= controlMetrics.Total.Global;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			]	= controlMetrics.Client.Global;
	finalRects[GUI_CONTROL_AREA_OLD_BORDER_LEFT		]	= {rectTotal.Offset , n2s1_t{(s1_t)control.Border.Left, rectTotal.Size.y}};
	finalRects[GUI_CONTROL_AREA_OLD_BORDER_TOP		]	= {rectTotal.Offset , n2s1_t{rectTotal.Size.x, (s1_t)control.Border.Top}};
	finalRects[GUI_CONTROL_AREA_OLD_BORDER_RIGHT	]	= {rectTotal.Offset + n2s1_t{s1_t(rectTotal.Size.x - control.Border.Right), 0} , n2s1_t{(s1_t)control.Border.Right, controlMetrics.Total.Global.Size.y}};
	finalRects[GUI_CONTROL_AREA_OLD_BORDER_BOTTOM	]	= {rectTotal.Offset + n2s1_t{0, s1_t(rectTotal.Size.y - control.Border.Bottom)}, n2s1_t{rectTotal.Size.x, (s1_t)control.Border.Bottom}};
	//finalRects[GUI_CONTROL_AREA_OLD_BACKGROUND	]	= rectTotal;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			].Offset.x	-= control.Margin.Left	;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			].Offset.y	-= control.Margin.Top	;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			].Size	.x	+= control.Margin.Left	+ control.Margin.Right	;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			].Size	.y	+= control.Margin.Top	+ control.Margin.Bottom	;

	// --- Draw control corners
	n2s1_t						startOffset				= controlMetrics.Total.Global.Offset;
	triangles[0]			= {startOffset, startOffset + n2u1_t{control.Border.Left, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Left, 0U}.s1()	};
	triangles[1]			= {startOffset, startOffset + n2u1_t{0U, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Left, control.Border.Top}.s1()	};

	s1_t						startOffsetX			= startOffset.x + controlMetrics.Total.Global.Size.x - control.Border.Right;
	startOffset				= {startOffsetX, controlMetrics.Total.Global.Offset.y};
	triangles[2]			= {startOffset, startOffset + n2u1_t{0U, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Right, 0U}.s1()	};
	triangles[3]			= {startOffset + n2u1_t{control.Border.Right, 0U}.s1(), startOffset + n2u1_t{0U, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Right, control.Border.Top}.s1()	};

	s1_t						startOffsetY			= startOffset.y + controlMetrics.Total.Global.Size.y - control.Border.Bottom;
	startOffset				= {controlMetrics.Total.Global.Offset.x, startOffsetY};
	triangles[4]			= {startOffset, startOffset + n2u1_t{0U, control.Border.Bottom}.s1(), startOffset + n2u1_t{control.Border.Left, 0U}.s1()	};
	triangles[5]			= {startOffset + n2u1_t{control.Border.Right, 0U}.s1(), startOffset + n2u1_t{0U, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Right, control.Border.Top}.s1()	};

	startOffset				= controlMetrics.Total.Global.Offset + controlMetrics.Total.Global.Size - n2u1_t{control.Border.Right, control.Border.Bottom}.s1();
	triangles[6]			= {startOffset, startOffset + n2u1_t{control.Border.Right, control.Border.Bottom}.s1(), startOffset + n2u1_t{control.Border.Right, 0U}.s1()	};
	triangles[7]			= {startOffset, startOffset + n2u1_t{0U, control.Border.Bottom}.s1(), startOffset + n2u1_t{control.Border.Right, control.Border.Bottom}.s1()	};
	return 0;
}

sttc gpk::cid_t	controlDockTarget	(cnst SGUI & gui, cid_t idTarget, GUI_CONTROL_LIMIT side) nxpt {
	for(u2_t iControl = 0; iControl < gui.Controls.States.size() && idTarget != CID_INVALID; ++iControl) {
		if((u2_t)idTarget >= gui.Controls.States.size())
			return idTarget;
		if(false == controlHidden(gui.Controls, idTarget))
			return idTarget;
		cnst rectcid & dock = gui.Controls.Constraints[idTarget].DockToControl;
		switch(side) {
		default: return CID_INVALID;
		case GUI_CONTROL_LIMIT_Left  : idTarget = dock.Left  ; break;
		case GUI_CONTROL_LIMIT_Top   : idTarget = dock.Top   ; break;
		case GUI_CONTROL_LIMIT_Right : idTarget = dock.Right ; break;
		case GUI_CONTROL_LIMIT_Bottom: idTarget = dock.Bottom; break;
		}
	}
	return CID_INVALID;
}

sttc	gpk::err_t controlUpdateMetrics	(SGUI & gui, cid_t iControl, cnst n2u1_t & _targetSize)					{
	if_fail_fef(gpk::controlInvalid(gui.Controls, iControl), "Invalid control id: %u.", iControl);

	SControlState		& controlState			= gui.Controls.States[iControl];
	cnst SControlPlacement	& controlPlacement	= gui.Controls.Placement[iControl];
	n2f3_t				scale					= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	if(::gpk::fabs(1.0 - scale.x) < 0.001) scale.x = 1.0;
	if(::gpk::fabs(1.0 - scale.y) < 0.001) scale.y = 1.0;
	n2f3_t				scaledPosition			= {controlPlacement.Area.Offset	.x * scale.x, controlPlacement.Area.Offset	.y * scale.y};
	n2f3_t				scaledSize				= {controlPlacement.Area.Size	.x * scale.x, controlPlacement.Area.Size	.y * scale.y};
	//rect<f3_t>	scaledBorders		= {(control.Border.Left * scale.x), (control.Border.Top * scale.y), (control.Border.Right * scale.x), (control.Border.Bottom * scale.y)};
	//rect<f3_t>	scaledMargins		= {(control.Margin.Left * scale.x), (control.Margin.Top * scale.y), (control.Margin.Right * scale.x), (control.Margin.Bottom * scale.y)};

	SControlArea			& controlMetrics		= gui.Controls.Area[iControl];
	rect2f3_t				rectText				= {};
	SControlText			& controlText			= gui.Controls.Text[iControl];
	cnst SRasterFont	& selectedFont			= *gui.Fonts[in_range(controlText.FontSelected, (s1_t)0, (s1_t)gui.Fonts.size()) ? controlText.FontSelected : gui.SelectedFont];

	cnst n2u0_t			fontCharSize			= selectedFont.CharSize;
	rectText.Size			= {(f3_t)(fontCharSize.x * controlText.Text.size()), (f3_t)fontCharSize.y};
	rectText.Size.InPlaceScale(scale.x, scale.y);

	cnst SControlConstraints	& controlConstraints	= gui.Controls.Constraints[iControl];
	rectu1_c		ncSizes					= controlNCRect(controlPlacement);
	rectf3_c		ncSizesScaled			= {(ncSizes.Left * scale.x), (ncSizes.Top * scale.y), (ncSizes.Right * scale.x), (ncSizes.Bottom * scale.y)};
	n2f3_c			ncTotalSize				= {(f3_t)ncSizes.Left + ncSizes.Right, (f3_t)ncSizes.Top + ncSizes.Bottom};
	n2f3_c			ncTotalSizeScaled		= ncTotalSize.GetScaled(scale.x, scale.y);
	if(controlConstraints.AttachSizeToText.x) scaledSize.x	= rectText.Size.x + ncTotalSizeScaled.x;
	if(controlConstraints.AttachSizeToText.y) scaledSize.y	= rectText.Size.y + ncTotalSizeScaled.y;
	b8_c			isValidParent			= 0 == controlInvalid(gui.Controls, controlState.Parent);
	n2u1_c			targetSize				= isValidParent ? gui.Controls.Area[controlState.Parent].Client.Global.Size.u1() : _targetSize.u1();

	if(controlConstraints.AttachSizeToControl.x == iControl) { if(controlConstraints.DockToControl.Left	!= -1) {} else { scaledPosition.x = 0; } scaledSize.x = targetSize.x; } else if(false == controlInvalid(gui.Controls, controlConstraints.AttachSizeToControl.x)) { if(controlConstraints.DockToControl.Left != -1) {} else { scaledPosition.x = 0;} scaledSize.x = gui.Controls.Area[controlConstraints.AttachSizeToControl.x].Total.Global.Size.x; }
	if(controlConstraints.AttachSizeToControl.y == iControl) { if(controlConstraints.DockToControl.Top	!= -1) {} else { scaledPosition.y = 0; } scaledSize.y = targetSize.y; } else if(false == controlInvalid(gui.Controls, controlConstraints.AttachSizeToControl.y)) { if(controlConstraints.DockToControl.Top  != -1) {} else { scaledPosition.y = 0;} scaledSize.y = gui.Controls.Area[controlConstraints.AttachSizeToControl.y].Total.Global.Size.y; }

	controlMetrics.Client.Local	= {{(s1_t)ncSizesScaled.Left, (s1_t)ncSizesScaled.Top}, (scaledSize - ncTotalSizeScaled).s1()};
	n2f3_t				minSizeScaled			= (controlConstraints.SizeMinMax.Min.f3().InPlaceScale(scale.x, scale.y));
	n2f3_t				maxSizeScaled			= (controlConstraints.SizeMinMax.Max.f3().InPlaceScale(scale.x, scale.y));
	scaledSize.InPlaceClamp(minSizeScaled, maxSizeScaled);
	realignRectangle(targetSize.u2(), rect2s1_t{scaledPosition.s1(), scaledSize.s1()}, controlMetrics.Total.Local, controlPlacement.Align);
	controlMetrics.Total .Global	= controlMetrics.Total	.Local;
	controlMetrics.Client.Global	= controlMetrics.Client	.Local;
	controlMetrics.Client.Global.Offset	+= controlMetrics.Total	.Local.Offset;
	if(isValidParent) {
		SControlArea		& parentMetrics			= gui.Controls.Area[controlState.Parent];
		controlMetrics.Client.Global.Offset	+= parentMetrics.Client.Global.Offset;
		controlMetrics.Total .Global.Offset	+= parentMetrics.Client.Global.Offset;
	}

	cnst rectcid		& dockToControl			= controlConstraints.DockToControl;
	if(dockToControl.Right != -1) {
		cnst cid_t idDock = ::controlDockTarget(gui, dockToControl.Right, GUI_CONTROL_LIMIT_Right);
		if(idDock != CID_INVALID) {
		if_fail_fef(gpk::controlInvalid(gui.Controls, idDock), "Invalid control id: %i.", idDock);
		controlUpdateMetricsTopToDown(gui, idDock, targetSize.u1(), true);
		cnst SControlPlacement	& other				= gui.Controls.Placement[idDock];
		cnst SControlArea		& otherMetrics		= gui.Controls.Area	[idDock];
		if(bit_true(other.Align, ALIGN_RIGHT) && bit_false(other.Align, ALIGN_HCENTER)) {
			controlMetrics.Total .Global.Offset.x	= otherMetrics.Total.Global.Offset.x - controlMetrics.Total.Global.Size.x;
			controlMetrics.Client.Global.Offset.x	= (s1_t)(controlMetrics.Total.Global.Offset.x + ncSizesScaled.Left);
			s1_t						diffToSubstract		= targetSize.x - otherMetrics.Total.Local.Offset.x;
			if(controlConstraints.AttachSizeToControl.x == iControl) {
				controlMetrics.Total .Global.Size.x		-= diffToSubstract;
				controlMetrics.Client.Global.Size.x		-= diffToSubstract;
				controlMetrics.Total .Global.Offset.x	+= diffToSubstract;
				controlMetrics.Client.Global.Offset.x	+= diffToSubstract;
			}
		}
		else {
			controlMetrics.Total .Global.Offset.x	= otherMetrics.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x;
			controlMetrics.Client.Global.Offset.x	= (s1_t)(controlMetrics.Total.Global.Offset.x + ncSizesScaled.Left);
		}
		}
	}
	if(dockToControl.Bottom != -1) {
		cnst cid_t idDock = ::controlDockTarget(gui, dockToControl.Bottom, GUI_CONTROL_LIMIT_Bottom);
		if(idDock != CID_INVALID) {
		if_fail_fef(gpk::controlInvalid(gui.Controls, idDock), "Invalid control id: %i.", idDock);
		cnst SControlPlacement	& other					= gui.Controls.Placement[idDock];
		cnst SControlArea		& otherMetrics			= gui.Controls.Area[idDock];
		if(bit_true(other.Align, ALIGN_BOTTOM	) && bit_false(other.Align, ALIGN_VCENTER)) {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (s1_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		} else {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (s1_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		}
		}
	}
	if(dockToControl.Left != -1) {
		cnst cid_t idDock = ::controlDockTarget(gui, dockToControl.Left, GUI_CONTROL_LIMIT_Left);
		if(idDock != CID_INVALID) {
		if_fail_fef(gpk::controlInvalid(gui.Controls, idDock), "Invalid control id: %i.", idDock);
		controlUpdateMetricsTopToDown(gui, idDock, targetSize, true);
		cnst SControlPlacement	& other			= gui.Controls.Placement[idDock];
		cnst SControlArea		& otherMetrics	= gui.Controls.Area	[idDock];
		if(bit_true(other.Align, ALIGN_RIGHT) && bit_false(other.Align, ALIGN_HCENTER)) {
			controlMetrics.Total	.Global.Offset.x	= otherMetrics				.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x - controlMetrics.Total.Global.Size.x;
			controlMetrics.Client	.Global.Offset.x	= s1_t(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
		else {
			controlMetrics.Total	.Global.Offset.x	= otherMetrics				.Total.Global.Offset.x;
			controlMetrics.Client	.Global.Offset.x	= s1_t(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
		}
	}
	if(dockToControl.Top	!= -1) {
		cnst cid_t idDock = ::controlDockTarget(gui, dockToControl.Top, GUI_CONTROL_LIMIT_Top);
		if(idDock != CID_INVALID) {
		if_fail_fef(gpk::controlInvalid(gui.Controls, idDock), "Invalid control id: %i.", idDock);
		cnst SControlPlacement	& other					= gui.Controls.Placement[idDock];
		cnst SControlArea		& otherMetrics			= gui.Controls.Area	[idDock];
		if(bit_true(other.Align, ALIGN_BOTTOM	) && bit_false(other.Align, ALIGN_VCENTER)) {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= controlMetrics.Total.Global.Offset.y + ncSizes.Top ;
		}
		else {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y + controlMetrics.Total.Global.Offset.y;
			controlMetrics.Client.Global.Offset.y	= controlMetrics.Total.Global.Offset.y + ncSizes.Top ;
		}
		}
	}
	{ // calculate text rectangle
		cnst rect2s1_t		& targetRect			= controlMetrics.Client.Global;
		controlMetrics.Text.Offset	= {};
		controlMetrics.Text.Size	= rectText.Size.s1();
		realignRectangle(targetRect.Size.u2(), controlMetrics.Text, controlMetrics.Text, controlText.Align);
		controlMetrics.Text.Offset	+= controlMetrics.Client.Global.Offset.s1();
	}
	::buildControlGeometry(controlPlacement, controlMetrics, gui.Zoom, controlMetrics.Rectangles, controlMetrics.Triangles);
	gui.Controls.SetUpdated(iControl, true);
	
	return 0;
}

gpk::err_t 		gpk::controlUpdateMetricsTopToDown							(SGUI & gui, cid_t iControl, cnst n2u1_t & targetSize, b8_t forceUpdate)				{
	if(false == gui.Controls.IsUpdated(iControl) || forceUpdate)
		if_fail_fef(::controlUpdateMetrics(gui, iControl, targetSize), "Failed to update metrics for control %i", iControl);

	vcid					& children												= gui.Controls.Children[iControl];
	for(u2_t iChild = 0; iChild < children.size(); ++iChild)
		if_fail_fef(gpk::controlUpdateMetricsTopToDown(gui, children[iChild], targetSize, forceUpdate), "Failed to update metrics for control %i", children[iChild]);
	return 0;
}

gpk::err_t 		gpk::guiUpdateMetrics									(SGUI & gui, cnst n2u1_t & targetSize, b8_t forceUpdate)								{
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
		if(controlInvalid(gui.Controls, gui.Controls.States[iControl].Parent) && false == controlInvalid(gui.Controls, (cid_t)iControl))
			if_fail_fef(gpk::controlUpdateMetricsTopToDown(gui, (cid_t)iControl, targetSize, forceUpdate), "Failed to update metrics for control %i", iControl);
	return 0;
}

sttc	cid_t	updateGUIControlHovered	(SGUI & gui, cid_t iControl, SControlState & controlState, cnst SInput & inputSystem, b8_t disabled)	nxpt	{
	if(controlState.IsHovered()) {
		if(inputSystem.ButtonDown(0) && false == controlState.IsPressed())
			gui.Controls.SetPressed(iControl, true);
		else if(inputSystem.ButtonUp(0)) {
			gui.Controls.SetPressed(iControl, false);
			gui.Controls.SetDragging(iControl, false);
		}
	}
	else {
		if(false == disabled) {
			gui.Controls.SetHovered(iControl, true);//controlFlags.Disabled;
		}
	}
	return controlState.IsHovered() ? iControl : -1;
}

sttc	cid_t	controlProcessInput		(SGUI & gui, cnst SInput & input, cid_t iControl)														{
	//--------------------
	SControlState		& controlState			= gui.Controls.States[iControl];
	cid_t				controlHovered			= -1;
	cid_t				controlBranch			= -1;
	if(controlState.IsPressed() && input.MouseCurrent.ButtonState[0]) {
		if(input.MouseCurrent.Position != input.MousePrevious.Position)
			gui.Controls.SetDragging(iControl, true);
		return iControl;
	}
	{
		vcid					& children				= gui.Controls.Children[iControl];
		for(u2_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) {
			if(controlHidden(gui.Controls, children[iChild]))
				continue;
			cid_t				controlPressed			= ::controlProcessInput(gui, input, children[iChild]);
			if(gui.Controls.States.size() > (u2_t)controlPressed) {
				//controlState.Hover		= false;
				gui.Controls.SetPressed(iControl, false);
				controlHovered			= controlPressed;
				controlBranch			= children[iChild];
			}
		}
	}
	if(input.ButtonDown(0) && controlInvalid(gui.Controls, controlState.Parent) && false == controlInvalid(gui.Controls, controlBranch))
		if_fail_e(controlToFront(gui.Controls, controlBranch));
	if(controlHovered == -1 && in_range(gui.CursorPos.s1(), gui.Controls.Area[iControl].Total.Global)) {
		if(controlState.IsPressed() && input.MouseCurrent.ButtonState[0] && input.MouseCurrent.Position != input.MousePrevious.Position)
			gui.Controls.SetDragging(iControl, true);
		controlHovered			= ::updateGUIControlHovered(gui, iControl, controlState, input, controlDisabled(gui.Controls, iControl) || controlHidden(gui.Controls, iControl));
	}
	else {
		gui.Controls.SetHovered(iControl, false);

		if(input.ButtonUp(0)) {
			gui.Controls.SetPressed(iControl, false);
			gui.Controls.SetDragging(iControl, false);
		}
	}
	return controlHovered;
}

sttc ::gpk::b8_t guiEventMatches(cnst ::gpk::SEventSystem & pattern, cnst ::gpk::SEventSystem & event) {
	if(pattern.Type != event.Type)
		rtrn false;
	if(0 == pattern.Data.size())
		rtrn true;
	::gpk::vcu0_t patternInput = pattern.Data, eventInput = event.Data;
	::gpk::SEView<::gpk::u0_t> patternChild = {}, eventChild = {};
	if(0 > patternChild.Load(patternInput) || 0 > eventChild.Load(eventInput) || patternChild.Type != eventChild.Type)
		rtrn false;
	if(::gpk::SYSTEM_EVENT_GUI != event.Type || ::gpk::EVENT_GUI_CONTROL_StateChange != eventChild.Type)
		rtrn true;
	::gpk::SChangeControlState patternChange = {}, eventChange = {};
	::gpk::vcu0_t patternData = patternChild.Data, eventData = eventChild.Data;
	if(0 > ::gpk::loadPOD(patternData, patternChange) || 0 > ::gpk::loadPOD(eventData, eventChange))
		rtrn false;
	if(patternChange.Id != ::gpk::CID_INVALID && patternChange.Id != eventChange.Id)
		rtrn false;
	if(patternChange.Set && 0 == (patternChange.Set & eventChange.Set))
		rtrn false;
	if(patternChange.Clear && 0 == (patternChange.Clear & eventChange.Clear))
		rtrn false;
	rtrn true;
}

sttc ::gpk::err_t guiControlEventScriptRun(::gpk::SGUI & gui, ::gpk::cid_t iControl, cnst ::gpk::SEventSystem & event) {
	cnst ::gpk::vcst_t script = gui.Controls.Script[iControl];
	if(0 == script.size())
		rtrn 0;
	::gpk::SJSONReader table = {};
	if(0 > ::gpk::jsonParse(table, script) || ::gpk::JSON_TYPE_OBJECT != table.Token[0].Type)
		rtrn 0;
	::gpk::aobj<::gpk::vcst_t> keys = {};
	if_fail_fe(::gpk::jsonObjectKeyList(table, 0, keys));
	::gpk::b8_t isEventTable = false;
	::gpk::err_t executed = 0;
	for(::gpk::u2_t iKey = 0; iKey < keys.size(); ++iKey) {
		::gpk::SEventSystem pattern = {};
		cnst ::gpk::err_t recognized = ::guiScriptEventPattern(keys[iKey], iControl, pattern);
		if_fail_fe(recognized);
		if(0 == recognized)
			continue;
		isEventTable = true;
		if(false == ::guiEventMatches(pattern, event))
			continue;
		cnst ::gpk::err_t iSource = ::gpk::jsonObjectValueGet(table, 0, keys[iKey]);
		if_fail_fe(iSource);
		::gpk::string source = {};
		if(::gpk::JSON_TYPE_STRING == table.Token[iSource].Type) {
			::gpk::vcst_t sourceView = {};
			if_fail_fe(::gpk::jsonObjectGetString(table, (::gpk::u2_t)iSource, sourceView));
			source = sourceView;
		}
		else
			if_fail_fe(::gpk::jsonWrite(table.Tree[iSource], table.View, source));
		cnst ::gpk::SEventSystem * runtimeEvent = ::gpk::SYSTEM_EVENT_GUI == event.Type ? 0 : &event;
		cnst ::gpk::SEventSystem * guiEvent = ::gpk::SYSTEM_EVENT_GUI == event.Type ? &event : 0;
		if_fail_fef(::guiScriptRunSource(gui, iControl, source, runtimeEvent, guiEvent), "iControl:%i, Event:%.*s, Script:%.*s", iControl, GPK_VIEW_ARGS(keys[iKey]), GPK_VIEW_ARGS(source));
		++executed;
	}
	if(isEventTable)
		rtrn executed;
	::gpk::SEventSystem actionPattern = {};
	if_fail_fe(::guiScriptEventPattern("Action", iControl, actionPattern));
	if(false == ::guiEventMatches(actionPattern, event))
		rtrn 0;
	if_fail_fef(::guiScriptRunSource(gui, iControl, script, {}, &event), "iControl:%i, Script:%.*s", iControl, GPK_VIEW_ARGS(script));
	rtrn 1;
}

sttc ::gpk::err_t guiEventScriptsRun(::gpk::SGUI & gui, cnst ::gpk::SEventSystem & event) {
	if(::gpk::SYSTEM_EVENT_GUI == event.Type) {
		::gpk::vcu0_t eventInput = event.Data;
		::gpk::SEView<::gpk::u0_t> eventChild = {};
		if(0 <= eventChild.Load(eventInput) && ::gpk::EVENT_GUI_CONTROL_StateChange == eventChild.Type) {
			::gpk::SChangeControlState change = {};
			::gpk::vcu0_t eventData = eventChild.Data;
			if(0 <= ::gpk::loadPOD(eventData, change) && false == ::gpk::controlInvalid(gui, change.Id))
				rtrn ::guiControlEventScriptRun(gui, change.Id, event);
		}
	}
	::gpk::err_t executed = 0;
	for(::gpk::u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		if(::gpk::controlInvalid(gui, (::gpk::cid_t)iControl))
			continue;
		cnst ::gpk::err_t result = ::guiControlEventScriptRun(gui, (::gpk::cid_t)iControl, event);
		if_fail_fe(result);
		executed += result;
	}
	rtrn executed;
}

gpk::err_t 		gpk::guiProcessInput	(SGUI & gui, cnst SInput & input, vpobj<SEventSystem> sysEvents)	{
	if(0 == gui.LastSize.LengthSquared()) 
		return 0;

	gui.Controls.EventQueue.clear();
	// shouldn't this be somewhere else? gui.Controls.StatesPrevious = gui.Controls.States;
	if_fail_e(guiUpdateMetrics(gui, gui.LastSize, false));
	cid_t				controlHovered			= -1;
	acid					rootControlsToProcess	= {};
	rootControlsToProcess.reserve(1000);
	for(u2_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {	// Only process root parents
		cnst SControlState	& controlState			= gui.Controls.States[iControl];
		if(controlState.IsUnused())
			continue;

		gui.Controls.SetAction((cid_t)iControl, false);

		if(controlState.IsDisabled())
			continue;

		if(controlHidden(gui.Controls, (cid_t)iControl))
			continue;

		if(false == controlInvalid(gui.Controls, controlState.Parent))
			continue;

		rootControlsToProcess.push_back((cid_t)iControl);
	}

	gui.CursorPos			= {(f2_t)input.MouseCurrent.Position.x, (f2_t)input.MouseCurrent.Position.y}; //+= {(f2_t)input.MouseCurrent.Deltas.x, (f2_t)input.MouseCurrent.Deltas.y};
	for(u2_t iControl = 0, countControls = rootControlsToProcess.size(); iControl < countControls; ++iControl) {
		if(controlHidden(gui.Controls, rootControlsToProcess[iControl]))
			continue;

		cid_t				controlPressed		= ::controlProcessInput(gui, input, rootControlsToProcess[iControl]);
		if(gui.Controls.States.size() > (u2_t)controlPressed)
			controlHovered			= controlPressed;
	}
	for(u2_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {
		if(input.ButtonDown(0))
			gui.Controls.SetFocused((cid_t)iControl, (cid_t)iControl == controlHovered);
		if(iControl != (u2_t)controlHovered) {
			gui.Controls.SetHovered((cid_t)iControl, false);;
			if(0 == input.MouseCurrent.ButtonState[0]) {
				gui.Controls.SetPressed((cid_t)iControl, false);
				gui.Controls.SetDragging((cid_t)iControl, false);
			}
		}
		SControlState & controlState = gui.Controls.States[iControl];
		if(controlState.IsDragging() && controlInvalid(gui.Controls, controlState.Parent)) {
			n2s1_t mouseDeltas = (input.MouseCurrent.Position - input.MousePrevious.Position).s1();
			mouseDeltas.InPlaceScale(1 / gui.Zoom.DPI.x, 1 / gui.Zoom.DPI.y);
			mouseDeltas.InPlaceScale(1 / gui.Zoom.ZoomLevel);
			if(mouseDeltas.x || mouseDeltas.y) {
				SControlPlacement & controlPlacement = gui.Controls.Placement[iControl];
				controlPlacement.Area.Offset.x += ((controlPlacement.Align & ALIGN_HCENTER) == ALIGN_RIGHT ) ? -mouseDeltas.x : mouseDeltas.x;
				controlPlacement.Area.Offset.y += ((controlPlacement.Align & ALIGN_VCENTER) == ALIGN_BOTTOM) ? -mouseDeltas.y : mouseDeltas.y;
				if_fail_fe(controlMetricsInvalidate(gui.Controls, (cid_t)iControl));
			}
		}
	}

	//for(u2_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {
	//	cnst GUI_CONTROL_FLAG	statePrevious		= gui.Controls.StatesPrevious[iControl].Flag;
	//	cnst GUI_CONTROL_FLAG	stateCurrent		= gui.Controls.States[iControl].Flag;
	//	if(statePrevious != stateCurrent)
	//		if_fail_fe(eventEnqueueGuiControlStateChange(gui.Controls.EventQueue, (cid_t)iControl, statePrevious, stateCurrent));
	//}
	for(u2_t iEvent = 0; iEvent < sysEvents.size(); ++iEvent)
		if(sysEvents[iEvent])
			if_fail_fef(::guiEventScriptsRun(gui, *sysEvents[iEvent]), "Runtime event:%u", iEvent);
	for(u2_t iEvent = 0; iEvent < gui.Controls.EventQueue.size(); ++iEvent)
		if(gui.Controls.EventQueue[iEvent])
			if_fail_fef(::guiEventScriptsRun(gui, *gui.Controls.EventQueue[iEvent]), "GUI event:%u", iEvent);
	return controlHovered;
}

gpk::cid_t			gpk::createScreenLayout(SGUI & gui, cid_t parent) {
	cid_t				controlId;
	if_fail_fe(controlId = controlCreate(gui));

	gui.Controls.Placement	[controlId].Border				= 
	gui.Controls.Placement	[controlId].Margin				= {};
	gui.Controls.Constraints[controlId].AttachSizeToControl	= {controlId, controlId};
	gui.Controls.Draw		[controlId].NoClient			= true;
	gui.Controls.States		[controlId].Mask				&= ~GUI_CONTROL_FLAG_Hovered;

	if(parent != CID_INVALID)
		if_fail_fe(gpk::controlSetParent(gui.Controls, controlId, parent));

	return controlId;
}

gpk::cid_t			gpk::guiCreateControlList	(SGUI & gui, DIRECTION direction, view<vcst_c> buttonText, cid_t iParent, cnst SControlPlacement & args, ALIGN textAlign, vcid out_ids) {
	cid_t					result						= -1;
	for(u1_t iButton = 0; iButton < buttonText.size(); ++iButton) {
		cid_t				idControl					= {};
		if_fail_fe(idControl = controlCreate(gui));
		if(out_ids.size() > iButton)
			out_ids[iButton]		= idControl;

		if(0 == iButton)
			result					= idControl;

		SControlPlacement	& control					= gui.Controls.Placement[idControl];
		control					= args;
		if(direction & (DIRECTION_LEFT | DIRECTION_RIGHT))
			control.Area.Offset.x	+= s1_t(args.Area.Size.x * iButton);
		else
			control.Area.Offset.y	+= s1_t(args.Area.Size.y * iButton);

		SControlText			& controlText				= gui.Controls.Text[idControl];
		controlText.Text		= buttonText[iButton];
		controlText.Align		= textAlign;

		SControlConstraints	& itemConstraints	= gui.Controls.Constraints[idControl];
		if(direction & (DIRECTION_UP | DIRECTION_DOWN))
			itemConstraints.AttachSizeToControl.x	= iParent;
		else if(direction & (DIRECTION_LEFT | DIRECTION_RIGHT)) {
			itemConstraints.AttachSizeToText.x		= true;
			itemConstraints.AttachSizeToControl.y	= iParent;
		}

		if(iButton) {
			if(direction & DIRECTION_LEFT)
				itemConstraints.DockToControl.Left	= out_ids[iButton - 1];
			else if(direction & DIRECTION_RIGHT)
				itemConstraints.DockToControl.Right	= out_ids[iButton - 1];
			else if(direction & DIRECTION_UP)
				itemConstraints.DockToControl.Top	= out_ids[iButton - 1];
			else if(direction & DIRECTION_DOWN)
				itemConstraints.DockToControl.Bottom	= out_ids[iButton - 1];

		}
		if_fail_fef(gpk::controlSetParent(gui.Controls, idControl, iParent), "idControl: %i, buttonText[iButton]: %.*s", idControl, GPK_VIEW_ARGS(buttonText[iButton])); // Make a tplt and macro for this pattern
	}
	return result;
}

gpk::cid_t			gpk::guiCreateControlList		(SGUI & gui, view<vcst_c> buttonText, cid_t iParent, n2u1_c & buttonSize, n2s1_c & offset, ALIGN controlAlign, ALIGN textAlign, vcid out_ids) {
	SControlPlacement		controlArgs					= {};
	controlArgs.Align	= controlAlign;
	controlArgs.Area	= {offset, buttonSize.s1()};
	controlArgs.Border	= {1, 1, 1, 1};
	controlArgs.Margin	= {1, 1, 1, 1};
	return guiCreateControlList(gui, {}, buttonText, iParent, controlArgs, textAlign, out_ids);
}
