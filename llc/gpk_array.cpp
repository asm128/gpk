#include "gpk_array.h"
#include "gpk_stdstring.h"

::gpk::error_t			gpk::filterPostfix	(::gpk::view<const ::gpk::vcc> input, const ::gpk::vcc postfix, ::gpk::aobj<::gpk::vcc> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		const ::gpk::vcc & currentInput = input[iInput]; 
		if((postfix.size() < currentInput.size() || (nullIncluded && postfix.size() == currentInput.size())) && 0 == strncmp(currentInput.end() - postfix.size(), postfix.begin(), postfix.size())) 
			filtered.push_back(currentInput); 
	} 
	return 0; 
}

::gpk::error_t			gpk::filterPrefix	(::gpk::view<const ::gpk::vcc> input, const ::gpk::vcc prefix, ::gpk::aobj<::gpk::vcc> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		const ::gpk::vcc & currentInput = input[iInput]; 
		if((prefix .size() < currentInput.size() || (nullIncluded && prefix .size() == currentInput.size())) && 0 == strncmp(currentInput.begin(), prefix .begin(), prefix .size())) 
			filtered.push_back(currentInput); 
	}
	return 0; 
}

::gpk::error_t			gpk::camelCase		(::gpk::vcc input, ::gpk::achar & camelCased) {
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
