#include "gpk_log.h"
#include "gpk_runtime.h"
#include "gpk_module.h"
#include "gpk_view.h"
#include "gpk_member_registry.h"

#ifndef GPK_RUNTIME_MODULE_H_23627
#define GPK_RUNTIME_MODULE_H_23627

#if defined(GPK_LINUX) || defined(GPK_ANDROID)
#	include <dlfcn.h>
#endif

using ::gpk::sc_t;

GPK_DECLARE_MODULE_FUNCTION(gpk_moduleTitle		, sc_t* outputBuffer, uint8_t* outputBufferLength					);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleVersion	, uint32_t* version													);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleCreate	, void** customApplication, ::gpk::SRuntimeValues* runtimeValues	);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleDelete	, void** customApplication											);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleSetup		, void*	 customApplication											);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleCleanup	, void*	 customApplication											);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleRender	, void*	 customApplication											);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleUpdate	, void*	 customApplication, bool requestedExit						);

namespace gpk
{
#pragma pack(push, 1)
	struct SRuntimeModule : public SModuleBase {
		GPKMFND(, gpk_moduleTitle	, Title		)				= 0;
		GPKMFND(, gpk_moduleVersion	, Version	)				= 0;
		GPKMFND(, gpk_moduleCreate	, Create	)				= 0;
		GPKMFND(, gpk_moduleDelete	, Delete	)				= 0;
		GPKMFND(, gpk_moduleSetup	, Setup		)				= 0;
		GPKMFND(, gpk_moduleCleanup	, Cleanup	)				= 0;
		GPKMFND(, gpk_moduleRender	, Render	)				= 0;
		GPKMFND(, gpk_moduleUpdate	, Update	)				= 0;

		void													* Application					= 0;
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

	::gpk::error_t					loadRuntimeModule				(::gpk::SRuntimeModule& loadedModule, const ::gpk::vcs& moduleName);
}

#endif // GPK_RUNTIME_MODULE_H_23627
