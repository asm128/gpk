// These sources are best viewed in Visual Studio 2017 or newer with a screen of at least 1920x1080 screen, Consolas font and zoom set to 81 %.
// Ogarnd - Copyright (c) 2016-2024
#include "gpk_array_pod.h"
#include "gpk_array_obj.h"

#ifndef GPK_ENUM_H_23627
#define GPK_ENUM_H_23627

#define enum_printf verbose_printf

namespace gpk
{
	//stxp	vcst_c		UNDEFINED_ENUM_TYPE_STR					= GPK_CXS("Undefined enumeration type.");
	stxp	vcst_c		INVALID_ENUM_VALUE_STR		= GPK_CXS("INVALID");
	stxp	vcst_c		UNDEFINED_ENUM_VALUE_STR	= GPK_CXS("Undefined enumeration value.");
	stxp	vcst_c		UNRESOLVED_ENUM_LABEL_STR	= GPK_CXS("Unresolved enumeration value name.");
	stxp	vcst_c		UNRESOLVED_ENUM_NAME_STR	= GPK_CXS("Enum definition name not set.");

	// This tplt is intended to store the name of an enumeration, the values of such enumeration and a string representing each value.
	// The implementation separates names from values for improving search speed by reducing the memory usage when performing searches for names/values.
	tpl_tstct enum_definition {
		tdfT(_t);
		stxp	T					INVALID_VALUE			= (T)(-1);

		::gpk::vcsc_t					Name					= UNRESOLVED_ENUM_NAME_STR;
		::gpk::apod<T>					Values					= {};
		::gpk::avcsc_t					Names					= {};
		::gpk::avcsc_t					Titles					= {};
		::gpk::avcsc_t					Descriptions			= {};

		stin	enum_definition<T>&	get				()											{
			static	enum_definition<T>		valueRegistry;
			return valueRegistry;
		}
		stin	T					init					(::gpk::vcsc_c & enumName)						{
			enum_definition<T>&				instanceHere			= get();

			if( instanceHere.Name != enumName || (instanceHere.Values.size() && (instanceHere.Values[0] != INVALID_VALUE)) )
				verbose_printf("Initializing enumeration type: '%s'.", enumName.begin());

			if(instanceHere.Name.size() != enumName.size())
				instanceHere.Name			= enumName;

			return INVALID_VALUE;
		}
		err_t				get_value				(::gpk::vcsc_c & name, T & value)		const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(name == Names[i]) {
					value				= Values[i];
					return 0;
				}
			enum_printf("Enumeration value not found! Name: %s.", ::gpk::toString(name).begin());
			value				= INVALID_VALUE;
			return -1;
		}
		err_t				get_value				(const char* name, T & value)				const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(0 == ::strcmp(name, Names[i].begin())) {
					value				= Values[i];
					return 0;
				}
			enum_printf("Enumeration value not found! Name: %s.", name);
			value				= INVALID_VALUE;
			return -1;
		}
		T					get_value				(::gpk::vcsc_c & name)					const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i) {
				if(name == Names[i])
					return Values[i];
			}
			enum_printf("Enumeration value not found! Name: %s.", ::gpk::toString(name).begin());
			return INVALID_VALUE;
		}
		err_t				get_value_by_index		(uint32_t index, T & value)					const			{
			if(index < Values.size()) {
				value				= Values[index];
				return 0;
			}
			enum_printf("Enumeration index out of range! Index: 0x%" GPK_FMT_U2 ".", index);
			value				= INVALID_VALUE;
			return -1;
		}
		T					get_value_by_index		(uint32_t index)					const			{
			retval_gwarn_if(INVALID_VALUE, index >= Values.size(), "Enumeration index out of range! Index: 0x%" GPK_FMT_U2 ".", index);
			return Values[index];
		}
		err_t				get_label_by_index		(uint32_t index, ::gpk::vcsc_t & value)		const			{
			if(index < Names.size()) {
				value				= Names[index];
				return 0;
			}
			value				= ::gpk::UNDEFINED_ENUM_VALUE_STR;
			enum_printf("Enumeration index out of range! Index: 0x%" GPK_FMT_U2 ".", index);
			return -1;
		}
		::gpk::vcsc_t			get_label_by_index		(uint32_t index)					const			{
			if(index < Names.size())
				return Names[index];
			enum_printf("Enumeration index out of range! Index: 0x%" GPK_FMT_U2 ".", index);
			return ::gpk::UNDEFINED_ENUM_VALUE_STR;
		}
		err_t				get_value_index			(::gpk::vcsc_c & name, int32_t & index)	const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(name == Names[i]) {
					index				= (int32_t)i;
					return index;
				}
			enum_printf("Enumeration value not found! Name: %s.", name.begin());
			return index				= -1;
		}
		int32_t				get_value_index			(::gpk::vcsc_c & name)					const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i) {
				if(name == Names[i])
					return (int32_t)i;
			}
			enum_printf("Enumeration value not found! Name: %s.", name.begin());
			return -1;
		}
		err_t				get_value_index			(const T & value, int32_t & index)		const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(value == Values[i]) {
					index					= (int32_t)i;
					return 0;
				}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return index					= -1;
		}
		int32_t				get_value_index			(const T & value)				const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i) {
				if(value == Values[i])
					return (int32_t)i;
			}
			enum_printf("Enumeration value not found! Value: %llX.", (uint64_t)value);
			return -1;
		}
		err_t				get_value_label			(const T & value, ::gpk::vcsc_t & name)	const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i]) {
					name				= Names[i];
					return 0;
				}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			name				= ::gpk::UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
		err_t				get_value_desc			(const T & value, ::gpk::vcsc_t & name)	const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i]) {
					name				= Descriptions[i];
					return 0;
				}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			name				= ::gpk::UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
		::gpk::vcsc_c&	get_value_label			(const T & value)				const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i) {
				if(value == Values[i])
					return Names[i];
			}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return ::gpk::UNRESOLVED_ENUM_LABEL_STR;
		}
		::gpk::vcsc_c&	get_value_desc			(const T & value)				const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i) {
				if(value == Values[i])
					return Descriptions[i];
			}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return ::gpk::UNRESOLVED_ENUM_LABEL_STR;
		}
		err_t				add_value				(const T & value, ::gpk::vcsc_c & name, ::gpk::vcsc_c & title, ::gpk::vcsc_c & description)	{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(Values[i] == value) {
					rww_if(name != Names[i], "Enumeration value already defined! Type: '%s'. Value: 0x%llX. Previous name: %s. New name: %s. Second definition ignored..."
				, Name		.begin()
				, (uint64_t)value
				, Names[i]	.begin()
				, name		.begin()
				);
					return 0;	// Found same name and value combination. This is normal when values are defined as static const.
				}
			uint32_t				newIndex				= Values.push_back(value);
			gpk_necs(newIndex);
			gpk_necs(Names.push_back(name));
			gpk_necs(Titles.push_back(title));
			gpk_necs(Descriptions.push_back(description));
			verbose_printf("Added new value to enumeration definition. Enum name: %s. Index: %.02u, Value: 0x%llX. Value name: %s."
				, Name.begin()
				, (uint32_t)newIndex
				, (uint64_t)value
				, name.begin()
				);
			return newIndex;
		}
		err_t				add_value_auto			(::gpk::vcsc_c & name, ::gpk::vcsc_c & title, ::gpk::vcsc_c & description)	{
			for(uint32_t i=0, count = Names.size(); i < count; ++i) {
				ree_if(name == Names[i], "Enumeration value already defined! Type: '%s'. Value: 0x%llX. Previous name: %s. New name: %s. Second definition ignored..."
					, Name		.begin()
					, (uint64_t)Values[i]
					, Names[i]	.begin()
					, name		.begin()
					);
			}
			const T					value					= (T)Values.size();
			uint32_t				newIndex				= Values.push_back(value);
			gpk_necs(newIndex);
			gpk_necs(Names.push_back(name));
			gpk_necs(Titles.push_back(title));
			gpk_necs(Descriptions.push_back(description));
			verbose_printf("Added new value to enumeration definition. Enum name: %s. Index: %.02u, Value: 0x%llX. Value name: %s.", Name.begin(), (uint32_t)newIndex, (uint64_t)value, name.begin());
			return newIndex;
		}
	};

	tplt <tpnm TEnum>	inline	::gpk::enum_definition<TEnum>&	get_enum	()					noexcept	{ return ::gpk::enum_definition<TEnum>::get();	}
	tplt <tpnm TEnum>	inline	::gpk::enum_definition<TEnum>&	get_enum	(const TEnum & )	noexcept	{ return ::gpk::enum_definition<TEnum>::get();	}

	// This type is used to initialize an enumeration value.
	tpl_t struct genum_value {
		tdfT(_t);

		T					Value					= ::gpk::enum_definition<T>::INVALID_VALUE;
		::gpk::vcsc_t			Name					= INVALID_ENUM_VALUE_STR;
		::gpk::vcsc_t			Title					= INVALID_ENUM_VALUE_STR;
		::gpk::vcsc_t			Description				= INVALID_ENUM_VALUE_STR;
		//
		inxp				genum_value				()										= default;
		inxp				genum_value				(const genum_value & other)				= default;
							genum_value				(const T & value)																			: Value((T)value), Name(::gpk::get_enum<T>().get_value_name(value))		{}
#ifndef GPK_ESP8266
							genum_value				(const T & value, ::gpk::vcsc_c & name)														: Value((T)value), Name(name), Title(name), Description(name)			{ ::gpk::get_enum<T>().add_value(value, name, name, name);			}
							genum_value				(const T & value, ::gpk::vcsc_c & name, ::gpk::vcsc_c & description)						: Value((T)value), Name(name), Title(name), Description(description)	{ ::gpk::get_enum<T>().add_value(value, name, name, description);	}
							genum_value				(const T & value, ::gpk::vcsc_c & name, ::gpk::vcsc_c & title, ::gpk::vcsc_c & description)	: Value((T)value), Name(name), Title(title), Description(description)	{ ::gpk::get_enum<T>().add_value(value, name, title, description);	}
#else
							genum_value				(const T & value, ::gpk::vcsc_c & name)														: Value((T)value), Name(name), Title(name), Description(name)			{ }
							genum_value				(const T & value, ::gpk::vcsc_c & name, ::gpk::vcsc_c & description)						: Value((T)value), Name(name), Title(name), Description(description)	{ }
							genum_value				(const T & value, ::gpk::vcsc_c & name, ::gpk::vcsc_c & title, ::gpk::vcsc_c & description)	: Value((T)value), Name(name), Title(title), Description(description)	{ }
#endif
		inxp	oper	const	T&				()			const	{ return Value; }
	};

	tplt <tpnm TEnum, size_t nameLen>
	TEnum						get_value			(const char (&valueLabel)[nameLen])		{ return ::gpk::get_enum<TEnum>().get_value(::gpk::vcst_t{valueLabel}); }
	tplt <tpnm TEnum>	TEnum	get_value			(::gpk::vcsc_c & valueLabel)			{ return ::gpk::get_enum<TEnum>().get_value(valueLabel); }

	tplt <tpnm TEnum>	TEnum	get_value_camelcased(::gpk::vcsc_c & uncased)			{
		::gpk::asc_t					camelCased;
		::gpk::camelCase(uncased, camelCased);
		return ::gpk::get_enum<TEnum>().get_value(camelCased);
	}
	tplt<tpnm TEnum>	ndsi	uint32_t		get_value_count		()							nxpt	{ return ::gpk::get_enum<TEnum>().Values.size(); }
	tplt<tpnm TEnum>	ndsi	::gpk::vcsc_c&	get_value_label		(const TEnum & statusBit)			{ return ::gpk::get_enum<TEnum>().get_value_label(statusBit); }
	tplt<tpnm TEnum>	ndsi	::gpk::vcsc_c&	get_value_namev		(const TEnum & statusBit)			{ return ::gpk::get_enum<TEnum>().get_value_label(statusBit); }
	tplt<tpnm TEnum>	ndsi	sc_c*			get_value_namep		(const TEnum & statusBit)			{ return ::gpk::get_enum<TEnum>().get_value_label(statusBit).begin(); }
	tplt<tpnm TEnum>	ndsi	::gpk::vcsc_c&	get_value_descv		(const TEnum & statusBit)			{ return ::gpk::get_enum<TEnum>().get_value_desc (statusBit); }
	tplt<tpnm TEnum>	ndsi	sc_c*			get_value_descp		(const TEnum & statusBit)			{ return ::gpk::get_enum<TEnum>().get_value_desc (statusBit).begin(); }
	tplt<tpnm TEnum>	ndsi	::gpk::avcsc_c&	get_value_labels	()							nxpt	{ return ::gpk::get_enum<TEnum>().Names; }
	tplt<tpnm TEnum>	ndsi	::gpk::avcsc_c&	get_value_names		()							nxpt	{ return ::gpk::get_enum<TEnum>().Names; }
	tplt<tpnm TEnum>	ndsi	::gpk::avcsc_c&	get_value_descs		()							nxpt	{ return ::gpk::get_enum<TEnum>().Names; }
	tplt<tpnm TEnum>	ndsi	int32_t			get_value_index		(const TEnum & statusBit)			{ return ::gpk::get_enum<TEnum>().get_value_index(statusBit); }
	tplt<tpnm TEnum>	ndsi	::gpk::vcsc_c&	get_enum_namev		()							nxpt	{ return ::gpk::get_enum<TEnum>().Name;			}
	tplt<tpnm TEnum>	ndsi	::gpk::vcsc_c&	get_enum_namev		(const TEnum & )			nxpt	{ return ::gpk::get_enum<TEnum>().Name;			}
	tplt<tpnm TEnum>	ndsi	sc_c*			get_enum_namep		()							nxpt	{ return ::gpk::get_enum<TEnum>().Name.begin();	}
	tplt<tpnm TEnum>	ndsi	sc_c*			get_enum_namep		(const TEnum & )			nxpt	{ return ::gpk::get_enum<TEnum>().Name.begin();	}

	tplTstct genum_value_auto {
		T						Value				= ::gpk::enum_definition<T>::INVALID_VALUE;
		::gpk::vcsc_t			Name				= INVALID_ENUM_VALUE_STR;
		::gpk::vcsc_t			Title				= INVALID_ENUM_VALUE_STR;
		::gpk::vcsc_t			Description			= INVALID_ENUM_VALUE_STR;
		//
		inxp				genum_value_auto	()									= default;
		inxp				genum_value_auto	(const genum_value_auto & other)	= default;
							genum_value_auto	(::gpk::vcsc_c & name)														: Value((T)0), Name(name), Title(name), Description(name)			{ ::gpk::get_enum<T>().add_value_auto(name, name, name);			Value = ::gpk::get_value<T>(name); }
							genum_value_auto	(::gpk::vcsc_c & name, ::gpk::vcsc_c & description)							: Value((T)0), Name(name), Title(name), Description(description)	{ ::gpk::get_enum<T>().add_value_auto(name, name, description);		Value = ::gpk::get_value<T>(name); }
							genum_value_auto	(::gpk::vcsc_c & name, ::gpk::vcsc_c & title, ::gpk::vcsc_c & description)	: Value((T)0), Name(name), Title(title), Description(description)	{ ::gpk::get_enum<T>().add_value_auto(name, title, description);	Value = ::gpk::get_value<T>(name); }

		inxp	oper	const	T&		()			const	{ return Value; }
	};

} // namespace

// Defines the enumeration type, the invalid value (-1) and the flag operators
#define GDEFINE_ENUM_TYPE(EnumName, IntType)																	\
	enum EnumName : IntType {};																					\
	static	::gpk::u2_c		__sei_##EnumName##enumInit	= ::gpk::enum_definition<EnumName>::init(#EnumName);	\
	sinx	EnumName		oper &	(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		(a & (IntType)b);				}	\
	sinx	EnumName		oper ~	(EnumName  a)				noexcept	{ return (EnumName)		(~(IntType)a);					}	\
	sinx	EnumName		oper ^	(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		(a ^ (IntType)b);				}	\
	stin	EnumName&		oper |=	(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	( ((IntType&)a) |= (IntType)b); }	\
	stin	EnumName&		oper &=	(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	( ((IntType&)a) &= (IntType)b); }	\
	stin	EnumName&		oper ^=	(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	( ((IntType&)a) ^= (IntType)b); }	\
	sinx	EnumName		oper |	(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		(a | (IntType)b);				}
//
#define GDEFINE_ENUM_STRUCT(EnumName, IntType)																								\
	struct EnumName {																														\
		IntType				Value			;																								\
		inline	oper		IntType&		()			nxpt	{ return Value; }															\
		inxp	oper		const IntType&	()	const	nxpt	{ return Value; }															\
	};																																		\
	static	::gpk::u2_c		__sei_##EnumName##enumInit	= ::gpk::enum_definition<EnumName>::init(#EnumName);								\
	sinx	EnumName		oper &	(EnumName  a, EnumName b)	noexcept	{ return {a & (IntType)b};				}	\
	sinx	EnumName		oper ~	(EnumName  a)				noexcept	{ return {~(IntType)a};					}	\
	sinx	EnumName		oper ^	(EnumName  a, EnumName b)	noexcept	{ return {a ^ (IntType)b};				}	\
	stin	EnumName&		oper |=	(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	{ ((IntType&)a) |= (IntType)b}; }	\
	stin	EnumName&		oper &=	(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	{ ((IntType&)a) &= (IntType)b}; }	\
	stin	EnumName&		oper ^=	(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	{ ((IntType&)a) ^= (IntType)b}; }	\
	sinx	EnumName		oper |	(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		{a | (IntType)b};				}
//
#ifdef GPK_ATMEL
#	define GDEFINE_ENUM_VALUE(EnumName, ValueName, EnumValue)					\
	stxp	const EnumName	EnumName##_##ValueName	= (EnumName)(EnumValue);	\
	//static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {szof(#ValueName) - 1, F(#ValueName)})

#	define GDEFINE_ENUM_VALUED(EnumName, ValueName, EnumValue, EnumDescription)		\
	stxp	const EnumName	EnumName##_##ValueName	= (EnumName)(EnumValue);		\
	//static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {szof(#ValueName) - 1, F(#ValueName)})

#	define GDEFINE_ENUM_VALTD(EnumName, ValueName, EnumValue, EnumTitle, EnumDescription)	\
	stxp	const EnumName	EnumName##_##ValueName	= (EnumName)(EnumValue);				\
	//static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {szof(#ValueName) - 1, F(#ValueName)})
	
#else
#	define GDEFINE_ENUM_VALUE(EnumName, ValueName, EnumValue)							\
	stxp	const EnumName	EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {szof(#ValueName) - 1, #ValueName})

#	define GDEFINE_ENUM_AVALUE(EnumName, ValueName) static	const EnumName	EnumName##_##ValueName	= (EnumName)::gpk::genum_value_auto<EnumName>({szof(#ValueName) - 1, #ValueName})

#	define GDEFINE_ENUM_VALUED(EnumName, ValueName, EnumValue, EnumDescription)				\
	stxp	const EnumName	EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {szof(#ValueName) - 1, #ValueName}, {szof(EnumDescription) - 1, EnumDescription})

#	define GDEFINE_ENUM_VALTD(EnumName, ValueName, EnumValue, EnumTitle, EnumDescription)				\
	stxp	const EnumName	EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {szof(#ValueName) - 1, #ValueName}, {szof(EnumTitle) - 1, EnumTitle}, {szof(EnumDescription) - 1, EnumDescription})
#endif

#define GDEFINE_FLAG_TYPE		GDEFINE_ENUM_TYPE
#define GDEFINE_FLAG_VALUE		GDEFINE_ENUM_VALUE

#if defined(GPK_WINDOWS)
#	pragma warning(disable : 4063)	// On Windows, using enum types like we do cause the compiler to throw a warning when the warning level is set to 4
#endif

#define gpk_warning_unhandled_value(_enumValue)			warning_printf("Unhandled'%s' value:(0x%X)(%" GPK_FMT_S2 ")(%c) %s"	, ::gpk::get_enum_namep GPKREP3(_enumValue) ? char(_enumValue) : '?', ::gpk::get_value_namep(_enumValue))
#define gpk_enum_valued_log(_funcPrintf, _enumValue)	_funcPrintf("'%s':(0x%X)(%" GPK_FMT_S2 ")(%c)'%s'-'%s'"				, ::gpk::get_enum_namep GPKREP3(_enumValue) ? char(_enumValue) : '?', ::gpk::get_value_namep(_enumValue), ::gpk::get_value_descp(_enumValue))
#define gpk_enum_value_info(_enumValue)					gpk_enum_value_log	(info_printf	, _enumValue)
#define gpk_enum_value_warning(_enumValue)				gpk_enum_value_log	(warning_printf	, _enumValue)
#define gpk_enum_value_error(_enumValue)				gpk_enum_value_log	(error_printf	, _enumValue)
#define gpk_enum_valued_info(_enumValue)				gpk_enum_valued_log	(info_printf	, _enumValue)
#define gpk_enum_valued_warning(_enumValue)				gpk_enum_valued_log	(warning_printf	, _enumValue)
#define gpk_enum_valued_error(_enumValue)				gpk_enum_valued_log	(error_printf	, _enumValue)

namespace gpk
{
	tpl_t struct SGPKEnumValue {
		_t			Value;
		vcst_c		Name;
	};
}
#define GPK_ENUM_BEGIN(EnumName, IntType)																							\
	struct EnumName {																												\
		IntType									Value; 																				\
		stxp	vcst_c							Name	= GPK_CXS(#EnumName);														\
																																	\
		inxp									EnumName(IntType value = -1)		: Value(value) {}								\
																																	\
		inxp	oper							IntType	()				cnst nxpt	{ return Value;	}								\
		inxp	EnumName						oper ~	()				cnst nxpt	{ return {IntType(~Value)};				}		\
		inxp	EnumName						oper &	(EnumName b)	cnst nxpt	{ return {IntType(Value & b.Value)};	}		\
		inxp	EnumName						oper ^	(EnumName b)	cnst nxpt	{ return {IntType(Value ^ b.Value)};	}		\
		inxp	EnumName						oper |	(EnumName b)	cnst nxpt	{ return {IntType(Value | b.Value)};	}		\
		inln	EnumName&						oper |=	(EnumName b)	nxpt		{ return *this = {IntType(Value |= b.Value)}; }	\
		inln	EnumName&						oper &=	(EnumName b)	nxpt		{ return *this = {IntType(Value &= b.Value)}; }	\
		inln	EnumName&						oper ^=	(EnumName b)	nxpt		{ return *this = {IntType(Value ^= b.Value)}; }	\
																																	\
		::gpk::vcsc_c & 						getEnumName		()		cnst nxpt		{ return Name; }							\
		::gpk::vcsc_c & 						getValueName	()				cnst	{ return getValueName(Value); }				\
		::gpk::vcsc_c & 						getValueName	(IntType value)	cnst	{											\
			for(uint32_t i=0, count = ::gpk::size(Values); i < count; ++i) {														\
				if(value == Values[i].Value)																						\
					return Values[i].Name;																							\
			}																														\
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);											\
			return ::gpk::UNRESOLVED_ENUM_LABEL_STR;																				\
		}																															\
		stxp 	::gpk::SGPKEnumValue<IntType> 	Values	[]	= {

#define GPK_ENUM_VALUE(ValueName, IntegerValue) { IntegerValue, GPK_CXS(#ValueName) },

#define GPK_ENUM_END() \
			};	\
	};

namespace gpk
{
//#ifdef GPK_ESP8266
// 	GPK_ENUM_BEGIN	(RESULT, int8_t)
//	GPK_ENUM_VALUE	(Ok                , int8_t(::gpk::OS_OK                ))
//	GPK_ENUM_VALUE	(Error             , int8_t(::gpk::OS_ERROR             ))
//	GPK_ENUM_VALUE	(Busy              , int8_t(::gpk::OS_BUSY              ))
//	GPK_ENUM_VALUE	(Timeout           , int8_t(::gpk::OS_TIMEOUT           ))
//	GPK_ENUM_VALUE	(Full              , int8_t(::gpk::OS_FULL              ))
//	GPK_ENUM_VALUE	(Empty             , int8_t(::gpk::OS_EMPTY             ))
//	GPK_ENUM_VALUE	(Overrun           , int8_t(::gpk::OS_OVERRUN           ))
//	GPK_ENUM_VALUE	(Not_available     , int8_t(::gpk::OS_NOT_AVAILABLE     ))
//	GPK_ENUM_VALUE	(Not_found         , int8_t(::gpk::OS_NOT_FOUND         ))
//	GPK_ENUM_VALUE	(Invalid_parameter , int8_t(::gpk::OS_INVALID_PARAMETER ))
//	GPK_ENUM_VALUE	(Forbidden         , int8_t(::gpk::OS_FORBIDDEN         ))
//	GPK_ENUM_VALUE	(Restart           , int8_t(::gpk::OS_RESTART           ))
//	GPK_ENUM_VALUE	(Wake_up           , int8_t(::gpk::OS_WAKE_UP           ))
//	GPK_ENUM_VALUE	(Sleep             , int8_t(::gpk::OS_SLEEP             ))
//	GPK_ENUM_VALUE	(Offline           , int8_t(::gpk::OS_OFFLINE           ))
//	GPK_ENUM_VALUE	(Disconnected      , int8_t(::gpk::OS_DISCONNECTED      ))
//	GPK_ENUM_VALUE	(Connecting        , int8_t(::gpk::OS_CONNECTING        ))
//	GPK_ENUM_VALUE	(Connected         , int8_t(::gpk::OS_CONNECTED         ))
//	GPK_ENUM_VALUE	(Missing_data      , int8_t(::gpk::OS_MISSING_DATA      ))
//	GPK_ENUM_VALUE	(No_Memory         , int8_t(::gpk::OS_NO_MEMORY         ))
//	GPK_ENUM_VALUE	(Read              , int8_t(::gpk::OS_READ              ))
//	GPK_ENUM_VALUE	(Write             , int8_t(::gpk::OS_WRITE             ))
// 	GPK_ENUM_END	();
//	
// 	GPK_ENUM_BEGIN	(COMMAND, int8_t)
//	GPK_ENUM_VALUE	(Ok                , int8_t(::gpk::OS_OK                ) * -1)
//	GPK_ENUM_VALUE	(Error             , int8_t(::gpk::OS_ERROR             ) * -1)
//	GPK_ENUM_VALUE	(Busy              , int8_t(::gpk::OS_BUSY              ) * -1)
//	GPK_ENUM_VALUE	(Timeout           , int8_t(::gpk::OS_TIMEOUT           ) * -1)
//	GPK_ENUM_VALUE	(Full              , int8_t(::gpk::OS_FULL              ) * -1)
//	GPK_ENUM_VALUE	(Empty             , int8_t(::gpk::OS_EMPTY             ) * -1)
//	GPK_ENUM_VALUE	(Overrun           , int8_t(::gpk::OS_OVERRUN           ) * -1)
//	GPK_ENUM_VALUE	(Not_available     , int8_t(::gpk::OS_NOT_AVAILABLE     ) * -1)
//	GPK_ENUM_VALUE	(Not_found         , int8_t(::gpk::OS_NOT_FOUND         ) * -1)
//	GPK_ENUM_VALUE	(Invalid_parameter , int8_t(::gpk::OS_INVALID_PARAMETER ) * -1)
//	GPK_ENUM_VALUE	(Forbidden         , int8_t(::gpk::OS_FORBIDDEN         ) * -1)
//	GPK_ENUM_VALUE	(Restart           , int8_t(::gpk::OS_RESTART           ) * -1)
//	GPK_ENUM_VALUE	(Wake_up           , int8_t(::gpk::OS_WAKE_UP           ) * -1)
//	GPK_ENUM_VALUE	(Sleep             , int8_t(::gpk::OS_SLEEP             ) * -1)
//	GPK_ENUM_VALUE	(Offline           , int8_t(::gpk::OS_OFFLINE           ) * -1)
//	GPK_ENUM_VALUE	(Disconnected      , int8_t(::gpk::OS_DISCONNECTED      ) * -1)
//	GPK_ENUM_VALUE	(Connecting        , int8_t(::gpk::OS_CONNECTING        ) * -1)
//	GPK_ENUM_VALUE	(Connected         , int8_t(::gpk::OS_CONNECTED         ) * -1)
//	GPK_ENUM_VALUE	(Missing_data      , int8_t(::gpk::OS_MISSING_DATA      ) * -1)
//	GPK_ENUM_VALUE	(No_Memory         , int8_t(::gpk::OS_NO_MEMORY         ) * -1)
//	GPK_ENUM_VALUE	(Read              , int8_t(::gpk::OS_READ              ) * -1)
//	GPK_ENUM_VALUE	(Write             , int8_t(::gpk::OS_WRITE             ) * -1)
// 	GPK_ENUM_END	();
//	
//#else // !GPK_ESP8266
 	GDEFINE_ENUM_TYPE (RESULT, int8_t);
	GDEFINE_ENUM_VALUE(RESULT, Ok                , int8_t(::gpk::OS_OK                ));
	GDEFINE_ENUM_VALUE(RESULT, Error             , int8_t(::gpk::OS_ERROR             ));
	GDEFINE_ENUM_VALUE(RESULT, Busy              , int8_t(::gpk::OS_BUSY              ));
	GDEFINE_ENUM_VALUE(RESULT, Timeout           , int8_t(::gpk::OS_TIMEOUT           ));
	GDEFINE_ENUM_VALUE(RESULT, Full              , int8_t(::gpk::OS_FULL              ));
	GDEFINE_ENUM_VALUE(RESULT, Empty             , int8_t(::gpk::OS_EMPTY             ));
	GDEFINE_ENUM_VALUE(RESULT, Overrun           , int8_t(::gpk::OS_OVERRUN           ));
	GDEFINE_ENUM_VALUE(RESULT, Not_available     , int8_t(::gpk::OS_NOT_AVAILABLE     ));
	GDEFINE_ENUM_VALUE(RESULT, Not_found         , int8_t(::gpk::OS_NOT_FOUND         ));
	GDEFINE_ENUM_VALUE(RESULT, Invalid_parameter , int8_t(::gpk::OS_INVALID_PARAMETER ));
	GDEFINE_ENUM_VALUE(RESULT, Forbidden         , int8_t(::gpk::OS_FORBIDDEN         ));
	GDEFINE_ENUM_VALUE(RESULT, Restart           , int8_t(::gpk::OS_RESTART           ));
	GDEFINE_ENUM_VALUE(RESULT, Wake_up           , int8_t(::gpk::OS_WAKE_UP           ));
	GDEFINE_ENUM_VALUE(RESULT, Sleep             , int8_t(::gpk::OS_SLEEP             ));
	GDEFINE_ENUM_VALUE(RESULT, Offline           , int8_t(::gpk::OS_OFFLINE           ));
	GDEFINE_ENUM_VALUE(RESULT, Disconnected      , int8_t(::gpk::OS_DISCONNECTED      ));
	GDEFINE_ENUM_VALUE(RESULT, Connecting        , int8_t(::gpk::OS_CONNECTING        ));
	GDEFINE_ENUM_VALUE(RESULT, Connected         , int8_t(::gpk::OS_CONNECTED         ));
	GDEFINE_ENUM_VALUE(RESULT, Missing_data      , int8_t(::gpk::OS_MISSING_DATA      ));
	GDEFINE_ENUM_VALUE(RESULT, No_Memory         , int8_t(::gpk::OS_NO_MEMORY         ));
	GDEFINE_ENUM_VALUE(RESULT, Read              , int8_t(::gpk::OS_READ              ));
	GDEFINE_ENUM_VALUE(RESULT, Write             , int8_t(::gpk::OS_WRITE             ));

 	GDEFINE_ENUM_TYPE (COMMAND, int8_t);
	GDEFINE_ENUM_VALUE(COMMAND, Ok                , RESULT_Ok                * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Error             , RESULT_Error             * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Busy              , RESULT_Busy              * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Timeout           , RESULT_Timeout           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Full              , RESULT_Full              * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Empty             , RESULT_Empty             * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Overrun           , RESULT_Overrun           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Not_available     , RESULT_Not_available     * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Not_found         , RESULT_Not_found         * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Invalid_parameter , RESULT_Invalid_parameter * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Forbidden         , RESULT_Forbidden         * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Restart           , RESULT_Restart           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Wake_up           , RESULT_Wake_up           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Sleep             , RESULT_Sleep             * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Offline           , RESULT_Offline           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Disconnected      , RESULT_Disconnected      * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Connecting        , RESULT_Connecting        * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Connected         , RESULT_Connected         * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Missing_data      , RESULT_Missing_data      * -1);
	GDEFINE_ENUM_VALUE(COMMAND, No_Memory         , RESULT_No_Memory         * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Read              , RESULT_Read              * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Write             , RESULT_Write             * -1);
//#endif // GPK_ESP8266
}

#endif // GPK_ENUM_H_23627
