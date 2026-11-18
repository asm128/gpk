#include "gpk_ctti_stct.h"
#include "gpk_ctti_enum_json.h"
#include "gpk_append_json.h"
#include "gpk_array_obj.h"
#include "gpk_array_static.h"
#include "gpk_cstring.h"
#include "gpk_json.h"
#include "gpk_string.h"

#ifndef GPK_CTTI_STCT_JSON_H
#define GPK_CTTI_STCT_JSON_H

nmsp gpk
{
	tydf err_t (*FCTTIFieldJSONSave)(string & output, cnst void * instance);
	tydf err_t (*FCTTIFieldJSONLoad)(cnst SJSONReader & input, u2_t iNode, void * instance);
	stct SCTTIFieldJSONOverride {
		vcst_t				Name	= {};
		FCTTIFieldJSONSave	Save	= {};
		FCTTIFieldJSONLoad	Load	= {};
	};

	inln cnst SCTTIFieldJSONOverride * cttiFieldJSONOverride(view<cnst SCTTIFieldJSONOverride> overrides, vcst_t name) nxpt {
		for(u2_t iOverride = 0; iOverride < overrides.size(); ++iOverride)
			if(overrides[iOverride].Name == name)
				rtrn &overrides[iOverride];
		rtrn {};
	}

	stct SCTTIMemberJSONOpen {
		vcst_t	MemberName	= {};
		vcst_t	TypeName	= {};
	};

	inln err_t append_string(asc_t & output, cnst SCTTIMemberJSONOpen & member) {
		rtrn append_strings(output, '"', member.MemberName, "\":{\"type\":\"", member.TypeName, "\",\"value\":");
	}

	stct SCTTIStructDeclaration {
		vcst_t	Name	= {};
		vcst_t	File	= {};
		u2_t	Line	= {};
		uP_t	Size	= {};
		b8_t	IsPOD	= false;
	};

	inln aobj<SCTTIStructDeclaration> & cttiStructDeclarations() nxpt {
		sttc aobj<SCTTIStructDeclaration> declarations = {};
		rtrn declarations;
	}

	inln cnst SCTTIStructDeclaration & cttiStruct(vcst_t name) nxpt {
		cnst aobj<SCTTIStructDeclaration> & declarations = cttiStructDeclarations();
		for(u2_t iDeclaration = 0; iDeclaration < declarations.size(); ++iDeclaration)
			if(declarations[iDeclaration].Name == name)
				rtrn declarations[iDeclaration];
		sttc cnst SCTTIStructDeclaration missing = {};
		rtrn missing;
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t cttiStructRegister() {
		cnst auto & metadata = cttiStruct<T>();
		cnst vcst_t name = metadata.View(metadata.StructName);
		aobj<SCTTIStructDeclaration> & declarations = cttiStructDeclarations();
		for(u2_t iDeclaration = 0; iDeclaration < declarations.size(); ++iDeclaration)
			if(declarations[iDeclaration].Name == name) {
				if_true_fe(declarations[iDeclaration].Size != SCTTIStructRegistry<T>::Size);
				rtrn iDeclaration;
			}
		rtrn declarations.push_back
			({ name
			, SCTTIStructRegistry<T>::File
			, SCTTIStructRegistry<T>::Line
			, SCTTIStructRegistry<T>::Size
			, SCTTIStructRegistry<T>::IsPOD
			});
	}

	tplt<tpnm TStruct, tpnm TFunc, tpnm... TMembers>
	err_t cttiVisitMembers(TStruct & instance, u2_c index, TFunc func, type_pack<TMembers...>) {
		u0_t (&bytes)[sizeof(TStruct)] = reinterpret_cast<u0_t(&)[sizeof(TStruct)]>(instance);
		uP_t offset = {};
		u2_t current = {};
		err_t result = -1;
		( [&]() {
			stxp uP_t alignment = cttiMemberAlignment<TStruct, TMembers>();
			offset += (alignment - offset % alignment) % alignment;
			if(current++ == index)
				result = func(reinterpret_cast<TMembers&>(bytes[offset]));
			offset += sizeof(TMembers);
		}(), ... );
		rtrn result;
	}
	tplt<tpnm TStruct, tpnm TFunc, tpnm... TMembers>
	err_t cttiVisitMembers(cnst TStruct & instance, u2_c index, TFunc func, type_pack<TMembers...>) {
		cnst u0_t (&bytes)[sizeof(TStruct)] = reinterpret_cast<cnst u0_t(&)[sizeof(TStruct)]>(instance);
		uP_t offset = {};
		u2_t current = {};
		err_t result = -1;
		( [&]() {
			stxp uP_t alignment = cttiMemberAlignment<TStruct, TMembers>();
			offset += (alignment - offset % alignment) % alignment;
			if(current++ == index)
				result = func(reinterpret_cast<cnst TMembers&>(bytes[offset]));
			offset += sizeof(TMembers);
		}(), ... );
		rtrn result;
	}

	tplt<tpnm T>
	inln aobj<SCTTIFieldJSONOverride> & cttiFieldJSONOverrides() nxpt {
		sttc aobj<SCTTIFieldJSONOverride> overrides = {};
		rtrn overrides;
	}

	tplt<tpnm T>
	err_t cttiFieldJSONOverrideRegister(vcst_t name, FCTTIFieldJSONSave save, FCTTIFieldJSONLoad load) {
		aobj<SCTTIFieldJSONOverride> & overrides = cttiFieldJSONOverrides<rm_csvl_t<T>>();
		for(u2_t iOverride = 0; iOverride < overrides.size(); ++iOverride)
			if(overrides[iOverride].Name == name) {
				if(save) overrides[iOverride].Save = save;
				if(load) overrides[iOverride].Load = load;
				rtrn iOverride;
			}
		rtrn overrides.push_back({name, save, load});
	}

	tplt<tpnm T>
	inln cnst SCTTIFieldJSONOverride * cttiFieldJSONOverride(vcst_t name) nxpt {
		cnst aobj<SCTTIFieldJSONOverride> & overrides = cttiFieldJSONOverrides<rm_csvl_t<T>>();
		for(u2_t iOverride = 0; iOverride < overrides.size(); ++iOverride)
			if(overrides[iOverride].Name == name)
				rtrn &overrides[iOverride];
		rtrn {};
	}

	tplt<tpnm T, tpnm TFunc>
	err_t cttiVisit(T & instance, u2_c index, TFunc func) {
		usng TStruct = rm_csvl_t<T>;
		usng TIndices = tpnm ctti_index_pack_make<SCTTIStructRegistry<TStruct>::Value.Count>::Type;
		usng TMembers = tpnm SCTTIFieldTypes<TStruct, TIndices>::Type;
		rtrn cttiVisitMembers(instance, index, func, TMembers{});
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t saveCTTI(string & output, cnst T & input);
	tplt<tpnm T> requires ctti_struct<T>
	err_t saveCTTI(string & output, cnst T & input, view<cnst SCTTIFieldJSONOverride> overrides);
	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(cnst SJSONReader & input, u2_c iNode, T & output);
	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(cnst SJSONReader & input, u2_c iNode, T & output, view<cnst SCTTIFieldJSONOverride> overrides);

	inln err_t cttiValueSave(string & output, b8_c input) {
		rtrn append_strings(output, str(input));
	}
	tplt<tpnm T> requires ctti_integer<T>
	err_t cttiValueSave(string & output, cnst T & input) {
		if constexpr(ctti_same<rm_csvl_t<T>, s0_t>::Value || ctti_same<rm_csvl_t<T>, s1_t>::Value || ctti_same<rm_csvl_t<T>, s2_t>::Value || ctti_same<rm_csvl_t<T>, s3_t>::Value)
			rtrn append_strings(output, str((s3_t)input));
		else
			rtrn append_strings(output, str((u3_t)input));
	}
	tplt<tpnm T> requires ctti_decimal<T>
	err_t cttiValueSave(string & output, cnst T & input) {
		sc_t text[64] = {};
		cnst err_t length = sprintf_s(text, "%.17g", (f3_t)input);
		if_fail_fe(length);
		rtrn output.append({text, (u2_t)length});
	}
	tplt<tpnm T> requires ctti_struct<T>
	err_t cttiValueSave(string & output, cnst T & input) {
		rtrn saveCTTI(output, input);
	}
	tplt<tpnm T>
	concept ctti_json_xy = requires(cnst T & value) { value.x; value.y; };
	tplt<tpnm T>
	concept ctti_json_rectangle = requires(cnst T & value) { value.Left; value.Top; value.Right; value.Bottom; };
	tplt<tpnm T>
	concept ctti_json_area = requires(cnst T & value) { value.Offset; value.Size; };
	tplt<tpnm T>
	concept ctti_json_minmax = requires(cnst T & value) { value.Min; value.Max; };
	tplt<tpnm T>
	concept ctti_json_triangle = requires(cnst T & value) { value.A; value.B; value.C; };

	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_xy<T>)
	err_t cttiValueSave(string & output, cnst T & input) {
		u2_c offset = output.size();
		if_fail_fe(append_strings(output, "{\"x\":"));
		if_fail_fe(cttiValueSave(output, input.x));
		if_fail_fe(append_strings(output, ",\"y\":"));
		if_fail_fe(cttiValueSave(output, input.y));
		if_fail_fe(output.push_back('}'));
		rtrn output.size() - offset;
	}
	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_rectangle<T>)
	err_t cttiValueSave(string & output, cnst T & input) {
		u2_c offset = output.size();
		if_fail_fe(append_strings(output, "{\"Left\":"));
		if_fail_fe(cttiValueSave(output, input.Left));
		if_fail_fe(append_strings(output, ",\"Top\":"));
		if_fail_fe(cttiValueSave(output, input.Top));
		if_fail_fe(append_strings(output, ",\"Right\":"));
		if_fail_fe(cttiValueSave(output, input.Right));
		if_fail_fe(append_strings(output, ",\"Bottom\":"));
		if_fail_fe(cttiValueSave(output, input.Bottom));
		if_fail_fe(output.push_back('}'));
		rtrn output.size() - offset;
	}
	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_area<T>)
	err_t cttiValueSave(string & output, cnst T & input) {
		u2_c offset = output.size();
		if_fail_fe(append_strings(output, "{\"Offset\":"));
		if_fail_fe(cttiValueSave(output, input.Offset));
		if_fail_fe(append_strings(output, ",\"Size\":"));
		if_fail_fe(cttiValueSave(output, input.Size));
		if_fail_fe(output.push_back('}'));
		rtrn output.size() - offset;
	}
	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_minmax<T>)
	err_t cttiValueSave(string & output, cnst T & input) {
		u2_c offset = output.size();
		if_fail_fe(append_strings(output, "{\"Min\":"));
		if_fail_fe(cttiValueSave(output, input.Min));
		if_fail_fe(append_strings(output, ",\"Max\":"));
		if_fail_fe(cttiValueSave(output, input.Max));
		if_fail_fe(output.push_back('}'));
		rtrn output.size() - offset;
	}
	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_triangle<T>)
	err_t cttiValueSave(string & output, cnst T & input) {
		u2_c offset = output.size();
		if_fail_fe(append_strings(output, "{\"A\":"));
		if_fail_fe(cttiValueSave(output, input.A));
		if_fail_fe(append_strings(output, ",\"B\":"));
		if_fail_fe(cttiValueSave(output, input.B));
		if_fail_fe(append_strings(output, ",\"C\":"));
		if_fail_fe(cttiValueSave(output, input.C));
		if_fail_fe(output.push_back('}'));
		rtrn output.size() - offset;
	}
	inln err_t cttiValueSave(string & output, cnst string & input) {
		rtrn appendJSONString(output, input);
	}
	inln err_t cttiValueSave(string & output, vcst_t input) {
		rtrn appendJSONString(output, input);
	}
	tplt<tpnm T>
	err_t cttiValueSave(string & output, view<T> input) {
		u2_c offset = output.size();
		if_fail_fe(output.push_back('['));
		for(u2_t i = 0; i < input.size(); ++i) {
			if(i) if_fail_fe(output.push_back(','));
			if_fail_fe(cttiValueSave(output, input[i]));
		}
		if_fail_fe(output.push_back(']'));
		rtrn output.size() - offset;
	}
	tplt<tpnm T, u2_t N>
	err_t cttiValueSave(string & output, cnst T (&input)[N]) {
		rtrn cttiValueSave(output, view<cnst T>{input});
	}
	tplt<tpnm T, u2_t N>
	err_t cttiValueSave(string & output, cnst array_static<T, N> & input) {
		rtrn cttiValueSave(output, view<cnst T>{input.begin(), input.size()});
	}
	tplt<tpnm T>
	err_t cttiValueSave(string & output, cnst array_pod<T> & input) {
		rtrn cttiValueSave(output, view<cnst T>{input.begin(), input.size()});
	}
	tplt<tpnm T>
	err_t cttiValueSave(string & output, cnst array_obj<T> & input) {
		rtrn cttiValueSave(output, view<cnst T>{input.begin(), input.size()});
	}

	inln err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, b8_t & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_BOOLEAN);
		rtrn jsonObjectGetBoolean(input, iNode, output);
	}
	tplt<tpnm T> requires ctti_integer<T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_INTEGER);
		rtrn jsonObjectGetInteger(input, iNode, output);
	}
	tplt<tpnm T> requires ctti_decimal<T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if(JSON_TYPE_INTEGER == input.Token[iNode].Type) {
			s3_t integer = {};
			if_fail_fe(jsonObjectGetInteger(input, iNode, integer));
			output = (T)integer;
			rtrn iNode;
		}
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_DECIMAL);
		rtrn jsonObjectGetDecimal(input, iNode, output);
	}
	tplt<tpnm T> requires ctti_struct<T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		rtrn loadCTTI(input, iNode, output);
	}
	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_xy<T>)
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_OBJECT);
		cnst err_t iX = jsonObjectValueGet(input, iNode, "x");
		if(0 <= iX) if_fail_fe(cttiValueLoad(input, (u2_t)iX, output.x));
		cnst err_t iY = jsonObjectValueGet(input, iNode, "y");
		if(0 <= iY) if_fail_fe(cttiValueLoad(input, (u2_t)iY, output.y));
		rtrn iNode;
	}
	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_rectangle<T>)
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_OBJECT);
		cnst err_t iLeft = jsonObjectValueGet(input, iNode, "Left");
		if(0 <= iLeft) if_fail_fe(cttiValueLoad(input, (u2_t)iLeft, output.Left));
		cnst err_t iTop = jsonObjectValueGet(input, iNode, "Top");
		if(0 <= iTop) if_fail_fe(cttiValueLoad(input, (u2_t)iTop, output.Top));
		cnst err_t iRight = jsonObjectValueGet(input, iNode, "Right");
		if(0 <= iRight) if_fail_fe(cttiValueLoad(input, (u2_t)iRight, output.Right));
		cnst err_t iBottom = jsonObjectValueGet(input, iNode, "Bottom");
		if(0 <= iBottom) if_fail_fe(cttiValueLoad(input, (u2_t)iBottom, output.Bottom));
		rtrn iNode;
	}
	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_area<T>)
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_OBJECT);
		cnst err_t iOffset = jsonObjectValueGet(input, iNode, "Offset");
		if(0 <= iOffset) if_fail_fe(cttiValueLoad(input, (u2_t)iOffset, output.Offset));
		cnst err_t iSize = jsonObjectValueGet(input, iNode, "Size");
		if(0 <= iSize) if_fail_fe(cttiValueLoad(input, (u2_t)iSize, output.Size));
		rtrn iNode;
	}
	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_minmax<T>)
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_OBJECT);
		cnst err_t iMin = jsonObjectValueGet(input, iNode, "Min");
		if(0 <= iMin) if_fail_fe(cttiValueLoad(input, (u2_t)iMin, output.Min));
		cnst err_t iMax = jsonObjectValueGet(input, iNode, "Max");
		if(0 <= iMax) if_fail_fe(cttiValueLoad(input, (u2_t)iMax, output.Max));
		rtrn iNode;
	}
	tplt<tpnm T> requires (false == ctti_struct<T> && ctti_json_triangle<T>)
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_OBJECT);
		cnst err_t iA = jsonObjectValueGet(input, iNode, "A");
		if(0 <= iA) if_fail_fe(cttiValueLoad(input, (u2_t)iA, output.A));
		cnst err_t iB = jsonObjectValueGet(input, iNode, "B");
		if(0 <= iB) if_fail_fe(cttiValueLoad(input, (u2_t)iB, output.B));
		cnst err_t iC = jsonObjectValueGet(input, iNode, "C");
		if(0 <= iC) if_fail_fe(cttiValueLoad(input, (u2_t)iC, output.C));
		rtrn iNode;
	}
	inln err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, string & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_STRING);
		rtrn jsonObjectGetString(input, iNode, output);
	}
	inln err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, vcst_t & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_STRING);
		rtrn jsonObjectGetString(input, iNode, output);
	}
	tplt<tpnm T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, view<T> output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_ARRAY);
		cnst err_t count = jsonArraySize(input, iNode);
		if_fail_fe(count);
		if_true_fe((u2_t)count != output.size());
		for(u2_t i = 0; i < output.size(); ++i) {
			cnst err_t iValue = jsonArrayValueGet(input, iNode, i);
			if_fail_fe(iValue);
			if_fail_fe(cttiValueLoad(input, (u2_t)iValue, output[i]));
		}
		rtrn iNode;
	}
	tplt<tpnm T, u2_t N>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T (&output)[N]) {
		rtrn cttiValueLoad(input, iNode, view<T>{output});
	}
	tplt<tpnm T, u2_t N>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, array_static<T, N> & output) {
		rtrn cttiValueLoad(input, iNode, view<T>{output.begin(), output.size()});
	}
	tplt<tpnm T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, array_pod<T> & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_ARRAY);
		cnst err_t count = jsonArraySize(input, iNode);
		if_fail_fe(count);
		if_fail_fe(output.resize((u2_t)count));
		rtrn cttiValueLoad(input, iNode, view<T>{output.begin(), output.size()});
	}
	tplt<tpnm T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, array_obj<T> & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_ARRAY);
		cnst err_t count = jsonArraySize(input, iNode);
		if_fail_fe(count);
		if_fail_fe(output.resize((u2_t)count));
		rtrn cttiValueLoad(input, iNode, view<T>{output.begin(), output.size()});
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t saveCTTI(string & output, cnst T & input, view<cnst SCTTIFieldJSONOverride> overrides) {
		u2_c offset = output.size();
		cnst auto & metadata = cttiStruct<rm_csvl_t<T>>();
		if_fail_fe(output.push_back('{'));
		for(u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
			cnst vcst_t memberName = metadata.View(metadata.Members[iMember].Name);
			if_fail_fe(appendOpenKey(output, memberName, 0 != iMember));
			cnst SCTTIFieldJSONOverride * fieldOverride = cttiFieldJSONOverride(overrides, memberName);
			if(0 == fieldOverride)
				fieldOverride = cttiFieldJSONOverride<T>(memberName);
			if(fieldOverride && fieldOverride->Save) {
				if_fail_fe(fieldOverride->Save(output, &input));
			}
			else {
				if_fail_fe(cttiVisit(input, iMember, [&](cnst auto & member) { rtrn cttiValueSave(output, member); }));
			}
		}
		if_fail_fe(output.push_back('}'));
		rtrn output.size() - offset;
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t saveCTTI(string & output, cnst T & input) {
		rtrn saveCTTI(output, input, {});
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(cnst SJSONReader & input, u2_c iNode, T & output, view<cnst SCTTIFieldJSONOverride> overrides) {
		cnst auto & metadata = cttiStruct<rm_csvl_t<T>>();
		if_true_fe(iNode >= input.Token.size() || input.Token[iNode].Type != JSON_TYPE_OBJECT);
		for(u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
			cnst vcst_t memberName = metadata.View(metadata.Members[iMember].Name);
			cnst err_t iValue = jsonObjectValueGet(input, iNode, memberName);
			if(0 > iValue)
				continue;
			cnst SCTTIFieldJSONOverride * fieldOverride = cttiFieldJSONOverride(overrides, memberName);
			if(0 == fieldOverride)
				fieldOverride = cttiFieldJSONOverride<T>(memberName);
			if(fieldOverride && fieldOverride->Load) {
				if_fail_fe(fieldOverride->Load(input, (u2_t)iValue, &output));
			}
			else {
				if_fail_fe(cttiVisit(output, iMember, [&](auto & member) { rtrn cttiValueLoad(input, (u2_t)iValue, member); }));
			}
		}
		rtrn 0;
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(cnst SJSONReader & input, u2_c iNode, T & output) {
		rtrn loadCTTI(input, iNode, output, {});
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(cnst SJSONReader & input, T & output, view<cnst SCTTIFieldJSONOverride> overrides) {
		rtrn loadCTTI(input, 0, output, overrides);
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(cnst SJSONReader & input, T & output) {
		rtrn loadCTTI(input, 0, output);
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(vcst_t & input, T & output, view<cnst SCTTIFieldJSONOverride> overrides) {
		SJSONReader reader = {};
		if_fail_fe(jsonParse(reader, input));
		if_fail_fe(loadCTTI(reader, output, overrides));
		rtrn input.slice(input, reader.StateRead.IndexCurrentChar + 1);
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(vcst_t & input, T & output) {
		rtrn loadCTTI(input, output, {});
	}

	tplt<size_t NDeclaration>
	err_t cttiMemberJSON
		( cnst SCTTIParseResult<NDeclaration> & metadata
		, string & output
		) {
		output.clear();
		if_fail_fe(output.push_back('{'));
		for(u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
			cnst SCTTIMember & member = metadata.Members[iMember];
			vcst_c memberName = metadata.View(member.Name);
			vcst_c typeName = metadata.View(member.TypeName);
			if(iMember)
				if_fail_fe(output.push_back(','));
			if_fail_fe(append_strings(output, SCTTIMemberJSONOpen{memberName, typeName}));
			if(member.Value.empty()) {
				if_fail_fe(append_strings(output, "null"));
			}
			else {
				vcst_c value = metadata.View(member.Value);
				b8_c stringLiteral = value.size() > 1 && '"' == value[0] && '"' == value[value.size() - 1];
				b8_c keywordLiteral = value == "true" || value == "false" || value == "null" || value == "{}" || value == "[]";
				b8_c numberLiteral = value.size() && (('-' == value[0]) || ('+' == value[0]) || (value[0] >= '0' && value[0] <= '9'));
				b8_c floatSuffix = numberLiteral && value.size() && ('f' == value[value.size() - 1] || 'F' == value[value.size() - 1]);
				if(stringLiteral || keywordLiteral || numberLiteral) {
					if_fail_fe(output.append({value.begin(), value.size() - floatSuffix}));
				}
				else {
					if_fail_fe(append_strings(output, '"', value, '"'));
				}
			}
			if_fail_fe(append_strings(output, ",\"modifiers\":["));
			b8_t firstModifier = true;
			for(u2_t iModifier = member.Modifiers.Offset, modifierStop = member.Modifiers.Offset + member.Modifiers.Count; iModifier < modifierStop;) {
				while(iModifier < modifierStop && cttiSpace(metadata.SourceText[iModifier]))
					++iModifier;
				cnst u2_t modifierOffset = iModifier;
				while(iModifier < modifierStop && false == cttiSpace(metadata.SourceText[iModifier]))
					++iModifier;
				if(modifierOffset == iModifier)
					break;
				if(false == firstModifier)
					if_fail_fe(output.push_back(','));
				if_fail_fe(append_strings(output, '"', vcst_t{&metadata.SourceText[modifierOffset], iModifier - modifierOffset}, '"'));
				firstModifier = false;
			}
			if_fail_fe(append_strings(output, "]}"));
		}
		if_fail_fe(output.push_back('}'));
		rtrn output.size();
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t cttiMemberJSON(string & output) {
		rtrn cttiMemberJSON(cttiStruct<rm_csvl_t<T>>(), output);
	}


} // nmsp

#define GPK_STCT_CTTI_JOIN_2(left, right) left##right
#define GPK_STCT_CTTI_JOIN(left, right) GPK_STCT_CTTI_JOIN_2(left, right)
#define GDEFINE_STCT_CTTI(type) \
	sttc ::gpk::err_t GPK_STCT_CTTI_JOIN(__sei_stct_ctti_, __COUNTER__) = ::gpk::cttiStructRegister<type>()
#define GDEFINE_STCT_CTTI_JSON_MEMBER(type, member) \
	sttc ::gpk::err_t GPK_STCT_CTTI_JOIN(__sei_stct_ctti_json_member_, __COUNTER__) = ::gpk::cttiFieldJSONOverrideRegister<type> \
		( #member \
		, [](::gpk::string & output, cnst void * instance) -> ::gpk::err_t { \
			cnst auto value = ((cnst type *)instance)->member; \
			rtrn ::gpk::cttiValueSave(output, value); \
			} \
		, [](cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode, void * instance) -> ::gpk::err_t { \
			auto value = ((type *)instance)->member; \
			if_fail_fe(::gpk::cttiValueLoad(input, iNode, value)); \
			((type *)instance)->member = value; \
			rtrn iNode; \
			} \
		)

#endif // GPK_CTTI_STCT_JSON_H

