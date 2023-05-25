// These sources are best viewed in Visual Studio 2017 or newer with a screen of at least 1920x1080 screen, Consolas font and zoom set to 81 %.
// Pablo Ariel Zorrilla Cepeda (asm128) Copyright (c) 2016-2022 - Distributed under the MIT License.
#include "gpk_array_pod.h"
#include "gpk_array_obj.h"

#ifndef GPK_ENUM_H_982364987234987234
#define GPK_ENUM_H_982364987234987234

#define gpk_enum_warning_printf // warning_printf

namespace gpk
{
	//stacxpr	::gpk::vcc				UNDEFINED_ENUM_TYPE_STR					= "Undefined enumeration type.";
	stacxpr	::gpk::vcc			INVALID_ENUM_VALUE_STR		= {7, "INVALID"};
	stacxpr	::gpk::vcc			UNDEFINED_ENUM_VALUE_STR	= {27, "Undefined enumeration value."};
	stacxpr	::gpk::vcc			UNRESOLVED_ENUM_LABEL_STR	= {33, "Unresolved enumeration value name."};
	stacxpr	::gpk::vcc			UNRESOLVED_ENUM_NAME_STR	= {33, "Enum definition name not set."};

	// This template is intended to store the name of an enumeration, the values of such enumeration and a string representing each value.
	// The implementation separates names from values for improving search speed by reducing the memory usage when performing searches for names/values.
	template <typename T>
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
			error_printf("Enumeration value not found! Name: %s.", name.begin());
			value						= INVALID_VALUE;
			return -1;
		}
		::gpk::error_t				get_value				(const char* name, T & value)				const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(0 == ::strcmp(name, Names[i].begin())) {
					value						= Values[i];
					return 0;
				}
			error_printf("Enumeration value not found! Name: %s.", name);
			value						= INVALID_VALUE;
			return -1;
		}
		T							get_value				(const ::gpk::vcc & name)					const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i) {
				if(name == Names[i])
					return Values[i];
			}
			error_printf("Enumeration value not found! Name: %s.", name.begin());
			return INVALID_VALUE;
		}
		::gpk::error_t				get_value_by_index		(uint32_t index, T & value)					const			{
			if(index < Values.size()) {
				value						= Values[index];
				return 0;
			}
			gpk_enum_warning_printf("Enumeration index out of range! Index: 0x%u.", index);
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
			gpk_enum_warning_printf("Enumeration index out of range! Index: 0x%u.", index);
			return -1;
		}
		::gpk::vcc					get_label_by_index		(uint32_t index)							const			{
			if(index < Names.size())
				return Names[index];
			else {
				gpk_enum_warning_printf("Enumeration index out of range! Index: 0x%u.", index);
				return ::gpk::UNDEFINED_ENUM_VALUE_STR;
			}
		}
		::gpk::error_t				get_value_index			(const ::gpk::vcc & name, int32_t & index)	const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(name == Names[i]) {
					index						= (int32_t)i;
					return index;
				}
			error_printf("Enumeration value not found! Name: %s.", name.begin());
			return index				= -1;
		}
		int32_t						get_value_index			(const ::gpk::vcc & name)					const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i) {
				if(name == Names[i])
					return (int32_t)i;
			}
			error_printf("Enumeration value not found! Name: %s.", name.begin());
			return -1;
		}
		::gpk::error_t				get_value_index			(const T & value, int32_t & index)		const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(value == Values[i]) {
					index					= (int32_t)i;
					return 0;
				}
			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return index							= -1;
		}
		int32_t						get_value_index			(const T & value)						const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i) {
				if(value == Values[i])
					return (int32_t)i;
			}
			gpk_enum_warning_printf("Enumeration value not found! Value: %llX.", (uint64_t)value);
			return -1;
		}
		::gpk::error_t				get_value_label			(const T & value, ::gpk::vcc & name)	const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i]) {
					name						= Names[i];
					return 0;
				}
			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			name						= ::gpk::UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
		::gpk::error_t				get_value_desc			(const T & value, ::gpk::vcc & name)	const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i]) {
					name						= Descriptions[i];
					return 0;
				}
			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			name						= ::gpk::UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
		const ::gpk::vcc&			get_value_label			(const T & value)						const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i) {
				if(value == Values[i])
					return Names[i];
			}
			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return ::gpk::UNRESOLVED_ENUM_LABEL_STR;
		}
		const ::gpk::vcc&			get_value_desc			(const T & value)						const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i) {
				if(value == Values[i])
					return Descriptions[i];
			}
			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return ::gpk::UNRESOLVED_ENUM_LABEL_STR;
		}
		::gpk::error_t				add_value				(const T & value, const ::gpk::vcc & name, const ::gpk::vcc & title, const ::gpk::vcc & description)	{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(Values[i] == value) {
					rww_if(Names[i] != name, "Enumeration value already defined! Type: '%s'. Value: 0x%llX. Previous name: %s. New name: %s. Second definition ignored..."
						, Name		.begin()
						, (uint64_t)value
						, Names[i]	.begin()
						, name		.begin()
						);
					return 0;	// Found same name and value combination. This is normal when values are defined as static const.
				}
			uint32_t						newIndex				= Values.push_back(value);
			gpk_necall(newIndex, "Failed to add value to enumeration definition. Value: 0x%llX. Name: %s.", (uint64_t)value, name.begin());
			gpk_necall(Names.push_back(name), "Failed to add value name to enumeration definition. Value: 0x%llX. Name: %s.", (uint64_t)value, name.begin());
			gpk_necall(Titles.push_back(title), "Failed to add value name to enumeration definition. Value: 0x%llX. Name: %s.", (uint64_t)value, name.begin());
			gpk_necall(Descriptions.push_back(description), "Failed to add value description to enumeration definition. Value: 0x%llX. Name: %s.", (uint64_t)value, name.begin());
			verbose_printf("Added new value to enumeration definition. Enum name: %s. Index: %.02u, Value: 0x%llX. Value name: %s."
				, Name.begin()
				, (uint32_t)newIndex
				, (uint64_t)value
				, name.begin()
				);
			return 0;
		}
	};

	// This type is used to initialize an enumeration value.
	template <typename T>
	struct genum_value {
		T					Value					= ::gpk::enum_definition<T>::INVALID_VALUE;
		::gpk::vcc			Name					= INVALID_ENUM_VALUE_STR;
		::gpk::vcc			Title					= INVALID_ENUM_VALUE_STR;
		::gpk::vcc			Description				= INVALID_ENUM_VALUE_STR;
		//
		inlcxpr				genum_value				()																											= default;
		inlcxpr				genum_value				(const genum_value & other)																					= default;
		inlcxpr				genum_value				(const T & value)																						: Value((T)value), Name(::gpk::enum_definition<T>::get().get_value_name(value))				{}
		inlcxpr				genum_value				(const T & value, const ::gpk::vcc & name)															: Value((T)value), Name(name), Title(name), Description(name)			{ ::gpk::enum_definition<T>::get().add_value(value, name, name, name);			}
		inlcxpr				genum_value				(const T & value, const ::gpk::vcc & name, const ::gpk::vcc & description)							: Value((T)value), Name(name), Title(name), Description(description)	{ ::gpk::enum_definition<T>::get().add_value(value, name, name, description);		}
		inlcxpr				genum_value				(const T & value, const ::gpk::vcc & name, const ::gpk::vcc & title, const ::gpk::vcc & description)	: Value((T)value), Name(name), Title(title), Description(description)	{ ::gpk::enum_definition<T>::get().add_value(value, name, title, description);	}

		inlcxpr	operator	const	T&		()																				const	{ return Value; }
	};

	template <typename _tEnum, size_t _sLen>
	_tEnum														get_value			(const char (&valueLabel)[_sLen])		{ return ::gpk::enum_definition<_tEnum>::get().get_value({valueLabel, _sLen - 1});	}
	template <typename _tEnum>	_tEnum							get_value			(const ::gpk::vcc & valueLabel)			{ return ::gpk::enum_definition<_tEnum>::get().get_value(valueLabel);	}

	template <typename _tEnum>	uint32_t						get_value_count		()										{ return ::gpk::enum_definition<_tEnum>::get().Values.size();	}
	template <typename _tEnum>	const ::gpk::vcc&				get_value_label		(const _tEnum & statusBit)				{ return ::gpk::enum_definition<_tEnum>::get().get_value_label	(statusBit);	}
	template <typename _tEnum>	const ::gpk::vcc&				get_value_namev		(const _tEnum & statusBit)				{ return ::gpk::enum_definition<_tEnum>::get().get_value_label	(statusBit);	}
	template <typename _tEnum>	const char*						get_value_namep		(const _tEnum & statusBit)				{ return ::gpk::enum_definition<_tEnum>::get().get_value_label	(statusBit).begin();	}
	template <typename _tEnum>	const ::gpk::vcc&				get_value_descv		(const _tEnum & statusBit)				{ return ::gpk::enum_definition<_tEnum>::get().get_value_desc	(statusBit);	}
	template <typename _tEnum>	const ::gpk::vcc&				get_value_descp		(const _tEnum & statusBit)				{ return ::gpk::enum_definition<_tEnum>::get().get_value_desc	(statusBit);	}
	template <typename _tEnum>	const ::gpk::view<::gpk::vcc>&	get_value_labels	()										{ return ::gpk::enum_definition<_tEnum>::get().Names;	}
	template <typename _tEnum>	const ::gpk::view<::gpk::vcc>&	get_value_names		()										{ return ::gpk::enum_definition<_tEnum>::get().Names;	}
	template <typename _tEnum>	const ::gpk::view<::gpk::vcc>&	get_value_descs		()										{ return ::gpk::enum_definition<_tEnum>::get().Names;	}
	template <typename _tEnum>	int32_t							get_value_index		(const _tEnum & statusBit)				{ return ::gpk::enum_definition<_tEnum>::get().get_value_index	(statusBit);	}
	template <typename _tEnum>	const ::gpk::vcc&				get_enum_namev		()							noexcept	{ return ::gpk::enum_definition<_tEnum>::get().Name;			}
	template <typename _tEnum>	const char*						get_enum_namep		()							noexcept	{ return ::gpk::enum_definition<_tEnum>::get().Name.begin();	}
	template <typename _tEnum>	const ::gpk::vcc&				get_enum_namev		(const _tEnum & )			noexcept	{ return ::gpk::enum_definition<_tEnum>::get().Name;			}
	template <typename _tEnum>	const char*						get_enum_namep		(const _tEnum & )			noexcept	{ return ::gpk::enum_definition<_tEnum>::get().Name.begin();	}

} // namespace

// Defines the enumeration type, the invalid value (-1) and the flag operators
#define GDEFINE_ENUM_TYPE(EnumName, IntType)																			\
	enum EnumName : IntType {};																							\
	static	const uint32_t			__sei_##EnumName##enumInit	= ::gpk::enum_definition<EnumName>::init(#EnumName);	\
	stincxp	EnumName				operator &					(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		(a & (IntType)b);				}	\
	stincxp	EnumName				operator ~					(EnumName  a)				noexcept	{ return (EnumName)		(~(IntType)a);					}	\
	stincxp	EnumName				operator ^					(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		(a ^ (IntType)b);				}	\
	stainli	EnumName&				operator |=					(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	( ((IntType&)a) |= (IntType)b); }	\
	stainli	EnumName&				operator &=					(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	( ((IntType&)a) &= (IntType)b); }	\
	stainli	EnumName&				operator ^=					(EnumName &a, EnumName b)	noexcept	{ return (EnumName&)	( ((IntType&)a) ^= (IntType)b); }	\
	stincxp	EnumName				operator |					(EnumName  a, EnumName b)	noexcept	{ return (EnumName)		(a | (IntType)b);				}	\
	template<EnumName> const char*	get_enum_namep				(const EnumName &)			noexcept	{ return #EnumName;	} \

#define GDEFINE_ENUM_VALUE(EnumName, ValueName, EnumValue)									\
	stacxpr	const EnumName		EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	static	const EnumName		__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {sizeof(#ValueName) - 1, #ValueName})

#define GDEFINE_ENUM_VALUED(EnumName, ValueName, EnumValue, EnumDescription)				\
	stacxpr	const EnumName		EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	static	const EnumName		__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {sizeof(#ValueName) - 1, #ValueName}, {sizeof(EnumDescription) - 1, EnumDescription})

#define GDEFINE_ENUM_VALTD(EnumName, ValueName, EnumValue, EnumTitle, EnumDescription)				\
	stacxpr	const EnumName		EnumName##_##ValueName			= (EnumName)(EnumValue);	\
	static	const EnumName		__sei_##EnumName##_##ValueName	= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), {sizeof(#ValueName) - 1, #ValueName}, {sizeof(EnumTitle) - 1, EnumTitle}, {sizeof(EnumDescription) - 1, EnumDescription})

#define GDEFINE_FLAG_TYPE		GDEFINE_ENUM_TYPE
#define GDEFINE_FLAG_VALUE		GDEFINE_ENUM_VALUE

#if defined(GPK_WINDOWS)
#	pragma warning(disable : 4063)	// On Windows, using enum types like we do cause the compiler to throw a warning when the warning level is set to 4
#endif

#define gpk_warning_unhandled_value(valueUnhandled)		warning_printf("Unhandled %s value: %s (0x%X)(%X)(%c)", ::gpk::get_enum_namep(valueUnhandled), ::gpk::get_value_namep(valueUnhandled), valueUnhandled, valueUnhandled, (char)valueUnhandled)

#endif // GPK_ENUM_H_982364987234987234
