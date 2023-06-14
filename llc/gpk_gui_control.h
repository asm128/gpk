#include "gpk_input.h"
#include "gpk_array_static.h"
#include "gpk_tri2.h"
#include "gpk_rect.h"
#include "gpk_align.h"

#include "gpk_sysevent.h"

#include "gpk_image_color.h"


#ifndef GPK_GUI_CONTROL_H_29038749823613
#define GPK_GUI_CONTROL_H_29038749823613

namespace gpk
{
#pragma pack(push, 1)
	typedef	int32_t			cid_t;
	typedef	const cid_t		ccid_t;

	typedef	n2<cid_t>		n2cid;
	typedef	rect<cid_t>		rectcid;
	typedef	apod<cid_t>		acid;
	typedef	view<cid_t>		vcid;
	typedef	view<ccid_t>	vccid;

	GDEFINE_ENUM_TYPE(GUI_COLOR_MODE, uint8_t)
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, Default	, 0);
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, 3D		, 1);
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, Flat		, 2);

	GDEFINE_ENUM_TYPE(GUI_CONTROL_STATE, uint16_t)
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Hovered	, 0x001);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Pressed	, 0x002);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Hidden	, 0x004);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Focused	, 0x008);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Selected	, 0x010);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Open		, 0x020);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Locked	, 0x040);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Expanded	, 0x080);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Disabled	, 0x100);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Updated	, 0x200);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Unused	, 0x400);

	inlcxpr	bool	isHovered			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Hovered ); }
	inlcxpr	bool	isPressed			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Pressed ); }
	inlcxpr	bool	isHidden			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Hidden  ); }
	inlcxpr	bool	isFocused			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Focused ); }
	inlcxpr	bool	isSelected			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Selected); }
	inlcxpr	bool	isOpen				(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Open    ); }
	inlcxpr	bool	isLocked			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Locked  ); }
	inlcxpr	bool	isExpanded			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Expanded); }
	inlcxpr	bool	isDisabled			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Disabled); }
	inlcxpr	bool	isUpdated			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Updated ); }
	inlcxpr	bool	isUnused			(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Unused  ); }

	inlcxpr	bool	setHovered			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Hovered ) && (GUI_CONTROL_STATE_Hovered  & (value ? bit_set(state, GUI_CONTROL_STATE_Hovered ) : bit_clear(state, GUI_CONTROL_STATE_Hovered ))); }
	inlcxpr	bool	setPressed			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Pressed ) && (GUI_CONTROL_STATE_Pressed  & (value ? bit_set(state, GUI_CONTROL_STATE_Pressed ) : bit_clear(state, GUI_CONTROL_STATE_Pressed ))); }
	inlcxpr	bool	setHidden			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Hidden  ) && (GUI_CONTROL_STATE_Hidden   & (value ? bit_set(state, GUI_CONTROL_STATE_Hidden  ) : bit_clear(state, GUI_CONTROL_STATE_Hidden  ))); }
	inlcxpr	bool	setFocused			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Focused ) && (GUI_CONTROL_STATE_Focused  & (value ? bit_set(state, GUI_CONTROL_STATE_Focused ) : bit_clear(state, GUI_CONTROL_STATE_Focused ))); }
	inlcxpr	bool	setSelected			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Selected) && (GUI_CONTROL_STATE_Selected & (value ? bit_set(state, GUI_CONTROL_STATE_Selected) : bit_clear(state, GUI_CONTROL_STATE_Selected))); }
	inlcxpr	bool	setOpen				(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Open    ) && (GUI_CONTROL_STATE_Open     & (value ? bit_set(state, GUI_CONTROL_STATE_Open    ) : bit_clear(state, GUI_CONTROL_STATE_Open    ))); }
	inlcxpr	bool	setLocked			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Locked  ) && (GUI_CONTROL_STATE_Locked   & (value ? bit_set(state, GUI_CONTROL_STATE_Locked  ) : bit_clear(state, GUI_CONTROL_STATE_Locked  ))); }
	inlcxpr	bool	setExpanded			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Expanded) && (GUI_CONTROL_STATE_Expanded & (value ? bit_set(state, GUI_CONTROL_STATE_Expanded) : bit_clear(state, GUI_CONTROL_STATE_Expanded))); }
	inlcxpr	bool	setDisabled			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Disabled) && (GUI_CONTROL_STATE_Disabled & (value ? bit_set(state, GUI_CONTROL_STATE_Disabled) : bit_clear(state, GUI_CONTROL_STATE_Disabled))); }
	inlcxpr	bool	setUpdated			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Updated ) && (GUI_CONTROL_STATE_Updated  & (value ? bit_set(state, GUI_CONTROL_STATE_Updated ) : bit_clear(state, GUI_CONTROL_STATE_Updated ))); }
	inlcxpr	bool	setUnused			(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Unused  ) && (GUI_CONTROL_STATE_Unused   & (value ? bit_set(state, GUI_CONTROL_STATE_Unused  ) : bit_clear(state, GUI_CONTROL_STATE_Unused  ))); }

	GDEFINE_ENUM_TYPE (GUI_CONTROL_EVENT, uint16_t)
	GDEFINE_ENUM_VALUE(GUI_CONTROL_EVENT, StateChange, 0x000);

	// Holds the state changes after an update call. 
	struct SControlEvent {
		bool			MouseIn				: 1;
		bool			MouseOut			: 1;
		bool			Pushed				: 1;
		bool			Released			: 1;
		bool			Show				: 1;
		bool			Hide				: 1;
		bool			FocusIn				: 1;
		bool			FocusOut			: 1;

		bool			SelectIn			: 1;
		bool			SelectOut			: 1;
		bool			Open				: 1;
		bool			Close				: 1;
		bool			Expand				: 1;
		bool			Collapse			: 1;

		bool			Execute				: 1;
		bool			Resize				: 1;
		bool			Create				: 1;

		bool			HandleStateChange	(const GUI_CONTROL_STATE oldState, const GUI_CONTROL_STATE newState) {
			bool				handled				= false;
			if(bit_test(oldState, GUI_CONTROL_STATE_Hovered  ) != bit_test(newState, GUI_CONTROL_STATE_Hovered  ))	(bit_test(newState, GUI_CONTROL_STATE_Hovered) ? MouseIn : MouseOut ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Pressed  ) != bit_test(newState, GUI_CONTROL_STATE_Pressed  ))	handled = bit_test(newState, GUI_CONTROL_STATE_Pressed) ? Pushed = true : Released = Execute = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Hidden   ) != bit_test(newState, GUI_CONTROL_STATE_Hidden   ))	(bit_test(newState, GUI_CONTROL_STATE_Hidden   )	 ? Hide    : Hide     ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Focused  ) != bit_test(newState, GUI_CONTROL_STATE_Focused  ))	(bit_test(newState, GUI_CONTROL_STATE_Focused  )	 ? FocusIn : FocusOut ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Selected ) != bit_test(newState, GUI_CONTROL_STATE_Selected ))	(bit_test(newState, GUI_CONTROL_STATE_Selected )	 ? SelectIn: SelectOut) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Open     ) != bit_test(newState, GUI_CONTROL_STATE_Open     ))	(bit_test(newState, GUI_CONTROL_STATE_Open     )	 ? Open    : Close    ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Expanded ) != bit_test(newState, GUI_CONTROL_STATE_Expanded ))	(bit_test(newState, GUI_CONTROL_STATE_Expanded )	 ? Expand  : Collapse ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Updated  ) != bit_test(newState, GUI_CONTROL_STATE_Updated  ))	(bit_test(newState, GUI_CONTROL_STATE_Updated  )	 ? Resize  : Resize   ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Unused   ) != bit_test(newState, GUI_CONTROL_STATE_Unused	) && false == bit_test(newState, GUI_CONTROL_STATE_Unused))
				Create = handled	= true;
			return handled;
		}
	};

	struct SControlState {
		cid_t				Parent			= -1;
		GUI_CONTROL_STATE	State			= {};
		GUI_CONTROL_STATE	Mask			= (GUI_CONTROL_STATE)-1;

#define CONTROL_STATE_SET_FUNCTION(MemberName)					\
	if(false == bit_test(Mask, GUI_CONTROL_STATE_##MemberName))	\
		return false;											\
	const GUI_CONTROL_STATE	oldState = State;					\
	set##MemberName(State, Mask, value);						\
	events.HandleStateChange(oldState, State);					\
	return bit_test(State, GUI_CONTROL_STATE_##MemberName)

		bool				SetHovered		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Hovered ); }
		bool				SetPressed		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Pressed ); }
		bool				SetHidden		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Hidden  ); }
		bool				SetFocused		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Focused ); }
		bool				SetSelected		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Selected); }
		bool				SetOpen			(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Open    ); }
		bool				SetLocked		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Locked  ); }
		bool				SetExpanded		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Expanded); }
		bool				SetDisabled		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Disabled); }
		bool				SetUpdated		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Updated ); }
		bool				SetUnused		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Unused  ); }

		inlcxpr	bool		IsHovered		()	const	noexcept	{ return isHovered  (State, Mask); }
		inlcxpr	bool		IsPressed		()	const	noexcept	{ return isPressed  (State, Mask); }
		inlcxpr	bool		IsHidden		()	const	noexcept	{ return isHidden   (State, Mask); }
		inlcxpr	bool		IsFocused		()	const	noexcept	{ return isFocused  (State, Mask); }
		inlcxpr	bool		IsSelected		()	const	noexcept	{ return isSelected (State, Mask); }
		inlcxpr	bool		IsOpen			()	const	noexcept	{ return isOpen     (State, Mask); }
		inlcxpr	bool		IsLocked		()	const	noexcept	{ return isLocked   (State, Mask); }
		inlcxpr	bool		IsExpanded		()	const	noexcept	{ return isExpanded (State, Mask); }
		inlcxpr	bool		IsDisabled		()	const	noexcept	{ return isDisabled (State, Mask); }
		inlcxpr	bool		IsUpdated		()	const	noexcept	{ return isUpdated  (State, Mask); }
		inlcxpr	bool		IsUnused		()	const	noexcept	{ return isUnused   (State, Mask); }

	};
	stacxpr	size_t SIZEOF_SCONTROL	= sizeof(SControlState);

	struct SControlMode {
		bool			NoHover				: 1;
		bool			NoFocus				: 1;
		bool			NoSelect			: 1;
		bool			NoExecute			: 1;
	};

	struct SRelativeBit {
		bool			XOffset				: 1; //
		bool			YOffset				: 1; //
		bool			XSize				: 1; //
		bool			YSize				: 1; //
		bool			BorderLeft			: 1; //
		bool			BorderTop			: 1; //
		bool			BorderRight			: 1; //
		bool			BorderBottom		: 1; //
	};

	GDEFINE_ENUM_TYPE(GUI_CONTROL_PALETTE, uint8_t)
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, NORMAL				, 0);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, DISABLED			, 1);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, HOVER				, 2);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, PRESSED				, 3);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED			, 4);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_DISABLED	, 5);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_HOVER		, 6);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_PRESSED	, 7);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, EXECUTE				, 8);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, OUTDATED			, 9);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, COUNT				, 10);
	

	struct SControlDraw {
		uint8_t			ColorMode			: 2;
		uint8_t			FrameOut			: 1;
		uint8_t			NoBorder			: 1;	// skip drawing border area
		uint8_t			NoClient			: 1;	// skip drawing client area
		
		bool			UseNewPalettes		: 1;	// -- Legacy property (don't use in the new implementation!)

		int16_t			ColorTheme			= 0;
		::gpk::asti16<::gpk::GUI_CONTROL_PALETTE_COUNT>	
						Palettes			= {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9,}};
	};

	enum CONTROL_AREA_BASE : uint8_t 
		{ CONTROL_AREA_BASE_BACKGROUND		= 0
		, CONTROL_AREA_BASE_CONTENT
		, CONTROL_AREA_BASE_BORDER_LEFT
		, CONTROL_AREA_BASE_BORDER_TOP
		, CONTROL_AREA_BASE_BORDER_RIGHT
		, CONTROL_AREA_BASE_BORDER_BOTTOM
		, CONTROL_AREA_BASE_CORNER_TOP_LEFT
		, CONTROL_AREA_BASE_CORNER_RIGHT_TOP
		, CONTROL_AREA_BASE_CORNER_BOTTOM_RIGHT
		, CONTROL_AREA_BASE_CORNER_LEFT_BOTTOM
		, CONTROL_AREA_BASE_COUNT
		};

	enum GUI_CONTROL_AREA_OLD : uint8_t
		{ GUI_CONTROL_AREA_OLD_BACKGROUND	= 0
		, GUI_CONTROL_AREA_OLD_CLIENT
		, GUI_CONTROL_AREA_OLD_BORDER_LEFT
		, GUI_CONTROL_AREA_OLD_BORDER_TOP
		, GUI_CONTROL_AREA_OLD_BORDER_RIGHT
		, GUI_CONTROL_AREA_OLD_BORDER_BOTTOM
		, GUI_CONTROL_AREA_OLD_COUNT
		};


	enum UI_CONTROL_AREA : uint8_t
		{ UI_CONTROL_AREA_BACKGROUND		= 0
		, UI_CONTROL_AREA_CLIENT
		, UI_CONTROL_AREA_BORDER_LEFT
		, UI_CONTROL_AREA_BORDER_TOP
		, UI_CONTROL_AREA_BORDER_RIGHT
		, UI_CONTROL_AREA_BORDER_BOTTOM
		, UI_CONTROL_AREA_TEXT_BACKGROUND
		, UI_CONTROL_AREA_TEXT_FACE
		, UI_CONTROL_AREA_MARGIN_LEFT
		, UI_CONTROL_AREA_MARGIN_TOP
		, UI_CONTROL_AREA_MARGIN_RIGHT
		, UI_CONTROL_AREA_MARGIN_BOTTOM
		, UI_CONTROL_AREA_CORNER_LEFT_TOP_TOP
		, UI_CONTROL_AREA_CORNER_LEFT_TOP_LEFT
		, UI_CONTROL_AREA_CORNER_RIGHT_TOP_TOP
		, UI_CONTROL_AREA_CORNER_RIGHT_TOP_RIGHT
		, UI_CONTROL_AREA_CORNER_LEFT_BOTTOM_LEFT
		, UI_CONTROL_AREA_CORNER_LEFT_BOTTOM_BOTTOM
		, UI_CONTROL_AREA_CORNER_RIGHT_BOTTOM_RIGHT
		, UI_CONTROL_AREA_CORNER_RIGHT_BOTTOM_BOTTOM
		, UI_CONTROL_AREA_COUNT
		};

	struct SControlRectangle {
		::gpk::rect2i16				Local				= {};
		::gpk::rect2i16				Global				= {};
	};

	struct SControlArea {
		::gpk::SControlRectangle	Total				= {};
		::gpk::SControlRectangle	Client				= {};
		::gpk::rect2i16				Text				= {};
		::gpk::rect2i16				Rectangles	[::gpk::GUI_CONTROL_AREA_OLD_COUNT]	= {};
		::gpk::tri2i16				Triangles	[8]		= {};
	};

	struct SControlAttachId {
		cid_t						IdControl			: 31;
		cid_t						Total				: 1;
	};

	struct SControlConstraints {
		::gpk::n2cid				AttachSizeToControl	= {-1, -1};
		::gpk::n2<bool>				AttachSizeToText	= {};
		::gpk::rectcid				DockToControl		= {-1, -1, -1, -1};
		::gpk::minmax2i16			SizeMinMax			= {{}, {0x7FFF, 0x7FFF}};
	};

	struct SControlImage {
		::gpk::rtbgra8d32			Temp				= {};
		::gpk::v2bgra				Image				= {};
		::gpk::n2i16				ImageOffset			= {};
		uint8_t						ImageAlign			= ::gpk::ALIGN_CENTER;
		uint8_t						ImageInvertX		: 1;
		uint8_t						ImageInvertY		: 1;
		uint8_t						Rotation			: 2;
	};

	struct SControlPlacement {
		::gpk::rect2i16				Area				= {{}, {16, 16}};
		::gpk::rectu16				Margin				= {1, 1, 1, 1};
		::gpk::rectu16				Border				= {1, 1, 1, 1};
		::gpk::ALIGN				Align				= ::gpk::ALIGN_TOP_LEFT;
	};

	// The large amoutn of pointless casts written in this function is because idiots can't handle C types so some other retards decided to add this stupid rule into the standard .
	stincxp	void			controlNCSpacing	(const ::gpk::SControlPlacement & ctl, ::gpk::rectu16 & ncSpacing)	noexcept	{ ncSpacing = ::gpk::recti32{ctl.Border.Left + ctl.Margin.Left, ctl.Border.Top + ctl.Margin.Top, ctl.Border.Right + ctl.Margin.Right, ctl.Border.Bottom + ctl.Margin.Bottom}.u16();	 }
	stincxp	void			controlNCSpacing	(const ::gpk::SControlPlacement & ctl, ::gpk::n2u16   & ncSpacing)	noexcept	{ ncSpacing = ::gpk::n2i32  {ctl.Border.Left + ctl.Margin.Left + ctl.Border.Right + ctl.Margin.Right, ctl.Border.Top + ctl.Margin.Top + ctl.Border.Bottom + ctl.Margin.Bottom}.u16(); }
	stincxp	::gpk::rectu16	controlNCRect		(const ::gpk::SControlPlacement & ctl)	noexcept	{ return ::gpk::recti32		{ctl.Border.Left + ctl.Margin.Left, ctl.Border.Top + ctl.Margin.Top, ctl.Border.Right + ctl.Margin.Right, ctl.Border.Bottom + ctl.Margin.Bottom}.u16(); }
	stincxp	::gpk::n2u16	controlNCSpacing	(const ::gpk::SControlPlacement & ctl)	noexcept	{ return ::gpk::n2i32		{ctl.Border.Left + ctl.Margin.Left + ctl.Border.Right + ctl.Margin.Right, ctl.Border.Top + ctl.Margin.Top + ctl.Border.Bottom + ctl.Margin.Bottom}.u16(); }

	struct SControlText {
		::gpk::vcs			Text				= {};
		::gpk::ALIGN		Align				= ::gpk::ALIGN_CENTER;
		::gpk::minmaxu16	Selection			= {0, 1};
		int16_t				FontSelected		= -1;
	};

	struct SControlTheme {
		::gpk::astatic<::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>, ::gpk::GUI_CONTROL_PALETTE_COUNT>
							ColorCombos			= {};
	};
#pragma pack(pop)

	struct SControlTable {
		::gpk::apod<::gpk::SControlState		>	States			= {};
		::gpk::apod<::gpk::SControlPlacement	>	Placement		= {};
		::gpk::apod<::gpk::SControlArea			>	Metrics			= {};
		::gpk::apod<::gpk::SControlConstraints	>	Constraints		= {};
		::gpk::apod<::gpk::SControlMode			>	Modes			= {};
		::gpk::apod<::gpk::SControlDraw			>	Draw			= {};
		::gpk::apod<::gpk::SControlEvent		>	Events			= {};
		::gpk::apod<::gpk::SRelativeBit			>	RelativeBit		= {};

		::gpk::aobj<::gpk::SControlText			>	Text			= {};
		::gpk::aobj<::gpk::acid					>	Children		= {};
		::gpk::aobj<::gpk::SControlImage		>	Images			= {};

		::gpk::apod<::gpk::SSysEvent			>	EventQueue		= {};

		inline	bool		SetHovered		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetHovered  (Events[iControl], value); }
		inline	bool		SetPressed		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetPressed  (Events[iControl], value); }
		inline	bool		SetHidden		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetHidden   (Events[iControl], value); }
		inline	bool		SetFocused		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetFocused  (Events[iControl], value); }
		inline	bool		SetSelected		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetSelected (Events[iControl], value); }
		inline	bool		SetOpen			(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetOpen     (Events[iControl], value); }
		inline	bool		SetLocked		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetLocked   (Events[iControl], value); }
		inline	bool		SetExpanded		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetExpanded (Events[iControl], value); }
		inline	bool		SetDisabled		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetDisabled (Events[iControl], value); }
		inline	bool		SetUpdated		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetUpdated  (Events[iControl], value); }
		inline	bool		SetUnused		(uint32_t iControl, bool value)	noexcept	{ return States[iControl].SetUnused   (Events[iControl], value); }

		inlcxpr	bool		IsHovered		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsHovered  (); }
		inlcxpr	bool		IsPressed		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsPressed  (); }
		inlcxpr	bool		IsHidden		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsHidden   (); }
		inlcxpr	bool		IsFocused		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsFocused  (); }
		inlcxpr	bool		IsSelected		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsSelected (); }
		inlcxpr	bool		IsOpen			(uint32_t iControl)		const	noexcept	{ return States[iControl].IsOpen     (); }
		inlcxpr	bool		IsLocked		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsLocked   (); }
		inlcxpr	bool		IsExpanded		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsExpanded (); }
		inlcxpr	bool		IsDisabled		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsDisabled (); }
		inlcxpr	bool		IsUpdated		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsUpdated  (); }
		inlcxpr	bool		IsUnused		(uint32_t iControl)		const	noexcept	{ return States[iControl].IsUnused   (); }


	};
	::gpk::error_t			controlCreate					(::gpk::SControlTable & gui, ::gpk::vi16 defaultColors);
} // namespace

#endif // GPK_GUI_CONTROL_H_29038749823613
