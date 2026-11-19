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
		usng TMembers = tpnm SCTTIMemberTypes<TStruct, TIndices>::Type;
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
	tplt<tpnm T>
	err_t cttiValueSaveDelta(string & output, cnst T & value, cnst T & defaults) {
		if constexpr(false == ctti_struct<T>) {
			string valueJSON = {}, defaultJSON = {};
			if_fail_fe(cttiValueSave(valueJSON, value));
			if_fail_fe(cttiValueSave(defaultJSON, defaults));
			rtrn valueJSON == defaultJSON ? 0 : output.append(valueJSON);
		}
		else {
			string members = {};
			b8_t separator = false;
			cnst auto & metadata = cttiStruct<T>();
			for(u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
				cnst vcst_t memberName = metadata.View(metadata.Members[iMember].Name);
				string memberJSON = {};
				cnst SCTTIFieldJSONOverride * fieldOverride = cttiFieldJSONOverride<T>(memberName);
				if(fieldOverride && fieldOverride->Save) {
					string defaultJSON = {};
					if_fail_fe(fieldOverride->Save(memberJSON, &value));
					if_fail_fe(fieldOverride->Save(defaultJSON, &defaults));
					if(memberJSON == defaultJSON)
						continue;
				}
				else {
					cnst err_t delta = cttiVisit(value, iMember, [&](cnst auto & member) {
						rtrn cttiVisit(defaults, iMember, [&](cnst auto & defaultMember) {
							usng TMember = rm_csvl_t<decltype(member)>;
							usng TDefaultMember = rm_csvl_t<decltype(defaultMember)>;
							if constexpr(ctti_same<TMember, TDefaultMember>::Value)
								rtrn cttiValueSaveDelta(memberJSON, member, defaultMember);
							else
								rtrn 0;
						});
					});
					if_fail_fe(delta);
					if(0 == delta)
						continue;
				}
				if_fail_fe(appendOpenKey(members, memberName, separator));
				if_fail_fe(members.append(memberJSON));
				separator = true;
			}
			if_zero_fe(separator);
			rtrn append_strings(output, '{', members, '}');
		}
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

