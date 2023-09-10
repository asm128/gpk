// These sources are best viewed in Visual Studio 2017 or newer with a screen of at least 1920x1080 screen, Consolas font and zoom set to 81 %.
// Pablo Ariel Zorrilla Cepeda (asm128) Copyright (c) 2016-2022 - Distributed under the MIT License.
#include "gpk_array_pod.h"
#include "gpk_array_obj.h"

#ifndef GPK_ENUM_H_23627
#define GPK_ENUM_H_23627

#define enum_printf verbose_printf

namespace gpk
{
	//stacxpr	::gpk::vcc				UNDEFINED_ENUM_TYPE_STR					= "Undefined enumeration type.";
	stacxpr	::gpk::vcc			INVALID_ENUM_VALUE_STR		= {7, "INVALID"};
	stacxpr	::gpk::vcc			UNDEFINED_ENUM_VALUE_STR	= {27, "Undefined enumeration value."};
	stacxpr	::gpk::vcc			UNRESOLVED_ENUM_LABEL_STR	= {33, "Unresolved enumeration value name."};
	stacxpr	::gpk::vcc			UNRESOLVED_ENUM_NAME_STR	= {33, "Enum definition name not set."};

	// This tplt is intended to store the name of an enumeration, the values of such enumeration and a string representing each value.
	// The implementation separates names from values for improving search speed by reducing the memory usage when performing searches for names/values.
	tplt <tpnm T>
	struct enum_definition {
		typedef	T					TValue;
		stacxpr	T					INVALID_VALUE			= (T)(-1);

		::gpk::vcc					Name					= UNRESOLVED_ENUM_NAME_STR;
		::gpk::apod<T>				Values					= {};
		::gpk::avcc					Names					= {};
		::gpk::avcc					Titles					= {};
		::gpk::avcc					Descriptions			= {};

		stainli	enum_definition<T>&	get						()															{
			static	enum_definition<T>		valueRegistry;
			return valueRegistry;
		}
		stainli	T					init					(const ::gpk::vcc & enumName)								{
			enum_definition<T>&				instanceHere			= get();

			if( instanceHere.Name != enumName || (instanceHere.Values.size() && (instanceHere.Values[0] != INVALID_VALUE)) )
				verbose_printf("Initializing enumeration type: '%s'.", enumName.begin());

			if(instanceHere.Name.size() != enumName.size())
				instanceHere.Name			= enumName;

			return INVALID_VALUE;
		}
		::gpk::error_t				get_value				(const ::gpk::vcc & name, T & value)		const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(name == Names[i]) {
					value						= Values[i];
					return 0;
				}
			enum_printf("Enumeration value not found! Name: %s.", ::gpk::toString(name).begin());
			value						= INVALID_VALUE;
			return -1;
		}
		::gpk::error_t				get_value				(const char* name, T & value)				const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(0 == ::strcmp(name, Names[i].begin())) {
					value						= Values[i];
					return 0;
				}
			enum_printf("Enumeration value not found! Name: %s.", name);
			value						= INVALID_VALUE;
			return -1;
		}
		T							get_value				(const ::gpk::vcc & name)					const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i) {
				if(name == Names[i])
					return Values[i];
			}
			enum_printf("Enumeration value not found! Name: %s.", ::gpk::toString(name).begin());
			return INVALID_VALUE;
		}
		::gpk::error_t				get_value_by_index		(uint32_t index, T & value)					const			{
			if(index < Values.size()) {
				value						= Values[index];
				return 0;
			}
			enum_printf("Enumeration index out of range! Index: 0x%u.", index);
			value						= INVALID_VALUE;
			return -1;
		}
		T							get_value_by_index		(uint32_t index)							const			{
			retval_gwarn_if(INVALID_VALUE, index >= Values.size(), "Enumeration index out of range! Index: 0x%u.", index);
			return Values[index];
		}
		::gpk::error_t				get_label_by_index		(uint32_t index, ::gpk::vcc & value)		const			{
			if(index < Names.size()) {
				value						= Names[index];
				return 0;
			}
			value						= ::gpk::UNDEFINED_ENUM_VALUE_STR;
			enum_printf("Enumeration index out of range! Index: 0x%u.", index);
			return -1;
		}
		::gpk::vcc					get_label_by_index		(uint32_t index)							const			{
			if(index < Names.size())
				return Names[index];
			else {
				enum_printf("Enumeration index out of range! Index: 0x%u.", index);
				return ::gpk::UNDEFINED_ENUM_VALUE_STR;
			}
		}
		::gpk::error_t				get_value_index			(const ::gpk::vcc & name, int32_t & index)	const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(name == Names[i]) {
					index						= (int32_t)i;
					return index;
				}
			enum_printf("Enumeration value not found! Name: %s.", name.begin());
			return index				= -1;
		}
		int32_t						get_value_index			(const ::gpk::vcc & name)					const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i) {
				if(name == Names[i])
					return (int32_t)i;
			}
			enum_printf("Enumeration value not found! Name: %s.", name.begin());
			return -1;
		}
		::gpk::error_t				get_value_index			(const T & value, int32_t & index)		const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(value == Values[i]) {
					index					= (int32_t)i;
					return 0;
				}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return index							= -1;
		}
		int32_t						get_value_index			(const T & value)						const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i) {
				if(value == Values[i])
					return (int32_t)i;
			}
			enum_printf("Enumeration value not found! Value: %llX.", (uint64_t)value);
			return -1;
		}
		::gpk::error_t				get_value_label			(const T & value, ::gpk::vcc & name)	const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i]) {
					name						= Names[i];
					return 0;
				}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			name						= ::gpk::UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
		::gpk::error_t				get_value_desc			(const T & value, ::gpk::vcc & name)	const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i]) {
					name						= Descriptions[i];
					return 0;
				}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			name						= ::gpk::UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
		const ::gpk::vcc&			get_value_label			(const T & value)						const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i) {
				if(value == Values[i])
					return Names[i];
			}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return ::gpk::UNRESOLVED_ENUM_LABEL_STR;
		}
		const ::gpk::vcc&			get_value_desc			(const T & value)						const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i) {
				if(value == Values[i])
					return Descriptions[i];
			}
			enum_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return ::gpk::UNRESOLVED_ENUM_LABEL_STR;
		}
		::gpk::error_t				add_value				(const T & value, const ::gpk::vcc & name, const ::gpk::vcc & title, const ::gpk::vcc & description)	{
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
			uint32_t						newIndex				= Values.push_back(value);
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
		::gpk::error_t				add_value_auto			(const ::gpk::vcc & name, const ::gpk::vcc & title, const ::gpk::vcc & description)	{
			for(uint32_t i=0, count = Names.size(); i < count; ++i) {
				ree_if(name == Names[i], "Enumeration value already defined! Type: '%s'. Value: 0x%llX. Previous name: %s. New name: %s. Second definition ignored..."
					, Name		.begin()
					, (uint64_t)Values[i]
					, Names[i]	.begin()
					, name		.begin()
					);
			}
			const T							value					= (T)Values.size();
			uint32_t						newIndex				= Values.push_back(value);
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
	};

	tplt <tpnm TEnum>	inline	::gpk::enum_definition<TEnum>&	get_enum	()					noexcept	{ return ::gpk::enum_definition<TEnum>::get();	}
	tplt <tpnm TEnum>	inline	::gpk::enum_definition<TEnum>&	get_enum	(const TEnum & )	noexcept	{ return ::gpk::enum_definition<TEnum>::get();	}

	// This type is used to initialize an enumeration value.
	tplt <tpnm _tEnum>
	struct genum_value {
		typedef	_tEnum		T;

		T					Value					= ::gpk::enum_definition<T>::INVALID_VALUE;
		::gpk::vcc			Name					= INVALID_ENUM_VALUE_STR;
		::gpk::vcc			Title					= INVALID_ENUM_VALUE_STR;
		::gpk::vcc			Description				= INVALID_ENUM_VALUE_STR;
		//
		inlcxpr				genum_value				()										= default;
		inlcxpr				genum_value				(const genum_value & other)				= default;
							genum_value				(const T & value)						: Value((T)value), Name(::gpk::get_enum<T>().get_value_name(value))				{}
							genum_value				(const T & value, const ::gpk::vcc & name)																: Value((T)value), Name(name), Title(name), Description(name)			{ ::gpk::get_enum<T>().add_value(value, name, name, name);			}
							genum_value				(const T & value, const ::gpk::vcc & name, const ::gpk::vcc & description)								: Value((T)value), Name(name), Title(name), Description(description)	{ ::gpk::get_enum<T>().add_value(value, name, name, description);		}
							genum_value				(const T & value, const ::gpk::vcc & name, const ::gpk::vcc & title, const ::gpk::vcc & description)	: Value((T)value), Name(name), Title(title), Description(description)	{ ::gpk::get_enum<T>().add_value(value, name, title, description);	}

		inlcxpr	operator	const	T&				()			const	{ return Value; }
	};

	tplt <tpnm TEnum, size_t nameLen>
	TEnum						get_value			(const char (&valueLabel)[nameLen])		{ return ::gpk::get_enum<TEnum>().get_value(::gpk::vcs{valueLabel}); }
	tplt <tpnm TEnum>	TEnum	get_value			(const ::gpk::vcc & valueLabel)			{ return ::gpk::get_enum<TEnum>().get_value(valueLabel); }

	tplt <tpnm TEnum>	TEnum	get_value_camelcased(const ::gpk::vcc & uncased)			{
		::gpk::achar							camelCased;
		::gpk::camelCase(uncased, camelCased);
		return ::gpk::get_enum<TEnum>().get_value(camelCased);
	}
	tplt <tpnm TEnum>	uint32_t						get_value_count		()										{ return ::gpk::get_enum<TEnum>().Values.size(); }
	tplt <tpnm TEnum>	const ::gpk::vcc&				get_value_label		(const TEnum & statusBit)				{ return ::gpk::get_enum<TEnum>().get_value_label(statusBit); }
	tplt <tpnm TEnum>	const ::gpk::vcc&				get_value_namev		(const TEnum & statusBit)				{ return ::gpk::get_enum<TEnum>().get_value_label(statusBit); }
	tplt <tpnm TEnum>	const char*						get_value_namep		(const TEnum & statusBit)				{ return ::gpk::get_enum<TEnum>().get_value_label(statusBit).begin(); }
	tplt <tpnm TEnum>	const ::gpk::vcc&				get_value_descv		(const TEnum & statusBit)				{ return ::gpk::get_enum<TEnum>().get_value_desc (statusBit); }
	tplt <tpnm TEnum>	const ::gpk::vcc&				get_value_descp		(const TEnum & statusBit)				{ return ::gpk::get_enum<TEnum>().get_value_desc (statusBit); }
	tplt <tpnm TEnum>	const ::gpk::vvcc&				get_value_labels	()										{ return ::gpk::get_enum<TEnum>().Names; }
	tplt <tpnm TEnum>	const ::gpk::vvcc&				get_value_names		()										{ return ::gpk::get_enum<TEnum>().Names; }
	tplt <tpnm TEnum>	const ::gpk::vvcc&				get_value_descs		()										{ return ::gpk::get_enum<TEnum>().Names; }
	tplt <tpnm TEnum>	int32_t							get_value_index		(const TEnum & statusBit)				{ return ::gpk::get_enum<TEnum>().get_value_index(statusBit); }
	tplt <tpnm TEnum>	const ::gpk::vcc&				get_enum_namev		()							noexcept	{ return ::gpk::get_enum<TEnum>().Name;			}
	tplt <tpnm TEnum>	const ::gpk::vcc&				get_enum_namev		(const TEnum & )			noexcept	{ return ::gpk::get_enum<TEnum>().Name;			}
	tplt <tpnm TEnum>	const char*						get_enum_namep		()							noexcept	{ return ::gpk::get_enum<TEnum>().Name.begin();	}
	tplt <tpnm TEnum>	const char*						get_enum_namep		(const TEnum & )			noexcept	{ return ::gpk::get_enum<TEnum>().Name.begin();	}

	tplt <tpnm T>
	struct genum_value_auto {
		T					Value				= ::gpk::enum_definition<T>::INVALID_VALUE;
		::gpk::vcc			Name				= INVALID_ENUM_VALUE_STR;
		::gpk::vcc			Title				= INVALID_ENUM_VALUE_STR;
		::gpk::vcc			Description			= INVALID_ENUM_VALUE_STR;
		//
		inlcxpr				genum_value_auto	()									= default;
		inlcxpr				genum_value_auto	(const genum_value_auto & other)	= default;
							genum_value_auto	(const ::gpk::vcc & name)															: Value((T)0), Name(name), Title(name), Description(name)			{ ::gpk::get_enum<T>().add_value_auto(name, name, name);			Value = ::gpk::get_value<T>(name); }
							genum_value_auto	(const ::gpk::vcc & name, const ::gpk::vcc & description)							: Value((T)0), Name(name), Title(name), Description(description)	{ ::gpk::get_enum<T>().add_value_auto(name, name, description);		Value = ::gpk::get_value<T>(name); }
							genum_value_auto	(const ::gpk::vcc & name, const ::gpk::vcc & title, const ::gpk::vcc & description)	: Value((T)0), Name(name), Title(title), Description(description)	{ ::gpk::get_enum<T>().add_value_auto(name, title, description);	Value = ::gpk::get_value<T>(name); }

		inlcxpr	operator	const	T&		()			const	{ return Value; }
	};

} // namespace

// Defines the enumeration type, the invalid value (-1) and the flag operators
#define GDEFINE_ENUM_TYPE(EnumName, IntType)																			\
	enum EnumName : IntType {};				\
	static	const uint32_t		__sei_##EnumName##enumInit	= ::gpk::enum_definition<EnumName>::init(#EnumName);	\
	stincxp	EnumName			operator &					(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		(a & (IntType)b);				}	\
	stincxp	EnumName			operator ~					(EnumName  a)				noexcept	{ return (EnumName)		(~(IntType)a);					}	\
	stincxp	EnumName			operator ^					(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		(a ^ (IntType)b);				}	\
	stainli	EnumName&			operator |=					(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	( ((IntType&)a) |= (IntType)b); }	\
	stainli	EnumName&			operator &=					(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	( ((IntType&)a) &= (IntType)b); }	\
	stainli	EnumName&			operator ^=					(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	( ((IntType&)a) ^= (IntType)b); }	\
	stincxp	EnumName			operator |					(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		(a | (IntType)b);				}	\
	tplt<EnumName> const char*	get_enum_namep				(const EnumName &)			noexcept	{ return #EnumName;	} \

#ifdef GPK_ATMEL
#	define GDEFINE_ENUM_VALUE(EnumName, ValueName, EnumValue)					\
	stacxpr	const EnumName	EnumName##_##ValueName	= (EnumName)(EnumValue);	\
	//static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {sizeof(#ValueName) - 1, F(#ValueName)})

#	define GDEFINE_ENUM_VALUED(EnumName, ValueName, EnumValue, EnumDescription)		\
	stacxpr	const EnumName	EnumName##_##ValueName	= (EnumName)(EnumValue);		\
	//static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {sizeof(#ValueName) - 1, F(#ValueName)})

#	define GDEFINE_ENUM_VALTD(EnumName, ValueName, EnumValue, EnumTitle, EnumDescription)	\
	stacxpr	const EnumName	EnumName##_##ValueName	= (EnumName)(EnumValue);				\
	//static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {sizeof(#ValueName) - 1, F(#ValueName)})
	
#else
#	define GDEFINE_ENUM_VALUE(EnumName, ValueName, EnumValue)									\
	stacxpr	const EnumName	EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {sizeof(#ValueName) - 1, #ValueName})

#	define GDEFINE_ENUM_AVALUE(EnumName, ValueName) static	const EnumName	EnumName##_##ValueName	= (EnumName)::gpk::genum_value_auto<EnumName>({sizeof(#ValueName) - 1, #ValueName})

#	define GDEFINE_ENUM_VALUED(EnumName, ValueName, EnumValue, EnumDescription)				\
	stacxpr	const EnumName	EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {sizeof(#ValueName) - 1, #ValueName}, {sizeof(EnumDescription) - 1, EnumDescription})

#	define GDEFINE_ENUM_VALTD(EnumName, ValueName, EnumValue, EnumTitle, EnumDescription)				\
	stacxpr	const EnumName	EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	static	const EnumName	__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {sizeof(#ValueName) - 1, #ValueName}, {sizeof(EnumTitle) - 1, EnumTitle}, {sizeof(EnumDescription) - 1, EnumDescription})
#endif

#define GDEFINE_FLAG_TYPE		GDEFINE_ENUM_TYPE
#define GDEFINE_FLAG_VALUE		GDEFINE_ENUM_VALUE

#if defined(GPK_WINDOWS)
#	pragma warning(disable : 4063)	// On Windows, using enum types like we do cause the compiler to throw a warning when the warning level is set to 4
#endif

#define gpk_warning_unhandled_value(valueUnhandled)		warning_printf("Unhandled %s value: (0x%X)(%X)(%c) %s", ::gpk::get_enum_namep(valueUnhandled), valueUnhandled, valueUnhandled, (char)valueUnhandled, ::gpk::get_value_namep(valueUnhandled))
#define gpk_enum_value_info(_enumValue)					info_printf("'%s' value: (0x%X)(%X)(%c) '%s'", ::gpk::get_enum_namep(_enumValue), _enumValue, _enumValue, (char)_enumValue, ::gpk::get_value_namep(_enumValue))


namespace gpk
{
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
	GDEFINE_ENUM_VALUE(RESULT, Connecting        , int8_t(::gpk::OS_CONNECTING        ));
	GDEFINE_ENUM_VALUE(RESULT, Connected         , int8_t(::gpk::OS_CONNECTED         ));
	GDEFINE_ENUM_VALUE(RESULT, Missing_data      , int8_t(::gpk::OS_MISSING_DATA      ));
	GDEFINE_ENUM_VALUE(RESULT, No_Memory         , int8_t(::gpk::OS_NO_MEMORY         ));

	GDEFINE_ENUM_TYPE (COMMAND, int8_t);
	GDEFINE_ENUM_VALUE(COMMAND, Ok                , int8_t(::gpk::OS_OK                * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Error             , int8_t(::gpk::OS_ERROR             * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Busy              , int8_t(::gpk::OS_BUSY              * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Timeout           , int8_t(::gpk::OS_TIMEOUT           * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Full              , int8_t(::gpk::OS_FULL              * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Empty             , int8_t(::gpk::OS_EMPTY             * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Overrun           , int8_t(::gpk::OS_OVERRUN           * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Not_available     , int8_t(::gpk::OS_NOT_AVAILABLE     * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Not_found         , int8_t(::gpk::OS_NOT_FOUND         * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Invalid_parameter , int8_t(::gpk::OS_INVALID_PARAMETER * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Forbidden         , int8_t(::gpk::OS_FORBIDDEN         * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Restart           , int8_t(::gpk::OS_RESTART           * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Wake_up           , int8_t(::gpk::OS_WAKE_UP           * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Sleep             , int8_t(::gpk::OS_SLEEP             * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Offline           , int8_t(::gpk::OS_OFFLINE           * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Connecting        , int8_t(::gpk::OS_CONNECTING        * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Connected         , int8_t(::gpk::OS_CONNECTED         * -1));
	GDEFINE_ENUM_VALUE(COMMAND, Missing_data      , int8_t(::gpk::OS_MISSING_DATA      * -1));
	GDEFINE_ENUM_VALUE(COMMAND, No_Memory         , int8_t(::gpk::OS_NO_MEMORY         * -1));
}

#endif // GPK_ENUM_H_23627
