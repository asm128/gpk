#include "gpk_gui_control_state.h"

#include "gpk_tri2.h"
#include "gpk_rect.h"
#include "gpk_align.h"
#include "gpk_array_static.h"

#include "gpk_image_color.h"

#ifndef GPK_GUI_CONTROL_H_29038749823613
#define GPK_GUI_CONTROL_H_29038749823613

namespace gpk
{
#pragma pack(push, 1)
	typedef	n2<cid_t>		n2cid;
	typedef	rect<cid_t>		rectcid;

	template<uint32_t _sizeArray>	
	using	astcid	= ::gpk::astatic<cid_t, _sizeArray>;

	GDEFINE_ENUM_TYPE(GUI_COLOR_MODE, uint8_t)
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, Default	, 0);
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, 3D		, 1);
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, Flat		, 2);

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
		bool			MarginLeft			: 1; //
		bool			MarginTop			: 1; //
		bool			MarginRight			: 1; //
		bool			MarginBottom		: 1; //

		bool			MinXOffset			: 1; //
		bool			MinYOffset			: 1; //
		bool			MinXSize			: 1; //
		bool			MinYSize			: 1; //
		bool			MaxXOffset			: 1; //
		bool			MaxYOffset			: 1; //
		bool			MaxXSize			: 1; //
		bool			MaxYSize			: 1; //
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
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, ACTION				, 8);
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

	struct SControlRectangleGPU {
		::gpk::rect2f32				Local				= {};
		::gpk::rect2f32				Global				= {};
	};

	struct SControlAreaGPU {
		::gpk::SControlRectangleGPU	Total				= {};
		::gpk::SControlRectangleGPU	Client				= {};
		::gpk::rect2f32				Text				= {};
		::gpk::rect2f32				Rectangles	[::gpk::GUI_CONTROL_AREA_OLD_COUNT]	= {};
		::gpk::tri2f32				Triangles	[8]		= {};
	};

	struct SControlAttachId {
		cid_t						IdControl			: 15;
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

	stincxp	::gpk::rectu16	controlNCRect		(const ::gpk::SControlPlacement & ctl)	noexcept	{ return ctl.Border + ctl.Margin; }
	stacxpr	::gpk::n2u16	controlNCSpacing	(const ::gpk::SControlPlacement & ctl)	noexcept	{ const rectu16 rc = controlNCRect(ctl); return ::gpk::n2i32{rc.Left + rc.Right, rc.Top + rc.Bottom}.u16(); }
	stincxp	void			controlNCSpacing	(const ::gpk::SControlPlacement & ctl, ::gpk::rectu16 & ncSpacing)	noexcept	{ ncSpacing = controlNCRect(ctl); }
	stincxp	void			controlNCSpacing	(const ::gpk::SControlPlacement & ctl, ::gpk::n2u16   & ncSpacing)	noexcept	{ ncSpacing = controlNCSpacing(ctl); }

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
		::gpk::apod<::gpk::SRelativeBit			>	RelativeBit		= {};
		::gpk::aobj<::gpk::SControlText			>	Text			= {};
		::gpk::aobj<::gpk::SControlImage		>	Images			= {};
		::gpk::aobj<::gpk::acid					>	Children		= {};
		//
		::gpk::apobj<::gpk::SSystemEvent		>	EventQueue		= {};

		inline	bool		SetHovered		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetHovered  (iControl, EventQueue, value); }
		inline	bool		SetPressed		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetPressed  (iControl, EventQueue, value); }
		inline	bool		SetHidden		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetHidden   (iControl, EventQueue, value); }
		inline	bool		SetFocused		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetFocused  (iControl, EventQueue, value); }
		inline	bool		SetSelected		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetSelected (iControl, EventQueue, value); }
		inline	bool		SetDisabled		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetDisabled (iControl, EventQueue, value); }
		inline	bool		SetUpdated		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetUpdated  (iControl, EventQueue, value); }
		inline	bool		SetAction		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetAction   (iControl, EventQueue, value); }
		inline	bool		SetUnused		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetUnused   (iControl, EventQueue, value); }

		inline	bool		IsHovered		(cid_t iControl)		const	noexcept	{ return States[iControl].IsHovered  (); }
		inline	bool		IsPressed		(cid_t iControl)		const	noexcept	{ return States[iControl].IsPressed  (); }
		inline	bool		IsHidden		(cid_t iControl)		const	noexcept	{ return States[iControl].IsHidden   (); }
		inline	bool		IsFocused		(cid_t iControl)		const	noexcept	{ return States[iControl].IsFocused  (); }
		inline	bool		IsSelected		(cid_t iControl)		const	noexcept	{ return States[iControl].IsSelected (); }
		inline	bool		IsDisabled		(cid_t iControl)		const	noexcept	{ return States[iControl].IsDisabled (); }
		inline	bool		IsUpdated		(cid_t iControl)		const	noexcept	{ return States[iControl].IsUpdated  (); }
		inline	bool		IsAction		(cid_t iControl)		const	noexcept	{ return States[iControl].IsAction	(); }
		inline	bool		IsUnused		(cid_t iControl)		const	noexcept	{ return States[iControl].IsUnused   (); }
	};

	::gpk::cid_t		controlCreate					(::gpk::SControlTable & gui, ::gpk::vi16 defaultColors);
} // namespace

#endif // GPK_GUI_CONTROL_H_29038749823613
