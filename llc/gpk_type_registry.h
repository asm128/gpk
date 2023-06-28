#include "gpk_array.h"

#ifndef GPK_TYPE_REGISTRY_H_23627
#define GPK_TYPE_REGISTRY_H_23627

namespace gpk
{
	struct STypeRegistry {
		::gpk::apod<::gpk::vcc>				Names;
		::gpk::au32				MemberCount;
	};

	::gpk::STypeRegistry&				typeRegistrySingleton					()		noexcept;

	template<typename _t>
	struct STypeRegistrator {

		operator							int										()					{ return 0; }

											STypeRegistrator						()					{
			::gpk::STypeRegistry					& typeRegistry							= ::gpk::typeRegistrySingleton();
			typeRegistry.Names		.push_back({_t::gpk_type_name(), _t::gpk_type_name_len()});
			typeRegistry.MemberCount.push_back(0);
		}
	};

#define GPK_BEGIN_POD(_structName)	struct _structName {

#define GPK_END_POD(_structName)							\
	public:											\
		stacxpr	const char*					gpk_type_name							()		noexcept	{ stacxpr	const char name[] = #_structName; return name; }						\
		stacxpr	uint32_t					gpk_type_name_len						()		noexcept	{ stacxpr	const char name[] = #_structName; return ::gpk::size(name) - 1; }	\
	};												\
	static	const int					gpk_tr_##_structName					= ::gpk::STypeRegistrator<_structName>();

#define GPK_STRUCT_MEMBER_DESC_NAME(_name)	_gpk_dsc_##_name

#define GPK_STRUCT_MEMBER(_type, _name, ...)				\
	struct GPK_STRUCT_MEMBER_DESC_NAME(_name) {																			\
		stacxpr	::gpk::vcc					gpk_member_type_name					()		noexcept	{ constexpr ::gpk::vcc name = #_type; return name; }					\
		stacxpr	const char*					gpk_member_name							()		noexcept	{ constexpr ::gpk::vcc name = #_name; return name; }					\
	};										\
	_type								_name
}

#endif // GPK_TYPE_REGISTRY_H_23627
