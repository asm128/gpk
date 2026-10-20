#include "gpk_datatype.h"
#include "gpk_label.h"
#include "gpk_array_obj.h"

#ifndef GPK_POD_DEFINITION_H_23627
#define GPK_POD_DEFINITION_H_23627

namespace gpk
{
	struct SPODDefinition {
		au0_t			DefaultData;
		aobj<label>		Labels;
		au2_t			Offsets;
		apod<DATA_TYPE>	Types;

		err_t				push_any		(DATA_TYPE type, vcst_t labelName, vcu0_c & defaultValue) {
			label				ownedLabel		= labelName;
			if_true_fef(false == (ownedLabel == labelName), "Failed to retain label:'%.*s'.", GPK_VIEW_ARGS(labelName));
			if_true_fef(0 <= Labels.find(ownedLabel), "'%.*s' already exists.", GPK_VIEW_ARGS(labelName));
			cnst SDataTypeID	typeInfo		= type;
			if_true_fef(typeInfo.IsUniform() && typeInfo.TotalBytes() != defaultValue.size(), "Default byte count:%" GPK_FMT_U2 " does not match type byte count:%" GPK_FMT_U2 ".", defaultValue.size(), typeInfo.TotalBytes());
			if_fail_fe(Labels.reserve(Labels.size() + 1));
			if_fail_fe(Offsets.reserve(Offsets.size() + 1));
			if_fail_fe(Types.reserve(Types.size() + 1));
			if_fail_fe(DefaultData.reserve(DefaultData.size() + defaultValue.size()));
			u2_c				defaultOffset	= DefaultData.size();
			if_fail_fe(DefaultData.append(defaultValue));
			if_fail_fe(Types.push_back(type));
			if_fail_fe(Offsets.push_back(defaultOffset));
			return Labels.push_back(ownedLabel);
		}

		tplt<tpnm _tValue>
		err_t				push_any		(vcst_t labelName, const _tValue defaultValue) {
			return push_any(dataType(defaultValue), labelName, vcu0_t{(const u0_t*)&defaultValue, szof(_tValue)});
		}

		err_t				push_u8			(vcst_t label, u0_t defaultValue = u0_t(DEBUG_BUILD ? 0xFFU : 0U)) { return push_any(label, defaultValue); }
		err_t				push_u16		(vcst_t label, u1_t defaultValue = u1_t(DEBUG_BUILD ? 0xFFFFU : 0U)) { return push_any(label, defaultValue); }
		err_t				push_u32		(vcst_t label, u2_t defaultValue = u2_t(DEBUG_BUILD ? 0xFFFFFFFFU : 0U)) { return push_any(label, defaultValue); }
		err_t				push_u64		(vcst_t label, u3_t defaultValue = u3_t(DEBUG_BUILD ? 0xFFFFFFFFFFFFFFFFULL : 0ULL)) { return push_any(label, defaultValue); }
		err_t				push_i8			(vcst_t label, s0_t defaultValue = s0_t(DEBUG_BUILD ? -1 : 0)) { return push_any(label, defaultValue); }
		err_t				push_i16		(vcst_t label, s1_t defaultValue = s1_t(DEBUG_BUILD ? -1 : 0)) { return push_any(label, defaultValue); }
		err_t				push_i32		(vcst_t label, s2_t defaultValue = s2_t(DEBUG_BUILD ? -1 : 0)) { return push_any(label, defaultValue); }
		err_t				push_i64		(vcst_t label, s3_t defaultValue = s3_t(DEBUG_BUILD ? -1 : 0)) { return push_any(label, defaultValue); }
		err_t				push_f32		(vcst_t label, f2_t defaultValue = (f2_t)DEBUG_BUILD) { return push_any(label, defaultValue); }
		err_t				push_f64		(vcst_t label, f3_t defaultValue = (f3_t)DEBUG_BUILD) { return push_any(label, defaultValue); }
		err_t				push_uchar		(vcst_t label, uc_t defaultValue = uc_t(DEBUG_BUILD ? 0xFFU : 0U)) { return push_any(label, defaultValue); }
		err_t				push_char		(vcst_t label, sc_t defaultValue = sc_t(DEBUG_BUILD ? 0xFFU : 0U)) { return push_any(label, defaultValue); }
	};
} // namespace

#endif // GPK_POD_DEFINITION_H_23627


