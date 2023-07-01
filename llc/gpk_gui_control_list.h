#include "gpk_gui.h"

#ifndef GPK_GUI_CONTROL_LIST_H_23627
#define GPK_GUI_CONTROL_LIST_H_23627

namespace gpk
{
	::gpk::cid_t			guiCreateControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::view<const ::gpk::vcc> buttonText, ::gpk::cid_t iParent, const ::gpk::SControlPlacement & args, ::gpk::ALIGN textAlign, ::gpk::vcid out_ids);
	stainli	::gpk::cid_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::view<const ::gpk::vcc> buttonText, ::gpk::cid_t iParent, const ::gpk::SControlPlacement & args, ::gpk::ALIGN textAlign, ::gpk::acid & out_ids)									{ 
		const uint32_t offset = out_ids.size();
		gpk_necs(out_ids.resize(offset + buttonText.size()));
		return ::gpk::guiCreateControlList(gui, direction, buttonText, iParent, args, textAlign, vcid{out_ids.begin() + offset, buttonText.size()}); 
	}
	template<typename _tUIEnum>
	stainli	::gpk::cid_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::cid_t iParent, const ::gpk::SControlPlacement & args, ::gpk::ALIGN textAlign, ::gpk::acid & out_ids) {
		return ::gpk::guiCreateControlList(gui, direction, ::gpk::get_value_labels<_tUIEnum>(), iParent, args, textAlign, out_ids);
	}

	::gpk::cid_t			guiCreateControlList		(::gpk::SGUI & gui, ::gpk::view<const ::gpk::vcc> buttonText, ::gpk::cid_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vcid out_ids);
	template<typename _tUIEnum>
	stainli	::gpk::cid_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::cid_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vcid out_ids) {
		return ::gpk::guiCreateControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	}
	stainli	::gpk::cid_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::view<const ::gpk::vcc> buttonText, ::gpk::cid_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::acid & out_ids)	{ 
		const uint32_t offsetId = out_ids.size();
		out_ids.resize(offsetId + buttonText.size()); 
		return ::gpk::guiCreateControlList(gui, buttonText, iParent, buttonSize, offset, controlAlign, textAlign, vcid{out_ids.begin() + offsetId, buttonText.size()}); 
	}

	template<typename _tUIEnum>
	stainli	::gpk::cid_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::cid_t iParent, uint16_t buttonWidth, int16_t yOffset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::acid & out_ids) {
		return ::gpk::guiCreateControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, {0, yOffset}, controlAlign, textAlign, out_ids);
	}
	template<typename _tUIEnum>
	stainli	::gpk::cid_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::cid_t iParent, uint16_t buttonWidth, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::acid & out_ids) {
		return ::gpk::guiCreateControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, offset, controlAlign, textAlign, out_ids);
	}
	template<typename _tUIEnum>
	stainli	::gpk::cid_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::cid_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::acid & out_ids) {
		return ::gpk::guiCreateControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	} 

	::gpk::cid_t			createScreenLayout			(::gpk::SGUI & gui, ::gpk::cid_t parent = -1);

	template<typename _tEnum>
	::gpk::error_t			setupControlListWithRoot	(::gpk::SGUI & gui, ::gpk::acid & outIds, ::gpk::cid_t parent, const ::gpk::n2u16 & buttonSize, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign = ::gpk::ALIGN_CENTER)		{
		::gpk::cid_t				root						= outIds[outIds.push_back(::gpk::createScreenLayout(gui, parent))];
		::gpk::vcvcc				labels						= ::gpk::get_value_labels<_tEnum>();
		gpk_necs(labels.slice(labels, 1));

		return ::gpk::guiCreateControlList(gui, labels, root, buttonSize, {}, controlAlign, textAlign, outIds);
	}

	struct SControlList {
		cid_t					IdControl				= (cid_t)-1;
		::gpk::DIRECTION		Orientation				= ::gpk::DIRECTION_RIGHT;
		cid_t					IndexParentList			= (cid_t)-1;
		cid_t					IndexParentItem			= (cid_t)-1;
		::gpk::acid				IdControls				= {};
		cid_t					IdSelected				= (cid_t)-1;
		::gpk::acid				IdMultiselect			= {};
		::gpk::ai64				Events					= {};
	};

	::gpk::error_t			controlListInitialize	(::gpk::SGUI & gui, ::gpk::SControlList & menu);
	::gpk::error_t			controlListArrange		(::gpk::SGUI & gui, ::gpk::SControlList & menu);
	::gpk::error_t			controlListPush			(::gpk::SGUI & gui, ::gpk::SControlList & menu, const ::gpk::vcs & text, int64_t eventCode = -1);

	struct SPaletteGrid {
		cid_t					IdControl				= -1;
		::gpk::acid				IdControls				= {};
		::gpk::g8bgra			Colors					= {};
	};

	struct SEditBox {
		cid_t					IdControl				= -1;
		::gpk::acid				IdControls				= {};
		::gpk::g8bgra			Colors					= {};
	};


	::gpk::error_t			paletteGridInitialize	(::gpk::SGUI & gui, ::gpk::SPaletteGrid & palette);
	::gpk::error_t			paletteGridColorsSet	(::gpk::SGUI & gui, ::gpk::SPaletteGrid & palette, const ::gpk::g8bgra & colors);
}

#endif // GPK_GUI_CONTROL_LIST_H_23627
