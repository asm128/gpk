// These sources are best viewed in Visual Studio 2017 or newer with a screen of at least 1920x1080 screen, Consolas font and zoom set to 81 %.
// Ogarnd - Copyright (c) 2016-2024
#include "gpk_string.h"
#include "gpk_array_obj.h"
#include "gpk_ctti_enum.h"
#include "gpk_enum_operators.h"

#ifndef GPK_ENUM_H_23627
#define GPK_ENUM_H_23627

#define enum_printf verbose_printf

nmsp gpk
{
	//stxp	vcst_c		UNDEFINED_ENUM_TYPE_STR					= "Undefined enumeration type.";
	stxp	vcst_c		INVALID_ENUM_VALUE_STR		= "INVALID";
	stxp	vcst_c		UNDEFINED_ENUM_VALUE_STR	= "Undefined enumeration value.";
	stxp	vcst_c		UNRESOLVED_ENUM_LABEL_STR	= "Unresolved enumeration value name.";
	stxp	vcst_c		UNRESOLVED_ENUM_NAME_STR	= "Enum definition name not set.";

	tplt<tpnm T>
	stct enum_definition;

	stct SEnumDefinition {
		vcst_t		Name			= {};
		cnst void *	Instance		= {};
		uP_t		ValueSize		= {};
		view<vcst_c>	(*Names)		(cnst void *)						= {};
		err_t		(*Value)		(cnst void *, vcst_t, void *)			= {};
		err_t		(*Label)		(cnst void *, cnst void *, vcst_t &)	= {};
	};

	inln aobj<SEnumDefinition> & enumDefinitions() nxpt {
		sttc aobj<SEnumDefinition> definitions = {};
		rtrn definitions;
	}

	inln cnst SEnumDefinition * enumDefinition(vcst_c name) nxpt {
		cnst aobj<SEnumDefinition> & definitions = enumDefinitions();
		for(u2_t iDefinition = 0; iDefinition < definitions.size(); ++iDefinition)
			if(definitions[iDefinition].Name == name)
				rtrn &definitions[iDefinition];
		rtrn {};
	}

	tplt<tpnm T>
	err_t enumDefinitionRegister(enum_definition<T> & definition);

	// This tplt is intended to store the name of an enumeration, the values of such enumeration and a string representing each value.
	// The implementation separates names from values for improving search speed by reducing the memory usage when performing searches for names/values.
	tpl_tstct enum_definition {
		tdfT(_t);
		stxp	T		INVALID_VALUE				= (T)(-1);

		vcst_t			Name						= UNRESOLVED_ENUM_NAME_STR;
		apod<T>			Values						= {};
		aobj<vcst_t>	Names						= {};
		aobj<vcst_t>	Titles						= {};
		aobj<vcst_t>	Descriptions				= {};

		stin	enum_definition<T>&	get				()											{
			sttc	enum_definition<T>		valueRegistry;
			return valueRegistry;
		}
		stin	T					init			(vcst_t enumName)					{
			enum_definition<T>&				instanceHere			= get();

			if( instanceHere.Name != enumName || (instanceHere.Values.size() && (instanceHere.Values[0] != INVALID_VALUE)) )
				verbose_printf("Initializing enumeration type: '%.*s'.", (int)enumName.size(), enumName.begin());

			if(instanceHere.Name.size() != enumName.size())
				instanceHere.Name			= enumName;
			if_fail_ve(INVALID_VALUE, enumDefinitionRegister(instanceHere));

			return INVALID_VALUE;
		}
		err_t				get_value				(vcst_t name, T & value)		cnst	{
			cnst err_t iValue = Names.find(name);
			if(0 <= iValue) {
				value				= Values[iValue];
				return 0;
			}
			enum_printf("Enumeration value not found! Name: %.*s.", (int)name.size(), name.begin());
			value				= INVALID_VALUE;
			return -1;
		}
		//err_t				get_value				(cnst sc_t* name, T & value)				cnst			{
		//	for(::gpk::u2_t i=0, count = Names.size(); i<count; ++i)
		//		if(0 == ::strcmp(name, Names[i].begin())) {
		//			value				= Values[i];
		//			return 0;
		//		}
		//	enum_printf("Enumeration value not found! Name: %.*s.", name.size(), GPK_VIEW_ARGS(name));
		//	value				= INVALID_VALUE;
		//	return -1;
		//}
		T					get_value				(vcst_t name)					cnst			{
			cnst err_t iValue = Names.find(name);
			if(0 <= iValue)
				return Values[iValue];
			enum_printf("Enumeration value not found! Name: %.*s.", (int)name.size(), name.begin());
			return INVALID_VALUE;
		}
		err_t				get_value_by_index		(::gpk::u2_t index, T & value)					cnst			{
			if(index < Values.size()) {
				value				= Values[index];
				return 0;
			}
			enum_printf("Enumeration index out of range! Index: 0x%" GPK_FMT_U2 ".", index);
			value				= INVALID_VALUE;
			return -1;
		}
		T					get_value_by_index		(::gpk::u2_t index)					cnst			{
			if_true_vwf(INVALID_VALUE, index >= Values.size(), "Enumeration index out of range! Index: 0x%" GPK_FMT_U2 ".", index);
			return Values[index];
		}
		err_t				get_label_by_index		(::gpk::u2_t index, vcst_t & value)		cnst			{
			if(index < Names.size()) {
				value				= Names[index];
				return 0;
			}
			value				= UNDEFINED_ENUM_VALUE_STR;
			enum_printf("Enumeration index out of range! Index: 0x%" GPK_FMT_U2 ".", index);
			return -1;
		}
		vcst_t			get_label_by_index		(::gpk::u2_t index)					cnst			{
			if(index < Names.size())
				return Names[index];
			enum_printf("Enumeration index out of range! Index: 0x%" GPK_FMT_U2 ".", index);
			return UNDEFINED_ENUM_VALUE_STR;
		}
		err_t				get_value_index			(vcst_t name, s2_t & index)	cnst			{
			index = (s2_t)Names.find(name);
			if(0 <= index)
				return index;
			enum_printf("Enumeration value not found! Name: %.*s.", (int)name.size(), name.begin());
			return index				= -1;
		}
		s2_t				get_value_index			(vcst_t name)					cnst			{
			cnst s2_t index = (s2_t)Names.find(name);
			if(0 <= index)
				return index;
			enum_printf("Enumeration value not found! Name: %.*s.", (int)name.size(), name.begin());
			return -1;
		}
		err_t				get_value_index			(cnst T & value, s2_t & index)		cnst			{
			index = (s2_t)Values.find(value);
			if(0 <= index)
				return 0;
			enum_printf("Enumeration value not found! Value: 0x%llX.", (u3_t)value);
			return index					= -1;
		}
		s2_t				get_value_index			(cnst T & value)				cnst			{
			cnst s2_t index = (s2_t)Values.find(value);
			if(0 <= index)
				return index;
			enum_printf("Enumeration value not found! Value: %llX.", (u3_t)value);
			return -1;
		}
		err_t				get_value_label			(cnst T & value, vcst_t & name)	cnst			{
			cnst err_t iValue = Values.find(value);
			if(0 <= iValue) {
				name				= Names[iValue];
				return 0;
			}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (u3_t)value);
			name				= UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
		err_t				get_value_desc			(cnst T & value, vcst_t & name)	cnst			{
			cnst err_t iValue = Values.find(value);
			if(0 <= iValue) {
				name				= Descriptions[iValue];
				return 0;
			}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (u3_t)value);
			name				= UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
		vcst_t	get_value_label			(cnst T & value)				cnst			{
			cnst err_t iValue = Values.find(value);
			if(0 <= iValue)
				return Names[iValue];
			enum_printf("Enumeration value not found! Value: 0x%llX.", (u3_t)value);
			return UNRESOLVED_ENUM_LABEL_STR;
		}
		vcst_t	get_value_desc			(cnst T & value)				cnst			{
			cnst err_t iValue = Values.find(value);
			if(0 <= iValue)
				return Descriptions[iValue];
			enum_printf("Enumeration value not found! Value: 0x%llX.", (u3_t)value);
			return UNRESOLVED_ENUM_LABEL_STR;
		}
		err_t				add_value				(cnst T & value, vcst_t name, vcst_t title, vcst_t description)	{
			cnst err_t iValue = Names.find(name);
			if(0 <= iValue) {
				if_true_fef(Values[iValue] != value, "Enumeration name already has another value! Type:'%.*s'. Name:'%.*s'. Previous:0x%llX. New:0x%llX."
					, (int)Name.size(), Name.begin(), (int)name.size(), name.begin(), (u3_t)Values[iValue], (u3_t)value
					);
				return -1;
			}
			::gpk::u2_t				newIndex				= Values.size();
			if_fail_fe(::gpk::resize_obj(newIndex + 1, Values, Names, Titles, Descriptions));
			Values		[newIndex]	= value;
			Names		[newIndex]	= name;
			Titles		[newIndex]	= title;
			Descriptions[newIndex]	= description;
			verbose_printf("Added new value to enumeration definition. Enum name: %.*s. Index: %.02u, Value: 0x%llX. Value name: %.*s."
				, (int)Name.size(), Name.begin()
				, (::gpk::u2_t)newIndex
				, (u3_t)value
				, (int)name.size(), name.begin()
				);
			return newIndex;
		}
		err_t				add_value_auto			(vcst_t name, vcst_t title, vcst_t description)	{
			cnst err_t iValue = Names.find(name);
			if(0 <= iValue) {
				if_true_fef(true, "Enumeration value already defined! Type: '%.*s'. Value: 0x%llX. Previous name: %.*s. New name: %.*s. Second definition ignored..."
					, (int)Name		.size(), Name		.begin()
					, (u3_t)Values[iValue]
					, (int)Names[iValue]	.size(), Names[iValue]	.begin()
					, (int)name		.size(), name		.begin()
					);
			}
			::gpk::u2_t				valueCount				= {};
			for(::gpk::u2_t i=0; i < Values.size(); ++i)
				valueCount += get_value_index(Values[i]) == (s2_t)i;
			cnst T					value					= (T)valueCount;
			::gpk::u2_t				newIndex				= Values.size();
			if_fail_fe(::gpk::resize_obj(newIndex + 1, Values, Names, Titles, Descriptions));
			Values		[newIndex]	= value;
			Names		[newIndex]	= name;
			Titles		[newIndex]	= title;
			Descriptions[newIndex]	= description;
			verbose_printf("Added new value to enumeration definition. Enum name: %.*s. Index: %.02u, Value: 0x%llX. Value name: %.*s.", (int)Name.size(), Name.begin(), (::gpk::u2_t)newIndex, (u3_t)value, (int)name.size(), name.begin());
			return newIndex;
		}
	};

	tplt<tpnm T>
	err_t enumDefinitionRegister(enum_definition<T> & definition) {
		aobj<SEnumDefinition> & definitions = enumDefinitions();
		for(u2_t iDefinition = 0; iDefinition < definitions.size(); ++iDefinition)
			if(definitions[iDefinition].Name == definition.Name) {
				if_true_fe(definitions[iDefinition].ValueSize != sizeof(T));
				rtrn 0;
			}
		rtrn definitions.push_back
			({ definition.Name
			, &definition
			, sizeof(T)
			, [](cnst void * instance) -> view<vcst_c> {
				rtrn ((cnst enum_definition<T> *)instance)->Names;
				}
			, [](cnst void * instance, vcst_t name, void * value) -> err_t {
				rtrn ((cnst enum_definition<T> *)instance)->get_value(name, *(T *)value);
				}
			, [](cnst void * instance, cnst void * value, vcst_t & name) -> err_t {
				rtrn ((cnst enum_definition<T> *)instance)->get_value_label(*(cnst T *)value, name);
				}
			});
	}

	tplt<tpnm T> requires ctti_enum<T>
	err_t enumDefinitionCTTIRegister() {
		enum_definition<T> & definition = enum_definition<T>::get();
		definition.Name = cttiEnumTypeName<T>();
		if_fail_fe(enumDefinitionRegister(definition));
		cnst auto & metadata = cttiEnumTyped<T>();
		for(u2_t iValue = 0; iValue < metadata.Count; ++iValue) {
			if_zero_fe(metadata.Values[iValue].ValueResolved);
			cnst vcst_t name = cttiEnumValueName<T>(iValue);
			b8_t found = false;
			for(u2_t iRegistered = 0; iRegistered < definition.Names.size(); ++iRegistered)
				if(definition.Names[iRegistered] == name) {
					if_true_fe(definition.Values[iRegistered] != metadata.Values[iValue].Value);
					found = true;
					break;
				}
			if(false == found)
				if_fail_fe(definition.add_value(metadata.Values[iValue].Value, name, name, name));
		}
		rtrn 0;
	}

	tplt <tpnm TEnum>	inln	enum_definition<TEnum>&	get_enum	()					nxpt	{ return enum_definition<TEnum>::get();	}
	tplt <tpnm TEnum>	inln	enum_definition<TEnum>&	get_enum	(cnst TEnum & )	nxpt	{ return enum_definition<TEnum>::get();	}

	// This type is used to initialize an enumeration value.
	tpl_t struct genum_value {
		tdfT(_t);

		T					Value					= enum_definition<T>::INVALID_VALUE;
		vcst_t				Name					= INVALID_ENUM_VALUE_STR;
		vcst_t				Title					= INVALID_ENUM_VALUE_STR;
		vcst_t				Description				= INVALID_ENUM_VALUE_STR;
		//
		inxp				genum_value				()										= default;
		inxp				genum_value				(cnst genum_value & other)				= default;
							genum_value				(cnst T & value)													: Value((T)value), Name(get_enum<T>().get_value_namev(value))		{}
#ifndef GPK_ESP8266
							genum_value				(cnst T & value, vcst_t name)										: Value((T)value), Name(name), Title(name), Description(name)			{ get_enum<T>().add_value(value, name, name, name);			}
							genum_value				(cnst T & value, vcst_t name, vcst_t description)					: Value((T)value), Name(name), Title(name), Description(description)	{ get_enum<T>().add_value(value, name, name, description);	}
							genum_value				(cnst T & value, vcst_t name, vcst_t title, vcst_t description)	: Value((T)value), Name(name), Title(title), Description(description)	{ get_enum<T>().add_value(value, name, title, description);	}
#else
							genum_value				(cnst T & value, vcst_t name)										: Value((T)value), Name(name), Title(name), Description(name)			{ }
							genum_value				(cnst T & value, vcst_t name, vcst_t description)					: Value((T)value), Name(name), Title(name), Description(description)	{ }
							genum_value				(cnst T & value, vcst_t name, vcst_t title, vcst_t description)	: Value((T)value), Name(name), Title(title), Description(description)	{ }
#endif
		inxp	oper	cnst	T&				()			cnst	{ return Value; }
	};

	tplt <tpnm TEnum, size_t nameLen>
	TEnum										get_value			(cnst sc_t (&valueLabel)[nameLen])	{ return get_enum<TEnum>().get_value(vcst_t{valueLabel}); }
	tplt <tpnm TEnum>			TEnum			get_value			(vcst_t valueLabel)					{ return get_enum<TEnum>().get_value(valueLabel); }

	tplt <tpnm TEnum>			TEnum			get_value_camelcased(vcst_t uncased)					{
		string			camelCased;
		camelCase(uncased, camelCased);
		return get_enum<TEnum>().get_value(camelCased);
	}
	tplt<tpnm TEnum>	ndsi	u2_t			get_value_count		()							nxpt	{ return get_enum<TEnum>().Values.size(); }
	tplt<tpnm TEnum>	ndsi	s2_t			get_value_index		(cnst TEnum & statusBit)			{ return get_enum<TEnum>().get_value_index(statusBit); }
	tplt<tpnm TEnum>	ndsi	vcst_t			get_value_name		(cnst TEnum & statusBit)			{ return get_enum<TEnum>().get_value_label(statusBit); }
	tplt<tpnm TEnum>	ndsi	vcst_t			get_value_desc		(cnst TEnum & statusBit)			{ return get_enum<TEnum>().get_value_desc (statusBit); }
	tplt<tpnm TEnum>	ndsi	vcst_t			get_enum_name		()							nxpt	{ return get_enum<TEnum>().Name;			}
	tplt<tpnm TEnum>	ndsi	vcst_t			get_enum_name		(cnst TEnum & )				nxpt	{ return get_enum<TEnum>().Name;			}
	tplt<tpnm TEnum>	ndsi	view<vcst_c>	get_value_names		()							nxpt	{ return get_enum<TEnum>().Names; }
	tplt<tpnm TEnum>	ndsi	view<vcst_c>	get_value_descs		()							nxpt	{ return get_enum<TEnum>().Names; }
	// ---- Legacy
	tplt<tpnm TEnum>	ndsi	vcst_t			get_value_label		(cnst TEnum & statusBit)			{ return get_enum<TEnum>().get_value_label(statusBit); }
	tplt<tpnm TEnum>	ndsi	sc_c*			get_value_namep		(cnst TEnum & statusBit)			{ return get_enum<TEnum>().get_value_label(statusBit).begin(); }
	tplt<tpnm TEnum>	ndsi	vcst_t			get_value_namev		(cnst TEnum & statusBit)			{ return get_enum<TEnum>().get_value_label(statusBit); }
	tplt<tpnm TEnum>	ndsi	vcst_t			get_value_descv		(cnst TEnum & statusBit)			{ return get_enum<TEnum>().get_value_desc (statusBit); }
	tplt<tpnm TEnum>	ndsi	sc_c*			get_value_descp		(cnst TEnum & statusBit)			{ return get_enum<TEnum>().get_value_desc (statusBit).begin(); }
	tplt<tpnm TEnum>	ndsi	view<vcst_c>	get_value_labels	()							nxpt	{ return get_enum<TEnum>().Names; }
	tplt<tpnm TEnum>	ndsi	vcst_t			get_enum_namev		()							nxpt	{ return get_enum<TEnum>().Name;			}
	tplt<tpnm TEnum>	ndsi	vcst_t			get_enum_namev		(cnst TEnum & )				nxpt	{ return get_enum<TEnum>().Name;			}
	tplt<tpnm TEnum>	ndsi	sc_c*			get_enum_namep		()							nxpt	{ return get_enum<TEnum>().Name.begin();	}
	tplt<tpnm TEnum>	ndsi	sc_c*			get_enum_namep		(cnst TEnum & )				nxpt	{ return get_enum<TEnum>().Name.begin();	}

	tplTstct genum_value_auto {
		T		Value				= enum_definition<T>::INVALID_VALUE;
		vcst_t	Name				= INVALID_ENUM_VALUE_STR;
		vcst_t	Title				= INVALID_ENUM_VALUE_STR;
		vcst_t	Description			= INVALID_ENUM_VALUE_STR;
		//
		inxp	genum_value_auto	()									= default;
		inxp	genum_value_auto	(cnst genum_value_auto & other)	= default;
				genum_value_auto	(vcst_t name)													: Value((T)0), Name(name), Title(name), Description(name)			{ get_enum<T>().add_value_auto(name, name, name);			Value = get_value<T>(name); }
				genum_value_auto	(vcst_t name, vcst_t description)							: Value((T)0), Name(name), Title(name), Description(description)	{ get_enum<T>().add_value_auto(name, name, description);		Value = get_value<T>(name); }
				genum_value_auto	(vcst_t name, vcst_t title, vcst_t description)	: Value((T)0), Name(name), Title(title), Description(description)	{ get_enum<T>().add_value_auto(name, title, description);	Value = get_value<T>(name); }

		inxp	oper	cnst T&		()			cnst	{ return Value; }
	};
} // nmsp

// Defines the enumeration type, the invalid value (-1) and the flag operators
#define GDEFINE_ENUM_TYPE(EnumName, IntType)																							\
	enum EnumName : IntType {};																											\
	sttc	::gpk::u2_c		__sei_##EnumName##enumInit	= ::gpk::enum_definition<EnumName>::init(#EnumName);					\
	GDEFINE_ENUM_OPERATORS(EnumName)
//
#define GDEFINE_ENUM_STRUCT(EnumName, IntType)																								\
	struct EnumName {																														\
		IntType				Value			;																								\
		inln	oper		IntType&		()			nxpt	{ return Value; }															\
		inxp	oper		cnst IntType&	()	csnx	{ return Value; }															\
	};																																		\
	sttc	::gpk::u2_c		__sei_##EnumName##enumInit	= ::gpk::enum_definition<EnumName>::init(#EnumName);								\
	sinx	EnumName		oper &	(EnumName  a, EnumName b)	nxpt	{ return {a & (IntType)b};				}	\
	sinx	EnumName		oper ~	(EnumName  a)				nxpt	{ return {~(IntType)a};					}	\
	sinx	EnumName		oper ^	(EnumName  a, EnumName b)	nxpt	{ return {a ^ (IntType)b};				}	\
	stin	EnumName&		oper |=	(EnumName &a, EnumName b)	nxpt	{ return (EnumName&)	{ ((IntType&)a) |= (IntType)b}; }	\
	stin	EnumName&		oper &=	(EnumName &a, EnumName b)	nxpt	{ return (EnumName&)	{ ((IntType&)a) &= (IntType)b}; }	\
	stin	EnumName&		oper ^=	(EnumName &a, EnumName b)	nxpt	{ return (EnumName&)	{ ((IntType&)a) ^= (IntType)b}; }	\
	sinx	EnumName		oper |	(EnumName  a, EnumName b)	nxpt	{ return (EnumName)		{a | (IntType)b};				}
//
#ifdef GPK_ATMEL
#	define GDEFINE_ENUM_VALUE(EnumName, ValueName, EnumValue)					\
	stxp	cnst EnumName	EnumName##_##ValueName	= (EnumName)(EnumValue);	\
	//sttc	cnst EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {szof(#ValueName) - 1, F(#ValueName)})

#	define GDEFINE_ENUM_VALUED(EnumName, ValueName, EnumValue, EnumDescription)		\
	stxp	cnst EnumName	EnumName##_##ValueName	= (EnumName)(EnumValue);		\
	//sttc	cnst EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {szof(#ValueName) - 1, F(#ValueName)})

#	define GDEFINE_ENUM_VALTD(EnumName, ValueName, EnumValue, EnumTitle, EnumDescription)	\
	stxp	cnst EnumName	EnumName##_##ValueName	= (EnumName)(EnumValue);				\
	//sttc	cnst EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {szof(#ValueName) - 1, F(#ValueName)})
	
#else
#	define GDEFINE_ENUM_VALUE(EnumName, ValueName, EnumValue)							\
	stxp	cnst EnumName	EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	sttc	cnst EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), #ValueName)

#	define GDEFINE_ENUM_AVALUE(EnumName, ValueName) sttc	cnst EnumName	EnumName##_##ValueName	= (EnumName)::gpk::genum_value_auto<EnumName>(#ValueName)

#	define GDEFINE_ENUM_VALUED(EnumName, ValueName, EnumValue, EnumDescription)				\
	stxp	cnst EnumName	EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	sttc	cnst EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), #ValueName, EnumDescription)

#	define GDEFINE_ENUM_VALTD(EnumName, ValueName, EnumValue, EnumTitle, EnumDescription)				\
	stxp	cnst EnumName	EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	sttc	cnst EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), #ValueName, EnumTitle, EnumDescription)
#endif

#define GDEFINE_FLAG_TYPE		GDEFINE_ENUM_TYPE
#define GDEFINE_FLAG_VALUE		GDEFINE_ENUM_VALUE

#define GPK_ENUM_CTTI_JOIN_2(left, right) left##right
#define GPK_ENUM_CTTI_JOIN(left, right) GPK_ENUM_CTTI_JOIN_2(left, right)
#define GDEFINE_ENUM_CTTI(EnumName) \
	sttc ::gpk::err_t GPK_ENUM_CTTI_JOIN(__sei_enum_ctti_, __COUNTER__) = ::gpk::enumDefinitionCTTIRegister<EnumName>()

#ifdef GPK_WINDOWS
#	pragma warning(disable : 4063)	// On Windows, usng enum types like we do cause the compiler to throw a warning when the warning level is set to 4
#endif

#define gpk_warning_unhandled_value(_enumValue)			warning_printf("Unhandled'%s' value:(0x%X)(%" GPK_FMT_S2 ")(%c) %s"	, ::gpk::get_enum_namep GPKREP3(_enumValue) ? sc_t(_enumValue) : '?', ::gpk::get_value_namep(_enumValue))
#define gpk_enum_valued_log(_funcPrintf, _enumValue)	_funcPrintf("'%s':(0x%X)(%" GPK_FMT_S2 ")(%c)'%s'-'%s'"				, ::gpk::get_enum_namep GPKREP3(_enumValue) ? sc_t(_enumValue) : '?', ::gpk::get_value_namep(_enumValue), ::gpk::get_value_descp(_enumValue))
#define gpk_enum_value_info(_enumValue)					gpk_enum_value_log	(info_printf	, _enumValue)
#define gpk_enum_value_warning(_enumValue)				gpk_enum_value_log	(warning_printf	, _enumValue)
#define gpk_enum_value_error(_enumValue)				gpk_enum_value_log	(error_printf	, _enumValue)
#define gpk_enum_valued_info(_enumValue)				gpk_enum_valued_log	(info_printf	, _enumValue)
#define gpk_enum_valued_warning(_enumValue)				gpk_enum_valued_log	(warning_printf	, _enumValue)
#define gpk_enum_valued_error(_enumValue)				gpk_enum_valued_log	(error_printf	, _enumValue)

#define gpk_enum_value_log(printf_fn, enumVal)	printf_fn("'%s':(0x%X)(%" GPK_FMT_S2 ")(%c)'%s'", get_enum_namep GPKREP3(enumVal) ? sc_t(enumVal) : '?', get_value_namep(enumVal))
#define GDEFINE_ENUM_CASE_DEFAULT(TEnum, TValue)    default                 : rtrn #TValue
#define GDEFINE_ENUM_CASE(TEnum, TValue)            case TEnum##_##TValue   : rtrn #TValue

#endif // GPK_ENUM_H_23627
