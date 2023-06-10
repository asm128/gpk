#include "gpk_virtual_keyboard.h"
#include "gpk_gui.h"
#include "gpk_system_key.h"

#ifndef GPK_GUI_CONTROL_H_2903749802374
#define GPK_GUI_CONTROL_H_2903749802374

namespace gpk
{
	::gpk::error_t			virtualKeyboardSetup	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk, uint8_t rowWidth, const ::gpk::view<const uint16_t> & keys);
	::gpk::error_t			virtualKeyboardSetup437	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk);

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

#pragma pack(push, 1)
	::gpk::error_t					controlListInitialize						(::gpk::SGUI & gui, ::gpk::SControlList& menu);
	::gpk::error_t					controlListArrange							(::gpk::SGUI & gui, ::gpk::SControlList& menu);
	::gpk::error_t					controlListPush								(::gpk::SGUI & gui, ::gpk::SControlList& menu, const ::gpk::vcs& text, int64_t eventCode = -1);

	enum VIEWPORT_CONTROL : int8_t
		{ VIEWPORT_CONTROL_RESIZE_LEFT				= 0
		, VIEWPORT_CONTROL_RESIZE_TOP
		, VIEWPORT_CONTROL_RESIZE_RIGHT
		, VIEWPORT_CONTROL_RESIZE_BOTTOM
		, VIEWPORT_CONTROL_RESIZE_TOP_LEFT
		, VIEWPORT_CONTROL_RESIZE_TOP_RIGHT
		, VIEWPORT_CONTROL_RESIZE_BOTTOM_LEFT
		, VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT
		, VIEWPORT_CONTROL_TITLE
		, VIEWPORT_CONTROL_CLOSE
		, VIEWPORT_CONTROL_TARGET
		, VIEWPORT_CONTROL_COUNT
		};

	enum GUI_CONTROL_TYPE : int8_t
		{ GUI_CONTROL_TYPE_Viewport					= 0
		, GUI_CONTROL_TYPE_PaletteGrid
		, GUI_CONTROL_TYPE_ControlList
		, GUI_CONTROL_TYPE_COUNT
		};

	struct SViewport {
		int32_t					IdControl									= -1;
		::gpk::asti32<::gpk::VIEWPORT_CONTROL_COUNT>	IdControls									= {};
		int32_t					IdDesktopElement		= -1;
		GUI_CONTROL_TYPE		ControlType				= (GUI_CONTROL_TYPE)-1;
	};

	::gpk::error_t			viewportInitialize		(::gpk::SGUI & gui, ::gpk::SViewport& viewport);
#pragma pack(pop)

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


	::gpk::error_t			paletteGridInitialize	(::gpk::SGUI & gui, ::gpk::SPaletteGrid& palette);
	::gpk::error_t			paletteGridColorsSet	(::gpk::SGUI & gui, ::gpk::SPaletteGrid& palette, const ::gpk::v2bgra & colors);

	::gpk::error_t			guiSetupControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::SControl & args, ::gpk::ALIGN textAlign, ::gpk::vi32 out_ids);
	stainli	::gpk::error_t	guiSetupControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::SControl & args, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids)									{ 
		out_ids.resize(buttonText.size()); 
		return ::gpk::guiSetupControlList(gui, direction, buttonText, iParent, args, textAlign, vi32{out_ids}); 
	}
	template<typename _tUIEnum>
	static	::gpk::error_t	guiSetupControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, int32_t iParent, const ::gpk::SControl & args, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids) {
		return ::gpk::guiSetupControlList(gui, direction, ::gpk::get_value_labels<_tUIEnum>(), iParent, args, textAlign, out_ids);
	}

	stainli	::gpk::error_t	guiCreateControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::SControl & args, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids)									{ 
		out_ids.resize(out_ids.size() + buttonText.size()); 
		return ::gpk::guiSetupControlList(gui, direction, buttonText, iParent, args, textAlign, vi32{out_ids}); 
	}
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiCreateControlList	(::gpk::SGUI & gui, ::gpk::DIRECTION direction, int32_t iParent, const ::gpk::SControl & args, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids) {
		return ::gpk::guiSetupControlList(gui, direction, ::gpk::get_value_labels<_tUIEnum>(), iParent, args, textAlign, out_ids);
	}

	::gpk::error_t			guiSetupControlList		(::gpk::SGUI & gui, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vi32 out_ids);
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiSetupControlList		(::gpk::SGUI & gui, int32_t iParent, uint16_t buttonWidth, int16_t yOffset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vi32 out_ids) {
		return ::gpk::guiSetupControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, {0, yOffset}, controlAlign, textAlign, out_ids);
	}
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiSetupControlList		(::gpk::SGUI & gui, int32_t iParent, uint16_t buttonWidth, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vi32 out_ids) {
		return ::gpk::guiSetupControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, offset, controlAlign, textAlign, out_ids);
	}
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiSetupControlList		(::gpk::SGUI & gui, int32_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vi32 out_ids) {
		return ::gpk::guiSetupControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	}
	stainli	::gpk::error_t	guiSetupControlList		(::gpk::SGUI & gui, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids)	{ 
		out_ids.resize(buttonText.size()); 
		return ::gpk::guiSetupControlList(gui, buttonText, iParent, buttonSize, offset, controlAlign, textAlign, vi32{out_ids});
	}

	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiSetupControlList		(::gpk::SGUI & gui, int32_t iParent, uint16_t buttonWidth, int16_t yOffset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids) {
		return ::gpk::guiSetupControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, {0, yOffset}, controlAlign, textAlign, out_ids);
	}
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiSetupControlList		(::gpk::SGUI & gui, int32_t iParent, uint16_t buttonWidth, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids) {
		return ::gpk::guiSetupControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, offset, controlAlign, textAlign, out_ids);
	}
	template<typename _tUIEnum>
	stainli	::gpk::error_t	guiSetupControlList		(::gpk::SGUI & gui, int32_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::ai32 & out_ids) {
		return ::gpk::guiSetupControlList(gui, ::gpk::get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	}

	struct SUIInputBox {
		int32_t					IdRoot				= {};
		int32_t					IdText				= {};
		::gpk::SVirtualKeyboard	VirtualKeyboard		= {};
		::gpk::apod<char>		Text				= {};
		uint32_t				MaxLength			= 16;
		bool					Editing				= false;


		::gpk::error_t			SetText				(::gpk::SGUI & gui, ::gpk::vcs text) { 
			return ::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text = text});
		}
		::gpk::error_t			Edit				(::gpk::SGUI & gui, bool editing) { 
			Editing					= editing;
			gui.Controls.States[IdRoot].Hidden	= false == Editing;
			return one_if(Editing);
		}

		::gpk::error_t			Update				(::gpk::SGUI & gui, ::gpk::view<const ::gpk::SSysEvent> frameEvents, ::gpk::view<const uint32_t> processableControls) { 
			if(false == Editing)
				return 0;

			int32_t						handledControl		= 0;
			gpk_necs(::gpk::guiProcessControls(gui, processableControls, [&](int32_t iControl) {
				if(::gpk::virtualKeyboardHandleEvent(VirtualKeyboard, iControl))
					handledControl = iControl;
				return 0;
			}));

			::gpk::aobj<::gpk::SSysEvent>	sysEvents			= frameEvents;
			const ::gpk::SVirtualKeyboard	& vk				= VirtualKeyboard;
			for(uint32_t iEvent = 0; iEvent < VirtualKeyboard.Events.size(); ++iEvent) {
				switch(vk.Events[iEvent].Type) {
				case ::gpk::VK_EVENT_RELEASE:
					if(Text.size() < MaxLength) {
						Text.push_back((char)vk.Events[iEvent].ScanCode);
						::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text});
					}
					break;
				case ::gpk::VK_EVENT_EDIT:
						 if(vk.Events[iEvent].ScanCode == gpk::VK_SCANCODE_Backspace) { ::gpk::SSysEvent evt{::gpk::SYSEVENT_KEY_DOWN,}; evt.Data.push_back(gpk::SYSTEM_KEY_Backspace)	; sysEvents.push_back(evt); } 
					else if(vk.Events[iEvent].ScanCode == gpk::VK_SCANCODE_Enter	) { ::gpk::SSysEvent evt{::gpk::SYSEVENT_KEY_DOWN,}; evt.Data.push_back(gpk::SYSTEM_KEY_Enter	)	; sysEvents.push_back(evt); } 
					else if(vk.Events[iEvent].ScanCode == gpk::VK_SCANCODE_Escape	) { ::gpk::SSysEvent evt{::gpk::SYSEVENT_KEY_DOWN,}; evt.Data.push_back(gpk::SYSTEM_KEY_Escape	)	; sysEvents.push_back(evt); } 
					else if(vk.Events[iEvent].ScanCode == gpk::VK_SCANCODE_Clear	) { ::gpk::SSysEvent evt{::gpk::SYSEVENT_KEY_DOWN,}; evt.Data.push_back(gpk::SYSTEM_KEY_Clear	)	; sysEvents.push_back(evt); } 
				}
			}
			VirtualKeyboard.Events.clear();
			frameEvents				= sysEvents;
			bool						enterKeyPressed		= false;
			for(uint32_t iEvent = 0; iEvent < frameEvents.size(); ++iEvent) {
				switch(frameEvents[iEvent].Type) {
				default						: break;
				case ::gpk::SYSEVENT_CHAR	:
					if(Text.size() < MaxLength)
						if(frameEvents[iEvent].Data[0] >= 0x20 && frameEvents[iEvent].Data[0] <= 0x7F) {
							Text.push_back(frameEvents[iEvent].Data[0]);
							::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text});
						}
					break;
				case ::gpk::SYSEVENT_KEY_DOWN:
					switch(frameEvents[iEvent].Data[0]) {
					case ::gpk::SYSTEM_KEY_Escape		: Edit(gui, false);			break;
					case ::gpk::SYSTEM_KEY_Enter		: enterKeyPressed = true;	break; 
					case ::gpk::SYSTEM_KEY_Backspace	: Text.pop_back()	; ::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text}); break;
					case ::gpk::SYSTEM_KEY_Clear		: Text.clear()		; ::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text}); break;
					}
				}
			}
			return enterKeyPressed ? INT_MAX : handledControl ? handledControl : 0;
		}

	};

	::gpk::error_t			inputBoxCreate		(::gpk::SUIInputBox & inputBox, ::gpk::SGUI & gui, int32_t iParent = -1);
}

#endif
