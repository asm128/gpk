#include "gpk_gui_control_flag.h"

#include "gpk_tri2.h"
#include "gpk_rect.h"
#include "gpk_align.h"
#include "gpk_array_static.h"

#include "gpk_img_color.h"
#include "gpk_grid_color.h"
#include "gpk_minmax_n2.h"

#include "gpk_axis.h"

#ifndef GPK_GUI_CONTROL_H_23627
#define GPK_GUI_CONTROL_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	tydf	n2<cid_t>		n2cid;
	tydf	rect<cid_t>		rectcid;

	tplt<u2_t _sizeArray>	
	usng	astcid	= astatic<cid_t, _sizeArray>;

	GDEFINE_ENUM_TYPE(GUI_COLOR_MODE, u0_t)
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, Default	, 0);
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, 3D		, 1);
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, Flat		, 2);

	GDEFINE_ENUM_TYPE(GUI_CONTROL_LIMIT, u0_t);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Left		, ALIGN_LEFT		);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Top		, ALIGN_TOP		);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Right		, ALIGN_RIGHT	);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Bottom	, ALIGN_BOTTOM	);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Outer		, ALIGN_FAR		);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Inner		, ALIGN_NEAR		);
	

	GDEFINE_ENUM_TYPE (GUI_CONTROL_METRICS_FLAG, u1_t);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, XSize		, 1 << 0);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, YSize		, 1 << 1);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, XSizeMin	, 1 << 2);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, YSizeMin	, 1 << 3);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, XSizeMax	, 1 << 4);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, YSizeMax	, 1 << 5);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, XOffset	, 1 << 6);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, YOffset	, 1 << 7);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, XOffsetMin	, 1 << 8);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, YOffsetMin	, 1 << 9);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, XOffsetMax	, 1 << 10);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_METRICS_FLAG, YOffsetMax	, 1 << 11);

	struct SControlPlacement {
		rect2s1_t					Area				= {{}, {16, 16}};
		rectu1_t					Margin				= {1, 1, 1, 1};
		rectu1_t					Border				= {1, 1, 1, 1};
		ALIGN						Align				= ALIGN_TOP_LEFT;
		GUI_CONTROL_LIMIT			RelativeBorderSize	: 4;
		GUI_CONTROL_LIMIT			RelativeMarginSize	: 4;
		GUI_CONTROL_METRICS_FLAG	RelativeMetrics		= {};
	}; 

	sinx	rectu1_t	controlNCRect		(const SControlPlacement & ctl)	nxpt	{ return ctl.Border + ctl.Margin; }
	stxp	n2u1_t		controlNCSpacing	(const SControlPlacement & ctl)	nxpt	{ const rectu1_t rc = controlNCRect(ctl); return n2s2_t{rc.Left + rc.Right, rc.Top + rc.Bottom}.u1(); }
	sinx	void		controlNCSpacing	(const SControlPlacement & ctl, rectu1_t & ncSpacing)	nxpt	{ ncSpacing = controlNCRect(ctl); }
	sinx	void		controlNCSpacing	(const SControlPlacement & ctl, n2u1_t   & ncSpacing)	nxpt	{ ncSpacing = controlNCSpacing(ctl); }

	GDEFINE_ENUM_TYPE(GUI_CONTROL_PALETTE, u1_t)
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, NORMAL				, 0);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, HIDDEN				, 1);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, DISABLED			, 2);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, HOVER				, 3);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, PRESSED				, 4);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, DRAGGING			, 5);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED			, 6);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_HIDDEN		, 7);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_DISABLED	, 8);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_HOVER		, 9);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_PRESSED	,10);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_DRAGGING	,11);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, OUTDATED			,12);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, COUNT				,13);
	sinx	GUI_CONTROL_PALETTE	paletteIndexFromState			(bool disabled, const SControlState & controlState) {
		return disabled					? ::gpk::GUI_CONTROL_PALETTE_DISABLED
			: controlState.IsDragging() ? ::gpk::GUI_CONTROL_PALETTE_DRAGGING
			: controlState.IsPressed () ? ::gpk::GUI_CONTROL_PALETTE_PRESSED
			: controlState.IsSelected() ? ::gpk::GUI_CONTROL_PALETTE_SELECTED
			: controlState.IsHovered () ? ::gpk::GUI_CONTROL_PALETTE_HOVER
			: ::gpk::GUI_CONTROL_PALETTE_NORMAL
			;
	}
	struct SControlDraw {
		u0_t			ColorMode			: 2;
		u0_t			FrameOut			: 1;
		u0_t			NoBorder			: 1;	// skip drawing border area
		u0_t			NoClient			: 1;	// skip drawing client area
		u0_t			UseNewPalettes		: 1;
		s1_t			ColorTheme			= 0;
		asts1_t<GUI_CONTROL_PALETTE_COUNT>	
						Palettes			= {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}};
	};

	enum CONTROL_AREA_BASE : u0_t 
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

	enum GUI_CONTROL_AREA_OLD : u0_t
		{ GUI_CONTROL_AREA_OLD_CLIENT		= 0
		, GUI_CONTROL_AREA_OLD_BORDER_LEFT
		, GUI_CONTROL_AREA_OLD_BORDER_TOP
		, GUI_CONTROL_AREA_OLD_BORDER_RIGHT
		, GUI_CONTROL_AREA_OLD_BORDER_BOTTOM
		, GUI_CONTROL_AREA_OLD_COUNT
		};

	enum UI_CONTROL_AREA : u0_t
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
		rect2s1_t				Local				= {};
		rect2s1_t				Global				= {};
	};

	struct SControlArea {
		SControlRectangle	Total				= {};
		SControlRectangle	Client				= {};
		rect2s1_t			Text				= {};
		rect2s1_t			Rectangles	[GUI_CONTROL_AREA_OLD_COUNT]	= {};
		tri2s1_t				Triangles	[8]		= {};
	};

	struct SControlAttachId {
		cid_t						IdControl			: 15;
		cid_t						Total				: 1;
	};

	struct SControlConstraints {
		n2cid						AttachSizeToControl	= {-1, -1};
		n2<bool>					AttachSizeToText	= {};
		rectcid						DockToControl		= {-1, -1, -1, -1};
		minmax2s1_t					SizeMinMax			= {{}, {0x7FFF, 0x7FFF}};
	};

	struct SControlText {
		vcst_t						Text				= {};
		ALIGN						Align				= ALIGN_CENTER;
		minmaxu1_t					Selection			= {0, 1};
		s1_t						FontSelected		= -1;
	};

	struct SControlTheme {
		astatic<astu2_t<UI_CONTROL_AREA_COUNT>, GUI_CONTROL_PALETTE_COUNT>
									ColorCombos			= {};
	};
#pragma pack(pop)

	struct SControlImage {
		rtbgra8d32					Temp				= {};
		g8bgra						Image				= {};
		n2s1_t						ImageOffset			= {};
		u0_t						ImageAlign			= ALIGN_CENTER;
		u0_t						ImageInvertX		: 1;
		u0_t						ImageInvertY		: 1;
		u0_t						Rotation			: 2;
	};

	struct SControlTable {
		apod<SControlState		>	States				= {};
		apod<SControlState		>	StatesPrevious		= {};
		apod<SControlPlacement	>	Placement			= {};
		apod<SControlArea		>	Area				= {};
		apod<SControlConstraints>	Constraints			= {};
		apod<SControlDraw		>	Draw				= {};
		aobj<SControlText		>	Text				= {};
		aobj<SControlImage		>	Images				= {};
		aobj<acid				>	Children			= {};
		//
		apobj<SEventSystem		>	EventQueue			= {};

		inln	bool		SetUnused		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetUnused  (iControl, EventQueue, value); }
	 	inln	bool		SetHidden		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetHidden  (iControl, EventQueue, value); }
		inln	bool		SetDisabled		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetDisabled(iControl, EventQueue, value); }
		inln	bool		SetHovered		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetHovered (iControl, EventQueue, value); }
		inln	bool		SetFocused		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetFocused (iControl, EventQueue, value); }
		inln	bool		SetPressed		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetPressed (iControl, EventQueue, value); }
		inln	bool		SetDragging		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetDragging(iControl, EventQueue, value); }
		inln	bool		SetSelected		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetSelected(iControl, EventQueue, value); }
		inln	bool		SetUpdated		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetUpdated (iControl, EventQueue, value); }
		inln	bool		SetAction		(cid_t iControl, bool value)	nxpt	{ return States[iControl].SetAction  (iControl, EventQueue, value); }

		inln	bool		IsUnused		(cid_t iControl)		csnx	{ return States[iControl].IsUnused   (); }
		inln	bool		IsHidden		(cid_t iControl)		csnx	{ return States[iControl].IsHidden   (); }
		inln	bool		IsDisabled		(cid_t iControl)		csnx	{ return States[iControl].IsDisabled (); }
		inln	bool		IsHovered		(cid_t iControl)		csnx	{ return States[iControl].IsHovered  (); }
		inln	bool		IsFocused		(cid_t iControl)		csnx	{ return States[iControl].IsFocused  (); }
		inln	bool		IsPressed		(cid_t iControl)		csnx	{ return States[iControl].IsPressed  (); }
		inln	bool		IsDragging		(cid_t iControl)		csnx	{ return States[iControl].IsDragging (); }
		inln	bool		IsSelected		(cid_t iControl)		csnx	{ return States[iControl].IsSelected (); }
		inln	bool		IsUpdated		(cid_t iControl)		csnx	{ return States[iControl].IsUpdated  (); }
		inln	bool		IsAction		(cid_t iControl)		csnx	{ return States[iControl].IsAction   (); }
	};

	cid_t		controlCreate					(SControlTable & gui, vs1_t defaultColors);
} // nmsp

#endif // GPK_GUI_CONTROL_H_23627
