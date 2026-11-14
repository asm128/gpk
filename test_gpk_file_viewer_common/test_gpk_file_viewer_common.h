#include "gpk_framework.h"
#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control_list.h"
#include "gpk_gui_inputbox.h"
#include "gpk_path.h"

#ifndef TEST_GPK_FILE_VIEWER_COMMON_H_23627
#define TEST_GPK_FILE_VIEWER_COMMON_H_23627

usns gpk;

GDEFINE_ENUM_TYPE (FILE_VIEWER_LAYOUT, u0_t);
GDEFINE_ENUM_VALUE(FILE_VIEWER_LAYOUT, FLOATING, 0);
GDEFINE_ENUM_VALUE(FILE_VIEWER_LAYOUT, SPLIT   , 1);

stct SPathList {
	SControlList		Files				= {};
	SPathTree			PathContents		= {};
};
stct SFileEditor {
	SControlList		Lines				= {};
	string				FileText			= {};
	apod<vcst_t>		FileLines			= {};
	n2u2_t				Cursor				= {};
	cid_t				IdCursor			= -1;
	cid_t				IdSave				= -1;
	u2_t				FileLineWidthMax	= 0;
};
stct SFileViewer {
	pobj<SDialog>		Dialog				= {};
	astcid<2>			Viewports			= {};
	SVirtualKeyboard	VirtualKeyboard		= {};
	SFileEditor			Editor				=  {};
	SPathList 			Directory			=  {};
};

err_t	fileViewerSetup		(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vcst_t rootPath = {});
err_t	fileViewerUpdate	(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vpobj<SEventSystem> frameEvents);
err_t	fileViewerDraw		(SFileViewer & viewer, g8bgra target);

#endif // TEST_GPK_FILE_VIEWER_COMMON_H_23627
