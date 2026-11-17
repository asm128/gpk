#include "gpk_framework.h"
#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_inputbox.h"
#include "gpk_path.h"
#include "gpk_range.h"

#ifndef SIDE_H_23627
#define SIDE_H_23627

usns gpk;

GDEFINE_ENUM_TYPE (FILE_VIEWER_LAYOUT, u0_t);
GDEFINE_ENUM_VALUE(FILE_VIEWER_LAYOUT, FLOATING, 0);
GDEFINE_ENUM_VALUE(FILE_VIEWER_LAYOUT, SPLIT   , 1);

stct SPathList {
	cid_t				IdViewport			= -1;
	cid_t			Files				= -1;
	SPathTree			PathContents		= {};
};

stct SFileEditor {
	cid_t				IdViewport			= -1;
	cid_t			Lines				= -1;
	string				FilePath			= {};
	string				FileText			= {};
	apod<vcst_t>		FileLines			= {};
	apod<rangeu2_t>		TextSelections		= {};
	u2_t				SelectionAnchor		= (u2_t)-1;
	u2_t				SelectionActive		= (u2_t)-1;
	n2u2_t				Cursor				= {};
	cid_t				IdCursor			= -1;
	cid_t				IdSave				= -1;
	cid_t				IdKeyboard			= -1;
	u2_t				FileLineWidthMax	= 0;
};
stct SFileViewer {
	pobj<SDialog>		Dialog				= {};
	SVirtualKeyboard	VirtualKeyboard		= {};
	cid_t			EditorTabs			= -1;
	apobj<SFileEditor>	Editors				= {};
	apobj<SPathList>	Directories			= {};
	string				RootPath			= {};
	cid_t				IdEditorCurrent		= -1;
	cid_t				IdMenuFiles			= -1;
	cid_t				IdMenuExit			= -1;
};

err_t	fileViewerSetup		(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vcst_t rootPath = {});
err_t	fileViewerUpdate	(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vpobj<SEventSystem> frameEvents);
err_t	fileViewerDraw		(SFileViewer & viewer, g8bgra target);

#endif // SIDE_H_23627
