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

	GDEFINE_ENUM_TYPE(GUI_CONTROL_STATE, uint8_t)
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Hovered	, 0);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Pressed	, 1);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Hidden	, 2);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Focused	, 3);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Selected	, 4);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Open		, 5);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Locked	, 6);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Expanded	, 7);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Disabled	, 8);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Updated	, 9);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Unused	, 10);

	// Runtime internal states - I'm gonna ditch this crap which is useless compared to the enum form.
	struct SControlState {
		bool			Hovered				: 1;
		bool			Pressed				: 1;
		bool			Hidden				: 1;
		bool			Focused				: 1;
		bool			Selected			: 1;
		bool			Open				: 1;
		bool			Locked				: 1;
		bool			Expanded			: 1;
		bool			Disabled			: 1;
		bool			Updated				: 1;
		bool			Unused				: 1;

		//GPK_DEFAULT_OPERATOR(SControlState, GUI_CONTROL_STATE;
		GPK_DEFAULT_OPERATOR(SControlState
			, Hovered	== other.Hovered 
			&& Pressed	== other.Pressed 
			&& Hidden	== other.Hidden 
			&& Focused	== other.Focused 
			&& Selected	== other.Selected 
			&& Open		== other.Open 
			&& Locked	== other.Locked 
			&& Expanded	== other.Expanded 
			&& Disabled	== other.Disabled 
			&& Updated	== other.Updated 
			&& Unused	== other.Unused
			);

		inlcxpr	bool	IsHovered			(const SControlState & mask)		const	noexcept	{ return mask.Hovered  && Hovered  ; }
		inlcxpr	bool	IsPressed			(const SControlState & mask)		const	noexcept	{ return mask.Pressed  && Pressed  ; }
		inlcxpr	bool	IsHidden			(const SControlState & mask)		const	noexcept	{ return mask.Hidden   && Hidden   ; }
		inlcxpr	bool	IsFocused			(const SControlState & mask)		const	noexcept	{ return mask.Focused  && Focused  ; }
		inlcxpr	bool	IsSelected			(const SControlState & mask)		const	noexcept	{ return mask.Selected && Selected ; }
		inlcxpr	bool	IsOpen				(const SControlState & mask)		const	noexcept	{ return mask.Open     && Open     ; }
		inlcxpr	bool	IsLocked			(const SControlState & mask)		const	noexcept	{ return mask.Locked   && Locked   ; }
		inlcxpr	bool	IsExpanded			(const SControlState & mask)		const	noexcept	{ return mask.Expanded && Expanded ; }
		inlcxpr	bool	IsDisabled			(const SControlState & mask)		const	noexcept	{ return mask.Disabled && Disabled ; }
		inlcxpr	bool	IsUpdated			(const SControlState & mask)		const	noexcept	{ return mask.Updated  && Updated  ; }
		inlcxpr	bool	IsUnused			(const SControlState & mask)		const	noexcept	{ return mask.Unused   && Unused   ; }

		inline	bool	SetHovered			(const SControlState & mask, bool value)	noexcept	{ return mask.Hovered  && (Hovered  = value); }
		inline	bool	SetPressed			(const SControlState & mask, bool value)	noexcept	{ return mask.Pressed  && (Pressed  = value); }
		inline	bool	SetHidden			(const SControlState & mask, bool value)	noexcept	{ return mask.Hidden   && (Hidden   = value); }
		inline	bool	SetFocused			(const SControlState & mask, bool value)	noexcept	{ return mask.Focused  && (Focused  = value); }
		inline	bool	SetSelected			(const SControlState & mask, bool value)	noexcept	{ return mask.Selected && (Selected = value); }
		inline	bool	SetOpen				(const SControlState & mask, bool value)	noexcept	{ return mask.Open     && (Open     = value); }
		inline	bool	SetLocked			(const SControlState & mask, bool value)	noexcept	{ return mask.Locked   && (Locked   = value); }
		inline	bool	SetExpanded			(const SControlState & mask, bool value)	noexcept	{ return mask.Expanded && (Expanded = value); }
		inline	bool	SetDisabled			(const SControlState & mask, bool value)	noexcept	{ return mask.Disabled && (Disabled = value); }
		inline	bool	SetUpdated			(const SControlState & mask, bool value)	noexcept	{ return mask.Updated  && (Updated  = value); }
		inline	bool	SetUnused			(const SControlState & mask, bool value)	noexcept	{ return mask.Unused   && (Unused   = value); }


	};

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

		bool			HandleStateChange	(const SControlState & oldState, const SControlState & newState) {
			bool				handled				= false;
			if(oldState.Hovered  != newState.Hovered  )	(newState.Hovered  ? MouseIn : MouseOut ) = handled = true;
			if(oldState.Pressed  != newState.Pressed  )	handled = newState.Pressed ? Pushed = true : Released = Execute = true;
			if(oldState.Hidden   != newState.Hidden   )	(newState.Hidden   ? Hide    : Hide     ) = handled = true;
			if(oldState.Focused  != newState.Focused  )	(newState.Focused  ? FocusIn : FocusOut ) = handled = true;
			if(oldState.Selected != newState.Selected )	(newState.Selected ? SelectIn: SelectOut) = handled = true;
			if(oldState.Open     != newState.Open     )	(newState.Open     ? Open    : Close    ) = handled = true;
			if(oldState.Expanded != newState.Expanded )	(newState.Expanded ? Expand  : Collapse ) = handled = true;
			if(oldState.Updated  != newState.Updated  )	(newState.Updated  ? Resize  : Resize   ) = handled = true;
			if(oldState.Unused   != newState.Unused && false == newState.Unused)
				Create = handled	= true;
			return handled;
		}
	};

	struct SControl {
		cid_t			Parent			= -1;
		SControlState	State			= {};
		SControlState	Mask			= {};

#define CONTROL_STATE_SET_FUNCTION(MemberName) rvis_if(false, false == Mask.MemberName); \
	const SControlState		oldState = State;	\
	State.MemberName	= value;				\
	events.HandleStateChange(oldState, State);	\
	return State.MemberName	= value

		inline	bool	SetHovered		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Hovered ); }
		inline	bool	SetPressed		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Pressed ); }
		inline	bool	SetHidden		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Hidden  ); }
		inline	bool	SetFocused		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Focused ); }
		inline	bool	SetSelected		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Selected); }
		inline	bool	SetOpen			(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Open    ); }
		inline	bool	SetLocked		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Locked  ); }
		inline	bool	SetExpanded		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Expanded); }
		inline	bool	SetDisabled		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Disabled); }
		inline	bool	SetUpdated		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Updated ); }
		inline	bool	SetUnused		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Unused  ); }

		inlcxpr	bool	IsHovered		()	const	noexcept	{ return State.IsHovered  (Mask); }
		inlcxpr	bool	IsPressed		()	const	noexcept	{ return State.IsPressed  (Mask); }
		inlcxpr	bool	IsHidden		()	const	noexcept	{ return State.IsHidden   (Mask); }
		inlcxpr	bool	IsFocused		()	const	noexcept	{ return State.IsFocused  (Mask); }
		inlcxpr	bool	IsSelected		()	const	noexcept	{ return State.IsSelected (Mask); }
		inlcxpr	bool	IsOpen			()	const	noexcept	{ return State.IsOpen     (Mask); }
		inlcxpr	bool	IsLocked		()	const	noexcept	{ return State.IsLocked   (Mask); }
		inlcxpr	bool	IsExpanded		()	const	noexcept	{ return State.IsExpanded (Mask); }
		inlcxpr	bool	IsDisabled		()	const	noexcept	{ return State.IsDisabled (Mask); }
		inlcxpr	bool	IsUpdated		()	const	noexcept	{ return State.IsUpdated  (Mask); }
		inlcxpr	bool	IsUnused		()	const	noexcept	{ return State.IsUnused   (Mask); }

	};
	stacxpr	size_t SIZEOF_SCONTROL	= sizeof(SControl);

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
		::gpk::ALIGN				ImageAlign			= ::gpk::ALIGN_CENTER;
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
		::gpk::apod<::gpk::SControl				>	Controls		= {};
		::gpk::apod<::gpk::SControlPlacement	>	Placement		= {};
		::gpk::apod<::gpk::SControlArea			>	Metrics			= {};
		::gpk::apod<::gpk::SControlConstraints	>	Constraints		= {};
		::gpk::apod<::gpk::SControlState		>	States			= {};
		::gpk::apod<::gpk::SControlMode			>	Modes			= {};
		::gpk::apod<::gpk::SControlDraw			>	Draw			= {};
		::gpk::apod<::gpk::SControlEvent		>	Events			= {};
		::gpk::apod<::gpk::SRelativeBit			>	RelativeBit		= {};

		::gpk::aobj<::gpk::SControlText			>	Text			= {};
		::gpk::aobj<::gpk::acid					>	Children		= {};
		::gpk::aobj<::gpk::SControlImage		>	Images			= {};

		::gpk::apod<::gpk::SSysEvent			>	EventQueue		= {};
	};
	::gpk::error_t			controlCreate					(::gpk::SControlTable & gui, ::gpk::vi16 defaultColors);
} // namespace

#endif // GPK_GUI_CONTROL_H_29038749823613
