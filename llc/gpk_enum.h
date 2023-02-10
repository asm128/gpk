// These sources are best viewed in Visual Studio 2017 or newer with a screen of at least 1920x1080 screen, Consolas font and zoom set to 81 %.
// Pablo Ariel Zorrilla Cepeda (asm128) Copyright (c) 2016-2022 - Distributed under the MIT License.
#include "gpk_array.h"

#ifndef GPK_ENUM_H_982364987234987234
#define GPK_ENUM_H_982364987234987234

#define gpk_enum_warning_printf // warning_printf

namespace gpk
{
	static				const ::gpk::vcs				INVALID_ENUM_VALUE_STR					= "INVALID";
	//static constexpr	const ::gpk::vcs				UNDEFINED_ENUM_TYPE_STR					= "Undefined enumeration type.";
	static				const ::gpk::vcs				UNDEFINED_ENUM_VALUE_STR				= "Undefined enumeration value";
	static				const ::gpk::vcs				UNRESOLVED_ENUM_LABEL_STR				= "Unresolved enumeration value name";

	// This template is intended to store the name of an enumeration, the values of such enumeration and a string representing each value.
	// The implementation separates names from values for improving search speed by reducing the memory usage when performing searches for names/values.
	template <typename _tValue>
	struct enum_definition {
		typedef				_tValue							TValue;
		static constexpr	const _tValue					INVALID_VALUE							= (_tValue)(-1);

							::gpk::vcc						Name									= "";
							::gpk::apod<_tValue>			Values									= {};
							::gpk::apod<::gpk::vcc>			Names									= {};
							::gpk::apod<::gpk::vcc>			Descriptions							= {};

		inline												enum_definition							()															: Name(::gpk::vcs{"Enum definition name not set."})	{}
		inline static		enum_definition<_tValue>&		get										()															{
			static	enum_definition<_tValue>						valueRegistry;
			return valueRegistry;
		}
		inline static		_tValue							init									(const ::gpk::vcc & enumName)								{
					enum_definition<_tValue>&						instanceHere							= get();

			if( instanceHere.Name != enumName || (instanceHere.Values.size() && (instanceHere.Values[0] != INVALID_VALUE)) )
				verbose_printf("Initializing enumeration type: '%s'.", enumName.begin());

			static const ::gpk::vcc									newName									= instanceHere.Name = enumName;
			(void)newName;
			return INVALID_VALUE;
		}
							::gpk::error_t					get_value								(const ::gpk::vcc & name, _tValue& value)	const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(name == Names[i]) {
					value												= Values[i];
					return 0;
				}
			error_printf("Enumeration value not found! Name: %s.", name.begin());
			value															= INVALID_VALUE;
			return -1;
		}
							::gpk::error_t					get_value								(const char_t* name, _tValue& value)		const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(0 == ::strcmp(name, Names[i].begin())) {
					value												= Values[i];
					return 0;
				}
			error_printf("Enumeration value not found! Name: %s.", name);
			value															= INVALID_VALUE;
			return -1;
		}
							_tValue							get_value								(const ::gpk::vcc & name)					const			{
			for(uint32_t i=0, count = Names.size(); i<count; ++i)
				if(name == Names[i])
					return Values[i];

			error_printf("Enumeration value not found! Name: %s.", name.begin());
			return INVALID_VALUE;
		}
							::gpk::error_t					get_value_by_index						(uint32_t index, _tValue& value)			const			{
			if( index < Values.size() ) {
				value												= Values[index];
				return 0;
			}
			gpk_enum_warning_printf("Enumeration index out of range! Index: 0x%u.", index);
			value												= INVALID_VALUE;
			return -1;
		}
							_tValue							get_value_by_index						(uint32_t index)							const			{
			retval_gwarn_if(INVALID_VALUE, index >= Values.size(), "Enumeration index out of range! Index: 0x%u.", index)
			return Values[index];
		}
							::gpk::error_t					get_label_by_index						(uint32_t index, ::gpk::vcc & value)		const			{
			if( index < Names.size() ) {
				value												= Names[index];
				return 0;
			}
			value												= ::gpk::UNDEFINED_ENUM_VALUE_STR;
			gpk_enum_warning_printf("Enumeration index out of range! Index: 0x%u.", index);
			return -1;
		}
							::gpk::vcc						get_label_by_index						(uint32_t index)							const			{
			if( index < Names.size() ) {
				return Names[index];
			}
			else {
				gpk_enum_warning_printf("Enumeration index out of range! Index: 0x%u.", index);
				return ::gpk::UNDEFINED_ENUM_VALUE_STR;
			}
		}
							::gpk::error_t					get_value_index							(const ::gpk::vcc & name, int32_t& index)	const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(name == Names[i]) {
					index												= (int32_t)i;
					return 0;
				}
			error_printf("Enumeration value not found! Name: %s.", name.begin());
			return index											= -1;
		}
							int32_t							get_value_index							(const ::gpk::vcc & name)					const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(name == Names[i])
					return (int32_t)i;

			error_printf("Enumeration value not found! Name: %s.", name.begin());
			return -1;
		}
							::gpk::error_t					get_value_index							(const _tValue& value, int32_t& index)		const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(value == Values[i]) {
					index												= (int32_t)i;
					return 0;
				}
			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return index										= -1;
		}
							int32_t							get_value_index							(const _tValue& value)						const			{
			for(uint32_t i=0, count = Names.size(); i < count; ++i)
				if(value == Values[i])
					return (int32_t)i;

			gpk_enum_warning_printf("Enumeration value not found! Value: %llX.", (uint64_t)value);
			return -1;
		}
							::gpk::error_t					get_value_label							(const _tValue& value, ::gpk::vcc & name)	const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i]) {
					name												= Names[i];
					return 0;
				}
			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			name												= ::gpk::UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
							::gpk::error_t					get_value_desc							(const _tValue& value, ::gpk::vcc & name)	const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i]) {
					name												= Descriptions[i];
					return 0;
				}
			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			name												= ::gpk::UNRESOLVED_ENUM_LABEL_STR;
			return -1;
		}
							const ::gpk::vcc &				get_value_label							(const _tValue& value)						const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i])
					return Names[i];

			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return ::gpk::UNRESOLVED_ENUM_LABEL_STR;
		}
							const ::gpk::vcc &				get_value_desc							(const _tValue& value)						const			{
			for(uint32_t i=0, count = Values.size(); i < count; ++i)
				if(value == Values[i])
					return Descriptions[i];

			gpk_enum_warning_printf("Enumeration value not found! Value: 0x%llX.", (uint64_t)value);
			return ::gpk::UNRESOLVED_ENUM_LABEL_STR;
		}
							::gpk::error_t					add_value								(const _tValue& value, const ::gpk::vcc & name, const ::gpk::vcc & description)				{
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
			uint32_t												newIndex								= Values.push_back(value);
			gpk_necall(newIndex, "Failed to add value to enumeration definition. Value: 0x%llX. Name: %s.", (uint64_t)value, name.begin());
			gpk_necall(Names.push_back(name), "Failed to add value name to enumeration definition. Value: 0x%llX. Name: %s.", (uint64_t)value, name.begin());
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
	template <typename _tValue>
	struct genum_value {
									_tValue							Value					= ::gpk::enum_definition<_tValue>::INVALID_VALUE;
									::gpk::vcc						Name					= INVALID_ENUM_VALUE_STR;
									::gpk::vcc						Description				= INVALID_ENUM_VALUE_STR;
		//
		inline														genum_value				()																				= default;
		inline constexpr											genum_value				(const genum_value& other)														= default;
		inline														genum_value				(const _tValue& value)															: Value((_tValue)value), Name(::gpk::enum_definition<_tValue>::get().get_value_name(value))				{}
		inline constexpr											genum_value				(const _tValue& value, const ::gpk::vcc & name)									: Value((_tValue)value), Name(name), Description(name)			{ ::gpk::enum_definition<_tValue>::get().add_value(value, name, name);	}
		inline constexpr											genum_value				(const _tValue& value, const ::gpk::vcc & name, const ::gpk::vcc & description)	: Value((_tValue)value), Name(name), Description(description)	{ ::gpk::enum_definition<_tValue>::get().add_value(value, name, description);	}
		inline constexpr			operator						const	_tValue&		()												const	{ return Value; }
	};

	template <typename _tEnum>	uint32_t						get_value_count			()												{ return ::gpk::enum_definition<_tEnum>::get().Values.size();	}
	template <typename _tEnum>	const ::gpk::vcc &				get_value_label			(const _tEnum & statusBit					)	{ return ::gpk::enum_definition<_tEnum>::get().get_value_label	(statusBit);	}
	template <typename _tEnum>	const ::gpk::vcc &				get_value_desc			(const _tEnum & statusBit					)	{ return ::gpk::enum_definition<_tEnum>::get().get_value_desc	(statusBit);	}
	template <typename _tEnum>	const ::gpk::view<::gpk::vcc> &	get_value_labels		()												{ return ::gpk::enum_definition<_tEnum>::get().Names;	}
	template <typename _tEnum>	int32_t							get_value_index			(const _tEnum & statusBit					)	{ return ::gpk::enum_definition<_tEnum>::get().get_value_index	(statusBit);	}
	template <typename _tEnum>	_tEnum							get_value				(const ::gpk::vcc & valueLabel	)	{ return ::gpk::enum_definition<_tEnum>::get().get_value		(valueLabel);	}
	template <typename _tEnum, size_t _sLen>
								_tEnum							get_value				(const char (&valueLabel)[_sLen])				{ return ::gpk::enum_definition<_tEnum>::get().get_value(::gpk::vcs{valueLabel});	}

	template<typename _tEventType>
	struct SEvent {
		_tEventType				Type;
		::gpk::apod<ubyte_t>	Data;
	};
} // namespace

// Defines the enumeration type, the invalid value (-1) and the flag operators
#define GDEFINE_ENUM_TYPE(EnumName, IntType)																																											\
	enum EnumName : IntType {};																																															\
	static						const ::gpk::vcs				EnumName##_STR							= #EnumName;																										\
	/*static constexpr			const EnumName					EnumName##_INVALID						= ::gpk::enum_definition<EnumName>::INVALID_VALUE;																	*/\
	/*static						const EnumName					__sei_##EnumName##_INVALID				= (EnumName)::gpk::enum_definition<EnumName>::init(EnumName##_STR);													*/\
	static inline constexpr		EnumName						operator &								(EnumName  a, EnumName b)					noexcept	{ return (EnumName)		(a & (IntType)b);				}	\
	static inline constexpr		EnumName						operator ~								(EnumName  a)								noexcept	{ return (EnumName)		(~(IntType)a);					}	\
	static inline constexpr		EnumName						operator ^								(EnumName  a, EnumName b)					noexcept	{ return (EnumName)		(a ^ (IntType)b);				}	\
	static inline				EnumName&						operator |=								(EnumName &a, EnumName b)					noexcept	{ return (EnumName&)	( ((IntType&)a) |= (IntType)b); }	\
	static inline				EnumName&						operator &=								(EnumName &a, EnumName b)					noexcept	{ return (EnumName&)	( ((IntType&)a) &= (IntType)b); }	\
	static inline				EnumName&						operator ^=								(EnumName &a, EnumName b)					noexcept	{ return (EnumName&)	( ((IntType&)a) ^= (IntType)b); }	\
	static inline constexpr		EnumName						operator |								(EnumName  a, EnumName b)					noexcept	{ return (EnumName)		(a | (IntType)b);				}

#define GDEFINE_ENUM_VALUE(EnumName, ValueName, EnumValue)																																\
	static constexpr			const EnumName					EnumName##_##ValueName					= (EnumName)(EnumValue);														\
	static						const EnumName					__sei_##EnumName##_##ValueName			= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), ::gpk::vcs{#ValueName})

#define GDEFINE_ENUM_VALUED(EnumName, ValueName, EnumValue, EnumDescription)																																\
	static constexpr			const EnumName					EnumName##_##ValueName					= (EnumName)(EnumValue);														\
	static						const EnumName					__sei_##EnumName##_##ValueName			= (EnumName)::gpk::genum_value<EnumName>((EnumName)(EnumValue), ::gpk::vcs{#ValueName}, ::gpk::vcs{EnumDescription})

#define GDEFINE_FLAG_TYPE				GDEFINE_ENUM_TYPE
#define GDEFINE_FLAG_VALUE				GDEFINE_ENUM_VALUE

#if defined(GPK_WINDOWS)
#	pragma warning(disable : 4063)	// On Windows, using enum types like we do cause the compiler to throw a warning when the warning level is set to 4
#endif

#endif // GPK_ENUM_H_982364987234987234
