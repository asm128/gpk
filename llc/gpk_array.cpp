#include "gpk_array.h"
gpk::err_t	gpk::filterPostfix	(view<cnst vcst_t> input, vcst_t postfix, aobj<vcst_t> & filtered, bool nullIncluded) { 
	for(::gpk::u2_t iInput = 0; iInput < input.size(); ++iInput) { 
		vcst_t	currentInput = input[iInput]; 
		if((postfix.size() < currentInput.size() || (nullIncluded && postfix.size() == currentInput.size())) && 0 == strncmp(currentInput.end() - postfix.size(), postfix.begin(), postfix.size())) 
			filtered.push_back(currentInput); 
	} 
	return 0; 
}

gpk::err_t	gpk::filterPrefix	(view<cnst vcst_t> input, vcst_t prefix, aobj<vcst_t> & filtered, bool nullIncluded) { 
	for(::gpk::u2_t iInput = 0; iInput < input.size(); ++iInput) { 
		vcst_t	currentInput = input[iInput]; 
		if((prefix .size() < currentInput.size() || (nullIncluded && prefix .size() == currentInput.size())) && 0 == strncmp(currentInput.begin(), prefix .begin(), prefix .size())) 
			if_fail_fe(filtered.push_back(currentInput)); 
	}
	return 0; 
}


