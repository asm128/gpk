#include "gpk_array.h"
#include "gpk_stdstring.h"

gpk::err_t			gpk::filterPostfix	(view<vcu0_c> input, vcu0_c postfix, aobj<vcu0_t> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		vcu0_c & currentInput = input[iInput]; 
		if((postfix.size() < currentInput.size() || (nullIncluded && postfix.size() == currentInput.size())) && 0 == memcmp(currentInput.end() - postfix.size(), postfix.begin(), postfix.size())) 
			filtered.push_back(currentInput); 
	} 
	return 0; 
}

gpk::err_t			gpk::filterPrefix	(view<vcu0_c> input, vcu0_c prefix, aobj<vcu0_t> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		vcu0_c & currentInput = input[iInput]; 
		if((prefix .size() < currentInput.size() || (nullIncluded && prefix .size() == currentInput.size())) && 0 == memcmp(currentInput.begin(), prefix .begin(), prefix .size())) 
			filtered.push_back(currentInput); 
	}
	return 0; 
}

gpk::err_t			gpk::filterPostfix	(view<vcst_c> input, vcst_c postfix, aobj<vcst_t> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		vcst_c & currentInput = input[iInput]; 
		if((postfix.size() < currentInput.size() || (nullIncluded && postfix.size() == currentInput.size())) && 0 == strncmp(currentInput.end() - postfix.size(), postfix.begin(), postfix.size())) 
			filtered.push_back(currentInput); 
	} 
	return 0; 
}

gpk::err_t			gpk::filterPrefix	(view<vcst_c> input, vcst_c prefix, aobj<vcst_t> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		vcst_c & currentInput = input[iInput]; 
		if((prefix .size() < currentInput.size() || (nullIncluded && prefix .size() == currentInput.size())) && 0 == strncmp(currentInput.begin(), prefix .begin(), prefix .size())) 
			filtered.push_back(currentInput); 
	}
	return 0; 
}

gpk::err_t			gpk::camelCase		(vcsc_t input, asc_t & camelCased) {
	bool						capsNext			= true;
	for(uint32_t i = 0; i < input.size(); ++i) {
		char						current				= input[i];
		if(current == '_' || current == '-')
			capsNext	= true;
		else {
			if(capsNext) {
				toupper(current);
				capsNext	= false;
			}
			//else
			//	tolower(vc{&camelCased[1], camelCased.size() - 1});
			camelCased.push_back(current);
		}
	}
	return 0;
}
