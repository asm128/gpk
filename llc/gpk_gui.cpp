#include "gpk_gui.h"
#include "gpk_gui_theme.h"
#include "gpk_gui_text.h"
#include "gpk_base64.h"
#include "gpk_json_expression.h"
#include "gpk_label.h"
#include "gpk_rect_align.h"

usns gpk;

gpk::cid_t		gpk::controlCreateChild	(SGUI & gui, cid_t iParent)						{
	cid_t				idControl				= controlCreate(gui);
	controlSetParent(gui.Controls, idControl, iParent);
	return idControl;
}

gpk::cid_t		gpk::controlCreate		(SGUI & gui)							{
	if_fail_fe(guiSetupDefaultColors(gui.Colors));

	if(0 == gui.Fonts.size())
		if_fail_fe(guiSetupDefaultFonts(gui));

	if(gui.Fonts.size() <= gui.SelectedFont)
		gui.SelectedFont = u1_t(gui.Fonts.size() - 1);

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
		if(::gpk::controlInvalid(gui, idChild))
			continue;
		cnst ::gpk::vcst_t identifier = gui.Controls.Identifier[idChild];
		if(identifier.size()) {
			if_fail_fe(::gpk::appendOpenKey(output, identifier, separator));
			if_fail_fe(::guiControlJSONSave(output, gui, idChild));
			separator = true;
		}
		else
			if_fail_fe(::guiControlChildrenJSONSave(output, gui, idChild, separator));
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
	if_true_fe(iNode >= input.Token.size() || input.Token[iNode].Type != JSON_TYPE_OBJECT);
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		if(controlInvalid(gui, (cid_t)iControl) || 0 == gui.Controls.Identifier[iControl].size() || ::guiControlHasIdentifiedAncestor(gui, (cid_t)iControl))
			continue;
		cnst err_t iControlNode = jsonObjectValueGet(input, iNode, gui.Controls.Identifier[iControl]);
		if(0 <= iControlNode)
			if_fail_fe(::guiControlJSONLoad(gui, (cid_t)iControl, input, (u2_t)iControlNode));
	}
	rtrn 0;
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

gpk::err_t gpk::guiScriptRun(::gpk::SGUI & gui, ::gpk::cid_t iControl) {
	if_true_fef(controlInvalid(gui, iControl), "iControl:%i", iControl);
	cnst vcst_t identifier = gui.Controls.Identifier[iControl];
	cnst vcst_t script = gui.Controls.Script[iControl];
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
		if_fail_fef(guiJSONLoad(gui, source), "iControl:%i, Identifier:%.*s, Patch:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(script));
		rtrn 0;
	}
	string valuesJSON = {};
	if(scoped) {
		if_fail_fef(::guiControlTreeJSONSave(valuesJSON, gui, iRoot), "iControl:%i, Identifier:%.*s, iRoot:%i, RootIdentifier:%.*s", iControl, GPK_VIEW_ARGS(identifier), iRoot, GPK_VIEW_ARGS(rootIdentifier));
	}
	else {
		if_fail_fef(guiJSONSave(valuesJSON, gui), "iControl:%i, Identifier:%.*s", iControl, GPK_VIEW_ARGS(identifier));
	}
	SJSONReader values = {};
	if_fail_fef(jsonParse(values, valuesJSON), "iControl:%i, Identifier:%.*s, ValuesSize:%u", iControl, GPK_VIEW_ARGS(identifier), valuesJSON.size());
	string patchJSON = {};
	if_fail_fef(::guiJSONTemplateResolveNode(source, 0, values, 0, patchJSON), "iControl:%i, Identifier:%.*s, Script:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(script));
	SJSONReader patch = {};
	if_fail_fef(jsonParse(patch, patchJSON), "iControl:%i, Identifier:%.*s, Patch:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(patchJSON));
	if_fail_fef(guiJSONLoad(gui, patch), "iControl:%i, Identifier:%.*s, Patch:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(patchJSON));
	rtrn 0;
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

sttc	gpk::err_t controlUpdateMetrics	(SGUI & gui, cid_t iControl, cnst n2<u1_t> & _targetSize)					{
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
	scaledSize.x			= max(minSizeScaled.x, min(maxSizeScaled.x, scaledSize.x));
	scaledSize.y			= max(minSizeScaled.y, min(maxSizeScaled.y, scaledSize.y));
	realignRectangle(targetSize.u2(), rect2<s1_t>{scaledPosition.s1(), scaledSize.s1()}, controlMetrics.Total.Local, controlPlacement.Align);
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

gpk::err_t 		gpk::controlUpdateMetricsTopToDown							(SGUI & gui, cid_t iControl, cnst n2<u1_t> & targetSize, b8_t forceUpdate)				{
	if(false == gui.Controls.IsUpdated(iControl) || forceUpdate)
		if_fail_fef(::controlUpdateMetrics(gui, iControl, targetSize), "Failed to update metrics for control %i", iControl);

	vcid					& children												= gui.Controls.Children[iControl];
	for(u2_t iChild = 0; iChild < children.size(); ++iChild)
		if_fail_fef(gpk::controlUpdateMetricsTopToDown(gui, children[iChild], targetSize, forceUpdate), "Failed to update metrics for control %i", children[iChild]);
	return 0;
}

gpk::err_t 		gpk::guiUpdateMetrics									(SGUI & gui, cnst n2<u1_t> & targetSize, b8_t forceUpdate)								{
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

gpk::err_t 		gpk::guiProcessInput	(SGUI & gui, cnst SInput & input, vpobj<SEventSystem> sysEvents)	{
	if(0 == gui.LastSize.LengthSquared()) 
		return 0;

	gui.Controls.EventQueue.clear();
	// shouldn't this be somewhere else? gui.Controls.StatesPrevious = gui.Controls.States;
	if_fail_e(guiUpdateMetrics(gui, gui.LastSize, false));
	cid_t				controlHovered			= -1;
	acid					rootControlsToProcess	= {};
	rootControlsToProcess.reserve(1000);
	(void)sysEvents;
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
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		if(false == gui.Controls.States[iControl].IsAction())
			continue;
		cnst vcst_t identifier = gui.Controls.Identifier[iControl];
		cnst vcst_t script = gui.Controls.Script[iControl];
		if(script.size())
			if_fail_fef(guiScriptRun(gui, (cid_t)iControl), "iControl:%u, Identifier:%.*s, Script:%.*s", iControl, GPK_VIEW_ARGS(identifier), GPK_VIEW_ARGS(script));
	}
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
