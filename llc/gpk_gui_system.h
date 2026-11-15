#include "gpk_gui.h"
#include "gpk_label.h"
#include "gpk_array_static.h"
#include "gpk_gui_keyboard.h"
#include "gpk_ctti_enum.h"

#ifndef GPK_GUI_SYSTEM_H_23627
#define GPK_GUI_SYSTEM_H_23627

nmsp gpk
{
	CTTI_ENUM(GUI_TYPE,
	enum GUI_TYPE : u1_t
		{ GUI_TYPE_Control_list = 0
		, GUI_TYPE_Control_grid
		, GUI_TYPE_Slider
		, GUI_TYPE_Dropdown
		, GUI_TYPE_Keyboard
		, GUI_TYPE_COUNT
		};
	);

	cid_t			guiControlListInitialize	(SGUI & gui, cid_t idParent = CID_INVALID);
	err_t			guiControlListArrange		(SGUI & gui, cid_t idRoot, DIRECTION direction);
	cid_t			guiControlListPush			(SGUI & gui, cid_t idRoot, DIRECTION direction, vcst_c & text);
	tplT cid_t		guiControlListCreate
		( SGUI & gui
		, DIRECTION direction
		, view<T> stringifiable
		, cid_t idParent = CID_INVALID
		) {
		cid_t idRoot = guiControlListInitialize(gui, idParent);
		if_fail_fe(idRoot);
		for(u2_t iControl = 0; iControl < stringifiable.size(); ++iControl) {
			cnst auto itemView = ::gpk::str(stringifiable[iControl]);
			if_fail_fe(guiControlListPush(gui, idRoot, direction, label(itemView.Storage)));
		}
		return idRoot;
	}
	stin	cid_t	guiControlListCreate		(SGUI & gui, DIRECTION direction, view<vcst_c> strings, cid_t idParent = CID_INVALID) {
		cid_t idRoot = guiControlListInitialize(gui, idParent);
		if_fail_fe(idRoot);
		for(u2_t iControl = 0; iControl < strings.size(); ++iControl)
			if_fail_fe(guiControlListPush(gui, idRoot, direction, strings[iControl]));
		return idRoot;
	}
	stin	cid_t	guiControlListCreate		(SGUI & gui, DIRECTION direction, view<cnst string> strings, cid_t idParent = CID_INVALID) {
		cid_t idRoot = guiControlListInitialize(gui, idParent);
		if_fail_fe(idRoot);
		for(u2_t iControl = 0; iControl < strings.size(); ++iControl)
			if_fail_fe(guiControlListPush(gui, idRoot, direction, ::gpk::str(view<sc_c>{(sc_c*)strings[iControl].begin(), strings[iControl].size()})));
		return idRoot;
	}

	cid_t			guiControlGridControl	(cnst SGUI & gui, cid_t idGrid, DIRECTION direction, cnst n2u2_t & cell);
	cid_t			guiControlGridControl	(cnst SGUI & gui, cid_t idGrid, DIRECTION direction, u2_t index);
	
	err_t			guiControlGridArrange
			( SGUI & gui
			, cid_t idGrid
			, DIRECTION direction
			, n2s1_c & cellSize
			, n2s1_c & spacing = {}
			, n2s1_c & offset = {}
			, ALIGN align = ALIGN_TOP_LEFT
			);
		tplT cid_t		guiControlGridCreate
			( SGUI & gui
			, view<T> stringifiable
			, cid_t parent
			, DIRECTION direction
			, u2_t columnCount
			, n2s1_c & cellSize
			, n2s1_c & spacing = {}
			, n2s1_c & offset = {}
			, ALIGN align = ALIGN_TOP_LEFT
			, ALIGN textAlign = ALIGN_CENTER
			) {
			if_true_fe(0 == columnCount);
			if_true_fe(0 >= cellSize.x || 0 >= cellSize.y);
			if_true_fe(0 > spacing.x || 0 > spacing.y);
			if_true_fe(direction != DIRECTION_RIGHT && direction != DIRECTION_LEFT && direction != DIRECTION_DOWN && direction != DIRECTION_UP);
			cid_t idGrid = controlCreate(gui);
			if_fail_fe(idGrid);
			SControlPlacement & placement = gui.Controls.Placement[idGrid];
			placement.Border = {};
			placement.Margin = {};
			gui.Controls.Draw[idGrid].NoClient = true;
			gui.Controls.States[idGrid].Mask &= ~GUI_CONTROL_FLAG_Hovered;
			if(parent != CID_INVALID)
				if_fail_fe(controlSetParent(gui, idGrid, parent));
			cnst n2u2_t metrics = {columnCount, (u2_t)((stringifiable.size() + columnCount - 1) / columnCount)};
			b8_c horizontal = direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT;
			u2_c listCount = horizontal ? metrics.y : metrics.x;
			u2_c listCapacity = horizontal ? metrics.x : metrics.y;
			for(u2_t iList = 0; iList < listCount; ++iList) {
				u2_c listBegin = iList * listCapacity;
				u2_c listSize = ::gpk::min(listCapacity, stringifiable.size() - listBegin);
				cnst cid_t idList = guiControlListCreate(gui, direction, view<cnst T>{stringifiable.begin() + listBegin, listSize}, idGrid);
				if_fail_fe(idList);
				cnst acid & controls = gui.Controls.Children[idList];
				for(u2_t iControl = 0; iControl < controls.size(); ++iControl)
					gui.Controls.Text[controls[iControl]].Align = textAlign;
			}
			if_fail_fe(guiControlGridArrange(gui, idGrid, direction, cellSize, spacing, offset, align));
			return idGrid;
		}
	cid_t			guiCreateDropdown
		( SGUI & gui
		, view<acid> system
		, view<vcst_c> buttonText
		, cid_t idAnchor
		, cnst SControlPlacement & args
		, ALIGN textAlign
		);
	err_t			dropdownExpand				(SGUI & gui, cid_t idDropdown);
	err_t			dropdownCollapse			(SGUI & gui, cid_t idDropdown);

	cid_t			guiCreateKeyboard
		( SGUI & gui
		, view<acid> system
		, u0_t rowWidth = GUI_KEYBOARD_Default_row_width
		, vcu0_c & keys = GUI_KEYBOARD_Default_keys
		);

}

#endif // GPK_GUI_SYSTEM_H_23627
