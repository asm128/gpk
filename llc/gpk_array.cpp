#include "gpk_array.h"

::gpk::error_t									gpk::filterPostfix					(::gpk::view_array<const ::gpk::vcc> input, const ::gpk::vcc postfix, ::gpk::array_obj<::gpk::vcc> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		const ::gpk::vcc & currentInput = input[iInput]; 
		if((postfix.size() < currentInput.size() || (nullIncluded && postfix.size() == currentInput.size())) && 0 == strncmp(currentInput.end() - postfix.size(), postfix.begin(), postfix.size())) 
			filtered.push_back(currentInput); 
	} 
	return 0; 
}

::gpk::error_t									gpk::filterPrefix					(::gpk::view_array<const ::gpk::vcc> input, const ::gpk::vcc prefix, ::gpk::array_obj<::gpk::vcc> & filtered, bool nullIncluded) { 
	for(uint32_t iInput = 0; iInput < input.size(); ++iInput) { 
		const ::gpk::vcc & currentInput = input[iInput]; 
		if((prefix .size() < currentInput.size() || (nullIncluded && prefix .size() == currentInput.size())) && 0 == strncmp(currentInput.begin(), prefix .begin(), prefix .size())) 
			filtered.push_back(currentInput); 
	}
	return 0; 
}

