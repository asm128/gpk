#include "gpk_log.h"
#include "gpk_cgi_runtime.h"
#include "gpk_module.h"
#include "gpk_array.h"
#include "gpk_member_registry.h"

#ifndef GPK_RUNTIME_MODULE_H_23084728934
#define GPK_RUNTIME_MODULE_H_23084728934

#if !defined(GPK_WINDOWS)
#	include <dlfcn.h>
#endif

GPK_DECLARE_MODULE_FUNCTION(gpk_moduleTitle		, char_t* outputBuffer, uint8_t* outputBufferLength				);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleVersion	, uint32_t* version												);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleCreate	, void** customApplication, ::gpk::SCGIFramework * framework	);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleDelete	, void** customApplication										);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleSetup		, void*	 customApplication										);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleCleanup	, void*	 customApplication										);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleRender	, void*	 customApplication, ::gpk::array_pod<char_t> & output	);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleUpdate	, void*	 customApplication, bool requestedExit					);

namespace gpk
{
#pragma pack(push, 1)
	struct SCGIModule : public SModuleBase {
		GPKMFND(, gpk_moduleTitle	, Title		)	= 0;
		GPKMFND(, gpk_moduleVersion	, Version	)	= 0;
		GPKMFND(, gpk_moduleCreate	, Create	)	= 0;
		GPKMFND(, gpk_moduleDelete	, Delete	)	= 0;
		GPKMFND(, gpk_moduleSetup	, Setup		)	= 0;
		GPKMFND(, gpk_moduleCleanup	, Cleanup	)	= 0;
		GPKMFND(, gpk_moduleRender	, Render	)	= 0;
		GPKMFND(, gpk_moduleUpdate	, Update	)	= 0;

		void											* Application;
		GPKM_REGISTRY
			(	GPKM_NAME(Title		)
			,	GPKM_NAME(Version	)
			,	GPKM_NAME(Create	)
			,	GPKM_NAME(Delete	)
			,	GPKM_NAME(Setup		)
			,	GPKM_NAME(Cleanup	)
			,	GPKM_NAME(Render	)
			,	GPKM_NAME(Update	)
			);
	};
#pragma pack(pop)

	::gpk::error_t									loadCGIModule				(::gpk::SCGIModule& loadedModule, const ::gpk::view_const_char& moduleName);


}

#endif // GPK_RUNTIME_MODULE_H_23084728934
