#include "gpk_engine_explorer_app_menu.h"
#include "gpk_engine_gui.h"
#include "gpk_gui_inputbox.h"

#ifndef GPK_ENGINE_EXPLORER_H
#define GPK_ENGINE_EXPLORER_H

namespace gpk
{
	struct SEditorRigidBody		{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorImage			{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorMesh			{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorScene			{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorBuffer		{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorSurface		{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorPalette		{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorControl		{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorControlText	{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorLayout		{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };
	struct SEditorSettings		{ ::gpk::acid IdControl = {}; int32_t IdEditable = -1; };

	struct SEditorApplication {
		::gpk::SUIInputBox	InputBox			= {};
		::gpk::SEngineGUI	GUI					= {};

		::gpk::apapodc		InputHistory;
	};

	struct SEditor : public SEditorApplication {
		::gpk::ai32			Dialogs;
		::gpk::ai32			Buttons;
		::gpk::ai32			Hover;
		::gpk::ai32			Selection;
		::gpk::ai32			Expand;

		SEditorRigidBody	EditorRigidBody		= {};

		SEditorImage		EditorImage			= {};
		SEditorMesh			EditorMesh			= {};
		SEditorScene		EditorScene			= {};
		SEditorBuffer		EditorBuffer		= {};
		SEditorSurface		EditorSurface		= {};

		SEditorPalette		EditorPalette		= {};
		SEditorLayout		EditorLayout		= {};
		SEditorControl		EditorControl		= {};
		SEditorControlText	EditorControlText	= {};
	};

	::gpk::error_t		editorCreate	(::gpk::SEditor & editor);
	::gpk::error_t		editorUpdate	(::gpk::SEditor & editor, ::gpk::SInput & input, ::gpk::view<::gpk::SSysEvent> systemEvents);
	//::gpk::error_t	editorCreate	(::gpk::SGUI & gui, ::gpk::SLayoutEditor & editor);
	//::gpk::error_t	editorCreate	(::gpk::SGUI & gui, ::gpk::SPaletteEditor & editor);
} // namespace

#endif // GPK_ENGINE_EXPLORER_H
