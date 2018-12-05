#include "gpk_label.h"
#include "gpk_array.h"

#ifndef GPK_TYPE_REGISTRY_H_9028347293847
#define GPK_TYPE_REGISTRY_H_9028347293847

namespace gpk
{
	struct STypeRegistry {
		::gpk::array_pod<::gpk::label>					Names;
		::gpk::array_pod<uint32_t>						MemberCount;
	};			

	::gpk::STypeRegistry&							typeRegistrySingleton					()		noexcept;

	template<typename _t>
	struct STypeRegistrator {

		operator										int										()					{ return 0; }

														STypeRegistrator						()					{
			::gpk::STypeRegistry								& typeRegistry							= ::gpk::typeRegistrySingleton();
			typeRegistry.Names		.push_back({_t::gpk_type_name(), _t::gpk_type_name_len()});
			typeRegistry.MemberCount.push_back(0);
		}
	};

#define GPK_BEGIN_POD(_structName)	struct _structName {

#define GPK_END_POD(_structName)																																											\
	public:																																																	\
		static constexpr	const char*					gpk_type_name							()		noexcept	{ static constexpr const char name[] = #_structName; return name; }						\
		static constexpr	uint32_t					gpk_type_name_len						()		noexcept	{ static constexpr const char name[] = #_structName; return ::gpk::size(name) - 1; }	\
	};																																																		\
	static				const int					gpk_tr_##_structName					= ::gpk::STypeRegistrator<_structName>();

#define GPK_STRUCT_MEMBER_DESC_NAME(_name)	_gpk_dsc_##_name

#define GPK_STRUCT_MEMBER(_type, _name, ...)																																								\
	struct GPK_STRUCT_MEMBER_DESC_NAME(_name) {																																						\
		static constexpr	const char*					gpk_member_type_name					()		noexcept	{ static constexpr const char	name[]	= #_type; return name; }					\
		static constexpr	const char*					gpk_member_type_name_label				()		noexcept	{ static const ::gpk::label		name	= #_name; return name; }					\
		static constexpr	uint32_t					gpk_member_type_name_len				()		noexcept	{ static constexpr const char	name[]	= #_type; return ::gpk::size(name) - 1; }	\
		static constexpr	const char*					gpk_member_name							()		noexcept	{ static constexpr const char	name[]	= #_name; return name; }					\
		static constexpr	const char*					gpk_member_name_label					()		noexcept	{ static const ::gpk::label		name	= #_name; return name; }					\
		static constexpr	uint32_t					gpk_member_name_len						()		noexcept	{ static constexpr const char	name[]	= #_name; return ::gpk::size(name) - 1; }	\
	};																																																\
						_type						_name
}

#endif // GPK_TYPE_REGISTRY_H_9028347293847
