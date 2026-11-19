#include "gpk_framework.h"
#include "gpk_gui_json.h"
#include "gpk_gui_keyboard.h"
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

stct SFileScroll {
	cid_t				IdRoot				= -1;
	cid_t				IdDecrease			= -1;
	cid_t				IdIncrease			= -1;
	cid_t				IdThumb				= -1;
	minmax<s3_t>		Limits				= {};
	s3_t				Value				= 0;
};

stct SFileViewport {
	cid_t				IdRoot				= -1;
	cid_t				IdTitle			= -1;
	cid_t				IdClient			= -1;
	cid_t				IdClose			= -1;
	cid_t				IdMinimize			= -1;
	cid_t				IdResize			= -1;
	SFileScroll			ScrollX				= {};
	SFileScroll			ScrollY				= {};
};

stct SFileControlList {
	cid_t				IdRoot				= -1;
	acid				IdControls			= {};
};

stct SFileKeyboard {
	cid_t				IdRoot				= -1;
	acid				IdCharacters			= {};
	acid				IdActions			= {};
};

stct SFileCommand {
	cid_t				IdRoot				= -1;
	cid_t				IdText				= -1;
	string				Text				= {};
	u2_t				Cursor				= 0;
	u2_t				MaxLength			= 4096;
	b8_t				Editing				= false;
};

stct SPathList {
	SFileViewport		Viewport			= {};
	SFileControlList	Files				= {};
	SPathTree			PathContents		= {};
};

stct SFileEditor {
	SFileViewport		Viewport			= {};
	SFileControlList	Lines				= {};
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
	cid_t				IdTab				= -1;
	u2_t				FileLineWidthMax	= 0;
};

stct SJSONExpressionTool {
	SFileViewport		Viewport			= {};
	SFileControlList	Inputs				= {};
	SFileControlList	Presets				= {};
	aobj<string>		PresetExpressions	= {};
	cid_t				IdSuggestions		= -1;
	acid				IdSuggestionOptions	= {};
	SJSONReader			InputJSON			= {};
	cnst SJSONNode		* SuggestionNode	= 0;
	aobj<string>		SuggestionText		= {};
	aobj<string>		SuggestionExpressions = {};
	apod<u2_t>			SuggestionChildSlots = {};
	apod<u2_t>			SuggestionVisible	= {};
	string				SuggestionBase		= {};
	string				SuggestionSource	= {};
	SFileCommand		Command				= {};
	SFileEditor		Draft				= {};
	cid_t				IdRun				= -1;
	cid_t				InputEditor			= -1;
};

stct SFileViewer {
	SJSONFile			GUIJSON				= {};
	SFileKeyboard		Keyboard			= {};
	SFileControlList	EditorTabs			= {};
	apobj<SFileEditor>	Editors				= {};
	apobj<SPathList>	Directories			= {};
	SJSONExpressionTool JSONExpressions		= {};
	string				RootPath			= {};
	cid_t				IdEditorCurrent		= -1;
	cid_t				IdMenuFiles			= -1;
	cid_t				IdMenuJSON			= -1;
	cid_t				IdMenuExit			= -1;
};

err_t	fileViewerSetup		(SGUI & gui, SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vcst_t rootPath = {});
err_t	fileViewerUpdate	(SGUI & gui, cnst SInput & input, SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vpobj<SEventSystem> frameEvents);
err_t	fileViewerDraw		(SGUI & gui, SFileViewer & viewer, g8bgra target);

#endif // TEST_GPK_FILE_VIEWER_COMMON_H_23627
