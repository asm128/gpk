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
		stincxp	::gpk::u2_t											get_member_size					()							noexcept	{ return sizeof(_nameSpace::_memberType);	}	\
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
		stincxp	::gpk::u2_t											get_member_size					()							noexcept	{ return sizeof(GPK_MODULE_FUNCTION_NAME(_memberType));	}	\
	};												\
	GPK_MODULE_FUNCTION_NAME(_memberType)		_memberName

namespace gpk {
#pragma pack(push, 1)
	tplt <tpnm... _tArgs>	struct member_registry {
		stincxp	const ::gpk::u2_t						get_member_count				()							noexcept	{ return (::gpk::u2_t)sizeof...(_tArgs); }
		stainli	const view<const STypeIdentifier	>&	get_types						()										{ static const STypeIdentifier	results[] = {{{_tArgs::get_member_namespace(), (::gpk::u2_t)-1}, {_tArgs::get_member_type_name(), (::gpk::u2_t)-1}}...,	{}}; static const view<const STypeIdentifier	> results_view = {get_member_count(), results}; return results_view; }
		stainli	const view<const label			>&	get_names						()										{ static const label				results[] = {{_tArgs::get_member_name			(), (::gpk::u2_t)-1}...,	""}; static const view<const label			> results_view = {get_member_count(), results}; return results_view; }
		stainli	const view<const label			>&	get_display_names				()										{ static const label				results[] = {{_tArgs::get_member_display_name	(), (::gpk::u2_t)-1}...,	""}; static const view<const label			> results_view = {get_member_count(), results}; return results_view; }
		stainli	const view<const label			>&	get_descriptions				()										{ static const label				results[] = {{_tArgs::get_member_description	(), (::gpk::u2_t)-1}...,	""}; static const view<const label			> results_view = {get_member_count(), results}; return results_view; }
		stainli	const view<const DATA_TYPE		>	get_data_type_ids				()							noexcept	{ static const DATA_TYPE			results[] = {_tArgs::get_member_data_type_id	()...,	DATA_TYPE_UNKNOWN}; return {get_member_count(), results}; }
		stainli	const view<const ::gpk::u2_t		>&	get_member_sizes				()							noexcept	{ static const ::gpk::u2_t				results[] = {(::gpk::u2_t)_tArgs::get_member_size	()...,	0}; static const view<const ::gpk::u2_t				> results_view = {get_member_count(), results}; return results_view; }
	}; // struct
#	define GPKM_NAMED_REGISTRY(registryName, ...)	typedef ::gpk::member_registry<__VA_ARGS__> TRegistry;		static	const TRegistry&	get_member_registry	()	{ static const TRegistry registryName; return registryName;	}

	tplt<tpnm _tStruct>
	inline	auto &						get_member_registry()	{ return _tStruct::get_member_registry(); }
#pragma pack(pop)
} // namespace

#define	GPKMFND(_nameSpace, _memberType, _memberName)				GPKFM(_nameSpace, _memberType, _memberName, ::gpk::DATA_TYPE_FUN	, #_memberName, #_memberName)
#define	GPKMOND(_nameSpace, _memberType, _memberName)				GPKM(_nameSpace, _memberType, _memberName, ::gpk::DATA_TYPE_OBJ		, #_memberName, #_memberName)
#define	GPKMBND(_nameSpace, _memberType, _memberName, _dataTypeId)	GPKM(_nameSpace, _memberType, _memberName, _dataTypeId	, #_memberName, #_memberName)
#define GPKM_REGISTRY(...)											GPKM_NAMED_REGISTRY(GPKM_REGISTRY_NAME, __VA_ARGS__)
#define GPKM_GET_MEMBER_REGISTRY(objectType)						objectType::get_member_registry()

#endif // GPK_MEMBER_REGISTRY_H_23627


