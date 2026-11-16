#include "gpk_gui.h"
#include "gpk_label.h"
#include "gpk_array_static.h"

#ifndef GPK_GUI_CONTROL_LIST_H_23627
#define GPK_GUI_CONTROL_LIST_H_23627

nmsp gpk
{

	struct SControlList {
		cid_t			IdControl				= (cid_t)-1;
		DIRECTION		Orientation				= DIRECTION_RIGHT;
		cid_t			IndexParentList			= (cid_t)-1;
		cid_t			IndexParentItem			= (cid_t)-1;
		acid			IdControls				= {};
		as3_t			Events					= {};
	};

	err_t			controlListInitialize	(SGUI & gui, SControlList & menu);
	err_t			controlListArrange		(SGUI & gui, SControlList & menu);
	err_t			controlListPush			(SGUI & gui, SControlList & menu, vcst_c text, s2_t eventCode = -1);
	tplT	err_t	controlListCreate		(SGUI & gui, SControlList & menu, view<T>  stringifiable, cid_t parent = CID_INVALID, DIRECTION orientation = DIRECTION_RIGHT) {
		if_fail_fe(controlListInitialize(gui, menu));
		menu.Orientation	= orientation;
		if(parent != CID_INVALID)
			if_fail_fe(controlSetParent(gui, menu.IdControl, parent));
		for(u2_t iControl = 0; iControl < stringifiable.size(); ++iControl) {
			cnst auto itemView = ::gpk::str(stringifiable[iControl]);
			if_fail_fe(controlListPush(gui, menu, label(itemView.Storage)));
		}
		return 0;
	}
	tplT	err_t	controlListCreate		(SGUI & gui, SControlList & menu, view<cnst view<T>> strings, cid_t parent = CID_INVALID, DIRECTION orientation = DIRECTION_RIGHT) {
		if_fail_fe(controlListInitialize(gui, menu));
		menu.Orientation	= orientation;
		if(parent != CID_INVALID)
			if_fail_fe(controlSetParent(gui, menu.IdControl, parent));
		for(u2_t iControl = 0; iControl < strings.size(); ++iControl)
			if_fail_fe(controlListPush(gui, menu, ::gpk::str(view<sc_c>((sc_c*)(strings[iControl].begin()), strings[iControl].size()))));
		return 0;
	}
	tplT	err_t	controlListCreate		(SGUI & gui, SControlList & menu, view<cnst apod<T>> strings, cid_t parent = CID_INVALID, DIRECTION orientation = DIRECTION_RIGHT) {
		if_fail_fe(controlListInitialize(gui, menu));
		menu.Orientation	= orientation;
		if(parent != CID_INVALID)
			if_fail_fe(controlSetParent(gui, menu.IdControl, parent));
		for(u2_t iControl = 0; iControl < strings.size(); ++iControl)
			if_fail_fe(controlListPush(gui, menu, ::gpk::str(view<sc_c>((sc_c*)(strings[iControl].begin()), strings[iControl].size())))); //::gpk::str(strings[iControl])));
		return 0;
	}
	stin	err_t	controlListCreate		(SGUI & gui, SControlList & menu, view<vcst_c> strings, cid_t parent = CID_INVALID, DIRECTION orientation = DIRECTION_RIGHT)		{ return controlListCreate(gui, menu, view<vcsc_c>{strings.begin(), strings.size()}, parent, orientation); }
	stin	err_t	controlListCreate		(SGUI & gui, SControlList & menu, view<vstr_c> strings, cid_t parent = CID_INVALID, DIRECTION orientation = DIRECTION_RIGHT)		{ return controlListCreate(gui, menu, view<vsc_c> {strings.begin(), strings.size()}, parent, orientation); }
	stin	err_t	controlListCreate		(SGUI & gui, SControlList & menu, view<cnst string> strings, cid_t parent = CID_INVALID, DIRECTION orientation = DIRECTION_RIGHT)	{ return controlListCreate(gui, menu, view<asc_c> {strings.begin(), strings.size()}, parent, orientation); }

	struct SPaletteGrid {
		cid_t			IdControl				= -1;
		acid			IdControls				= {};
		g8bgra			Colors					= {};
	};

	struct SEditBox {
		cid_t			IdControl				= -1;
		acid			IdControls				= {};
		g8bgra			Colors					= {};
	};


	err_t			paletteGridInitialize	(SGUI & gui, SPaletteGrid & palette);
	err_t			paletteGridColorsSet	(SGUI & gui, SPaletteGrid & palette, cnst g8bgra & colors);
}

#endif // GPK_GUI_CONTROL_LIST_H_23627
