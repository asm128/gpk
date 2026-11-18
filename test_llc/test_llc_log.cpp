#include "gpk_log.h"

#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(LOG_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(LOG_TEST_RESULT, COMPILE_TIME, 0, "append_log() or gpk_log() failed during constant evaluation.");
GDEFINE_ENUM_VALUED(LOG_TEST_RESULT, RUNTIME_COUNT, 1, "gpk_log() returned the wrong runtime byte count.");
GDEFINE_ENUM_VALUED(LOG_TEST_RESULT, RUNTIME_TEXT, 2, "gpk_log() sent the wrong bytes to the configured log callback.");

static_assert(::gpk::append_log("value", ':', "42") == 8);
static_assert(::gpk::_gpk_log(3, "file.cpp", 42, "func", "value", ':', "42") == 30);
stxp ::gpk::b8_t testLogMacroConstant() { rtrn gpk_log(3, "x") > 2; }
static_assert(::testLogMacroConstant());

sttc ::gpk::sc_t LOG_CAPTURE[64] = {};
sttc ::gpk::u2_t LOG_CAPTURE_SIZE = 0;

sttc ::gpk::err_t testLogWrite(cnst ::gpk::sc_t * text, ::gpk::u2_t textLen) {
	if(0 == text && 0 != textLen)
		rtrn -1;
	if(LOG_CAPTURE_SIZE + textLen > ::gpk::size(LOG_CAPTURE))
		rtrn -1;
	for(::gpk::u2_t iChar = 0; iChar < textLen; ++iChar)
		LOG_CAPTURE[LOG_CAPTURE_SIZE++] = text[iChar];
	rtrn textLen;
}

sttc ::gpk::err_t testLogPrint(cnst ::gpk::sc_t * text) {
	::gpk::u2_t textLen = 0;
	if(text)
		while(text[textLen])
			++textLen;
	rtrn testLogWrite(text, textLen);
}

::gpk::err_t test_log(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t) {
	::gpk::log_print_t oldPrint = {};
	::gpk::log_write_t oldWrite = {};
	if_fail_fe(::gpk::getLogCallbacks(oldPrint, oldWrite));
	if_fail_fe(::gpk::setupLogCallbacks(::testLogPrint, ::testLogWrite));
	LOG_CAPTURE_SIZE = 0;

	cnst ::gpk::err_t result = ::gpk::_gpk_log(3, "file.cpp", 42, "func", "value", ':', "42");
	cnst ::gpk::u2_t capturedSize = LOG_CAPTURE_SIZE;
	cnst ::gpk::sc_t expected[] = "3|file.cpp(42){func}:value:42\n";
	::gpk::b8_t textMatches = capturedSize == ::gpk::size(expected) - 1;
	for(::gpk::u2_t iChar = 0; textMatches && iChar < capturedSize; ++iChar)
		textMatches = LOG_CAPTURE[iChar] == expected[iChar];

	cnst ::gpk::err_t restoreResult = ::gpk::setupLogCallbacks(oldPrint, oldWrite);
	GPK_TEST_CHECK(errors, LOG_TEST_RESULT_RUNTIME_COUNT, result != 30 || restoreResult < 0);
	GPK_TEST_CHECK(errors, LOG_TEST_RESULT_RUNTIME_TEXT, false == textMatches);
	rtrn 0;
}
