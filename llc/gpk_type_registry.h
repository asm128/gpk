#include "gpk_array.h"

#ifndef GPK_TYPE_REGISTRY_H_23627
#define GPK_TYPE_REGISTRY_H_23627

namespace gpk
{
	struct STypeRegistry {
		::gpk::avcc				Names;
		::gpk::au32				MemberCount;

		static	STypeRegistry&	GetGlobalInstance	()	noexcept	{
			static STypeRegistry		instance;
			return instance;
		}
	};

	tplt<tpnm _t>
	struct STypeRegistrator {
		operator				int					()	{ return 0; }

								STypeRegistrator	()	{
			::gpk::STypeRegistry		& typeRegistry		= ::gpk::STypeRegistry::GetGlobalInstance();
			typeRegistry.Names		.push_back({_t::gpk_type_name(), _t::gpk_type_name_len()});
			typeRegistry.MemberCount.push_back(0);
		}
	};

#define GPK_BEGIN_POD(_structName)	struct _structName {

#define GPK_END_POD(_structName)																									\
	public:																															\
		stincxp	::gpk::vcc	gpk_type_name			()		noexcept	{ return {::gpk::size(#_structName) - 1, #_structName}; }	\
		stincxp	uint32_t	gpk_type_name_len		()		noexcept	{ return ::gpk::size(#_structName) - 1; }					\
	};																																\
	static	const int	gpk_tr_##_structName	= ::gpk::STypeRegistrator<_structName>();

#define GPK_STRUCT_MEMBER_DESC_NAME(_name)	_gpk_dsc_##_name

#define GPK_STRUCT_MEMBER(_type, _name, ...)																			\
	struct GPK_STRUCT_MEMBER_DESC_NAME(_name) {																			\
		stincxp	::gpk::vcc	gpk_member_type_name	()		noexcept	{ return {::gpk::size(#_type) - 1, #_type}; }	\
		stincxp	::gpk::vcc	gpk_member_name			()		noexcept	{ return {::gpk::size(#_name) - 1, #_name}; }	\
	};																													\
	_type				_name
}

#endif // GPK_TYPE_REGISTRY_H_23627
