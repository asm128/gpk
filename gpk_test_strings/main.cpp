#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"
#include "gpk_find.h"

// Splits a file into file.split.## parts.
int									main							(int , char ** )		{
	const ::gpk::view_const_string text0[1] = {"habia una vez un bru un brujito en gulubu.\r\n"};
	//const ::gpk::view_const_string text1[1] = {"habia una vez un bru un brujito en gulubu.\r\n"};
	const ::gpk::view_const_string words[11] = {"habia", "una", "vez", "un", "bru", "un", "brujito", "en", "gulubu", ".", "\r\n"};
	info_printf("%u:%s.", text0[0].size(), text0[0].begin());
	for(uint32_t iWord = 0; iWord < ::gpk::size(words); ++iWord) {
		::gpk::view_const_char left, right;
		::gpk::view_const_char word = words[iWord];
		::gpk::view_const_char text = text0[0];
		int32_t iPos = ::gpk::split(word, text, left, right);
		info_printf("Word (%u): '%s'. Position: %i.", iWord, ::gpk::toString(word).begin(), iPos);
		info_printf("Left (%u): '%s'. Right(%u): '%s'", left.size(), ::gpk::toString(left).begin(), right.size(), ::gpk::toString(right).begin());
	}
	return 0;
}
