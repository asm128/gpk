#include "gpk_type_registry.h"

		::gpk::STypeRegistry&				typeRegistrySingleton				()			{
	static ::gpk::STypeRegistry						singletonTypeRegistry;
	return singletonTypeRegistry;	
}


