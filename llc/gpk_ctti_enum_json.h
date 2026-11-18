#include "gpk_ctti_enum.h"
#include "gpk_append_json.h"
#include "gpk_array_static.h"
#include "gpk_json.h"

#ifndef GPK_CTTI_ENUM_JSON_H
#define GPK_CTTI_ENUM_JSON_H

nmsp gpk
{
	tplt<tpnm T> requires ctti_enum<T>
	err_t cttiValueSave(string & output, cnst T & input) {
		cnst vcst_t name = cttiEnumName(input);
		if(name.size())
			rtrn appendJSONString(output, name);
		rtrn append_strings(output, str((s3_t)input));
	}

	tplt<tpnm T> requires ctti_enum<T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		usng TEnum = rm_csvl_t<T>;
		cnst auto & metadata = cttiEnumTyped<TEnum>();
		if(JSON_TYPE_STRING == input.Token[iNode].Type) {
			vcst_t name = {};
			if_fail_fe(jsonObjectGetString(input, iNode, name));
			for(u2_t iValue = 0; iValue < metadata.Count; ++iValue)
				if(metadata.Values[iValue].ValueResolved
				&& (name == cttiEnumValueName<TEnum>(iValue) || name == cttiEnumValueName<TEnum>(iValue, true))
				) {
					output = metadata.Values[iValue].Value;
					rtrn iNode;
				}
			rtrn -1;
		}
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_INTEGER);
		s3_t numeric = {};
		if_fail_fe(jsonObjectGetInteger(input, iNode, numeric));
		output = (T)numeric;
		rtrn iNode;
	}
} // nmsp

#endif // GPK_CTTI_ENUM_JSON_H
