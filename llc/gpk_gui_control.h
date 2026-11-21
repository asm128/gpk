#include "gpk_gui_control_flag.h"
#include "gpk_gui_image.h"

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

namespace gpk
{
#pragma pack(push, 1)
	typedef	n2<cid_t>		n2cid;
	typedef	rect<cid_t>		rectcid;

	tplt<::gpk::u2_t _sizeArray>	
	using	astcid	= astatic<cid_t, _sizeArray>;

	GDEFINE_ENUM_TYPE(GUI_COLOR_MODE, u0_t)
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, Default	, 0);
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, 3D		, 1);
	GDEFINE_ENUM_VALUE(GUI_COLOR_MODE, Flat		, 2);

	GDEFINE_ENUM_TYPE(GUI_CONTROL_LIMIT, u0_t);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Left		, ::gpk::ALIGN_LEFT		);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Top		, ::gpk::ALIGN_TOP		);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Right		, ::gpk::ALIGN_RIGHT	);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Bottom	, ::gpk::ALIGN_BOTTOM	);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Outer		, ::gpk::ALIGN_FAR		);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_LIMIT, Inner		, ::gpk::ALIGN_NEAR		);
	
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
		rectu1_t						Margin				= {1, 1, 1, 1};
		rectu1_t						Border				= {1, 1, 1, 1};
		ALIGN						Align				= ::gpk::ALIGN_TOP_LEFT;
		GUI_CONTROL_LIMIT			RelativeBorderSize	: 4;
		GUI_CONTROL_LIMIT			RelativeMarginSize	: 4;
		GUI_CONTROL_METRICS_FLAG	RelativeMetrics		= {};
	}; 

	sinx	::gpk::rectu1_t	controlNCRect		(const ::gpk::SControlPlacement & ctl)	noexcept	{ return ctl.Border + ctl.Margin; }
	stxp	::gpk::n2u1_t	controlNCSpacing	(const ::gpk::SControlPlacement & ctl)	noexcept	{ const rectu1_t rc = controlNCRect(ctl); return ::gpk::n2s2_t{rc.Left + rc.Right, rc.Top + rc.Bottom}.u1(); }
	sinx	void			controlNCSpacing	(const ::gpk::SControlPlacement & ctl, ::gpk::rectu1_t & ncSpacing)	noexcept	{ ncSpacing = controlNCRect(ctl); }
	sinx	void			controlNCSpacing	(const ::gpk::SControlPlacement & ctl, ::gpk::n2u1_t   & ncSpacing)	noexcept	{ ncSpacing = controlNCSpacing(ctl); }

	GDEFINE_ENUM_TYPE(GUI_CONTROL_PALETTE, u0_t)
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
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, DRAGGING			, 10);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, COUNT				, 11);
	

	struct SControlDraw {
		::gpk::u0_t			ColorMode			: 2;
		::gpk::u0_t			FrameOut			: 1;
		::gpk::u0_t			NoBorder			: 1;	// skip drawing border area
		::gpk::u0_t			NoClient			: 1;	// skip drawing client area
		
		bool			UseNewPalettes		: 1;	// -- Legacy property (don't use in the new implementation!)

		::gpk::s1_t			ColorTheme			= 0;
		::gpk::asts1_t<::gpk::GUI_CONTROL_PALETTE_COUNT>	
					Palettes			= {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};
	};

	enum CONTROL_AREA_BASE : ::gpk::u0_t 
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

	enum GUI_CONTROL_AREA_OLD : ::gpk::u0_t
		{ GUI_CONTROL_AREA_OLD_CLIENT		= 0
		, GUI_CONTROL_AREA_OLD_BORDER_LEFT
		, GUI_CONTROL_AREA_OLD_BORDER_TOP
		, GUI_CONTROL_AREA_OLD_BORDER_RIGHT
		, GUI_CONTROL_AREA_OLD_BORDER_BOTTOM
		, GUI_CONTROL_AREA_OLD_COUNT
		};


	enum UI_CONTROL_AREA : ::gpk::u0_t
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
		::gpk::rect2s1_t				Local				= {};
		::gpk::rect2s1_t				Global				= {};
	};

	struct SControlArea {
		::gpk::SControlRectangle	Total				= {};
		::gpk::SControlRectangle	Client				= {};
		::gpk::rect2s1_t				Text				= {};
		::gpk::rect2s1_t				Rectangles	[::gpk::GUI_CONTROL_AREA_OLD_COUNT]	= {};
		::gpk::tri2s1_t				Triangles	[8]		= {};
	};

	struct SControlRectangleGPU {
		::gpk::rect2f2_t				Local				= {};
		::gpk::rect2f2_t				Global				= {};
	};

	struct SControlAreaGPU {
		::gpk::SControlRectangleGPU	Total				= {};
		::gpk::SControlRectangleGPU	Client				= {};
		::gpk::rect2f2_t				Text				= {};
		::gpk::rect2f2_t				Rectangles	[::gpk::GUI_CONTROL_AREA_OLD_COUNT]	= {};
		::gpk::tri2f2_t				Triangles	[8]		= {};
	};

	struct SControlAttachId {
		cid_t						IdControl			: 15;
		cid_t						Total				: 1;
	};

	struct SControlConstraints {
		::gpk::n2cid				AttachSizeToControl	= {-1, -1};
		::gpk::n2<bool>				AttachSizeToText	= {};
		::gpk::rectcid				DockToControl		= {-1, -1, -1, -1};
		::gpk::minmax2s1_t			SizeMinMax			= {{}, {0x7FFF, 0x7FFF}};
	};

	struct SControlText {
		::gpk::vcst_t					Text				= {};
		::gpk::ALIGN				Align				= ::gpk::ALIGN_CENTER;
		::gpk::minmaxu1_t			Selection			= {0, 1};
		::gpk::s1_t						FontSelected		= -1;
	};

	struct SControlTheme {
		::gpk::astatic<::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>, ::gpk::GUI_CONTROL_PALETTE_COUNT>
									ColorCombos			= {};
	};
#pragma pack(pop)

} // namespace

#endif // GPK_GUI_CONTROL_H_23627
