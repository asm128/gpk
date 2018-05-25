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

	::gpk::STypeRegistry&							typeRegistrySingleton					()	noexcept;

	template<typename _t>
	struct STypeRegistrator {

		operator										int										()			{ return 0; }

														STypeRegistrator						()			{
			::gpk::STypeRegistry								& typeRegistry							= typeRegistrySingleton();
			typeRegistry.Names.push_back({_t::gpk_type_name(), _t::gpk_type_name_len()});
			typeRegistry.MemberCount.push_back(0);
		}
	};


#define GPK_BEGIN_POD(_structName)	struct _structName {																																					\
										static constexpr	const char*			gpk_type_name		()		noexcept	{ static constexpr const char name[] = #_structName; return name; }					\
										static constexpr	uint32_t			gpk_type_name_len	()		noexcept	{ static constexpr const char name[] = #_structName; return ::gpk::size(name) - 1; }

#define GPK_END_POD(_structName)	}; \
	static	const int									gpk_tr_##_structName		= ::gpk::STypeRegistrator<_structName>();


}

#endif // GPK_TYPE_REGISTRY_H_9028347293847
