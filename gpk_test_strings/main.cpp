#define GPK_STDOUT_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_file.h"
#include "gpk_runtime.h"

using namespace gpk;

// Splits a file into file.split.## parts.
static ::gpk::err_t					testStringsMain					(::gpk::SRuntimeValues &)	{
	const ::gpk::vcst_t text = "habia una vez un bru un brujito en gulubu.\r\n";
	const ::gpk::vcst_t text1 = "\n \r \n habia una vez un bru un brujito en gulubu.\r\n";
	const ::gpk::vcst_t words[11] = {"habia", "una", "vez", "un", "bru", "un", "brujito", "en", "gulubu", ".", "\r\n"};
	info_printf("%u:%s.", text.size(), text.begin());
	for(uint32_t iWord = 0; iWord < ::gpk::size(words); ++iWord) {
		const ::gpk::view<sc_t> word = words[iWord];

		::gpk::view<sc_t> left, right;
		const int32_t iPos = ::gpk::split(word, text, left, right);
		info_printf("Word (%u): '%s'. Position: %i.", iWord, GPK_VIEW_ARGS(word).begin(), iPos);
		info_printf("Left (%u): '%s'. Right(%u): '%s'", left.size(), GPK_VIEW_ARGS(left).begin(), right.size(), GPK_VIEW_ARGS(right).begin());
	}

	info_printf("%u:%s.", text1.size(), text1.begin());
	for(uint32_t iWord = 0; iWord < ::gpk::size(words); ++iWord) {
		const ::gpk::view<sc_t> word = words[iWord];

		::gpk::view<sc_t> left, right;
		const int32_t iPos = ::gpk::split(word, text1, left, right);
		info_printf("Word (%u): '%s'. Position: %i.", iWord, GPK_VIEW_ARGS(word).begin(), iPos);
		info_printf("Left (%u): '%s'. Right(%u): '%s'", left.size(), GPK_VIEW_ARGS(left).begin(), right.size(), GPK_VIEW_ARGS(right).begin());
	}

	::gpk::view<sc_t> trimmed;
	::gpk::rtrim(trimmed, text); info_printf("trimmed (%u): %s.", trimmed.size(), GPK_VIEW_ARGS(trimmed).begin());
	::gpk::ltrim(trimmed, text); info_printf("trimmed (%u): %s.", trimmed.size(), GPK_VIEW_ARGS(trimmed).begin());
	::gpk::trim (trimmed, text); info_printf("trimmed (%u): %s.", trimmed.size(), GPK_VIEW_ARGS(trimmed).begin());
	::gpk::rtrim(trimmed, text1); info_printf("trimmed (%u): %s.", trimmed.size(), GPK_VIEW_ARGS(trimmed).begin());
	::gpk::ltrim(trimmed, text1); info_printf("trimmed (%u): %s.", trimmed.size(), GPK_VIEW_ARGS(trimmed).begin());
	::gpk::trim (trimmed, text1); info_printf("trimmed (%u): %s.", trimmed.size(), GPK_VIEW_ARGS(trimmed).begin());
	return 0;
}

GPK_OS_ENTRY_POINT(::testStringsMain);
