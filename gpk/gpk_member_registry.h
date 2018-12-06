/// Copyright 2016-2017 - asm128
#include "gpk_type_identifier.h"
#include "gpk_datatype.h"
#include "gpk_view_array.h"

#ifndef GPK_MEMBER_REGISTRY_H_9214982364982734
#define GPK_MEMBER_REGISTRY_H_9214982364982734

#define GPKM_REGISTRY_NAME		gpk_member_registry
#define GPKM_NAME(_memberName)	_gpk_info_##_memberName##_	// This mangles the member name for use as the type name.

#define GPKM(_nameSpace, _memberType, _memberName, _dataTypeId, _displayName, _memberDescription)																					\
	struct GPKM_NAME(_memberName)	{																																												\
		typedef _nameSpace::_memberType			TMember;	\
		static inline constexpr	const char_t*												get_member_namespace			()							noexcept	{ return #_nameSpace;						}	\
		static inline constexpr	const char_t*												get_member_type_name			()							noexcept	{ return #_memberType;						}	\
		static inline constexpr	const char_t*												get_member_name					()							noexcept	{ return #_memberName;						}	\
		static inline constexpr	const char_t*												get_member_display_name			()							noexcept	{ return _displayName;						}	\
		static inline constexpr	const char_t*												get_member_description			()							noexcept	{ return _memberDescription;				}	\
		static inline constexpr	::gpk::DATA_TYPE											get_member_data_type_id			()							noexcept	{ return _dataTypeId;						}	\
		static inline constexpr	uint32_t													get_member_size					()							noexcept	{ return sizeof(_nameSpace::_memberType);	}	\
	};																																																				\
	_nameSpace::_memberType		_memberName

namespace gpk {
#pragma pack(push, 1)
	template <typename... _tArgs>	struct member_registry {
		static inline constexpr	const uint32_t												get_member_count				()							noexcept	{ return (uint32_t)sizeof...(_tArgs);																																																														}
		static inline			const ::gpk::view_array<const ::gpk::STypeIdentifier	>&	get_types						()										{ static const ::gpk::STypeIdentifier	results[] = {{{			_tArgs::get_member_namespace(), (uint32_t)-1}, {_tArgs::get_member_type_name(), (uint32_t)-1}}...,	{}	}; static const ::gpk::view_array<const ::gpk::STypeIdentifier	> results_view = {results}; return results_view;	}
		static inline			const ::gpk::view_array<const ::gpk::label				>&	get_names						()										{ static const ::gpk::label				results[] = {{			_tArgs::get_member_name			(), (uint32_t)-1}	...,											""	}; static const ::gpk::view_array<const ::gpk::label			> results_view = {results}; return results_view;	}
		static inline			const ::gpk::view_array<const ::gpk::label				>&	get_display_names				()										{ static const ::gpk::label				results[] = {{			_tArgs::get_member_display_name	(), (uint32_t)-1}	...,											""	}; static const ::gpk::view_array<const ::gpk::label			> results_view = {results}; return results_view;	}
		static inline			const ::gpk::view_array<const ::gpk::label				>&	get_descriptions				()										{ static const ::gpk::label				results[] = {{			_tArgs::get_member_description	(), (uint32_t)-1}	...,											""	}; static const ::gpk::view_array<const ::gpk::label			> results_view = {results}; return results_view;	}
		static inline constexpr	const ::gpk::view_array<const ::gpk::DATA_TYPE			>&	get_data_type_ids				()							noexcept	{ static const ::gpk::DATA_TYPE			results[] = {			_tArgs::get_member_data_type_id	()					...,					::gpk::DATA_TYPE_UNKNOWN	}; static const ::gpk::view_array<const ::gpk::DATA_TYPE		> results_view = {results}; return results_view;	}
		static inline constexpr	const ::gpk::view_array<const uint32_t					>&	get_member_sizes				()							noexcept	{ static const uint32_t					results[] = {(uint32_t)	_tArgs::get_member_size			()					...,											 0	}; static const ::gpk::view_array<const uint32_t				> results_view = {results}; return results_view;	}
	}; // struct
#	define GPKM_NAMED_REGISTRY(registryName, ...)	typedef ::gpk::member_registry<__VA_ARGS__> TRegistry;		static	const TRegistry&	get_member_registry	()	{ static const TRegistry registryName; return registryName;	}
#pragma pack(pop)
} // namespace 

#define	GPKMNDO(_memberType, _memberName)				GPKM(_memberType, _memberName, ::gpk::DATA_TYPE_OBJECT, #_memberName, #_memberName)
#define	GPKMNDF(_nameSpace, _memberType, _memberName, _dataTypeId)				GPKM(_nameSpace, _memberType, _memberName, _dataTypeId, #_memberName, #_memberName)
#define GPKM_REGISTRY(...)								GPKM_NAMED_REGISTRY(GPKM_REGISTRY_NAME, __VA_ARGS__)
#define GPKM_GET_MEMBER_REGISTRY(objectType)			objectType::get_member_registry()

#endif // GPK_MEMBER_REGISTRY_H_9214982364982734
