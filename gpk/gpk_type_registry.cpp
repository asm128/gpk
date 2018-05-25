#include "gpk_type_registry.h"

		::gpk::STypeRegistry&				gpk::typeRegistrySingleton				()		noexcept	{
	static ::gpk::STypeRegistry						singletonTypeRegistry;
	return singletonTypeRegistry;	
}


