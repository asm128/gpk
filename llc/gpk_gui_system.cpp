#include "gpk_gui_system.h"
#include "gpk_gui_keyboard.h"

usns gpk;

gpk::cid_t gpk::guiCreateKeyboard(SGUI & gui, view<acid> system, u0_t rowWidth, vcu0_c & keys) {
	if_true_fe(system.size() <= GUI_TYPE_Keyboard);
	if_true_fe(0 == rowWidth);
	cid_t idRoot = controlCreate(gui);
	if_fail_fe(idRoot);
	cnst n2u0_t charSize = gui.Fonts[gui.SelectedFont]->CharSize;
	cnst n2s1_t sizeKey = (charSize + n2u0_t{6, 6}).s1();
	stxp u1_t SIZE_BUTTON = 88;
	cnst n2s1_t sizeKeypad = {s1_t(sizeKey.x * rowWidth + 4 + SIZE_BUTTON), s1_t(sizeKey.y * (keys.size() / rowWidth) + 4)};

	SControlPlacement & controlRoot = gui.Controls.Placement[idRoot];
	controlRoot.Area.Size = sizeKeypad;
	controlRoot.Align = ALIGN_CENTER_BOTTOM;
	for(u2_t iKey = 0; iKey < keys.size(); ++iKey) {
		cid_t idKey = controlCreate(gui);
		if_fail_fe(idKey);
		SControlPlacement & controlKey = gui.Controls.Placement[idKey];
		controlKey.Area.Size = sizeKey;
		controlKey.Area.Offset.x = s1_t((iKey % rowWidth) * sizeKey.x);
		controlKey.Area.Offset.y = s1_t((iKey / rowWidth) * sizeKey.y);
		controlKey.Border = {1, 1, 1, 1};
		gui.Controls.Text[idKey].Align = ALIGN_CENTER;
		gui.Controls.Text[idKey].Text = {(sc_c*)&keys[iKey], 1};
		if_fail_fe(controlSetParent(gui, idKey, idRoot));
	}
	acid idActions = {};
	cnst view<vcst_c> actionLabels = get_value_labels<VK_SCANCODE>();
	if_fail_fe(idActions.resize(actionLabels.size()));
	if_fail_fe(guiCreateControlList(gui, actionLabels, idRoot, {SIZE_BUTTON, 20}, {}, ALIGN_TOP_RIGHT, ALIGN_CENTER, idActions));
	if_fail_fe(system[GUI_TYPE_Keyboard].push_back(idRoot));
	return idRoot;
}

sttc gpk::n2u2_t guiControlGridMetrics(cnst gpk::SGUI & gui, gpk::cid_t idGrid, gpk::DIRECTION direction) {
	if(gpk::controlInvalid(gui, idGrid))
		rtrn {};
	cnst gpk::acid & lists = gui.Controls.Children[idGrid];
	if(0 == lists.size())
		rtrn {};
	u2_t listCapacity = 0;
	for(u2_t iList = 0; iList < lists.size(); ++iList)
		listCapacity = ::gpk::max(listCapacity, gui.Controls.Children[lists[iList]].size());
	b8_c horizontal = direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT;
	rtrn horizontal
		? n2u2_t{listCapacity, lists.size()}
		: n2u2_t{lists.size(), listCapacity}
		;
}

gpk::cid_t gpk::guiControlGridControl(cnst SGUI & gui, cid_t idGrid, DIRECTION direction, cnst n2u2_t & cell) {
	cnst n2u2_t metrics = ::guiControlGridMetrics(gui, idGrid, direction);
	if(cell.x >= metrics.x || cell.y >= metrics.y)
		rtrn CID_INVALID;
	cnst acid & lists = gui.Controls.Children[idGrid];
	b8_c horizontal = direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT;
	u2_c iList = horizontal ? cell.y : cell.x;
	u2_t iControl = horizontal ? cell.x : cell.y;
	if(direction == DIRECTION_LEFT)
		iControl = metrics.x - 1 - cell.x;
	else if(direction == DIRECTION_UP)
		iControl = metrics.y - 1 - cell.y;
	if(iList >= lists.size())
		rtrn CID_INVALID;
	cnst acid & controls = gui.Controls.Children[lists[iList]];
	if(iControl >= controls.size())
		rtrn CID_INVALID;
	rtrn controls[iControl];
}

gpk::cid_t gpk::guiControlGridControl(cnst SGUI & gui, cid_t idGrid, DIRECTION, u2_t index) {
	if(controlInvalid(gui, idGrid))
		rtrn CID_INVALID;
	cnst acid & lists = gui.Controls.Children[idGrid];
	for(u2_t iList = 0; iList < lists.size(); ++iList) {
		cnst acid & controls = gui.Controls.Children[lists[iList]];
		if(index < controls.size())
			rtrn controls[index];
		index -= controls.size();
	}
	rtrn CID_INVALID;
}

gpk::err_t gpk::guiControlGridArrange
	( SGUI & gui
	, cid_t idGrid
	, DIRECTION direction
	, n2s1_c & cellSize
	, n2s1_c & spacing
	, n2s1_c & offset
	, ALIGN align
	) {
	if_fail_fe(controlInvalid(gui, idGrid));
	cnst n2u2_t metrics = ::guiControlGridMetrics(gui, idGrid, direction);
	n2s1_c gridSize =
		{ (s1_t)(metrics.x * cellSize.x + (metrics.x ? metrics.x - 1 : 0) * spacing.x)
		, (s1_t)(metrics.y * cellSize.y + (metrics.y ? metrics.y - 1 : 0) * spacing.y)
		};
	SControlPlacement & gridPlacement = gui.Controls.Placement[idGrid];
	gridPlacement.Area = {offset, gridSize};
	gridPlacement.Align = align;
	cnst acid & lists = gui.Controls.Children[idGrid];
	b8_c horizontal = direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT;
	for(u2_t iList = 0; iList < lists.size(); ++iList) {
		cnst cid_t idList = lists[iList];
		SControlPlacement & listPlacement = gui.Controls.Placement[idList];
		listPlacement.Area = horizontal
			? rect2s1_t{{0, (s1_t)(iList * (cellSize.y + spacing.y))}, {gridSize.x, cellSize.y}}
			: rect2s1_t{{(s1_t)(iList * (cellSize.x + spacing.x)), 0}, {cellSize.x, gridSize.y}}
			;
		listPlacement.Align = ALIGN_TOP_LEFT;
		gui.Controls.Draw[idList].NoClient = true;
		gui.Controls.States[idList].Mask &= ~GUI_CONTROL_FLAG_Hovered;
		cnst acid & controls = gui.Controls.Children[idList];
		for(u2_t iControl = 0; iControl < controls.size(); ++iControl) {
			cnst cid_t idControl = controls[iControl];
			SControlPlacement & placement = gui.Controls.Placement[idControl];
			SControlConstraints & constraints = gui.Controls.Constraints[idControl];
			n2s1_t cell = horizontal ? n2s1_t{(s1_t)iControl, 0} : n2s1_t{0, (s1_t)iControl};
			if(direction == DIRECTION_LEFT)
				cell.x = (s1_t)(metrics.x - 1 - iControl);
			else if(direction == DIRECTION_UP)
				cell.y = (s1_t)(metrics.y - 1 - iControl);
			placement.Area =
				{ { (s1_t)(cell.x * (cellSize.x + spacing.x)), (s1_t)(cell.y * (cellSize.y + spacing.y)) }
				, cellSize
				};
			placement.Align = ALIGN_TOP_LEFT;
			constraints.AttachSizeToControl = {CID_INVALID, CID_INVALID};
			constraints.DockToControl = {CID_INVALID, CID_INVALID, CID_INVALID, CID_INVALID};
		}
	}
	rtrn controlMetricsInvalidate(gui, idGrid);
}

gpk::cid_t			gpk::guiControlListInitialize	(SGUI & gui, cid_t idParent) {
	cid_t idRoot = controlCreate(gui);
	if_fail_fe(idRoot);
	gui.Controls.Placement[idRoot].Border				= {};
	gui.Controls.Placement[idRoot].Margin				= {};
	gui.Controls.Text[idRoot].Text						= " ";
	gui.Controls.Constraints[idRoot].AttachSizeToText	= {false, false};
	if(idParent != CID_INVALID)
		if_fail_fe(controlSetParent(gui, idRoot, idParent));
	return idRoot;
}

gpk::err_t			gpk::guiControlListArrange		(SGUI & gui, cid_t idRoot, DIRECTION direction) {
	if_fail_fe(controlInvalid(gui, idRoot));
	cnst SRasterFont & selectedFont = *gui.Fonts[in_range(gui.Controls.Text[idRoot].FontSelected, (s1_t)0, (s1_t)gui.Fonts.size()) ? gui.Controls.Text[idRoot].FontSelected : gui.SelectedFont];
	cnst acid & controls = gui.Controls.Children[idRoot];
	cid_t idPreviousVisible = CID_INVALID;
	for(u2_t iItem = 0; iItem < controls.size(); ++iItem) {
		cnst cid_t idControl = controls[iItem];
		SControlConstraints & constraints = gui.Controls.Constraints[idControl];
		SControlPlacement & placement = gui.Controls.Placement[idControl];
		SControlText & text = gui.Controls.Text[idControl];
		constraints.DockToControl = {CID_INVALID, CID_INVALID, CID_INVALID, CID_INVALID};
		constraints.AttachSizeToControl = {CID_INVALID, CID_INVALID};
		if(gui.Controls.States[idControl].IsHidden())
			continue;
		cnst n2u0_t fontCharSize = selectedFont.CharSize;
		placement.Area.Size.y = fontCharSize.y + controlNCSpacing(placement).y;
		switch(direction) {
		default:
		case DIRECTION_LEFT:
		case DIRECTION_RIGHT:
			placement.Area.Size.x = s1_t(fontCharSize.x * text.Text.size() + controlNCSpacing(placement).x);
			placement.Area.Offset.x = 0;
			if(idPreviousVisible != CID_INVALID)
				constraints.DockToControl.Right = idPreviousVisible;
			break;
		case DIRECTION_UP:
		case DIRECTION_DOWN:
			constraints.AttachSizeToControl.x = idRoot;
			placement.Area.Offset.y = 0;
			break;
		}
		if(idPreviousVisible != CID_INVALID) {
			if(direction == DIRECTION_LEFT)
				constraints.DockToControl.Left = idPreviousVisible;
			else if(direction == DIRECTION_UP)
				constraints.DockToControl.Top = idPreviousVisible;
			else if(direction == DIRECTION_DOWN)
				constraints.DockToControl.Bottom = idPreviousVisible;
			else
				constraints.DockToControl.Right = idPreviousVisible;
		}
		idPreviousVisible = idControl;
	}

	SControlPlacement & rootPlacement = gui.Controls.Placement[idRoot];
	rootPlacement.Area.Size = {};
	cnst n2u0_t fontCharSize = selectedFont.CharSize;
	for(u2_t iItem = 0; iItem < controls.size(); ++iItem) {
		cnst cid_t idControl = controls[iItem];
		if(gui.Controls.States[idControl].IsHidden())
			continue;
		cnst SControlPlacement & placement = gui.Controls.Placement[idControl];
		cnst SControlText & text = gui.Controls.Text[idControl];
		switch(direction) {
		default:
		case DIRECTION_LEFT:
		case DIRECTION_RIGHT:
			rootPlacement.Area.Size.x += placement.Area.Size.x;
			rootPlacement.Area.Size.y = max(rootPlacement.Area.Size.y, placement.Area.Size.y);
			break;
		case DIRECTION_UP:
		case DIRECTION_DOWN:
			rootPlacement.Area.Size.y += placement.Area.Size.y;
			rootPlacement.Area.Size.x = max(rootPlacement.Area.Size.x, (s1_t)(text.Text.size() * fontCharSize.x + controlNCSpacing(placement).x));
			break;
		}
	}
	if(direction == DIRECTION_DOWN || direction == DIRECTION_UP)
		rootPlacement.Area.Size.x += 16;
	return 0;
}

gpk::cid_t			gpk::guiControlListPush			(SGUI & gui, cid_t idRoot, DIRECTION direction, vcst_c & text) {
	if_fail_fe(controlInvalid(gui, idRoot));
	SControlPlacement placement = {};
	placement.Margin = {4, 2, 4, 2};
	cid_t idControl = CID_INVALID;
	if_fail_fe(guiCreateControlList(gui, direction, view<vcst_c>{&text, 1}, idRoot, placement, ALIGN_CENTER_LEFT, vcid{&idControl, 1}));
	gui.Controls.Constraints[idControl].AttachSizeToText = {};
	if_fail_fe(guiControlListArrange(gui, idRoot, direction));
	return idControl;
}

gpk::cid_t			gpk::guiCreateDropdown			(SGUI & gui, view<acid> system, view<vcst_c> buttonText, cid_t idAnchor, cnst SControlPlacement & args, ALIGN textAlign) {
	if_true_fe(system.size() <= GUI_TYPE_Dropdown);
	if_fail_fe(controlInvalid(gui, idAnchor));
	cnst cid_t			idParent					= controlParent(gui, idAnchor);
	if_fail_fe(controlInvalid(gui, idParent));
	cnst acid			& siblings					= gui.Controls.Children[idParent];
	for(u2_t iSibling = 0; iSibling < siblings.size(); ++iSibling) {
		cnst cid_t			idSibling					= siblings[iSibling];
		cnst SControlConstraints & constraints				= gui.Controls.Constraints[idSibling];
		if(constraints.DockToControl.Left == idAnchor && constraints.DockToControl.Bottom == idAnchor) {
			if(0 > system[GUI_TYPE_Dropdown].find(idSibling))
				if_fail_fe(system[GUI_TYPE_Dropdown].push_back(idSibling));
			return idSibling;
		}
	}

	cnst cid_t			idDropdown					= controlCreateChild(gui, idParent);
	SControlPlacement	& placement					= gui.Controls.Placement[idDropdown];
	placement.Border								= {};
	placement.Margin								= {};
	placement.Area.Size							= {args.Area.Size.x, (s1_t)(args.Area.Size.y * buttonText.size())};
	SControlConstraints	& constraints				= gui.Controls.Constraints[idDropdown];
	constraints.DockToControl.Left					= idAnchor;
	constraints.DockToControl.Bottom					= idAnchor;
	gui.Controls.Draw[idDropdown].NoClient			= true;
	gui.Controls.States[idDropdown].Mask				&= ~GUI_CONTROL_FLAG_Hovered;
	gui.Controls.States[idDropdown].Mask				&= ~GUI_CONTROL_FLAG_Action;
	acid				idOptions					= {};
	if_fail_fe(guiCreateControlList(gui, DIRECTION_DOWN, buttonText, idDropdown, args, textAlign, idOptions));
	gui.Controls.SetHidden(idDropdown, true);
	if_fail_fe(system[GUI_TYPE_Dropdown].push_back(idDropdown));
	return idDropdown;
}

gpk::err_t			gpk::dropdownExpand				(SGUI & gui, cid_t idDropdown) {
	if_fail_fe(controlInvalid(gui, idDropdown));
	cnst cid_t			idParent					= controlParent(gui, idDropdown);
	cnst cid_t			idAnchor					= gui.Controls.Constraints[idDropdown].DockToControl.Bottom;
	if_fail_fe(controlInvalid(gui, idParent));
	if_fail_fe(controlInvalid(gui, idAnchor));
	if_fail_fe(controlSetParent(gui, idDropdown, idAnchor));
	if_fail_fe(controlSetParent(gui, idDropdown, idParent));
	gui.Controls.SetHidden(idDropdown, false);
	return controlMetricsInvalidate(gui, idDropdown);
}

gpk::err_t			gpk::dropdownCollapse			(SGUI & gui, cid_t idDropdown) {
	if_fail_fe(controlInvalid(gui, idDropdown));
	gui.Controls.SetHidden(idDropdown, true);
	return 0;
}
