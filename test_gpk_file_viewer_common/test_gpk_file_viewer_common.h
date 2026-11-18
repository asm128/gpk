#include "gpk_framework.h"
#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control_list.h"
#include "gpk_gui_inputbox.h"
#include "gpk_append_json.h"
#include "gpk_json_expression.h"
#include "gpk_path.h"
#include "gpk_range.h"

#ifndef TEST_GPK_FILE_VIEWER_COMMON_H_23627
#define TEST_GPK_FILE_VIEWER_COMMON_H_23627

usns gpk;

GDEFINE_ENUM_TYPE (FILE_VIEWER_LAYOUT, u0_t);
GDEFINE_ENUM_VALUE(FILE_VIEWER_LAYOUT, FLOATING, 0);
GDEFINE_ENUM_VALUE(FILE_VIEWER_LAYOUT, SPLIT   , 1);

stct SPathList {
	cid_t				IdViewport			= -1;
	SControlList		Files				= {};
	SPathTree			PathContents		= {};
};
stct SFileEditor {
	cid_t				IdViewport			= -1;
	SControlList		Lines				= {};
	string				FilePath			= {};
	string				FileText			= {};
	apod<vcst_t>		FileLines			= {};
	apod<rangeu2_t>	TextSelections		= {};
	u2_t				SelectionAnchor		= (u2_t)-1;
	u2_t				SelectionActive		= (u2_t)-1;
	n2u2_t				Cursor				= {};
	cid_t				IdCursor			= -1;
	cid_t				IdSave				= -1;
	cid_t				IdKeyboard			= -1;
	u2_t				FileLineWidthMax	= 0;
};
stct SJSONExpressionTool {
	cid_t				IdViewport			= -1;
	SControlList		Inputs				= {};
	SControlList		Presets				= {};
	aobj<string>		PresetExpressions	= {};
	aobj<acid>			GUISystems			= {};
	cid_t				IdSuggestions		= -1;
	SJSONReader			InputJSON			= {};
	cnst SJSONNode		* SuggestionNode	= 0;
	aobj<string>		SuggestionText		= {};
	aobj<string>		SuggestionExpressions = {};
	apod<u2_t>			SuggestionChildSlots = {};
	apod<u2_t>			SuggestionVisible	= {};
	string				SuggestionBase		= {};
	string				SuggestionSource	= {};
	SInputBox			Command				= {};
	SFileEditor			Draft				= {};
	cid_t				IdRun				= -1;
	cid_t				InputEditor			= -1;
};
stct SFileViewer {
	pobj<SDialog>		Dialog				= {};
	SVirtualKeyboard	VirtualKeyboard		= {};
	SControlList		EditorTabs			= {};
	apobj<SFileEditor>	Editors				= {};
	apobj<SPathList>	Directories			= {};
	SJSONExpressionTool JSONExpressions		= {};
	string				RootPath			= {};
	cid_t				IdEditorCurrent		= -1;
	cid_t				IdMenuFiles			= -1;
	cid_t				IdMenuJSON			= -1;
	cid_t				IdMenuExit			= -1;
};

err_t	fileViewerSetup		(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vcst_t rootPath = {});
err_t	fileViewerUpdate	(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vpobj<SEventSystem> frameEvents);
err_t	fileViewerDraw		(SFileViewer & viewer, g8bgra target);

#endif // TEST_GPK_FILE_VIEWER_COMMON_H_23627
