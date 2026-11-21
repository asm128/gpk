#include "gpk_gui_control_flag.h"
#include "gpk_tri2.h"
#include "gpk_rect.h"
#include "gpk_align.h"
#include "gpk_array_static.h"

#include "gpk_grid_color.h"
#include "gpk_minmax_n2.h"

#include "gpk_axis.h"

#ifndef GPK_GUI_CONTROL_H_23627
#define GPK_GUI_CONTROL_H_23627

nmsp gpk
{
    tydf	n2<cid_t>		n2cid;
    tydf	rect<cid_t>		rectcid;

    tplt<u2_t _sizeArray>	
    usng	astcid	= astatic<cid_t, _sizeArray>;

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
        rectu1_t					Margin				= {1, 1, 1, 1};
        rectu1_t					Border				= {1, 1, 1, 1};
        ALIGN						Align				= ALIGN_TOP_LEFT;
        GUI_CONTROL_LIMIT			RelativeBorderSize	: 4;
        GUI_CONTROL_LIMIT			RelativeMarginSize	: 4;
        GUI_CONTROL_METRICS_FLAG	RelativeMetrics		= {};
    };

    sinx	rectu1_t	controlNCRect		(cnst SControlPlacement & ctl)	nxpt	{ return ctl.Border + ctl.Margin; }
    stxp	n2u1_t		controlNCSpacing	(cnst SControlPlacement & ctl)	nxpt	{ cnst rectu1_t rc = controlNCRect(ctl); return n2s2_t{rc.Left + rc.Right, rc.Top + rc.Bottom}.u1(); }
    sinx	void		controlNCSpacing	(cnst SControlPlacement & ctl, rectu1_t & ncSpacing)	nxpt	{ ncSpacing = controlNCRect(ctl); }
    sinx	void		controlNCSpacing	(cnst SControlPlacement & ctl, n2u1_t   & ncSpacing)	nxpt	{ ncSpacing = controlNCSpacing(ctl); }

    GDEFINE_ENUM_TYPE(GUI_CONTROL_PALETTE, u0_t);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, NORMAL, 0);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, HIDDEN, 1);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, DISABLED, 2);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, HOVER, 3);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, PRESSED, 4);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, DRAGGING, 5);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED, 6);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_HIDDEN, 7);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_DISABLED, 8);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_HOVER, 9);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_PRESSED, 10);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, SELECTED_DRAGGING, 11);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, OUTDATED, 12);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_PALETTE, COUNT, 13);
    sinx	GUI_CONTROL_PALETTE	paletteIndexFromState			(b8_t disabled, cnst SControlState & controlState) {
        return disabled					? ::gpk::GUI_CONTROL_PALETTE_DISABLED
            : controlState.IsDragging() ? ::gpk::GUI_CONTROL_PALETTE_DRAGGING
            : controlState.IsPressed () ? ::gpk::GUI_CONTROL_PALETTE_PRESSED
            : controlState.IsSelected() ? ::gpk::GUI_CONTROL_PALETTE_SELECTED
            : controlState.IsHovered () ? ::gpk::GUI_CONTROL_PALETTE_HOVER
            : controlState.IsHidden  () ? ::gpk::GUI_CONTROL_PALETTE_HIDDEN
            : ::gpk::GUI_CONTROL_PALETTE_NORMAL
            ;
    }
#pragma pack(push, 1)
    struct SControlDraw {
        u0_t			ColorMode			: 2;
        u0_t			FrameOut			: 1;
        u0_t			NoBorder			: 1;	// skip drawing border area
        u0_t			NoClient			: 1;	// skip drawing client area
        u0_t			UseNewPalettes		: 1;// -- Legacy property (don't use in the new implementation!)
        s1_t			ColorTheme			= 0;
        asts1_t<GUI_CONTROL_PALETTE_COUNT>	
                        Palettes			= {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}};
    };
#pragma pack(pop)
    GDEFINE_ENUM_TYPE(CONTROL_AREA_BASE, u0_t);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, BACKGROUND, 0);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, CONTENT, 1);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, BORDER_LEFT, 2);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, BORDER_TOP, 3);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, BORDER_RIGHT, 4);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, BORDER_BOTTOM, 5);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, CORNER_TOP_LEFT, 6);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, CORNER_RIGHT_TOP, 7);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, CORNER_BOTTOM_RIGHT, 8);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, CORNER_LEFT_BOTTOM, 9);
    GDEFINE_ENUM_VALUE(CONTROL_AREA_BASE, COUNT, 10);

    GDEFINE_ENUM_TYPE(GUI_CONTROL_AREA_OLD, u0_t);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_AREA_OLD, CLIENT, 0);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_AREA_OLD, BORDER_LEFT, 1);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_AREA_OLD, BORDER_TOP, 2);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_AREA_OLD, BORDER_RIGHT, 3);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_AREA_OLD, BORDER_BOTTOM, 4);
    GDEFINE_ENUM_VALUE(GUI_CONTROL_AREA_OLD, COUNT, 5);

    GDEFINE_ENUM_TYPE(UI_CONTROL_AREA, u0_t);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, BACKGROUND, 0);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, CLIENT, 1);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, BORDER_LEFT, 2);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, BORDER_TOP, 3);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, BORDER_RIGHT, 4);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, BORDER_BOTTOM, 5);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, TEXT_BACKGROUND, 6);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, TEXT_FACE, 7);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, MARGIN_LEFT, 8);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, MARGIN_TOP, 9);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, MARGIN_RIGHT, 10);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, MARGIN_BOTTOM, 11);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, CORNER_LEFT_TOP_TOP, 12);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, CORNER_LEFT_TOP_LEFT, 13);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, CORNER_RIGHT_TOP_TOP, 14);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, CORNER_RIGHT_TOP_RIGHT, 15);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, CORNER_LEFT_BOTTOM_LEFT, 16);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, CORNER_LEFT_BOTTOM_BOTTOM, 17);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, CORNER_RIGHT_BOTTOM_RIGHT, 18);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, CORNER_RIGHT_BOTTOM_BOTTOM, 19);
    GDEFINE_ENUM_VALUE(UI_CONTROL_AREA, COUNT, 20);
#pragma pack(push, 1)
    struct SControlRectangle {
        rect2s1_t			Local				= {};
        rect2s1_t			Global				= {};
    };
    struct SControlArea {
        SControlRectangle	Total				= {};
        SControlRectangle	Client				= {};
        rect2s1_t			Text				= {};
        rect2s1_t			Rectangles	[GUI_CONTROL_AREA_OLD_COUNT]	= {};
        tri2s1_t			Triangles	[8]		= {};
    };
    struct SControlConstraints {
        n2cid				AttachSizeToControl	= {-1, -1};
        n2b8_t				AttachSizeToText	= {};
        rectcid				DockToControl		= {-1, -1, -1, -1};
        minmax2s1_t			SizeMinMax			= {{}, {0x7FFF, 0x7FFF}};
    };
    struct SControlText {
        vcst_t				Text				= "";
        ALIGN				Align				= ALIGN_CENTER;
        minmaxu1_t			Selection			= {0, 1};
        s1_t				FontSelected		= -1;
    };
    struct SControlTheme {
        astatic<astu2_t<UI_CONTROL_AREA_COUNT>, GUI_CONTROL_PALETTE_COUNT>
                            ColorCombos			= {};
    };
    struct SControlImage {
        n2s1_t				ImageOffset			= {};
        u0_t				ImageAlign			= ALIGN_CENTER;
        u0_t				ImageInvertX: 1;
        u0_t				ImageInvertY: 1;
        u0_t				Rotation	: 2;
    };
    
#pragma pack(pop)
} // nmsp

#endif // GPK_GUI_CONTROL_H_23627
