#include "gpk_array.h"
#include "gpk_stdstring.h"

::gpk::error_t			gpk::filterPostfix	(::gpk::view<::gpk::vcsc_c> input, ::gpk::vcsc_c postfix, ::gpk::aobj<::gpk::vcsc_t> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		::gpk::vcsc_c & currentInput = input[iInput]; 
		if((postfix.size() < currentInput.size() || (nullIncluded && postfix.size() == currentInput.size())) && 0 == strncmp(currentInput.end() - postfix.size(), postfix.begin(), postfix.size())) 
			filtered.push_back(currentInput); 
	} 
	return 0; 
}

::gpk::error_t			gpk::filterPrefix	(::gpk::view<::gpk::vcsc_c> input, ::gpk::vcsc_c prefix, ::gpk::aobj<::gpk::vcsc_t> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		::gpk::vcsc_c & currentInput = input[iInput]; 
		if((prefix .size() < currentInput.size() || (nullIncluded && prefix .size() == currentInput.size())) && 0 == strncmp(currentInput.begin(), prefix .begin(), prefix .size())) 
			filtered.push_back(currentInput); 
	}
	return 0; 
}

::gpk::error_t			gpk::camelCase		(::gpk::vcsc_t input, ::gpk::asc_t & camelCased) {
	bool						capsNext			= true;
	for(uint32_t i = 0; i < input.size(); ++i) {
		char						current				= input[i];
		if(current == '_' || current == '-')
			capsNext	= true;
		else {
			if(capsNext) {
				::gpk::toupper(current);
				capsNext	= false;
			}
			//else
			//	::gpk::tolower(::gpk::vc{&camelCased[1], camelCased.size() - 1});
			camelCased.push_back(current);
		}
	}
	return 0;
}
