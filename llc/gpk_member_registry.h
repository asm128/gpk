/// Copyright 2016-2017 - asm128
#include "gpk_type_identifier.h"
#include "gpk_datatype.h"
#include "gpk_label.h"
#include "gpk_module.h"

#ifndef GPK_MEMBER_REGISTRY_H_23627
#define GPK_MEMBER_REGISTRY_H_23627

#define GPKM_REGISTRY_NAME		gpk_member_registry
#define GPKM_NAME(_memberName)	_gpk_info_##_memberName##_	// This mangles the member name for use as the type name.

#define GPKM(_nameSpace, _memberType, _memberName, _dataTypeId, _displayName, _memberDescription)				\
	struct GPKM_NAME(_memberName)	{						\
		typedef _nameSpace::_memberType		TMember;	\
		sinx	::gpk::vcst_t				get_member_namespace		()	nxpt	{ rtrn #_nameSpace;						}	\
		sinx	::gpk::vcst_t				get_member_type_name		()	nxpt	{ rtrn #_memberType;					}	\
		sinx	::gpk::vcst_t				get_member_name				()	nxpt	{ rtrn #_memberName;					}	\
		sinx	::gpk::vcst_t				get_member_display_name		()	nxpt	{ rtrn _displayName;					}	\
		sinx	::gpk::vcst_t				get_member_description		()	nxpt	{ rtrn _memberDescription;				}	\
		sinx	::gpk::DATA_TYPE			get_member_data_type_id		()	nxpt	{ rtrn _dataTypeId;						}	\
		sinx	u2_t						get_member_size				()	nxpt	{ rtrn szof(_nameSpace::_memberType);	}	\
	};												\
	_nameSpace::_memberType		_memberName

#define GPKFM(_nameSpace, _memberType, _memberName, _dataTypeId, _displayName, _memberDescription)									\
	struct GPKM_NAME(_memberName)	{																								\
		tydf GPK_MODULE_FUNCTION_NAME(_memberType)	TMember;																		\
		sinx	::gpk::vcst_t		get_member_namespace		()	nxpt	{ rtrn #_nameSpace;			}							\
		sinx	::gpk::vcst_t		get_member_type_name		()	nxpt	{ rtrn #_memberType;		}							\
		sinx	::gpk::vcst_t		get_member_name				()	nxpt	{ rtrn #_memberName;		}							\
		sinx	::gpk::vcst_t		get_member_display_name		()	nxpt	{ rtrn _displayName;		}							\
		sinx	::gpk::vcst_t		get_member_description		()	nxpt	{ rtrn _memberDescription;	}							\
		sinx	::gpk::DATA_TYPE	get_member_data_type_id		()	nxpt	{ rtrn _dataTypeId;			}							\
		sinx	::gpk::u2_t			get_member_size				()	nxpt	{ rtrn szof(GPK_MODULE_FUNCTION_NAME(_memberType)); }	\
	};																																\
	GPK_MODULE_FUNCTION_NAME(_memberType)	_memberName

namespace gpk {
#pragma pack(push, 1)
	tplt <tpnm... _tArgs>
	stct member_registry {
		sinx	u2_t						get_member_count	()	nxpt { rtrn (u2_t)sizeof...(_tArgs); }
		sinx	view<cnst STypeIdentifier>	get_types			()	nxpt { stxp cnst STypeIdentifier	results[] = {{_tArgs::get_member_namespace(), _tArgs::get_member_type_name()}..., {}}; stxp cnst view<cnst STypeIdentifier> results_view = {results}; return results_view; }
		sinx	view<vcst_c>				get_names			()	nxpt { stxp cnst vcst_t				results[] = {_tArgs ::get_member_name			()..., {}}; stxp cnst view<vcst_c> results_view = {results}; rtrn results_view;	}
		sinx	view<vcst_c>				get_display_names	()	nxpt { stxp cnst vcst_t				results[] = {_tArgs ::get_member_display_name	()..., {}}; stxp cnst view<vcst_c> results_view = {results}; rtrn results_view;	}
		sinx	view<vcst_c>				get_descriptions	()	nxpt { stxp cnst vcst_t				results[] = {_tArgs ::get_member_description	()..., {}}; stxp cnst view<vcst_c> results_view = {results}; rtrn results_view;	}
		sinx	view<cnst DATA_TYPE>		get_data_type_ids	()	nxpt { stxp cnst DATA_TYPE			results[] = {_tArgs ::get_member_data_type_id	()..., ::gpk::DATA_TYPE_UNKNOWN }; return results; }
		sinx	view<u2_c>					get_member_sizes	()	nxpt { stxp cnst u2_t				results[] = {_tArgs ::get_member_size			()..., 0}; stxp vcu2_c results_view = {results}; return results_view;	}
	}; // struct
#	define GPKM_NAMED_REGISTRY(registryName, ...)	typedef ::gpk::member_registry<__VA_ARGS__> TRegistry;		static	const TRegistry&	get_member_registry	()	{ static const TRegistry registryName; return registryName;	}

	tplt<tpnm _tStruct>
	inline	::gpk::err_t					get_member_registry()	{ return _tStruct::get_member_registry(); }
#pragma pack(pop)
} // namespace

#define	GPKMFND(_nameSpace, _memberType, _memberName)				GPKFM(_nameSpace, _memberType, _memberName, ::gpk::DATA_TYPE_FUN	, #_memberName, #_memberName)
#define	GPKMOND(_nameSpace, _memberType, _memberName)				GPKM(_nameSpace, _memberType, _memberName, ::gpk::DATA_TYPE_OBJ		, #_memberName, #_memberName)
#define	GPKMBND(_nameSpace, _memberType, _memberName, _dataTypeId)	GPKM(_nameSpace, _memberType, _memberName, _dataTypeId	, #_memberName, #_memberName)
#define GPKM_REGISTRY(...)											GPKM_NAMED_REGISTRY(GPKM_REGISTRY_NAME, __VA_ARGS__)
#define GPKM_GET_MEMBER_REGISTRY(objectType)						objectType::get_member_registry()

#endif // GPK_MEMBER_REGISTRY_H_23627
