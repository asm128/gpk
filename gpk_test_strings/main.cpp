#define GPK_STDOUT_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_file.h"

// Splits a file into file.split.## parts.
int									main							(int , char ** )		{
	const ::gpk::view_const_string text = "habia una vez un bru un brujito en gulubu.\r\n";
	const ::gpk::view_const_string text1 = "\n \r \n habia una vez un bru un brujito en gulubu.\r\n";
	const ::gpk::view_const_string words[11] = {"habia", "una", "vez", "un", "bru", "un", "brujito", "en", "gulubu", ".", "\r\n"};
	info_printf("%u:%s.", text.size(), text.begin());
	for(uint32_t iWord = 0; iWord < ::gpk::size(words); ++iWord) {
		const ::gpk::view_const_char word = words[iWord];

		::gpk::view_const_char left, right;
		const int32_t iPos = ::gpk::split(word, text, left, right);
		info_printf("Word (%u): '%s'. Position: %i.", iWord, ::gpk::toString(word).begin(), iPos);
		info_printf("Left (%u): '%s'. Right(%u): '%s'", left.size(), ::gpk::toString(left).begin(), right.size(), ::gpk::toString(right).begin());
	}

	info_printf("%u:%s.", text1.size(), text1.begin());
	for(uint32_t iWord = 0; iWord < ::gpk::size(words); ++iWord) {
		const ::gpk::view_const_char word = words[iWord];

		::gpk::view_const_char left, right;
		const int32_t iPos = ::gpk::split(word, text1, left, right);
		info_printf("Word (%u): '%s'. Position: %i.", iWord, ::gpk::toString(word).begin(), iPos);
		info_printf("Left (%u): '%s'. Right(%u): '%s'", left.size(), ::gpk::toString(left).begin(), right.size(), ::gpk::toString(right).begin());
	}

	::gpk::view_const_char trimmed;
	::gpk::rtrim(trimmed, text); info_printf("trimmed (%u): %s.", trimmed.size(), ::gpk::toString(trimmed).begin());
	::gpk::ltrim(trimmed, text); info_printf("trimmed (%u): %s.", trimmed.size(), ::gpk::toString(trimmed).begin());
	::gpk::trim (trimmed, text); info_printf("trimmed (%u): %s.", trimmed.size(), ::gpk::toString(trimmed).begin());
	::gpk::rtrim(trimmed, text1); info_printf("trimmed (%u): %s.", trimmed.size(), ::gpk::toString(trimmed).begin());
	::gpk::ltrim(trimmed, text1); info_printf("trimmed (%u): %s.", trimmed.size(), ::gpk::toString(trimmed).begin());
	::gpk::trim (trimmed, text1); info_printf("trimmed (%u): %s.", trimmed.size(), ::gpk::toString(trimmed).begin());
	return 0;
}
