#include "gpk_event_input.h"
#include "gpk_event_screen.h"
#include "gpk_gui_control_state.h"
#include "gpk_color.h"
#include "gpk_runtime.h"
#include "gpk_system_event.h"

#include "test_llc_suites.h"

#include <cstring>

GDEFINE_ENUM_TYPE(EVENT_TEST_PARENT, ::gpk::u0_t);
GDEFINE_ENUM_VALUE(EVENT_TEST_PARENT, Application, 0);
GDEFINE_ENUM_VALUE(EVENT_TEST_PARENT, Forwarded, 1);

GDEFINE_ENUM_TYPE(EVENT_TEST_CHILD, ::gpk::u0_t);
GDEFINE_ENUM_VALUE(EVENT_TEST_CHILD, Bytes, 0);
GDEFINE_ENUM_VALUE(EVENT_TEST_CHILD, Values, 1);
GDEFINE_ENUM_VALUE(EVENT_TEST_CHILD, POD, 2);
GDEFINE_ENUM_VALUE(EVENT_TEST_CHILD, Text, 3);
GDEFINE_ENUM_VALUE(EVENT_TEST_CHILD, Empty, 4);

// ALC and MLC keep these event values private while sharing the SEvent envelope.
GDEFINE_ENUM_TYPE(EVENT_TEST_RASTER, ::gpk::u0_t);
GDEFINE_ENUM_VALUE(EVENT_TEST_RASTER, Clear, 0);
GDEFINE_ENUM_VALUE(EVENT_TEST_RASTER, Text, 1);

GDEFINE_ENUM_TYPE(EVENT_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, VIEW_SERIALIZE       ,  0, "SEventView did not preserve its type, bytes, or input boundary.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, OWNED_COPY          ,  1, "SEvent did not own an independent copy of its payload.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, OWNED_SERIALIZE     ,  2, "SEvent did not preserve its type, bytes, or input boundary.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, WRAP_EXTRACT        ,  3, "eventWrapChild()/ExtractChild() did not preserve the nested event.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, ENQUEUE_RAW         ,  4, "The raw-byte eventEnqueueChild() overload changed the nested event.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, ENQUEUE_VIEW        ,  5, "The typed-view eventEnqueueChild() overload changed the nested event.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, ENQUEUE_POD         ,  6, "The POD eventEnqueueChild() overload changed the nested event.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, ENQUEUE_TEXT        ,  7, "The string-view eventEnqueueChild() consumer shape changed the text bytes.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, EMPTY_PAYLOAD       ,  8, "An empty child event did not round-trip as an empty payload.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, HANDLER_SUCCESS     ,  9, "eventExtractAndHandle() did not deliver the extracted child.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, HANDLER_RESULT      , 10, "eventExtractAndHandle() did not propagate the handler result.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, TRUNCATED_REJECTED  , 11, "A truncated child event was accepted or dispatched.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, FORWARDED_BYTES     , 12, "Forwarding an existing child Data view changed its bytes.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, SCREEN_RESIZE       , 13, "The normalized screen-resize event changed its dimensions.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, SCREEN_CLOSE        , 14, "The normalized screen-close event was not empty.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, KEYBOARD_EVENT      , 15, "The normalized keyboard event changed its key metadata.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, TEXT_EVENT          , 16, "The normalized text event changed its Unicode or UTF-8 metadata.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, INVALID_TEXT        , 17, "An invalid Unicode scalar was enqueued or changed the queue.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, DEVICE_STATUS_FRAME , 18, "The embedded device-status frame changed in the serialized stream.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, RASTER_CLEAR_FRAME  , 19, "The embedded raster-clear frame changed in the serialized stream.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, RASTER_TEXT_FRAME   , 20, "The embedded raster-text frame changed in the serialized stream.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, FRAME_BOUNDARY      , 21, "Concatenated embedded event frames did not preserve their boundaries.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, RUNTIME_INPUT       , 22, "SRuntimeValues::EventsIn did not post and return its event.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, RUNTIME_OUTPUT      , 23, "SRuntimeValues::EventsOut did not read and clear its events.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, RUNTIME_INDEPENDENT , 24, "SRuntimeValues input and output queues affected each other.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, MOUSE_EVENT         , 25, "The normalized mouse event changed its coordinates.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, TOUCH_EVENT         , 26, "The normalized touch event changed its coordinates.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, JOYPAD_EVENT        , 27, "The normalized joypad event changed its button.");
GDEFINE_ENUM_VALUED(EVENT_TEST_RESULT, GUI_EVENT           , 28, "The GUI control-state event changed its control or flag delta.");

#pragma pack(push, 1)
stct SEventTestPOD {
	::gpk::u2_t	Sequence	= {};
	::gpk::u1_t	Value		= {};
	::gpk::u0_t	Flags		= {};
};
#pragma pack(pop)

sttc bool eventTestBytesEqual(::gpk::vcu0_t left, ::gpk::vcu0_t right) {
	rtrn left.size() == right.size() && (0 == left.size() || 0 == ::memcmp(left.begin(), right.begin(), left.size()));
}

tplt<tpnm T>
sttc bool eventTestPODRead(::gpk::vcu0_t bytes, T & value) {
	if(bytes.size() != szof(T))
		rtrn false;
	::memcpy(&value, bytes.begin(), szof(T));
	rtrn true;
}

sttc ::gpk::err_t testEventSerialization(::gpk::ATestError & errors) {
	cnst ::gpk::u0_t raw[] = {0x10, 0x20, 0x30, 0x40};
	::gpk::SEView<EVENT_TEST_CHILD> sourceView = {EVENT_TEST_CHILD_Bytes, {raw}};
	::gpk::au0_t serializedView;
	if_fail_fe(sourceView.Save(serializedView));
	::gpk::vcu0_t viewInput = serializedView;
	::gpk::SEView<EVENT_TEST_CHILD> restoredView;
	cnst ::gpk::err_t viewResult = restoredView.Load(viewInput);
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_VIEW_SERIALIZE, ::gpk::failed(viewResult) || restoredView.Type != sourceView.Type || !eventTestBytesEqual(restoredView.Data, sourceView.Data) || viewInput.size());

	::gpk::SEvent<EVENT_TEST_CHILD> owned = sourceView;
	::gpk::SEvent<EVENT_TEST_CHILD> copied = owned;
	owned.Data[0] = 0xFF;
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_OWNED_COPY, copied.Data.size() != ::gpk::size(raw) || copied.Data[0] != raw[0] || copied.Data.begin() == owned.Data.begin());

	::gpk::au0_t serializedOwned;
	if_fail_fe(copied.Save(serializedOwned));
	::gpk::vcu0_t ownedInput = serializedOwned;
	::gpk::SEvent<EVENT_TEST_CHILD> restoredOwned;
	cnst ::gpk::err_t ownedResult = restoredOwned.Load(ownedInput);
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_OWNED_SERIALIZE, ::gpk::failed(ownedResult) || restoredOwned.Type != copied.Type || !eventTestBytesEqual(restoredOwned.Data, copied.Data) || ownedInput.size());
	rtrn 0;
}

sttc ::gpk::err_t testEventNesting(::gpk::ATestError & errors) {
	cnst ::gpk::u0_t raw[] = {1, 3, 5, 7};
	::gpk::SEvent<EVENT_TEST_PARENT> wrapped = {EVENT_TEST_PARENT_Application};
	if_fail_fe(::gpk::eventWrapChild(wrapped, EVENT_TEST_CHILD_Bytes, ::gpk::vcu0_t{raw}));
	::gpk::SEView<EVENT_TEST_CHILD> extracted;
	cnst ::gpk::err_t extractResult = wrapped.ExtractChild(extracted);
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_WRAP_EXTRACT, ::gpk::failed(extractResult) || extracted.Type != EVENT_TEST_CHILD_Bytes || !eventTestBytesEqual(extracted.Data, {raw}));

	::gpk::TEventQueue<EVENT_TEST_PARENT> queue;
	if_fail_fe(::gpk::eventEnqueueChild(queue, EVENT_TEST_PARENT_Application, EVENT_TEST_CHILD_Bytes, ::gpk::vcu0_t{raw}));
	cnst ::gpk::u1_t values[] = {0x1122, 0x3344, 0x5566};
	if_fail_fe(::gpk::eventEnqueueChild(queue, EVENT_TEST_PARENT_Application, EVENT_TEST_CHILD_Values, ::gpk::view<cnst ::gpk::u1_t>{values}));
	cnst SEventTestPOD pod = {77, 0xA55A, 0xC3};
	if_fail_fe(::gpk::eventEnqueueChild(queue, EVENT_TEST_PARENT_Application, EVENT_TEST_CHILD_POD, pod));
	cnst ::gpk::vcst_t text = {".join embedded"};
	if_fail_fe(::gpk::eventEnqueueChild<EVENT_TEST_PARENT>(queue, EVENT_TEST_PARENT_Application, EVENT_TEST_CHILD_Text, text));
	if_fail_fe(::gpk::eventEnqueueChild(queue, EVENT_TEST_PARENT_Application, EVENT_TEST_CHILD_Empty, ::gpk::vcu0_t{}));
	GPK_TEST_REQUIRE(errors, EVENT_TEST_RESULT_ENQUEUE_RAW, queue.size() != 5);

	::gpk::SEView<EVENT_TEST_CHILD> child;
	if_fail_fe(queue[0]->ExtractChild(child));
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_ENQUEUE_RAW, queue[0]->Type != EVENT_TEST_PARENT_Application || child.Type != EVENT_TEST_CHILD_Bytes || !eventTestBytesEqual(child.Data, {raw}));
	if_fail_fe(queue[1]->ExtractChild(child));
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_ENQUEUE_VIEW, child.Type != EVENT_TEST_CHILD_Values || !eventTestBytesEqual(child.Data, {(cnst ::gpk::u0_t*)values, szof(values)}));
	if_fail_fe(queue[2]->ExtractChild(child));
	SEventTestPOD restoredPOD;
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_ENQUEUE_POD, child.Type != EVENT_TEST_CHILD_POD || !eventTestPODRead(child.Data, restoredPOD) || 0 != ::memcmp(&restoredPOD, &pod, szof(pod)));
	if_fail_fe(queue[3]->ExtractChild(child));
	GPK_TEST_CHECKF(errors, EVENT_TEST_RESULT_ENQUEUE_TEXT, child.Type != EVENT_TEST_CHILD_Text || !eventTestBytesEqual(child.Data, {(cnst ::gpk::u0_t*)text.begin(), text.size()}), "Type:%u, bytes:%u, expected type:%u, bytes:%u.", (::gpk::u2_t)child.Type, child.Data.size(), (::gpk::u2_t)EVENT_TEST_CHILD_Text, text.size());
	if_fail_fe(queue[4]->ExtractChild(child));
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_EMPTY_PAYLOAD, child.Type != EVENT_TEST_CHILD_Empty || child.Data.size());

	bool handled = false;
	cnst ::gpk::function<::gpk::err_t(cnst ::gpk::SEView<EVENT_TEST_CHILD> &)> successHandler = [&handled, &raw](cnst ::gpk::SEView<EVENT_TEST_CHILD> & eventView) {
		handled = eventView.Type == EVENT_TEST_CHILD_Bytes && eventTestBytesEqual(eventView.Data, {raw});
		rtrn 0;
	};
	cnst ::gpk::err_t handleResult = ::gpk::eventExtractAndHandle(*queue[0], successHandler);
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_HANDLER_SUCCESS, ::gpk::failed(handleResult) || !handled);
	cnst ::gpk::function<::gpk::err_t(cnst ::gpk::SEView<EVENT_TEST_CHILD> &)> failureHandler = [](cnst ::gpk::SEView<EVENT_TEST_CHILD> &) { rtrn -7; };
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_HANDLER_RESULT, ::gpk::eventExtractAndHandle(*queue[0], failureHandler) != -7);

	::gpk::SEvent<EVENT_TEST_PARENT> truncated = {EVENT_TEST_PARENT_Application, ::gpk::vcu0_t{raw, 1}};
	bool malformedDispatched = false;
	cnst ::gpk::function<::gpk::err_t(cnst ::gpk::SEView<EVENT_TEST_CHILD> &)> malformedHandler = [&malformedDispatched](cnst ::gpk::SEView<EVENT_TEST_CHILD> &) { malformedDispatched = true; rtrn 0; };
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_TRUNCATED_REJECTED, ::gpk::succeeded(::gpk::eventExtractAndHandle(truncated, malformedHandler)) || malformedDispatched);

	// SSiege and LLS clients forward an already-extracted child Data view under another parent.
	::gpk::TEventQueue<EVENT_TEST_PARENT> forwarded;
	if_fail_fe(::gpk::eventEnqueueChild(forwarded, EVENT_TEST_PARENT_Forwarded, extracted.Type, extracted.Data));
	::gpk::SEView<EVENT_TEST_CHILD> forwardedChild;
	if_fail_fe(forwarded[0]->ExtractChild(forwardedChild));
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_FORWARDED_BYTES, forwarded[0]->Type != EVENT_TEST_PARENT_Forwarded || forwardedChild.Type != extracted.Type || !eventTestBytesEqual(forwardedChild.Data, extracted.Data));
	rtrn 0;
}

sttc ::gpk::err_t testEventPlatformInput(::gpk::ATestError & errors) {
	::gpk::apobj<::gpk::SEventSystem> queue;
	cnst ::gpk::n2u1_t dimensions = {480, 320};
	if_fail_fe(::gpk::eventEnqueueScreenResize(queue, dimensions));
	if_fail_fe(::gpk::eventEnqueueScreenClose(queue));
	::gpk::SInputKeyEvent key = {};
	key.Key = 13;
	key.ScanCode = 28;
	key.DeviceId = 0;
	key.KeySym = 0xFF0D;
	key.Modifiers = ::gpk::INPUT_MODIFIER_Alt;
	key.Time = 123456;
	if_fail_fe(::gpk::eventEnqueueKeyboardDown(queue, key));
	::gpk::SInputTextEvent text = {};
	text.CodePoint = 0x00E9;
	text.KeySym = 0x00E9;
	text.DeviceId = 0;
	text.ScanCode = 18;
	if_fail_fe(::gpk::eventEnqueueTextChar(queue, text));
	cnst ::gpk::n2s1_t mousePosition = {123, -45};
	if_fail_fe(::gpk::eventEnqueueMousePosition(queue, mousePosition));
	cnst ::gpk::n2u1_t touchPosition = {320, 240};
	if_fail_fe(::gpk::eventEnqueueTouchBegin(queue, touchPosition));
	cnst ::gpk::u0_t joypadButton = 2;
	if_fail_fe(::gpk::eventEnqueueJoyPadDown(queue, joypadButton));
	if_fail_fe(::gpk::eventEnqueueGuiControlStateChange(queue, 42, ::gpk::GUI_CONTROL_FLAG_Hidden, ::gpk::GUI_CONTROL_FLAG_Disabled));
	GPK_TEST_REQUIRE(errors, EVENT_TEST_RESULT_SCREEN_RESIZE, queue.size() != 8);

	::gpk::SScreenEventView screenEvent;
	if_fail_fe(queue[0]->ExtractChild(screenEvent));
	::gpk::n2u1_t restoredDimensions;
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_SCREEN_RESIZE, queue[0]->Type != ::gpk::SYSTEM_EVENT_Screen || screenEvent.Type != ::gpk::EVENT_SCREEN_Resize || !eventTestPODRead(screenEvent.Data, restoredDimensions) || restoredDimensions.x != dimensions.x || restoredDimensions.y != dimensions.y);
	if_fail_fe(queue[1]->ExtractChild(screenEvent));
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_SCREEN_CLOSE, screenEvent.Type != ::gpk::EVENT_SCREEN_Close || screenEvent.Data.size());

	::gpk::SEView<::gpk::EVENT_KEYBOARD> keyboardEvent;
	if_fail_fe(queue[2]->ExtractChild(keyboardEvent));
	::gpk::SInputKeyEvent restoredKey;
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_KEYBOARD_EVENT, queue[2]->Type != ::gpk::SYSTEM_EVENT_Keyboard || keyboardEvent.Type != ::gpk::EVENT_KEYBOARD_Down || !eventTestPODRead(keyboardEvent.Data, restoredKey) || 0 != ::memcmp(&restoredKey, &key, szof(key)));

	::gpk::SEView<::gpk::EVENT_TEXT> textEvent;
	if_fail_fe(queue[3]->ExtractChild(textEvent));
	::gpk::SInputTextEvent restoredText;
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_TEXT_EVENT, queue[3]->Type != ::gpk::SYSTEM_EVENT_Text || textEvent.Type != ::gpk::EVENT_TEXT_Char || !eventTestPODRead(textEvent.Data, restoredText) || restoredText.CodePoint != text.CodePoint || restoredText.KeySym != text.KeySym || restoredText.DeviceId != text.DeviceId || restoredText.ScanCode != text.ScanCode || restoredText.UTF8Length != 2 || restoredText.UTF8[0] != 0xC3 || restoredText.UTF8[1] != 0xA9);

	::gpk::SEView<::gpk::EVENT_MOUSE> mouseEvent;
	if_fail_fe(queue[4]->ExtractChild(mouseEvent));
	::gpk::n2s1_t restoredMouse;
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_MOUSE_EVENT, queue[4]->Type != ::gpk::SYSTEM_EVENT_Mouse || mouseEvent.Type != ::gpk::EVENT_MOUSE_Position || !eventTestPODRead(mouseEvent.Data, restoredMouse) || restoredMouse.x != mousePosition.x || restoredMouse.y != mousePosition.y);
	::gpk::SEView<::gpk::EVENT_TOUCH> touchEvent;
	if_fail_fe(queue[5]->ExtractChild(touchEvent));
	::gpk::n2u1_t restoredTouch;
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_TOUCH_EVENT, queue[5]->Type != ::gpk::SYSTEM_EVENT_Touch || touchEvent.Type != ::gpk::EVENT_TOUCH_Begin || !eventTestPODRead(touchEvent.Data, restoredTouch) || restoredTouch.x != touchPosition.x || restoredTouch.y != touchPosition.y);
	::gpk::SEView<::gpk::EVENT_JOYPAD> joypadEvent;
	if_fail_fe(queue[6]->ExtractChild(joypadEvent));
	::gpk::u0_t restoredButton = {};
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_JOYPAD_EVENT, queue[6]->Type != ::gpk::SYSTEM_EVENT_Joypad || joypadEvent.Type != ::gpk::EVENT_JOYPAD_Button_down || !eventTestPODRead(joypadEvent.Data, restoredButton) || restoredButton != joypadButton);
	::gpk::SEView<::gpk::EVENT_GUI_CONTROL> guiEvent;
	if_fail_fe(queue[7]->ExtractChild(guiEvent));
	::gpk::SChangeControlState stateChange;
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_GUI_EVENT, queue[7]->Type != ::gpk::SYSTEM_EVENT_GUI || guiEvent.Type != ::gpk::EVENT_GUI_CONTROL_StateChange || !eventTestPODRead(guiEvent.Data, stateChange) || stateChange.Id != 42 || stateChange.Clear != ::gpk::GUI_CONTROL_FLAG_Hidden || stateChange.Set != ::gpk::GUI_CONTROL_FLAG_Disabled);

	::gpk::SInputTextEvent invalidText = {};
	invalidText.CodePoint = 0xD800;
	cnst ::gpk::u2_t countBeforeInvalid = queue.size();
	cnst ::gpk::err_t invalidResult = ::gpk::eventEnqueueTextChar(queue, invalidText);
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_INVALID_TEXT, ::gpk::succeeded(invalidResult) || queue.size() != countBeforeInvalid);
	rtrn 0;
}

sttc ::gpk::err_t testEventEmbeddedStream(::gpk::ATestError & errors) {
	// ALC reports device status with the packed SDeviceResult system-event frame.
	// MLC sends Raster/Clear and Raster/Text events; ALC loads those system frames
	// and then the private raster child event.
	::gpk::au0_t wire;
	cnst ::gpk::SDeviceResult deviceStatus = {::gpk::RESULT_Busy};
	if_fail_fe(wire.append({(cnst ::gpk::u0_t*)&deviceStatus, szof(deviceStatus)}));

	cnst ::gpk::bgra clearColor = {0xE7, 0x39, 0x00, 0xFF};
	::gpk::SEventSystem clearFrame = {::gpk::SYSTEM_EVENT_Raster};
	::gpk::SEView<EVENT_TEST_RASTER> clear = {EVENT_TEST_RASTER_Clear, {(cnst ::gpk::u0_t*)&clearColor, szof(clearColor)}};
	if_fail_fe(clear.Save(clearFrame.Data));
	if_fail_fe(clearFrame.Save(wire));

	cnst ::gpk::vcst_t text = {"ILI9486"};
	::gpk::SEventSystem textFrame = {::gpk::SYSTEM_EVENT_Raster};
	::gpk::SEView<EVENT_TEST_RASTER> rasterText = {EVENT_TEST_RASTER_Text, {(cnst ::gpk::u0_t*)text.begin(), text.size()}};
	if_fail_fe(rasterText.Save(textFrame.Data));
	if_fail_fe(textFrame.Save(wire));

	::gpk::vcu0_t input = wire;
	::gpk::SEViewSystem systemEvent;
	if_fail_fe(systemEvent.Load(input));
	::gpk::RESULT restoredStatus = {};
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_DEVICE_STATUS_FRAME, systemEvent.Type != ::gpk::SYSTEM_EVENT_Device || !eventTestPODRead(systemEvent.Data, restoredStatus) || restoredStatus != ::gpk::RESULT_Busy);

	if_fail_fe(systemEvent.Load(input));
	::gpk::vcu0_t childInput = systemEvent.Data;
	::gpk::SEView<EVENT_TEST_RASTER> restoredRaster;
	if_fail_fe(restoredRaster.Load(childInput));
	::gpk::bgra restoredColor = {};
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_RASTER_CLEAR_FRAME, systemEvent.Type != ::gpk::SYSTEM_EVENT_Raster || restoredRaster.Type != EVENT_TEST_RASTER_Clear || !eventTestPODRead(restoredRaster.Data, restoredColor) || restoredColor != clearColor || childInput.size());

	if_fail_fe(systemEvent.Load(input));
	childInput = systemEvent.Data;
	if_fail_fe(restoredRaster.Load(childInput));
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_RASTER_TEXT_FRAME, systemEvent.Type != ::gpk::SYSTEM_EVENT_Raster || restoredRaster.Type != EVENT_TEST_RASTER_Text || !eventTestBytesEqual(restoredRaster.Data, {(cnst ::gpk::u0_t*)text.begin(), text.size()}) || childInput.size());
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_FRAME_BOUNDARY, input.size());
	rtrn 0;
}

sttc ::gpk::err_t testEventRuntimeQueues(::gpk::ATestError & errors) {
	::gpk::SRuntimeValues runtime;
	::gpk::PEventSystem inputEvent = {};
	inputEvent->Type = ::gpk::SYSTEM_EVENT_Keyboard;
	inputEvent->Data = ::gpk::vcu0_t{(cnst ::gpk::u0_t*)"in", 2};
	if_fail_fe(runtime.EventsIn.Post(inputEvent));
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_RUNTIME_INDEPENDENT, runtime.EventsIn.Size() != 1 || runtime.EventsOut.Size());
	::gpk::PEventSystem nextInput;
	cnst ::gpk::err_t nextResult = runtime.EventsIn.Next(nextInput);
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_RUNTIME_INPUT, nextResult != 1 || 0 == (::gpk::SEventSystem*)nextInput || nextInput->Type != inputEvent->Type || !eventTestBytesEqual(nextInput->Data, inputEvent->Data) || runtime.EventsIn.Size());

	::gpk::PEventSystem outputEvent = {};
	outputEvent->Type = ::gpk::SYSTEM_EVENT_Device;
	outputEvent->Data = ::gpk::vcu0_t{(cnst ::gpk::u0_t*)"out", 3};
	if_fail_fe(runtime.EventsOut.Post(outputEvent));
	::gpk::apobj<::gpk::SEventSystem> output;
	cnst ::gpk::err_t readResult = runtime.EventsOut.Read(output);
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_RUNTIME_OUTPUT, ::gpk::failed(readResult) || output.size() != 1 || output[0]->Type != outputEvent->Type || !eventTestBytesEqual(output[0]->Data, outputEvent->Data) || runtime.EventsOut.Size());
	GPK_TEST_CHECK(errors, EVENT_TEST_RESULT_RUNTIME_INDEPENDENT, runtime.EventsIn.Size() || runtime.EventsOut.Size() || 0 != runtime.EventsIn.Next(nextInput));
	rtrn 0;
}

::gpk::err_t test_event(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	if_fail_fe(testEventSerialization(errors));
	if_fail_fe(testEventNesting(errors));
	if_fail_fe(testEventPlatformInput(errors));
	if_fail_fe(testEventEmbeddedStream(errors));
	if_fail_fe(testEventRuntimeQueues(errors));
	rtrn 0;
}
