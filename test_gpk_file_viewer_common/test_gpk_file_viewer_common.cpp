#include "test_gpk_file_viewer_common.h"

#include "gpk_event_input.h"
#include "gpk_file.h"
#include "gpk_framework_module.h"
#include "gpk_gui_text.h"
#include "gpk_gui_theme.h"
#include "gpk_label.h"
#include "gpk_path.h"

stxp u2_c JSON_EXPRESSION_PRESET_LIMIT = 9;
stxp u2_c JSON_EXPRESSION_SUGGESTION_LIMIT = 12;

stxp cnst astu2_t<128> CP437_UNICODE_HIGH =
	{ 0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7
	, 0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5
	, 0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9
	, 0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192
	, 0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA
	, 0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB
	, 0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556
	, 0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510
	, 0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F
	, 0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567
	, 0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B
	, 0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580
	, 0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4
	, 0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229
	, 0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248
	, 0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
	};

sttc gpk::err_t unicodeToCP437(u2_t codePoint, u0_t & output) {
	if(codePoint < 0x80) {
		output = (u0_t)codePoint;
		rtrn 0;
	}
	for(u2_t iCode = 0; iCode < CP437_UNICODE_HIGH.size(); ++iCode)
		if(CP437_UNICODE_HIGH[iCode] == codePoint) {
			output = (u0_t)(0x80 + iCode);
			rtrn 0;
		}
	rtrn -1;
}

sttc gpk::cid_t controlIdentifierFind(cnst SControlTable & controls, vcst_t identifier) {
	for(u2_t iControl = 0; iControl < controls.Identifier.size(); ++iControl)
		if(controls.Identifier[iControl] == identifier)
			rtrn (cid_t)iControl;
	rtrn CID_INVALID;
}

sttc gpk::err_t identifierCompose(string & output, vcst_t prefix, vcst_t suffix) {
	output.clear();
	if_fail_fe(output.append(prefix));
	rtrn output.append(suffix);
}

sttc gpk::err_t indexedIdentifier(string & output, vcst_t prefix, u2_t index) {
	output.clear();
	rtrn append_strings(output, prefix, str(index));
}

sttc gpk::err_t controlIdentifierRequire(cnst SControlTable & controls, vcst_t identifier, cid_t & output) {
	output = ::controlIdentifierFind(controls, identifier);
	if_true_fef(controlInvalid(controls, output), "Missing JSON control '%.*s'.", GPK_VIEW_ARGS(identifier));
	rtrn output;
}

sttc gpk::err_t fileViewerListBind(cnst SControlTable & controls, vcst_t identifier, SFileControlList & output) {
	if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdRoot));
	output.IdControls = controls.Children[output.IdRoot];
	rtrn output.IdControls.size();
}

sttc gpk::err_t fileViewerScrollBind(cnst SControlTable & controls, vcst_t prefix, SFileScroll & output) {
	string identifier = {};
	if_fail_fe(::controlIdentifierRequire(controls, prefix, output.IdRoot));
	if_fail_fe(::identifierCompose(identifier, prefix, "_decrease"));
	if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdDecrease));
	if_fail_fe(::identifierCompose(identifier, prefix, "_increase"));
	if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdIncrease));
	if_fail_fe(::identifierCompose(identifier, prefix, "_thumb"));
	if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdThumb));
	rtrn 0;
}

sttc gpk::err_t fileViewerViewportBind(cnst SControlTable & controls, vcst_t prefix, SFileViewport & output) {
	string identifier = {};
	if_fail_fe(::controlIdentifierRequire(controls, prefix, output.IdRoot));
	if_fail_fe(::identifierCompose(identifier, prefix, "_title"));
	if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdTitle));
	if_fail_fe(::identifierCompose(identifier, prefix, "_client"));
	if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdClient));
	if_fail_fe(::identifierCompose(identifier, prefix, "_close"));
	if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdClose));
	if_fail_fe(::identifierCompose(identifier, prefix, "_scroll_x"));
	if_fail_fe(::fileViewerScrollBind(controls, identifier, output.ScrollX));
	if_fail_fe(::identifierCompose(identifier, prefix, "_scroll_y"));
	if_fail_fe(::fileViewerScrollBind(controls, identifier, output.ScrollY));
	rtrn 0;
}

sttc gpk::err_t fileViewerDirectoryBind(cnst SControlTable & controls, vcst_t prefix, SPathList & output) {
	string identifier = {};
	if_fail_fe(::fileViewerViewportBind(controls, prefix, output.Viewport));
	if_fail_fe(::identifierCompose(identifier, prefix, "_rows"));
	rtrn ::fileViewerListBind(controls, identifier, output.Files);
}

sttc gpk::err_t fileViewerEditorBind(cnst SControlTable & controls, vcst_t prefix, SFileEditor & output, b8_t toolbar) {
	string identifier = {};
	if_fail_fe(::fileViewerViewportBind(controls, prefix, output.Viewport));
	if_fail_fe(::identifierCompose(identifier, prefix, "_rows"));
	if_fail_fe(::fileViewerListBind(controls, identifier, output.Lines));
	if_fail_fe(::identifierCompose(identifier, prefix, "_cursor"));
	if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdCursor));
	if(toolbar) {
		if_fail_fe(::identifierCompose(identifier, prefix, "_save"));
		if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdSave));
		if_fail_fe(::identifierCompose(identifier, prefix, "_keyboard"));
		if_fail_fe(::controlIdentifierRequire(controls, identifier, output.IdKeyboard));
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerKeyboardBind(cnst SControlTable & controls, SFileKeyboard & output) {
	if_fail_fe(::controlIdentifierRequire(controls, "virtual_keyboard", output.IdRoot));
	cid_t idCharacters = -1;
	cid_t idActions = -1;
	if_fail_fe(::controlIdentifierRequire(controls, "virtual_keyboard_characters", idCharacters));
	if_fail_fe(::controlIdentifierRequire(controls, "virtual_keyboard_actions", idActions));
	output.IdCharacters = controls.Children[idCharacters];
	output.IdActions = controls.Children[idActions];
	if_true_fef(output.IdCharacters.size() != GUI_KEYBOARD_Default_keys.size(), "Keyboard character controls:%u, expected:%u.", output.IdCharacters.size(), GUI_KEYBOARD_Default_keys.size());
	if_true_fef(output.IdActions.size() < get_value_count<VK_SCANCODE>(), "Keyboard action controls:%u, expected:%u.", output.IdActions.size(), get_value_count<VK_SCANCODE>());
	rtrn 0;
}

sttc gpk::err_t fileViewerScrollSet(SFileScroll & scroll, s3_t value) {
	scroll.Value = max(scroll.Limits.Min, min(scroll.Limits.Max, value));
	rtrn 0;
}

sttc gpk::err_t fileViewerScrollUpdate(SGUI & gui, cnst SInput & input, SFileScroll & scroll, b8_t vertical, b8_t wheel) {
	if(controlInvalid(gui, scroll.IdRoot))
		rtrn 0;
	if(gui.Controls.IsAction(scroll.IdDecrease))
		if_fail_fe(::fileViewerScrollSet(scroll, scroll.Value - 1));
	if(gui.Controls.IsAction(scroll.IdIncrease))
		if_fail_fe(::fileViewerScrollSet(scroll, scroll.Value + 1));
	if(wheel && input.MouseCurrent.Deltas.z)
		if_fail_fe(::fileViewerScrollSet(scroll, scroll.Value - input.MouseCurrent.Deltas.z / 120));
	cnst b8_t visible = scroll.Limits.Max > scroll.Limits.Min;
	gui.Controls.SetHidden(scroll.IdRoot, false == visible);
	if(false == visible)
		rtrn 0;
	SControlPlacement & root = gui.Controls.Placement[scroll.IdRoot];
	SControlPlacement & thumb = gui.Controls.Placement[scroll.IdThumb];
	cnst s3_t span = scroll.Limits.Max - scroll.Limits.Min;
	cnst s1_t track = vertical
		? max((s1_t)0, (s1_t)(root.Area.Size.y - 36 - thumb.Area.Size.y))
		: max((s1_t)0, (s1_t)(root.Area.Size.x - 36 - thumb.Area.Size.x));
	cnst s1_t position = (s1_t)(18 + (span ? track * (scroll.Value - scroll.Limits.Min) / span : 0));
	if(vertical) thumb.Area.Offset.y = position;
	else thumb.Area.Offset.x = position;
	rtrn controlMetricsInvalidate(gui, scroll.IdRoot);
}

sttc b8_t fileViewerFolderHasParent(cnst SPathTree & paths) {
	vcst_t parent = {};
	rtrn 0 <= pathDirectory(paths.Root, parent) && parent.size() && parent != paths.Root;
}

sttc gpk::err_t fileViewerTextViewsClear(SGUI & gui, SFileEditor & editor) {
	for(u2_t iControl = 0; iControl < editor.Lines.IdControls.size(); ++iControl)
		if_fail_fe(controlTextSet(gui, editor.Lines.IdControls[iControl], {}));
	editor.FileLines.clear();
	editor.FileLineWidthMax = 0;
	rtrn 0;
}

sttc gpk::err_t fileViewerTextViewsRebuild(SFileEditor & editor) {
	editor.FileLines.clear();
	editor.FileLineWidthMax = 0;
	vcst_t remaining = editor.FileText;
	do {
		vcst_t line = {};
		vcst_t next = {};
		cnst gpk::err_t splitResult = split('\n', remaining, line, next);
		if(line.size() && '\r' == line[line.size() - 1])
			if_fail_fe(line.slice(line, 0, line.size() - 1));
		if_fail_fe(editor.FileLines.push_back(line));
		editor.FileLineWidthMax = max(editor.FileLineWidthMax, line.size());
		if(0 > splitResult)
			break;
		remaining = next;
	} while(true);
	rtrn 0;
}

sttc u2_t fileViewerLineOffset(cnst SFileEditor & editor, u2_t line) {
	u2_t offset = 0;
	u2_t iLine = 0;
	while(offset < editor.FileText.size() && iLine < line)
		if('\n' == editor.FileText[offset++])
			++iLine;
	rtrn offset;
}

sttc u2_t fileViewerCursorOffset(cnst SFileEditor & editor) {
	if(editor.Cursor.y >= editor.FileLines.size())
		rtrn editor.FileText.size();
	vcst_c line = editor.FileLines[editor.Cursor.y];
	rtrn ::fileViewerLineOffset(editor, editor.Cursor.y) + min(editor.Cursor.x, line.size());
}

sttc void fileViewerCursorOffsetSet(SFileEditor & editor, u2_t offset) {
	editor.Cursor = {};
	for(u2_t iCharacter = 0, countCharacter = min(offset, editor.FileText.size()); iCharacter < countCharacter; ++iCharacter) {
		if('\n' == editor.FileText[iCharacter]) {
			++editor.Cursor.y;
			editor.Cursor.x = 0;
		}
		else if('\r' != editor.FileText[iCharacter] || iCharacter + 1 >= editor.FileText.size() || '\n' != editor.FileText[iCharacter + 1])
			++editor.Cursor.x;
	}
}

sttc gpk::err_t fileViewerTextInsert(SGUI & gui, SFileEditor & editor, sc_t character) {
	u2_c offset = ::fileViewerCursorOffset(editor);
	if_fail_fe(::fileViewerTextViewsClear(gui, editor));
	if_fail_fe(editor.FileText.insert(offset, character));
	editor.TextSelections.clear();
	if_fail_fe(::fileViewerTextViewsRebuild(editor));
	if('\n' == character) {
		++editor.Cursor.y;
		editor.Cursor.x = 0;
	}
	else
		++editor.Cursor.x;
	rtrn 0;
}

sttc gpk::err_t fileViewerTextBackspace(SGUI & gui, SFileEditor & editor) {
	if(editor.Cursor.y >= editor.FileLines.size())
		rtrn 0;
	u2_c cursorOffset = ::fileViewerCursorOffset(editor);
	if(editor.Cursor.x) {
		if_fail_fe(::fileViewerTextViewsClear(gui, editor));
		if_fail_fe(editor.FileText.remove(cursorOffset - 1));
		--editor.Cursor.x;
	}
	else if(editor.Cursor.y) {
		vcst_c previousLine = editor.FileLines[editor.Cursor.y - 1];
		u2_c separatorOffset = ::fileViewerLineOffset(editor, editor.Cursor.y - 1) + previousLine.size();
		u2_c separatorCount = cursorOffset - separatorOffset;
		editor.Cursor.x = previousLine.size();
		--editor.Cursor.y;
		if_fail_fe(::fileViewerTextViewsClear(gui, editor));
		for(u2_t iCharacter = 0; iCharacter < separatorCount; ++iCharacter)
			if_fail_fe(editor.FileText.remove(separatorOffset));
	}
	editor.TextSelections.clear();
	rtrn ::fileViewerTextViewsRebuild(editor);
}

sttc gpk::err_t fileViewerTextDelete(SGUI & gui, SFileEditor & editor) {
	if(editor.TextSelections.size()) {
		apod<rangeu2_t> deletions = {};
		u2_t cursorOffset = editor.FileText.size();
		for(u2_t iSelection = 0; iSelection < editor.TextSelections.size(); ++iSelection) {
			rangeu2_c selection = editor.TextSelections[iSelection];
			u2_c selectionBegin = min(selection.Offset, editor.FileText.size());
			u2_c selectionEnd = (u2_t)min((u3_t)editor.FileText.size(), (u3_t)selection.Offset + selection.Count);
			if(selectionBegin < selectionEnd) {
				if_fail_fe(deletions.push_back({selectionBegin, selectionEnd - selectionBegin}));
				cursorOffset = min(cursorOffset, selectionBegin);
			}
		}
		if(deletions.size()) {
			for(u2_t iDeletion = 1; iDeletion < deletions.size(); ++iDeletion) {
				rangeu2_c deletion = deletions[iDeletion];
				u2_t iPrevious = iDeletion;
				while(iPrevious && deletions[iPrevious - 1].Offset > deletion.Offset) {
					deletions[iPrevious] = deletions[iPrevious - 1];
					--iPrevious;
				}
				deletions[iPrevious] = deletion;
			}
			u2_t countMerged = 1;
			for(u2_t iDeletion = 1; iDeletion < deletions.size(); ++iDeletion) {
				rangeu2_t & previous = deletions[countMerged - 1];
				rangeu2_c current = deletions[iDeletion];
				u3_c previousEnd = (u3_t)previous.Offset + previous.Count;
				u3_c currentEnd = (u3_t)current.Offset + current.Count;
				if(current.Offset <= previousEnd)
					previous.Count = (u2_t)(max(previousEnd, currentEnd) - previous.Offset);
				else
					deletions[countMerged++] = current;
			}
			if_fail_fe(deletions.resize(countMerged));
			if_fail_fe(::fileViewerTextViewsClear(gui, editor));
			for(u2_t iDeletion = deletions.size(); iDeletion; --iDeletion)
				for(u2_t iCharacter = 0; iCharacter < deletions[iDeletion - 1].Count; ++iCharacter)
					if_fail_fe(editor.FileText.remove(deletions[iDeletion - 1].Offset));
			editor.TextSelections.clear();
			if_fail_fe(::fileViewerTextViewsRebuild(editor));
			::fileViewerCursorOffsetSet(editor, cursorOffset);
			rtrn 0;
		}
		editor.TextSelections.clear();
	}
	if(editor.Cursor.y >= editor.FileLines.size())
		rtrn 0;
	u2_c cursorOffset = ::fileViewerCursorOffset(editor);
	if(cursorOffset >= editor.FileText.size())
		rtrn 0;
	u2_t count = ('\r' == editor.FileText[cursorOffset] && cursorOffset + 1 < editor.FileText.size() && '\n' == editor.FileText[cursorOffset + 1]) ? 2 : 1;
	if_fail_fe(::fileViewerTextViewsClear(gui, editor));
	for(u2_t iCharacter = 0; iCharacter < count; ++iCharacter)
		if_fail_fe(editor.FileText.remove(cursorOffset));
	editor.TextSelections.clear();
	rtrn ::fileViewerTextViewsRebuild(editor);
}

sttc gpk::err_t fileViewerEditorHandle(SGUI & gui, SFileViewer & viewer, SFileEditor & editor, cnst SGUIKeyboardEvent & eventToProcess) {
	if(editor.Cursor.y >= editor.FileLines.size())
		rtrn 0;
	if(VK_EVENT_Edit != eventToProcess.Type)
		rtrn 0;
	switch(eventToProcess.ScanCode) {
	default: break;
	case VK_SCANCODE_Escape:
		gui.Controls.SetHidden(viewer.Keyboard.IdRoot, true);
		break;
	case VK_SCANCODE_Delete		: if_fail_fe(::fileViewerTextDelete(gui, editor)); break;
	case VK_SCANCODE_Enter		: if_fail_fe(::fileViewerTextInsert(gui, editor, '\n')); break;
	case VK_SCANCODE_Backspace	: if_fail_fe(::fileViewerTextBackspace(gui, editor)); break;
	case VK_SCANCODE_Clear:
		if_fail_fe(::fileViewerTextViewsClear(gui, editor));
		editor.FileText.clear();
		editor.TextSelections.clear();
		editor.Cursor = {};
		if_fail_fe(::fileViewerTextViewsRebuild(editor));
		break;
	case VK_SCANCODE_Left:
		if(editor.Cursor.x)
			--editor.Cursor.x;
		else if(editor.Cursor.y) {
			--editor.Cursor.y;
			editor.Cursor.x = editor.FileLines[editor.Cursor.y].size();
		}
		break;
	case VK_SCANCODE_Right:
		if(editor.Cursor.x < editor.FileLines[editor.Cursor.y].size())
			++editor.Cursor.x;
		else if(editor.Cursor.y + 1 < editor.FileLines.size()) {
			++editor.Cursor.y;
			editor.Cursor.x = 0;
		}
		break;
	case VK_SCANCODE_Up:
		if(editor.Cursor.y) {
			--editor.Cursor.y;
			editor.Cursor.x = min(editor.Cursor.x, editor.FileLines[editor.Cursor.y].size());
		}
		break;
	case VK_SCANCODE_Down:
		if(editor.Cursor.y + 1 < editor.FileLines.size()) {
			++editor.Cursor.y;
			editor.Cursor.x = min(editor.Cursor.x, editor.FileLines[editor.Cursor.y].size());
		}
		break;
	case VK_SCANCODE_Home:
		editor.Cursor.x = 0;
		break;
	case VK_SCANCODE_End:
		editor.Cursor.x = editor.FileLines[editor.Cursor.y].size();
		break;
	case VK_SCANCODE_Page_Up:
		editor.Cursor.y = editor.Cursor.y > 20 ? editor.Cursor.y - 20 : 0;
		editor.Cursor.x = min(editor.Cursor.x, editor.FileLines[editor.Cursor.y].size());
		break;
	case VK_SCANCODE_Page_Down:
		editor.Cursor.y = min((u2_t)(editor.FileLines.size() - 1), (u2_t)(editor.Cursor.y + 20));
		editor.Cursor.x = min(editor.Cursor.x, editor.FileLines[editor.Cursor.y].size());
		break;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorPhysicalInput(SGUI & gui, SFileViewer & viewer, SFileEditor & editor, vpobj<SEventSystem> frameEvents) {
	for(u2_t iEvent = 0; iEvent < frameEvents.size(); ++iEvent) {
		cnst pobj<SEventSystem> & systemEvent = frameEvents[iEvent];
		if(systemEvent->Type == SYSTEM_EVENT_Text) {
			if_fail_fe(eventExtractAndHandle<EVENT_TEXT>(*systemEvent, [&gui, &editor](cnst SEventView<EVENT_TEXT> & eventText) {
				if(eventText.Type != EVENT_TEXT_Char || eventText.Data.size() < sizeof(u2_t))
					rtrn 0;
				u2_t codePoint = {};
				memcpy(&codePoint, eventText.Data.begin(), sizeof(codePoint));
				u0_t character = {};
				if(0 <= ::unicodeToCP437(codePoint, character))
					if_fail_fe(::fileViewerTextInsert(gui, editor, (sc_t)character));
				rtrn 0;
			}));
		}
		else if(systemEvent->Type == SYSTEM_EVENT_Keyboard) {
			if_fail_fe(eventExtractAndHandle<EVENT_KEYBOARD>(*systemEvent, [&gui, &viewer, &editor](cnst SEventView<EVENT_KEYBOARD> & eventKeyboard) {
				if(eventKeyboard.Type != EVENT_KEYBOARD_Down || 0 == eventKeyboard.Data.size())
					rtrn 0;
				VK_SCANCODE scanCode = {};
				switch(eventKeyboard.Data[0]) {
				default: rtrn 0;
				case SYSTEM_KEY_Escape		: scanCode = VK_SCANCODE_Escape; break;
				case SYSTEM_KEY_Enter		: scanCode = VK_SCANCODE_Enter; break;
				case SYSTEM_KEY_Backspace	: scanCode = VK_SCANCODE_Backspace; break;
				case SYSTEM_KEY_Clear		: scanCode = VK_SCANCODE_Clear; break;
				case SYSTEM_KEY_Delete		: scanCode = VK_SCANCODE_Delete; break;
				case SYSTEM_KEY_Left		: scanCode = VK_SCANCODE_Left; break;
				case SYSTEM_KEY_Right		: scanCode = VK_SCANCODE_Right; break;
				case SYSTEM_KEY_Up			: scanCode = VK_SCANCODE_Up; break;
				case SYSTEM_KEY_Down		: scanCode = VK_SCANCODE_Down; break;
				case SYSTEM_KEY_Home		: scanCode = VK_SCANCODE_Home; break;
				case SYSTEM_KEY_End			: scanCode = VK_SCANCODE_End; break;
				case SYSTEM_KEY_Prior		: scanCode = VK_SCANCODE_Page_Up; break;
				case SYSTEM_KEY_Next		: scanCode = VK_SCANCODE_Page_Down; break;
				}
				cnst SGUIKeyboardEvent editorEvent = {VK_EVENT_Edit, scanCode};
				rtrn ::fileViewerEditorHandle(gui, viewer, editor, editorEvent);
			}));
		}
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerFolderLoad(SGUI & gui, SPathList & directory, vcst_t path) {
	for(u2_t iControl = 0; iControl < directory.Files.IdControls.size(); ++iControl) {
		if_fail_fe(controlTextSet(gui, directory.Files.IdControls[iControl], {}));
		gui.Controls.SetSelected(directory.Files.IdControls[iControl], false);
	}
	string absolutePath = {};
	if_fail_fe(pathAbsolute(path, absolutePath));
	if_fail_fe(pathList(directory.PathContents, absolutePath, [](b8_t folder, vcst_c &) { rtrn folder ? 2 : 0; }));
	directory.Viewport.ScrollY.Value = 0;
	if_fail_fe(controlTextSet(gui, directory.Viewport.IdTitle, directory.PathContents.Root));
	rtrn 0;
}

sttc gpk::err_t fileViewerTextLoad(SGUI & gui, SFileViewer & viewer, SFileEditor & editor, vcst_t path) {
	if_fail_fe(::fileViewerTextViewsClear(gui, editor));
	editor.FilePath = path;
	editor.FileText.clear();
	editor.TextSelections.clear();
	editor.SelectionAnchor = (u2_t)-1;
	editor.SelectionActive = (u2_t)-1;
	if_fail_fef(fileToMemory(path, editor.FileText), "Failed to load '%.*s'.", GPK_VIEW_ARGS(path));
	if_fail_fe(::fileViewerTextViewsRebuild(editor));
	editor.Cursor = {0, 0};
	editor.Viewport.ScrollX.Value = 0;
	editor.Viewport.ScrollY.Value = 0;
	gui.Controls.SetHidden(viewer.Keyboard.IdRoot, true);
	gui.Controls.SetHidden(editor.IdSave, false);
	gui.Controls.SetHidden(editor.IdKeyboard, false);
	if_fail_fe(controlTextSet(gui, editor.Viewport.IdTitle, label(path)));
	rtrn 0;
}

sttc gpk::err_t fileViewerListWindowUpdate(SGUI & gui, cnst SInput & input, SPathList & directory) {
	SFileControlList & list = directory.Files;
	cnst SPathTree & items = directory.PathContents;
	b8_c hasParent = ::fileViewerFolderHasParent(items);
	u2_c itemCount = items.Name.size() + one_if(hasParent);
	cnst s1_t rowHeight = list.IdControls.size() ? max((s1_t)1, gui.Controls.Placement[list.IdControls[0]].Area.Size.y) : 1;
	n2s1_t clientSize = gui.Controls.Area[directory.Viewport.IdClient].Client.Global.Size;
	if(0 == clientSize.y)
		clientSize = gui.Controls.Placement[directory.Viewport.IdClient].Area.Size;
	cnst s3_t visibleCount = min((s3_t)list.IdControls.size(), max((s3_t)1, (s3_t)clientSize.y / rowHeight));
	cnst s3_t maxOffset = max((s3_t)0, (s3_t)itemCount - visibleCount);
	directory.Viewport.ScrollY.Limits = {0, maxOffset};
	if_fail_fe(::fileViewerScrollSet(directory.Viewport.ScrollY, directory.Viewport.ScrollY.Value));
	cnst b8_t wheel = gui.Controls.IsHovered(directory.Viewport.IdRoot) || gui.Controls.IsHovered(directory.Viewport.IdClient);
	if_fail_fe(::fileViewerScrollUpdate(gui, input, directory.Viewport.ScrollY, true, wheel));
	cnst s3_t firstVisible = directory.Viewport.ScrollY.Value;
	for(u2_t iControl = 0; iControl < list.IdControls.size(); ++iControl) {
		cnst cid_t idControl = list.IdControls[iControl];
		u2_c iItem = (u2_t)firstVisible + iControl;
		cnst b8_t visible = iControl < (u2_t)visibleCount && iItem < itemCount;
		vcst_t text = {};
		if(visible)
			text = hasParent && 0 == iItem ? vcst_t{".."} : items.Name[iItem - one_if(hasParent)];
		if(gui.Controls.Text[idControl].Text != text)
			if_fail_fe(controlTextSet(gui, idControl, text));
		gui.Controls.SetHidden(idControl, false == visible);
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerTextWindowUpdate(SGUI & gui, cnst SInput & input, SFileEditor & editor) {
	if(gui.Controls.IsHidden(editor.Viewport.IdRoot))
		rtrn 0;
	if(0 == editor.Lines.IdControls.size())
		rtrn 0;
	cnst s1_t rowHeight = max((s1_t)1, gui.Controls.Placement[editor.Lines.IdControls[0]].Area.Size.y);
	n2s1_t clientSize = gui.Controls.Area[editor.Viewport.IdClient].Client.Global.Size;
	if(0 == (clientSize.x | clientSize.y))
		clientSize = gui.Controls.Placement[editor.Viewport.IdClient].Area.Size;
	cnst u0_t characterWidth = max((u0_t)1, gui.Fonts[gui.SelectedFont]->CharSize.x);
	s3_t visibleCount = min((s3_t)editor.Lines.IdControls.size(), max((s3_t)1, (s3_t)(clientSize.y - 18) / rowHeight));
	s3_t visibleColumns = max((s3_t)1, (s3_t)(clientSize.x - 18) / characterWidth);
	cnst s3_t maxOffsetY = max((s3_t)0, (s3_t)editor.FileLines.size() - visibleCount);
	b8_c cursorActive = editor.Cursor.y < editor.FileLines.size();
	cnst s3_t maxOffsetX = max((s3_t)0, (s3_t)editor.FileLineWidthMax + one_if(cursorActive) - visibleColumns);
	editor.Viewport.ScrollY.Limits = {0, maxOffsetY};
	editor.Viewport.ScrollX.Limits = {0, maxOffsetX};
	if(cursorActive) {
		if(editor.Cursor.y < (u2_t)editor.Viewport.ScrollY.Value)
			editor.Viewport.ScrollY.Value = editor.Cursor.y;
		else if(editor.Cursor.y >= (u2_t)(editor.Viewport.ScrollY.Value + visibleCount))
			editor.Viewport.ScrollY.Value = editor.Cursor.y - visibleCount + 1;
		if(editor.Cursor.x < (u2_t)editor.Viewport.ScrollX.Value)
			editor.Viewport.ScrollX.Value = editor.Cursor.x;
		else if(editor.Cursor.x >= (u2_t)(editor.Viewport.ScrollX.Value + visibleColumns))
			editor.Viewport.ScrollX.Value = editor.Cursor.x - visibleColumns + 1;
	}
	cnst b8_t wheel = gui.Controls.IsHovered(editor.Viewport.IdRoot) || gui.Controls.IsHovered(editor.Viewport.IdClient);
	if_fail_fe(::fileViewerScrollUpdate(gui, input, editor.Viewport.ScrollY, true, wheel));
	if_fail_fe(::fileViewerScrollUpdate(gui, input, editor.Viewport.ScrollX, false, false));
	u2_c firstVisible = (u2_t)editor.Viewport.ScrollY.Value;
	u2_c firstColumn = (u2_t)editor.Viewport.ScrollX.Value;
	for(u2_t iControl = 0; iControl < editor.Lines.IdControls.size(); ++iControl) {
		cnst cid_t idControl = editor.Lines.IdControls[iControl];
		u2_c iLine = firstVisible + iControl;
		b8_c visible = iControl < (u2_t)visibleCount && iLine < editor.FileLines.size();
		if(visible) {
			vcst_c sourceLine = editor.FileLines[iLine];
			u2_c offset = min(firstColumn, sourceLine.size());
			u2_c count = min((u2_t)visibleColumns, (u2_t)(sourceLine.size() - offset));
			vcst_t visibleText = {};
			if_fail_fe(sourceLine.slice(visibleText, offset, count));
			if(gui.Controls.Text[idControl].Text != visibleText)
				if_fail_fe(controlTextSet(gui, idControl, visibleText));
			apod<STextState> & textState = gui.Controls.TextState[idControl];
			if_fail_fe(textState.resize(visibleText.size()));
			u2_c lineOffset = ::fileViewerLineOffset(editor, iLine);
			for(u2_t iCharacter = 0; iCharacter < textState.size(); ++iCharacter) {
				u2_c characterOffset = lineOffset + offset + iCharacter;
				b8_t selected = false;
				for(u2_t iSelection = 0; iSelection < editor.TextSelections.size(); ++iSelection) {
					rangeu2_c selection = editor.TextSelections[iSelection];
					if(characterOffset >= selection.Offset && (u3_t)characterOffset < (u3_t)selection.Offset + selection.Count) {
						selected = true;
						break;
					}
				}
				textState[iCharacter].SetSelected(idControl, iCharacter, gui.Controls.EventQueue, selected);
				textState[iCharacter].SetCursor(idControl, iCharacter, gui.Controls.EventQueue, iLine == editor.Cursor.y && offset + iCharacter == editor.Cursor.x);
			}
		}
		else {
			gui.Controls.TextState[idControl].clear();
			if(gui.Controls.Text[idControl].Text.size())
				if_fail_fe(controlTextSet(gui, idControl, {}));
		}
		gui.Controls.SetSelected(idControl, visible && iLine == editor.Cursor.y);
		gui.Controls.SetHidden(idControl, false == visible);
	}
	b8_c cursorVisible = cursorActive
		&& editor.Cursor.y >= firstVisible && editor.Cursor.y < firstVisible + visibleCount
		&& editor.Cursor.x >= firstColumn && editor.Cursor.x < firstColumn + visibleColumns;
	if(cursorVisible) {
		SControlPlacement & cursorPlacement = gui.Controls.Placement[editor.IdCursor];
		cursorPlacement.Area.Offset = {(s1_t)((editor.Cursor.x - firstColumn) * characterWidth), (s1_t)((editor.Cursor.y - firstVisible) * rowHeight)};
		cursorPlacement.Area.Size = {(s1_t)characterWidth, rowHeight};
		if_fail_fe(controlMetricsInvalidate(gui, editor.IdCursor));
	}
	gui.Controls.SetHidden(editor.IdCursor, false == cursorVisible);
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorMousePosition(SGUI & gui, cnst SInput & input, SFileEditor & editor, n2u2_t & cursor, u2_t & characterOffset, b8_t & overTextLine) {
	overTextLine = false;
	u2_c firstVisible = (u2_t)editor.Viewport.ScrollY.Value;
	u2_c firstColumn = (u2_t)editor.Viewport.ScrollX.Value;
	u0_c characterWidth = max((u0_t)1, gui.Fonts[gui.SelectedFont]->CharSize.x);
	for(u2_t iControl = 0; iControl < editor.Lines.IdControls.size(); ++iControl) {
		cnst cid_t idControl = editor.Lines.IdControls[iControl];
		if(gui.Controls.IsHidden(idControl) || false == gui.Controls.IsHovered(idControl))
			continue;
		u2_c iLine = firstVisible + iControl;
		if(iLine >= editor.FileLines.size())
			continue;
		cnst s2_t mouseOffsetX = (s2_t)input.MouseCurrent.Position.x - gui.Controls.Area[idControl].Text.Offset.x;
		u2_c visibleColumn = 0 >= mouseOffsetX ? 0 : (u2_t)(mouseOffsetX / characterWidth);
		u2_c iColumn = min((u2_t)(firstColumn + visibleColumn), editor.FileLines[iLine].size());
		cursor = {iColumn, iLine};
		characterOffset = ::fileViewerLineOffset(editor, iLine) + iColumn;
		overTextLine = true;
		break;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorSelectionUpdate(SGUI & gui, cnst SInput & input, SFileViewer & viewer, SFileEditor & editor, cid_t iEditor) {
	if(gui.Controls.IsHidden(editor.Viewport.IdRoot))
		rtrn 0;
	n2u2_t cursor = {};
	u2_t characterOffset = 0;
	b8_t overTextLine = false;
	if_fail_fe(::fileViewerEditorMousePosition(gui, input, editor, cursor, characterOffset, overTextLine));
	if(input.ButtonDown(0) && overTextLine) {
		viewer.IdEditorCurrent = iEditor;
		editor.Cursor = cursor;
		if(0 == input.KeyboardCurrent.KeyState[SYSTEM_KEY_Control])
			editor.TextSelections.clear();
		editor.SelectionAnchor = characterOffset;
		err_t iSelection = editor.TextSelections.push_back({characterOffset, 0});
		if_fail_fe(iSelection);
		editor.SelectionActive = (u2_t)iSelection;
	}
	if((u2_t)-1 != editor.SelectionAnchor && editor.SelectionActive < editor.TextSelections.size() && overTextLine) {
		editor.Cursor = cursor;
		rangeu2_t & selection = editor.TextSelections[editor.SelectionActive];
		selection.Offset = min(editor.SelectionAnchor, characterOffset);
		selection.Count = max(editor.SelectionAnchor, characterOffset) - selection.Offset;
	}
	if(input.ButtonUp(0) && (u2_t)-1 != editor.SelectionAnchor) {
		if(editor.SelectionActive < editor.TextSelections.size() && 0 == editor.TextSelections[editor.SelectionActive].Count)
			if_fail_fe(editor.TextSelections.remove(editor.SelectionActive));
		editor.SelectionAnchor = (u2_t)-1;
		editor.SelectionActive = (u2_t)-1;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerKeyboardUpdate(SGUI & gui, SFileViewer & viewer, SFileEditor & editor) {
	for(u2_t iKey = 0; iKey < viewer.Keyboard.IdCharacters.size(); ++iKey)
		if(gui.Controls.IsAction(viewer.Keyboard.IdCharacters[iKey]))
			if_fail_fe(::fileViewerTextInsert(gui, editor, (sc_t)GUI_KEYBOARD_Default_keys[iKey]));
	for(u2_t iKey = 0; iKey < viewer.Keyboard.IdActions.size() && iKey < get_value_count<VK_SCANCODE>(); ++iKey)
		if(gui.Controls.IsAction(viewer.Keyboard.IdActions[iKey])) {
			cnst SGUIKeyboardEvent event = {VK_EVENT_Edit, (VK_SCANCODE)iKey};
			if_fail_fe(::fileViewerEditorHandle(gui, viewer, editor, event));
		}
	rtrn 0;
}

sttc b8_t fileViewerJSONExpressionIdentifier(vcst_t viewKey) {
	if(0 == viewKey.size())
		rtrn false;
	cnst sc_t first = viewKey[0];
	if(false == ((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z') || '_' == first))
		rtrn false;
	for(u2_t iChar = 1; iChar < viewKey.size(); ++iChar) {
		cnst sc_t character = viewKey[iChar];
		if(false == ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || '_' == character))
			rtrn false;
	}
	rtrn true;
}

sttc gpk::err_t fileViewerJSONExpressionPathKeyAppend(string & expression, vcst_t viewKey) {
	if(expression.size())
		if_fail_fe(expression.push_back('.'));
	rtrn expression.append(viewKey);
}

sttc gpk::err_t fileViewerJSONExpressionLeafPath(cnst SJSONNode & node, view<vcst_c> views, string & expression) {
	cnst SJSONNode * current = &node;
	while(current && current->Token) {
		if(JSON_TYPE_ARRAY == current->Token->Type) {
			if(0 == current->Children.size())
				rtrn 0;
			if_fail_fe(expression.append("['0']"));
			current = current->Children[0];
			continue;
		}
		if(JSON_TYPE_OBJECT != current->Token->Type || current->Children.size() < 2)
			rtrn 0;
		b8_t found = false;
		for(u2_t iChild = 0; iChild + 1 < current->Children.size(); iChild += 2) {
			cnst SJSONNode * key = current->Children[iChild];
			if(0 == key || key->ObjectIndex < 0 || (u2_t)key->ObjectIndex >= views.size() || false == ::fileViewerJSONExpressionIdentifier(views[key->ObjectIndex]))
				continue;
			if_fail_fe(::fileViewerJSONExpressionPathKeyAppend(expression, views[key->ObjectIndex]));
			current = current->Children[iChild + 1];
			found = true;
			break;
		}
		if(false == found)
			rtrn 0;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionBranchCollect(cnst SJSONNode & node, view<vcst_c> views, vcst_t basePath, aobj<string> & expressions) {
	if(JSON_TYPE_OBJECT == node.Token->Type) {
		for(u2_t iChild = 0; iChild + 1 < node.Children.size(); iChild += 2) {
			cnst SJSONNode * key = node.Children[iChild];
			if(0 == key || key->ObjectIndex < 0 || (u2_t)key->ObjectIndex >= views.size() || false == ::fileViewerJSONExpressionIdentifier(views[key->ObjectIndex]))
				continue;
			string expression = basePath;
			if_fail_fe(::fileViewerJSONExpressionPathKeyAppend(expression, views[key->ObjectIndex]));
			if_fail_fe(::fileViewerJSONExpressionLeafPath(*node.Children[iChild + 1], views, expression));
			if_fail_fe(expressions.push_back(expression));
		}
	}
	else {
		string expression = basePath;
		if_fail_fe(::fileViewerJSONExpressionLeafPath(node, views, expression));
		if_fail_fe(expressions.push_back(expression));
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionPresetsBuild(SJSONExpressionTool & tool, cnst SJSONReader & reader) {
	tool.PresetExpressions.clear();
	if(0 == reader.Tree.size() || 0 == reader.Tree[0] || 0 == reader.Tree[0]->Token)
		rtrn 0;
	cnst SJSONNode & root = *reader.Tree[0];
	aobj<aobj<string>> expressionGroups = {};
	if(JSON_TYPE_OBJECT == root.Token->Type) {
		for(u2_t iBranch = 0; ; ++iBranch) {
			b8_t foundBranch = false;
			for(u2_t iRoot = 0; iRoot + 1 < root.Children.size(); iRoot += 2) {
				cnst SJSONNode * keyRoot = root.Children[iRoot];
				cnst SJSONNode * valueRoot = root.Children[iRoot + 1];
				if(0 == keyRoot || 0 == valueRoot || keyRoot->ObjectIndex < 0 || (u2_t)keyRoot->ObjectIndex >= reader.View.size() || false == ::fileViewerJSONExpressionIdentifier(reader.View[keyRoot->ObjectIndex]))
					continue;
				if(JSON_TYPE_OBJECT == valueRoot->Token->Type) {
					cnst u2_t iChild = iBranch * 2;
					if(iChild + 1 >= valueRoot->Children.size())
						continue;
					cnst SJSONNode * keyBranch = valueRoot->Children[iChild];
					if(0 == keyBranch || keyBranch->ObjectIndex < 0 || (u2_t)keyBranch->ObjectIndex >= reader.View.size() || false == ::fileViewerJSONExpressionIdentifier(reader.View[keyBranch->ObjectIndex]))
						continue;
					string branchPath = reader.View[keyRoot->ObjectIndex];
					if_fail_fe(::fileViewerJSONExpressionPathKeyAppend(branchPath, reader.View[keyBranch->ObjectIndex]));
					if_fail_fe(expressionGroups.resize(expressionGroups.size() + 1));
					if_fail_fe(::fileViewerJSONExpressionBranchCollect(*valueRoot->Children[iChild + 1], reader.View, branchPath, expressionGroups[expressionGroups.size() - 1]));
					foundBranch = true;
				}
				else if(0 == iBranch) {
					if_fail_fe(expressionGroups.resize(expressionGroups.size() + 1));
					if_fail_fe(::fileViewerJSONExpressionBranchCollect(*valueRoot, reader.View, reader.View[keyRoot->ObjectIndex], expressionGroups[expressionGroups.size() - 1]));
					foundBranch = true;
				}
			}
			if(false == foundBranch)
				break;
		}
	}
	else {
		if_fail_fe(expressionGroups.resize(1));
		if_fail_fe(::fileViewerJSONExpressionBranchCollect(root, reader.View, {}, expressionGroups[0]));
	}
	for(u2_t iExpression = 0; tool.PresetExpressions.size() < JSON_EXPRESSION_PRESET_LIMIT; ++iExpression) {
		b8_t foundExpression = false;
		for(u2_t iGroup = 0; iGroup < expressionGroups.size() && tool.PresetExpressions.size() < JSON_EXPRESSION_PRESET_LIMIT; ++iGroup) {
			if(iExpression >= expressionGroups[iGroup].size())
				continue;
			vcst_t output = {};
			if(-1 != jsonExpressionResolve(expressionGroups[iGroup][iExpression], reader, 0, output))
				if_fail_fe(tool.PresetExpressions.push_back(expressionGroups[iGroup][iExpression]));
			foundExpression = true;
		}
		if(false == foundExpression)
			break;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionPresetsRefresh(SGUI & gui, SJSONExpressionTool & tool) {
	tool.PresetExpressions.clear();
	if(tool.InputJSON.Tree.size() && tool.InputJSON.Tree[0] && tool.InputJSON.Tree[0]->Token)
		if_fail_fe(::fileViewerJSONExpressionPresetsBuild(tool, tool.InputJSON));
	for(u2_t iPreset = 0; iPreset < tool.Presets.IdControls.size(); ++iPreset) {
		cnst b8_t visible = iPreset < tool.PresetExpressions.size();
		gui.Controls.SetHidden(tool.Presets.IdControls[iPreset], false == visible);
		if(visible)
			if_fail_fe(controlTextSet(gui, tool.Presets.IdControls[iPreset], tool.PresetExpressions[iPreset]));
	}
	rtrn 0;
}

sttc b8_t fileViewerJSONExpressionStartsWith(vcst_t text, vcst_t prefix) {
	if(prefix.size() > text.size())
		rtrn false;
	for(u2_t iCharacter = 0; iCharacter < prefix.size(); ++iCharacter)
		if(text[iCharacter] != prefix[iCharacter])
			rtrn false;
	rtrn true;
}

sttc gpk::err_t fileViewerJSONExpressionSuggestionsBuild(SJSONExpressionTool & tool, cnst SJSONNode * node, vcst_t baseExpression) {
	tool.SuggestionNode = node;
	tool.SuggestionBase = baseExpression;
	tool.SuggestionText.clear();
	tool.SuggestionExpressions.clear();
	tool.SuggestionChildSlots.clear();
	if(0 == node || 0 == node->Token)
		rtrn 0;
	if(JSON_TYPE_OBJECT == node->Token->Type) {
		for(u2_t iChild = 0; iChild + 1 < node->Children.size(); iChild += 2) {
			cnst SJSONNode * key = node->Children[iChild];
			if(0 == key || key->ObjectIndex < 0 || (u2_t)key->ObjectIndex >= tool.InputJSON.View.size())
				continue;
			vcst_c keyText = tool.InputJSON.View[key->ObjectIndex];
			if(false == ::fileViewerJSONExpressionIdentifier(keyText))
				continue;
			string expression = tool.SuggestionBase;
			if_fail_fe(::fileViewerJSONExpressionPathKeyAppend(expression, keyText));
			if_fail_fe(tool.SuggestionText.push_back(string{keyText}));
			if_fail_fe(tool.SuggestionExpressions.push_back(expression));
			if_fail_fe(tool.SuggestionChildSlots.push_back(iChild + 1));
		}
	}
	else if(JSON_TYPE_ARRAY == node->Token->Type)
		for(u2_t iChild = 0; iChild < node->Children.size(); ++iChild) {
			sc_t indexText[32] = {};
			cnst err_t textLength = ::gpk::sprintf_s(indexText, "['%u']", iChild);
			if_fail_fe(textLength);
			string labelText = {};
			if_fail_fe(labelText.append({indexText, (u2_t)textLength}));
			string expression = tool.SuggestionBase;
			if_fail_fe(expression.append({indexText, (u2_t)textLength}));
			if_fail_fe(tool.SuggestionText.push_back(labelText));
			if_fail_fe(tool.SuggestionExpressions.push_back(expression));
			if_fail_fe(tool.SuggestionChildSlots.push_back(iChild));
		}
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionSuggestionsRefresh(SGUI & gui, SJSONExpressionTool & tool, b8_t filter) {
	tool.SuggestionSource = tool.Command.Text;
	tool.SuggestionVisible.clear();
	vcst_t partial = {};
	if(filter) {
		u2_t offset = 0;
		if(tool.SuggestionBase.size() && ::fileViewerJSONExpressionStartsWith(tool.Command.Text, tool.SuggestionBase)) {
			offset = tool.SuggestionBase.size();
			if(tool.Command.Text.size() > offset && '.' == tool.Command.Text[offset])
				++offset;
		}
		partial = {tool.Command.Text.begin() + offset, tool.Command.Text.size() - offset};
	}
	for(u2_t iSuggestion = 0; iSuggestion < tool.SuggestionText.size() && tool.SuggestionVisible.size() < JSON_EXPRESSION_SUGGESTION_LIMIT; ++iSuggestion)
		if(false == filter || ::fileViewerJSONExpressionStartsWith(tool.SuggestionText[iSuggestion], partial))
			if_fail_fe(tool.SuggestionVisible.push_back(iSuggestion));
	for(u2_t iOption = 0; iOption < tool.IdSuggestionOptions.size(); ++iOption) {
		cnst b8_t visible = tool.Command.Editing && iOption < tool.SuggestionVisible.size();
		gui.Controls.SetHidden(tool.IdSuggestionOptions[iOption], false == visible);
		if(visible)
			if_fail_fe(controlTextSet(gui, tool.IdSuggestionOptions[iOption], tool.SuggestionText[tool.SuggestionVisible[iOption]]));
	}
	gui.Controls.SetHidden(tool.IdSuggestions, false == (tool.Command.Editing && tool.SuggestionVisible.size()));
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionSuggestionsFocus(SGUI & gui, SJSONExpressionTool & tool) {
	if(0 == tool.InputJSON.Tree.size() || 0 == tool.InputJSON.Tree[0] || 0 == tool.InputJSON.Tree[0]->Token) {
		if_fail_fe(::fileViewerJSONExpressionSuggestionsBuild(tool, 0, {}));
		rtrn ::fileViewerJSONExpressionSuggestionsRefresh(gui, tool, false);
	}
	cnst SJSONNode * node = tool.InputJSON.Tree[0];
	string base = {};
	b8_t filter = tool.Command.Text.size() != 0;
	if(tool.Command.Text.size()) {
		vcst_t resolvedText = {};
		cnst err_t iResolved = jsonExpressionResolve(tool.Command.Text, tool.InputJSON, 0, resolvedText);
		if(iResolved >= 0 && (u2_t)iResolved < tool.InputJSON.Tree.size() && tool.InputJSON.Tree[iResolved] && tool.InputJSON.Tree[iResolved]->Token) {
			cnst SJSONNode * resolved = tool.InputJSON.Tree[iResolved];
			if(JSON_TYPE_OBJECT == resolved->Token->Type || JSON_TYPE_ARRAY == resolved->Token->Type) {
				node = resolved;
				base = tool.Command.Text;
				filter = false;
			}
		}
		u2_t iSeparator = tool.Command.Text.size();
		while(iSeparator && '.' != tool.Command.Text[iSeparator - 1])
			--iSeparator;
		if(iSeparator && node == tool.InputJSON.Tree[0]) {
			vcst_c parentExpression = {tool.Command.Text.begin(), iSeparator - 1};
			cnst err_t iParent = jsonExpressionResolve(parentExpression, tool.InputJSON, 0, resolvedText);
			if(iParent >= 0 && (u2_t)iParent < tool.InputJSON.Tree.size() && tool.InputJSON.Tree[iParent] && tool.InputJSON.Tree[iParent]->Token) {
				node = tool.InputJSON.Tree[iParent];
				base = parentExpression;
			}
		}
	}
	if_fail_fe(::fileViewerJSONExpressionSuggestionsBuild(tool, node, base));
	rtrn ::fileViewerJSONExpressionSuggestionsRefresh(gui, tool, filter);
}

sttc gpk::err_t fileViewerCommandSetText(SGUI & gui, SFileCommand & command, vcst_t text) {
	command.Text.clear();
	if_fail_fe(command.Text.append(text));
	command.Cursor = command.Text.size();
	rtrn controlTextSet(gui, command.IdText, label(command.Text));
}

sttc gpk::err_t fileViewerCommandInput(SGUI & gui, SFileCommand & command, vpobj<SEventSystem> frameEvents, b8_t & run) {
	for(u2_t iEvent = 0; iEvent < frameEvents.size(); ++iEvent) {
		cnst pobj<SEventSystem> & systemEvent = frameEvents[iEvent];
		if(systemEvent->Type == SYSTEM_EVENT_Text) {
			if_fail_fe(eventExtractAndHandle<EVENT_TEXT>(*systemEvent, [&command](cnst SEventView<EVENT_TEXT> & eventText) {
				if(eventText.Type != EVENT_TEXT_Char || eventText.Data.size() < sizeof(u2_t) || command.Text.size() >= command.MaxLength)
					rtrn 0;
				u2_t codePoint = {};
				memcpy(&codePoint, eventText.Data.begin(), sizeof(codePoint));
				u0_t character = {};
				if(0 <= ::unicodeToCP437(codePoint, character)) {
					if_fail_fe(command.Text.insert(command.Cursor, (sc_t)character));
					++command.Cursor;
				}
				rtrn 0;
			}));
		}
		else if(systemEvent->Type == SYSTEM_EVENT_Keyboard) {
			if_fail_fe(eventExtractAndHandle<EVENT_KEYBOARD>(*systemEvent, [&command, &run](cnst SEventView<EVENT_KEYBOARD> & eventKeyboard) {
				if(eventKeyboard.Type != EVENT_KEYBOARD_Down || 0 == eventKeyboard.Data.size())
					rtrn 0;
				switch(eventKeyboard.Data[0]) {
				default: break;
				case SYSTEM_KEY_Escape: command.Editing = false; break;
				case SYSTEM_KEY_Enter: run = true; break;
				case SYSTEM_KEY_Left: if(command.Cursor) --command.Cursor; break;
				case SYSTEM_KEY_Right: if(command.Cursor < command.Text.size()) ++command.Cursor; break;
				case SYSTEM_KEY_Home: command.Cursor = 0; break;
				case SYSTEM_KEY_End: command.Cursor = command.Text.size(); break;
				case SYSTEM_KEY_Backspace:
					if(command.Cursor) {
						if_fail_fe(command.Text.remove(command.Cursor - 1));
						--command.Cursor;
					}
					break;
				case SYSTEM_KEY_Delete:
					if(command.Cursor < command.Text.size())
						if_fail_fe(command.Text.remove(command.Cursor));
					break;
				}
				rtrn 0;
			}));
		}
	}
	rtrn controlTextSet(gui, command.IdText, label(command.Text));
}

sttc vcst_t fileViewerJSONExpressionInputView(vcst_t input) {
	if(input.size() >= 3 && 0xEF == (u0_t)input[0] && 0xBB == (u0_t)input[1] && 0xBF == (u0_t)input[2])
		rtrn {input.begin() + 3, input.size() - 3};
	rtrn input;
}

sttc gpk::err_t fileViewerJSONExpressionInputSelect(SGUI & gui, SFileViewer & viewer, cid_t iEditor) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	if(iEditor < 0 || (u2_t)iEditor >= viewer.Editors.size() || 0 == viewer.Editors[iEditor].get_ref())
		rtrn 0;
	tool.InputEditor = iEditor;
	tool.InputJSON.Tree.clear();
	if_fail_fe(tool.InputJSON.Reset());
	cnst vcst_t input = ::fileViewerJSONExpressionInputView(viewer.Editors[iEditor]->FileText);
	cnst bool inputParsed = input.size() && succeeded(jsonParse(tool.InputJSON, input));
	if_fail_fe(::fileViewerJSONExpressionPresetsRefresh(gui, tool));
	if(inputParsed && tool.InputJSON.Tree.size() && tool.InputJSON.Tree[0]) {
		if_fail_fe(::fileViewerJSONExpressionSuggestionsBuild(tool, tool.InputJSON.Tree[0], {}));
	}
	else {
		if_fail_fe(::fileViewerJSONExpressionSuggestionsBuild(tool, 0, {}));
	}
	if_fail_fe(::fileViewerJSONExpressionSuggestionsRefresh(gui, tool, false));
	for(u2_t iInput = 0; iInput < tool.Inputs.IdControls.size(); ++iInput)
		gui.Controls.SetSelected(tool.Inputs.IdControls[iInput], iInput == (u2_t)iEditor);
	tool.Draft.FilePath = viewer.Editors[iEditor]->FilePath;
	if_fail_fe(tool.Draft.FilePath.append(".expressions.json"));
	gui.Controls.SetHidden(tool.Draft.IdSave, false);
	if_fail_fe(controlTextSet(gui, tool.Viewport.IdTitle, label(viewer.Editors[iEditor]->FilePath)));
	if_fail_fe(controlTextSet(gui, tool.Draft.Viewport.IdTitle, label(tool.Draft.FilePath)));
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionInputsUpdate(SGUI & gui, SFileViewer & viewer) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	for(u2_t iInput = 0; iInput < tool.Inputs.IdControls.size(); ++iInput) {
		cnst b8_t visible = iInput < viewer.Editors.size() && viewer.Editors[iInput].get_ref() && viewer.Editors[iInput]->FilePath.size();
		gui.Controls.SetHidden(tool.Inputs.IdControls[iInput], false == visible);
		if(visible && gui.Controls.Text[tool.Inputs.IdControls[iInput]].Text != viewer.Editors[iInput]->FilePath)
			if_fail_fe(controlTextSet(gui, tool.Inputs.IdControls[iInput], viewer.Editors[iInput]->FilePath));
	}
	if(tool.InputEditor < 0 && viewer.IdEditorCurrent >= 0)
		if_fail_fe(::fileViewerJSONExpressionInputSelect(gui, viewer, viewer.IdEditorCurrent));
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionRun(SGUI & gui, SFileViewer & viewer) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	if(tool.InputEditor < 0 || (u2_t)tool.InputEditor >= viewer.Editors.size() || 0 == viewer.Editors[tool.InputEditor].get_ref() || 0 == tool.Command.Text.size())
		rtrn 0;
	SJSONReader inputJSON = {};
	cnst vcst_t input = ::fileViewerJSONExpressionInputView(viewer.Editors[tool.InputEditor]->FileText);
	if(0 == input.size() || failed(jsonParse(inputJSON, input)))
		rtrn 0;
	vcst_t output = {};
	cnst err_t iResult = jsonExpressionResolve(tool.Command.Text, inputJSON, 0, output);
	if(-1 == iResult)
		rtrn 0;
	string outputJSON = {};
	if(iResult >= 0) {
		if((u2_t)iResult >= inputJSON.Tree.size() || 0 == inputJSON.Tree[iResult])
			rtrn 0;
		if_fail_fe(jsonWrite(inputJSON.Tree[iResult], inputJSON.View, outputJSON));
	}
	else if(-5 == iResult) {
		if_fail_fe(outputJSON.append(output));
	}
	else {
		if_fail_fe(appendJSONString(outputJSON, output));
	}
	tool.Draft.FileText.clear();
	if_fail_fe(tool.Draft.FileText.append("[\n\t"));
	if_fail_fe(tool.Draft.FileText.append(outputJSON));
	if_fail_fe(tool.Draft.FileText.append("\n]"));
	tool.Draft.TextSelections.clear();
	if_fail_fe(::fileViewerTextViewsRebuild(tool.Draft));
	tool.Draft.Cursor = {0, 0};
	gui.Controls.SetHidden(tool.Draft.Viewport.IdRoot, false);
	if_fail_fe(controlToFront(gui, tool.Draft.Viewport.IdRoot));
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionUpdate(SGUI & gui, SFileViewer & viewer, vpobj<SEventSystem> frameEvents) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	if(gui.Controls.IsHidden(tool.Viewport.IdRoot))
		rtrn 0;
	if_fail_fe(::fileViewerJSONExpressionInputsUpdate(gui, viewer));
	for(u2_t iInput = 0; iInput < tool.Inputs.IdControls.size() && iInput < viewer.Editors.size(); ++iInput)
		if(gui.Controls.IsAction(tool.Inputs.IdControls[iInput]))
			if_fail_fe(::fileViewerJSONExpressionInputSelect(gui, viewer, (cid_t)iInput));
	for(u2_t iPreset = 0; iPreset < tool.Presets.IdControls.size() && iPreset < tool.PresetExpressions.size(); ++iPreset)
		if(gui.Controls.IsAction(tool.Presets.IdControls[iPreset]))
			if_fail_fe(::fileViewerCommandSetText(gui, tool.Command, tool.PresetExpressions[iPreset]));
	b8_t suggestionSelected = false;
	for(u2_t iOption = 0; iOption < tool.IdSuggestionOptions.size() && iOption < tool.SuggestionVisible.size(); ++iOption)
		if(gui.Controls.IsAction(tool.IdSuggestionOptions[iOption])) {
			cnst u2_t iSuggestion = tool.SuggestionVisible[iOption];
			if(iSuggestion >= tool.SuggestionExpressions.size() || iSuggestion >= tool.SuggestionChildSlots.size())
				continue;
			if_fail_fe(::fileViewerCommandSetText(gui, tool.Command, tool.SuggestionExpressions[iSuggestion]));
			cnst u2_t iChild = tool.SuggestionChildSlots[iSuggestion];
			if(tool.SuggestionNode && iChild < tool.SuggestionNode->Children.size()) {
				cnst SJSONNode * selectedNode = tool.SuggestionNode->Children[iChild];
				if(selectedNode && selectedNode->Token && (JSON_TYPE_OBJECT == selectedNode->Token->Type || JSON_TYPE_ARRAY == selectedNode->Token->Type))
					if_fail_fe(::fileViewerJSONExpressionSuggestionsBuild(tool, selectedNode, tool.SuggestionExpressions[iSuggestion]));
			}
			tool.Command.Editing = true;
			gui.Controls.SetFocused(tool.Command.IdText, true);
			if_fail_fe(::fileViewerJSONExpressionSuggestionsRefresh(gui, tool, false));
			suggestionSelected = true;
			break;
		}
	if(gui.Controls.IsAction(tool.Command.IdText)) {
		tool.Command.Editing = true;
		if_fail_fe(::fileViewerJSONExpressionSuggestionsFocus(gui, tool));
	}
	else if(tool.Command.Editing && false == suggestionSelected && false == gui.Controls.IsFocused(tool.Command.IdText))
		tool.Command.Editing = false;
	b8_t run = gui.Controls.IsAction(tool.IdRun);
	if(tool.Command.Editing)
		if_fail_fe(::fileViewerCommandInput(gui, tool.Command, frameEvents, run));
	if(tool.SuggestionSource != tool.Command.Text && false == suggestionSelected) {
		if_fail_fe(::fileViewerJSONExpressionSuggestionsFocus(gui, tool));
	}
	else if(false == tool.Command.Editing)
		gui.Controls.SetHidden(tool.IdSuggestions, true);
	if(run)
		if_fail_fe(::fileViewerJSONExpressionRun(gui, viewer));
	if(gui.Controls.IsAction(tool.Draft.IdSave) && tool.Draft.FilePath.size())
		if_fail_fef(fileFromMemory(tool.Draft.FilePath, tool.Draft.FileText), "Failed to save '%.*s'.", GPK_VIEW_ARGS(tool.Draft.FilePath));
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorUpdate(SGUI & gui, SFileViewer & viewer, SFileEditor & editor, cid_t iEditor) {
	if(gui.Controls.IsHidden(editor.Viewport.IdRoot))
		rtrn 0;
	if(gui.Controls.IsAction(editor.IdKeyboard)) {
		viewer.IdEditorCurrent = iEditor;
		gui.Controls.SetHidden(viewer.Keyboard.IdRoot, false == gui.Controls.IsHidden(viewer.Keyboard.IdRoot));
		if(false == gui.Controls.IsHidden(viewer.Keyboard.IdRoot))
			if_fail_fe(controlToFront(gui, viewer.Keyboard.IdRoot));
	}
	for(u2_t iControl = 0; iControl < editor.Lines.IdControls.size(); ++iControl)
		if(gui.Controls.IsAction(editor.Lines.IdControls[iControl])) {
			cnst u2_t iLine = (u2_t)editor.Viewport.ScrollY.Value + iControl;
			if(iLine < editor.FileLines.size())
				viewer.IdEditorCurrent = iEditor;
		}
	if(gui.Controls.IsAction(editor.IdSave) && editor.FilePath.size())
		if_fail_fef(fileFromMemory(editor.FilePath, editor.FileText), "Failed to save '%.*s'.", GPK_VIEW_ARGS(editor.FilePath));
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorInputUpdate(SGUI & gui, SFileViewer & viewer, vpobj<SEventSystem> frameEvents) {
	if(viewer.IdEditorCurrent < 0 || (u2_t)viewer.IdEditorCurrent >= viewer.Editors.size() || 0 == viewer.Editors[viewer.IdEditorCurrent].get_ref())
		rtrn 0;
	SFileEditor & editor = *viewer.Editors[viewer.IdEditorCurrent];
	if(gui.Controls.IsHidden(editor.Viewport.IdRoot)) {
		gui.Controls.SetHidden(viewer.Keyboard.IdRoot, true);
		rtrn 0;
	}
	if(false == gui.Controls.IsHidden(viewer.Keyboard.IdRoot))
		if_fail_fe(::fileViewerKeyboardUpdate(gui, viewer, editor));
	rtrn ::fileViewerEditorPhysicalInput(gui, viewer, editor, frameEvents);
}

sttc gpk::err_t fileViewerEditorOpen(SGUI & gui, SFileViewer & viewer, vcst_t path) {
	cid_t iEditor = -1;
	for(u2_t i = 0; i < viewer.Editors.size(); ++i)
		if(viewer.Editors[i].get_ref() && viewer.Editors[i]->FilePath == path) {
			iEditor = (cid_t)i;
			break;
		}
	if(iEditor < 0)
		for(u2_t i = 0; i < viewer.Editors.size(); ++i)
			if(viewer.Editors[i].get_ref() && 0 == viewer.Editors[i]->FilePath.size()) {
				iEditor = (cid_t)i;
				break;
			}
	if_true_fef(iEditor < 0, "All %u JSON editor viewports are in use.", viewer.Editors.size());
	SFileEditor & editor = *viewer.Editors[iEditor];
	if(editor.FilePath != path)
		if_fail_fe(::fileViewerTextLoad(gui, viewer, editor, path));
	gui.Controls.SetHidden(editor.Viewport.IdRoot, false);
	gui.Controls.SetHidden(editor.IdTab, false);
	vcst_t fileName = {};
	if_fail_fe(pathFilename(path, fileName));
	if_fail_fe(controlTextSet(gui, editor.IdTab, label(fileName)));
	viewer.IdEditorCurrent = iEditor;
	if_fail_fe(controlToFront(gui, editor.Viewport.IdRoot));
	for(u2_t i = 0; i < viewer.Editors.size(); ++i)
		if(viewer.Editors[i].get_ref())
			gui.Controls.SetSelected(viewer.Editors[i]->IdTab, i == (u2_t)iEditor && false == gui.Controls.IsHidden(viewer.Editors[i]->Viewport.IdRoot));
	rtrn 0;
}

sttc gpk::err_t fileViewerMenuUpdate(SGUI & gui, SFileViewer & viewer) {
	if(gui.Controls.IsAction(viewer.IdMenuFiles)) {
		for(u2_t iDirectory = 0; iDirectory < viewer.Directories.size(); ++iDirectory) {
			SPathList & directory = *viewer.Directories[iDirectory];
			if(false == gui.Controls.IsHidden(directory.Viewport.IdRoot))
				continue;
			gui.Controls.SetHidden(directory.Viewport.IdRoot, false);
			if(0 == directory.PathContents.Root.size())
				if_fail_fe(::fileViewerFolderLoad(gui, directory, viewer.RootPath));
			if_fail_fe(controlToFront(gui, directory.Viewport.IdRoot));
			break;
		}
	}
	if(gui.Controls.IsAction(viewer.IdMenuJSON)) {
		b8_c hidden = gui.Controls.IsHidden(viewer.JSONExpressions.Viewport.IdRoot);
		gui.Controls.SetHidden(viewer.JSONExpressions.Viewport.IdRoot, false == hidden);
		gui.Controls.SetHidden(viewer.JSONExpressions.Draft.Viewport.IdRoot, false == hidden);
		if(hidden) {
			if_fail_fe(controlToFront(gui, viewer.JSONExpressions.Viewport.IdRoot));
			if_fail_fe(controlToFront(gui, viewer.JSONExpressions.Draft.Viewport.IdRoot));
		}
	}
	gui.Controls.SetSelected(viewer.IdMenuJSON, false == gui.Controls.IsHidden(viewer.JSONExpressions.Viewport.IdRoot));
	if(gui.Controls.IsAction(viewer.IdMenuExit))
		rtrn APPLICATION_STATE_EXIT;
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor) {
		SFileEditor & editor = *viewer.Editors[iEditor];
		if(false == editor.FilePath.size())
			continue;
		b8_t hidden = gui.Controls.IsHidden(editor.Viewport.IdRoot);
		if(gui.Controls.IsAction(editor.IdTab)) {
			gui.Controls.SetHidden(editor.Viewport.IdRoot, false == hidden);
			hidden = false == hidden;
			if(false == hidden) {
				viewer.IdEditorCurrent = (cid_t)iEditor;
				if_fail_fe(controlToFront(gui, editor.Viewport.IdRoot));
			}
			else if(viewer.IdEditorCurrent == (cid_t)iEditor) {
				viewer.IdEditorCurrent = -1;
				gui.Controls.SetHidden(viewer.Keyboard.IdRoot, true);
			}
		}
		gui.Controls.SetSelected(editor.IdTab, false == hidden);
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerSelectionUpdate(SGUI & gui, SFileViewer & viewer) {
	for(u2_t iDirectory = 0; iDirectory < viewer.Directories.size(); ++iDirectory) {
		SPathList & directory = *viewer.Directories[iDirectory];
		if(gui.Controls.IsHidden(directory.Viewport.IdRoot))
			continue;
		u2_c firstVisible = (u2_t)directory.Viewport.ScrollY.Value;
		b8_c hasParent = ::fileViewerFolderHasParent(directory.PathContents);
		for(u2_t iFile = 0; iFile < directory.Files.IdControls.size(); ++iFile) {
			cnst cid_t idControl = directory.Files.IdControls[iFile];
			if(false == gui.Controls.IsAction(idControl))
				continue;
			u2_c iItem = firstVisible + iFile;
			for(u2_t iSelection = 0; iSelection < directory.Files.IdControls.size(); ++iSelection)
				gui.Controls.SetSelected(directory.Files.IdControls[iSelection], iSelection == iFile);
			if(hasParent && 0 == iItem) {
				vcst_t parent = {};
				if_fail_fe(pathDirectory(directory.PathContents.Root, parent));
				rtrn ::fileViewerFolderLoad(gui, directory, parent);
			}
			u2_c iPath = iItem - one_if(hasParent);
			if(iPath >= directory.PathContents.Name.size())
				rtrn 0;
			string path = {};
			if_fail_fe(pathNameCompose(directory.PathContents.Root, directory.PathContents.Name[iPath], path));
			if(directory.PathContents.Info[iPath].Folder)
				rtrn ::fileViewerFolderLoad(gui, directory, path);
			rtrn ::fileViewerEditorOpen(gui, viewer, path);
		}
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionBind(SGUI & gui, SFileViewer & viewer) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	if_fail_fe(::fileViewerViewportBind(gui.Controls, "json_expression_tool", tool.Viewport));
	if_fail_fe(::fileViewerListBind(gui.Controls, "json_expression_tool_inputs", tool.Inputs));
	if_fail_fe(::fileViewerListBind(gui.Controls, "json_expression_tool_presets", tool.Presets));
	if_fail_fe(::controlIdentifierRequire(gui.Controls, "json_expression_tool_suggestions", tool.IdSuggestions));
	tool.IdSuggestionOptions = gui.Controls.Children[tool.IdSuggestions];
	if_fail_fe(::controlIdentifierRequire(gui.Controls, "command_input", tool.Command.IdRoot));
	if_fail_fe(::controlIdentifierRequire(gui.Controls, "command", tool.Command.IdText));
	if_fail_fe(::controlIdentifierRequire(gui.Controls, "json_expression_tool_run", tool.IdRun));
	if_fail_fe(::fileViewerEditorBind(gui.Controls, "json_expression_output", tool.Draft, true));
	if_fail_fe(tool.Draft.FileText.append("[]"));
	if_fail_fe(::fileViewerTextViewsRebuild(tool.Draft));
	gui.Controls.SetHidden(tool.Draft.IdSave, true);
	gui.Controls.SetHidden(tool.Draft.IdKeyboard, true);
	rtrn 0;
}

gpk::err_t fileViewerSetup(SGUI & gui, SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vcst_t rootPath) {
	(void)layout;
	gui.ColorModeDefault = GUI_COLOR_MODE_3D;
	if_fail_fef(jsonFileRead(viewer.GUIJSON, "../gpk/test_gpk_file_viewer_common/file_viewer_gui.json"), "%s", "Failed to load the file viewer GUI JSON.");
	if_fail_fe(guiControlTableJSONLoad(gui, viewer.GUIJSON.Reader));

	if_fail_fe(::controlIdentifierRequire(gui.Controls, "menu_files", viewer.IdMenuFiles));
	if_fail_fe(::controlIdentifierRequire(gui.Controls, "menu_json", viewer.IdMenuJSON));
	if_fail_fe(::controlIdentifierRequire(gui.Controls, "menu_exit", viewer.IdMenuExit));
	if_fail_fe(::fileViewerListBind(gui.Controls, "editor_tabs", viewer.EditorTabs));
	if_fail_fe(::fileViewerKeyboardBind(gui.Controls, viewer.Keyboard));

	if_fail_fe(viewer.Directories.resize(4));
	for(u2_t iDirectory = 0; iDirectory < viewer.Directories.size(); ++iDirectory) {
		if_null_fe(viewer.Directories[iDirectory].create());
		string identifier = {};
		if_fail_fe(::indexedIdentifier(identifier, "directory_", iDirectory));
		if_fail_fe(::fileViewerDirectoryBind(gui.Controls, identifier, *viewer.Directories[iDirectory]));
	}

	if_fail_fe(viewer.Editors.resize(8));
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor) {
		if_null_fe(viewer.Editors[iEditor].create());
		string identifier = {};
		if_fail_fe(::indexedIdentifier(identifier, "editor_", iEditor));
		if_fail_fe(::fileViewerEditorBind(gui.Controls, identifier, *viewer.Editors[iEditor], true));
		if_fail_fe(::indexedIdentifier(identifier, "tab_", iEditor));
		if_fail_fe(::controlIdentifierRequire(gui.Controls, identifier, viewer.Editors[iEditor]->IdTab));
	}
	if_fail_fe(::fileViewerJSONExpressionBind(gui, viewer));

	viewer.RootPath.clear();
	if_fail_fe(viewer.RootPath.append(rootPath.size() ? rootPath : vcst_t{"."}));
	if_fail_fe(::fileViewerFolderLoad(gui, *viewer.Directories[0], viewer.RootPath));
	gui.Controls.SetHidden(viewer.Directories[0]->Viewport.IdRoot, false);
	gui.Controls.SetHidden(viewer.Keyboard.IdRoot, true);
	if_fail_fe(guiUpdateMetrics(gui, gui.LastSize, true));
	rtrn 0;
}

gpk::err_t fileViewerUpdate(SGUI & gui, cnst SInput & input, SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vpobj<SEventSystem> frameEvents) {
	(void)layout;
	err_t result = ::fileViewerMenuUpdate(gui, viewer);
	if(result)
		rtrn result;

	for(u2_t iDirectory = 0; iDirectory < viewer.Directories.size(); ++iDirectory)
		if(viewer.Directories[iDirectory].get_ref() && false == gui.Controls.IsHidden(viewer.Directories[iDirectory]->Viewport.IdRoot))
			if_fail_fe(::fileViewerListWindowUpdate(gui, input, *viewer.Directories[iDirectory]));
	if_fail_fe(::fileViewerSelectionUpdate(gui, viewer));
	if_fail_fe(::fileViewerJSONExpressionUpdate(gui, viewer, frameEvents));

	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor) {
		if(0 == viewer.Editors[iEditor].get_ref())
			continue;
		if_fail_fe(::fileViewerEditorUpdate(gui, viewer, *viewer.Editors[iEditor], (cid_t)iEditor));
		if_fail_fe(::fileViewerEditorSelectionUpdate(gui, input, viewer, *viewer.Editors[iEditor], (cid_t)iEditor));
	}
	if(false == viewer.JSONExpressions.Command.Editing)
		if_fail_fe(::fileViewerEditorInputUpdate(gui, viewer, frameEvents));
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if(viewer.Editors[iEditor].get_ref())
			if_fail_fe(::fileViewerTextWindowUpdate(gui, input, *viewer.Editors[iEditor]));
	if_fail_fe(::fileViewerTextWindowUpdate(gui, input, viewer.JSONExpressions.Draft));
	rtrn 0;
}

gpk::err_t fileViewerDraw(SGUI & gui, SFileViewer & viewer, g8bgra target) {
	(void)viewer;
	if_fail_fe(target.fill(SColorBGRA{16, 18, 20}));
	if_fail_fe(guiUpdateMetrics(gui, target.metrics().u1(), false));
	rtrn guiDraw(gui, target);
}
