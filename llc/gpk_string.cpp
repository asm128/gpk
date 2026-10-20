#include "gpk_string.h"
#include "gpk_stdstring.h"

gpk::err_t	gpk::camelCase		(vcst_t input, string & camelCased) {
	bool						capsNext			= true;
	for(::gpk::u2_t i = 0; i < input.size(); ++i) {
		char						current				= input[i];
		if(current == '_' || current == '-')
			capsNext	= true;
		else {
			if(capsNext) {
				::gpk::toupper(current);
				capsNext	= false;
			}
			//else
			//	tolower(vc{&camelCased[1], camelCased.size() - 1});
			if_fail_fe(camelCased.push_back(current));
		}
	}
	return 0;
}


