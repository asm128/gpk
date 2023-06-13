#include "gpk_gui.h"

#ifndef GPK_GUI_CONTROL_LIST_H_29037498023613
#define GPK_GUI_CONTROL_LIST_H_29037498023613

namespace gpk
{
	::gpk::error_t			guiCreateControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::SControlPlacement & args, ::gpk::ALIGN textAlign, ::gpk::vi32 out_ids);
	stainli	::gpk::error_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::SControlPlacement & args, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids)									{ 
		const uint32_t offset = out_ids.size();
		gpk_necs(out_ids.resize(offset + buttonText.size()));
		return ::gpk::guiCreateControlList(gui, direction, buttonText, iParent, args, textAlign, vi32{out_ids.begin() + offset, buttonText.size()}); 
	}
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, int32_t iParent, const ::gpk::SControlPlacement & args, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids) {
		return ::gpk::guiCreateControlList(gui, direction, ::gpk::get_value_labels<_tUIEnum>(), iParent, args, textAlign, out_ids);
	}

	::gpk::error_t			guiCreateControlList		(::gpk::SGUI & gui, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vi32 out_ids);
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiCreateControlList		(::gpk::SGUI & gui, int32_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vi32 out_ids) {
		return ::gpk::guiCreateControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	}
	stainli	::gpk::error_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids)	{ 
		const uint32_t offsetId = out_ids.size();
		out_ids.resize(offsetId + buttonText.size()); 
		return ::gpk::guiCreateControlList(gui, buttonText, iParent, buttonSize, offset, controlAlign, textAlign, vi32{out_ids.begin() + offsetId, buttonText.size()}); 
	}

	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiCreateControlList		(::gpk::SGUI & gui, int32_t iParent, uint16_t buttonWidth, int16_t yOffset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids) {
		return ::gpk::guiCreateControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, {0, yOffset}, controlAlign, textAlign, out_ids);
	}
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiCreateControlList		(::gpk::SGUI & gui, int32_t iParent, uint16_t buttonWidth, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids) {
		return ::gpk::guiCreateControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, offset, controlAlign, textAlign, out_ids);
	}
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiCreateControlList		(::gpk::SGUI & gui, int32_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids) {
		return ::gpk::guiCreateControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	}

	enum CONTROL_LIST_DIRECTION : int32_t
		{	CONTROL_LIST_DIRECTION_HORIZONTAL	= 1
		,	CONTROL_LIST_DIRECTION_VERTICAL		= 2
		};

	struct SControlList {
		int32_t							IdControl									= -1;
		::gpk::CONTROL_LIST_DIRECTION	Orientation									= ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL;
		int32_t							IndexParentList								= -1;
		int32_t							IndexParentItem								= -1;
		::gpk::ai32						IdControls									= {};
		int32_t							IdSelected									= -1;
		::gpk::ai32						IdMultiselect								= {};
		::gpk::ai64						Events										= {};
	};

	::gpk::error_t					controlListInitialize						(::gpk::SGUI & gui, ::gpk::SControlList& menu);
	::gpk::error_t					controlListArrange							(::gpk::SGUI & gui, ::gpk::SControlList& menu);
	::gpk::error_t					controlListPush								(::gpk::SGUI & gui, ::gpk::SControlList& menu, const ::gpk::vcs& text, int64_t eventCode = -1);

	struct SPaletteGrid {
		int32_t					IdControl				= -1;
		::gpk::ai32				IdControls				= {};
		::gpk::v2<::gpk::bgra>	Colors					= {};
	};

	struct SEditBox {
		int32_t					IdControl				= -1;
		::gpk::ai32				IdControls				= {};
		::gpk::v2<::gpk::bgra>	Colors					= {};
	};


	::gpk::error_t			paletteGridInitialize	(::gpk::SGUI & gui, ::gpk::SPaletteGrid & palette);
	::gpk::error_t			paletteGridColorsSet	(::gpk::SGUI & gui, ::gpk::SPaletteGrid & palette, const ::gpk::v2bgra & colors);
}

#endif // GPK_GUI_CONTROL_LIST_H_29037498023613
