#include "gpk_gui.h"

#ifndef GPK_GUI_CONTROL_LIST_H_23627
#define GPK_GUI_CONTROL_LIST_H_23627

namespace gpk
{
	cid_t			guiCreateControlList		(SGUI & gui, DIRECTION direction, view<vcst_c> buttonText, cid_t iParent, const SControlPlacement & args, ALIGN textAlign, vcid out_ids);
	stin	cid_t	guiCreateControlList		(SGUI & gui, DIRECTION direction, view<vcst_c> buttonText, cid_t iParent, const SControlPlacement & args, ALIGN textAlign, acid & out_ids)									{ 
		const u2_t offset = out_ids.size();
		if_fail_fe(out_ids.resize(offset + buttonText.size()));
		return guiCreateControlList(gui, direction, buttonText, iParent, args, textAlign, vcid{out_ids.begin() + offset, buttonText.size()}); 
	}
	tplt<tpnm _tUIEnum>
	stin	cid_t	guiCreateControlList		(SGUI & gui, DIRECTION direction, cid_t iParent, const SControlPlacement & args, ALIGN textAlign, acid & out_ids) {
		return guiCreateControlList(gui, direction, get_value_labels<_tUIEnum>(), iParent, args, textAlign, out_ids);
	}

	cid_t			guiCreateControlList		(SGUI & gui, view<vcst_c> buttonText, cid_t iParent, const n2u1_t & buttonSize, const n2s1_t & offset, ALIGN controlAlign, ALIGN textAlign, vcid out_ids);
	tplt<tpnm _tUIEnum>
	stin	cid_t	guiCreateControlList		(SGUI & gui, cid_t iParent, const n2u1_t & buttonSize, const n2s1_t & offset, ALIGN controlAlign, ALIGN textAlign, vcid out_ids) {
		return guiCreateControlList(gui, get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	}
	stin	cid_t	guiCreateControlList		(SGUI & gui, view<vcst_c> buttonText, cid_t iParent, const n2u1_t & buttonSize, const n2s1_t & offset, ALIGN controlAlign, ALIGN textAlign, acid & out_ids)	{ 
		const u2_t offsetId = out_ids.size();
		out_ids.resize(offsetId + buttonText.size()); 
		return guiCreateControlList(gui, buttonText, iParent, buttonSize, offset, controlAlign, textAlign, vcid{out_ids.begin() + offsetId, buttonText.size()}); 
	}

	tplt<tpnm _tUIEnum>
	stin	cid_t	guiCreateControlList		(SGUI & gui, cid_t iParent, u1_t buttonWidth, s1_t yOffset, ALIGN controlAlign, ALIGN textAlign, acid & out_ids) {
		return guiCreateControlList(gui, get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, {0, yOffset}, controlAlign, textAlign, out_ids);
	}
	tplt<tpnm _tUIEnum>
	stin	cid_t	guiCreateControlList		(SGUI & gui, cid_t iParent, u1_t buttonWidth, const n2s1_t & offset, ALIGN controlAlign, ALIGN textAlign, acid & out_ids) {
		return guiCreateControlList(gui, get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, offset, controlAlign, textAlign, out_ids);
	}
	tplt<tpnm _tUIEnum>
	stin	cid_t	guiCreateControlList		(SGUI & gui, cid_t iParent, const n2u1_t & buttonSize, const n2s1_t & offset, ALIGN controlAlign, ALIGN textAlign, acid & out_ids) {
		return guiCreateControlList(gui, get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	} 

	cid_t			createScreenLayout			(SGUI & gui, cid_t parent = -1);

	tplt<tpnm _tEnum>
	err_t			setupControlListWithRoot	(SGUI & gui, acid & outIds, cid_t parent, const n2u1_t & buttonSize, ALIGN controlAlign, ALIGN textAlign = ALIGN_CENTER)		{
		cid_t				root						= outIds[outIds.push_back(createScreenLayout(gui, parent))];
		view<view<sc_c>>			labels						= {get_value_labels<_tEnum>()};
		if_fail_fe(labels.slice(labels, 1));

		return guiCreateControlList(gui, labels, root, buttonSize, {}, controlAlign, textAlign, outIds);
	}

	struct SControlList {
		cid_t					IdControl				= (cid_t)-1;
		DIRECTION		Orientation				= DIRECTION_RIGHT;
		cid_t					IndexParentList			= (cid_t)-1;
		cid_t					IndexParentItem			= (cid_t)-1;
		acid				IdControls				= {};
		cid_t					IdSelected				= (cid_t)-1;
		acid				IdMultiselect			= {};
		as3_t			Events					= {};
	};

	err_t			controlListInitialize	(SGUI & gui, SControlList & menu);
	err_t			controlListArrange		(SGUI & gui, SControlList & menu);
	err_t			controlListPush			(SGUI & gui, SControlList & menu, vcst_c & text, s3_t eventCode = -1);

	struct SPaletteGrid {
		cid_t					IdControl				= -1;
		acid				IdControls				= {};
		g8bgra			Colors					= {};
	};

	struct SEditBox {
		cid_t					IdControl				= -1;
		acid				IdControls				= {};
		g8bgra			Colors					= {};
	};


	err_t			paletteGridInitialize	(SGUI & gui, SPaletteGrid & palette);
	err_t			paletteGridColorsSet	(SGUI & gui, SPaletteGrid & palette, const g8bgra & colors);
}

#endif // GPK_GUI_CONTROL_LIST_H_23627
