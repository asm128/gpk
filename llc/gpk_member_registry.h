/// Copyright 2016-2017 - asm128
#include "gpk_type_identifier.h"
#include "gpk_datatype.h"
#include "gpk_view.h"
#include "gpk_module.h"

#ifndef GPK_MEMBER_REGISTRY_H_23627
#define GPK_MEMBER_REGISTRY_H_23627

#define GPKM_REGISTRY_NAME		gpk_member_registry
#define GPKM_NAME(_memberName)	_gpk_info_##_memberName##_	// This mangles the member name for use as the type name.

#define GPKM(_nameSpace, _memberType, _memberName, _dataTypeId, _displayName, _memberDescription)				\
	struct GPKM_NAME(_memberName)	{						\
		typedef _nameSpace::_memberType			TMember;	\
		stincxp	const sc_t*										get_member_namespace			()							noexcept	{ return #_nameSpace;						}	\
		stincxp	const sc_t*										get_member_type_name			()							noexcept	{ return #_memberType;						}	\
		stincxp	const sc_t*										get_member_name					()							noexcept	{ return #_memberName;						}	\
		stincxp	const sc_t*										get_member_display_name			()							noexcept	{ return _displayName;						}	\
		stincxp	const sc_t*										get_member_description			()							noexcept	{ return _memberDescription;				}	\
		stincxp	::gpk::DATA_TYPE									get_member_data_type_id			()							noexcept	{ return _dataTypeId;						}	\
		stincxp	uint32_t											get_member_size					()							noexcept	{ return sizeof(_nameSpace::_memberType);	}	\
	};												\
	_nameSpace::_memberType		_memberName

#define GPKFM(_nameSpace, _memberType, _memberName, _dataTypeId, _displayName, _memberDescription)				\
	struct GPKM_NAME(_memberName)	{						\
		typedef GPK_MODULE_FUNCTION_NAME(_memberType)		TMember;	\
		stincxp	const sc_t*										get_member_namespace			()							noexcept	{ return #_nameSpace;						}	\
		stincxp	const sc_t*										get_member_type_name			()							noexcept	{ return #_memberType;						}	\
		stincxp	const sc_t*										get_member_name					()							noexcept	{ return #_memberName;						}	\
		stincxp	const sc_t*										get_member_display_name			()							noexcept	{ return _displayName;						}	\
		stincxp	const sc_t*										get_member_description			()							noexcept	{ return _memberDescription;				}	\
		stincxp	::gpk::DATA_TYPE									get_member_data_type_id			()							noexcept	{ return _dataTypeId;						}	\
		stincxp	uint32_t											get_member_size					()							noexcept	{ return sizeof(GPK_MODULE_FUNCTION_NAME(_memberType));	}	\
	};												\
	GPK_MODULE_FUNCTION_NAME(_memberType)		_memberName

namespace gpk {
#pragma pack(push, 1)
	tplt <tpnm... _tArgs>	struct member_registry {
		stincxp	const uint32_t										get_member_count				()							noexcept	{ return (uint32_t)sizeof...(_tArgs);							}
		stainli	const ::gpk::view<const ::gpk::STypeIdentifier	>&	get_types						()										{ static const ::gpk::STypeIdentifier	results[] = {{{			_tArgs::get_member_namespace(), (uint32_t)-1}, {_tArgs::get_member_type_name(), (uint32_t)-1}}...,	{}	}; static const ::gpk::view<const ::gpk::STypeIdentifier	> results_view = {results}; return results_view;	}
		stainli	const ::gpk::view<const ::gpk::label			>&	get_names						()										{ static const ::gpk::label				results[] = {{			_tArgs::get_member_name			(), (uint32_t)-1}	...,											""	}; static const ::gpk::view<const ::gpk::label			> results_view = {results}; return results_view;	}
		stainli	const ::gpk::view<const ::gpk::label			>&	get_display_names				()										{ static const ::gpk::label				results[] = {{			_tArgs::get_member_display_name	(), (uint32_t)-1}	...,											""	}; static const ::gpk::view<const ::gpk::label			> results_view = {results}; return results_view;	}
		stainli	const ::gpk::view<const ::gpk::label			>&	get_descriptions				()										{ static const ::gpk::label				results[] = {{			_tArgs::get_member_description	(), (uint32_t)-1}	...,											""	}; static const ::gpk::view<const ::gpk::label			> results_view = {results}; return results_view;	}
		stainli	const ::gpk::view<const ::gpk::DATA_TYPE		>	get_data_type_ids				()							noexcept	{ static const ::gpk::DATA_TYPE			results[] = {			_tArgs::get_member_data_type_id	()					...,					::gpk::DATA_TYPE_UNKNOWN	}; return results; }
		stainli	const ::gpk::view<const uint32_t				>&	get_member_sizes				()							noexcept	{ static const uint32_t					results[] = {(uint32_t)	_tArgs::get_member_size			()					...,											 0	}; static const ::gpk::view<const uint32_t				> results_view = {results}; return results_view;	}
	}; // struct
#	define GPKM_NAMED_REGISTRY(registryName, ...)	typedef ::gpk::member_registry<__VA_ARGS__> TRegistry;		static	const TRegistry&	get_member_registry	()	{ static const TRegistry registryName; return registryName;	}

	tplt<tpnm _tStruct>
	inline	::gpk::error_t					get_member_registry()	{ return _tStruct::get_member_registry(); }
#pragma pack(pop)
} // namespace

#define	GPKMFND(_nameSpace, _memberType, _memberName)				GPKFM(_nameSpace, _memberType, _memberName, ::gpk::DATA_TYPE_FUN	, #_memberName, #_memberName)
#define	GPKMOND(_nameSpace, _memberType, _memberName)				GPKM(_nameSpace, _memberType, _memberName, ::gpk::DATA_TYPE_OBJ		, #_memberName, #_memberName)
#define	GPKMBND(_nameSpace, _memberType, _memberName, _dataTypeId)	GPKM(_nameSpace, _memberType, _memberName, _dataTypeId	, #_memberName, #_memberName)
#define GPKM_REGISTRY(...)											GPKM_NAMED_REGISTRY(GPKM_REGISTRY_NAME, __VA_ARGS__)
#define GPKM_GET_MEMBER_REGISTRY(objectType)						objectType::get_member_registry()

#endif // GPK_MEMBER_REGISTRY_H_23627
