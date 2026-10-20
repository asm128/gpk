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

GPK_DECLARE_MODULE_FUNCTION(gpk_moduleTitle		, ::gpk::sc_t* outputBuffer, ::gpk::u0_t* outputBufferLength		);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleVersion	, ::gpk::u2_t* version												);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleCreate	, void*	* customApplication, ::gpk::SRuntimeValues* runtimeValues	);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleDelete	, void*	* customApplication											);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleSetup		, void	* customApplication											);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleCleanup	, void	* customApplication											);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleRender	, void	* customApplication											);
GPK_DECLARE_MODULE_FUNCTION(gpk_moduleUpdate	, void	* customApplication, bool requestedExit						);

namespace gpk
{
#pragma pack(push, 1)
	struct SRuntimeModule : public SModuleBase {
		GPKMFND(, gpk_moduleTitle	, Title		)	= 0;
		GPKMFND(, gpk_moduleVersion	, Version	)	= 0;
		GPKMFND(, gpk_moduleCreate	, Create	)	= 0;
		GPKMFND(, gpk_moduleDelete	, Delete	)	= 0;
		GPKMFND(, gpk_moduleSetup	, Setup		)	= 0;
		GPKMFND(, gpk_moduleCleanup	, Cleanup	)	= 0;
		GPKMFND(, gpk_moduleRender	, Render	)	= 0;
		GPKMFND(, gpk_moduleUpdate	, Update	)	= 0;

		void	* Application						= 0;
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

	err_t	loadRuntimeModule	(SRuntimeModule & loadedModule, cnst vcst_t & moduleName);
	err_t	unloadRuntimeModule	(SRuntimeModule & loadedModule);

	GDEFINE_ENUM_TYPE (RUNTIME_CALLBACK_ID, u0_t);
	GDEFINE_ENUM_VALUE(RUNTIME_CALLBACK_ID, NONE	, 0		);
	GDEFINE_ENUM_VALUE(RUNTIME_CALLBACK_ID, TITLE	, 0x01	);
	GDEFINE_ENUM_VALUE(RUNTIME_CALLBACK_ID, VERSION	, 0x02	);
	GDEFINE_ENUM_VALUE(RUNTIME_CALLBACK_ID, CREATE	, 0x04	);
	GDEFINE_ENUM_VALUE(RUNTIME_CALLBACK_ID, DELETE	, 0x08	);
	GDEFINE_ENUM_VALUE(RUNTIME_CALLBACK_ID, SETUP	, 0x10	);
	GDEFINE_ENUM_VALUE(RUNTIME_CALLBACK_ID, CLEANUP	, 0x20	);
	GDEFINE_ENUM_VALUE(RUNTIME_CALLBACK_ID, RENDER	, 0x40	);
	GDEFINE_ENUM_VALUE(RUNTIME_CALLBACK_ID, UPDATE	, 0x80	);

	GDEFINE_ENUM_TYPE (APPLICATION_STATE, u0_t);
	GDEFINE_ENUM_VALUE(APPLICATION_STATE, NORMAL	, 0);
	GDEFINE_ENUM_VALUE(APPLICATION_STATE, EXIT		, 1);
	GDEFINE_ENUM_VALUE(APPLICATION_STATE, BUSY		, 2);
	GDEFINE_ENUM_VALUE(APPLICATION_STATE, STAY		, 3);
}

#define GPK_DEFINE_APP_MODULE_INTERFACE(_mainClass, _moduleTitle)																	\
	::gpk::err_t	setup		(_mainClass& app);																					\
	::gpk::err_t	cleanup		(_mainClass& app);																					\
	::gpk::err_t	update		(_mainClass& app, bool systemRequestedExit);														\
	::gpk::err_t	draw		(_mainClass& app);																					\
	::gpk::err_t	GPK_STDCALL	gpk_moduleVersion	()																{ return 1; }	\
	::gpk::err_t	GPK_STDCALL	gpk_moduleCreate	(void*	* instanceApp, ::gpk::SRuntimeValues* runtimeValues)	{ try { *instanceApp = new _mainClass{*runtimeValues};											return 0;		} catch(...) {} return -1; }	\
	::gpk::err_t	GPK_STDCALL	gpk_moduleDelete	(void*	* instanceApp)											{ try { delete ((_mainClass*)*instanceApp);	*instanceApp = 0;									return 0;		} catch(...) {} return -1; }	\
	::gpk::err_t	GPK_STDCALL	gpk_moduleSetup		(void	* instanceApp)											{ try { const ::gpk::err_t result = setup		(*(_mainClass*)instanceApp);					return result;	} catch(...) {} return -1; }	\
	::gpk::err_t	GPK_STDCALL	gpk_moduleCleanup	(void	* instanceApp)											{ try { const ::gpk::err_t result = cleanup	(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
	::gpk::err_t	GPK_STDCALL	gpk_moduleUpdate	(void	* instanceApp, bool systemRequestedExit)				{ try { const ::gpk::err_t result = update	(*(_mainClass*)instanceApp, systemRequestedExit);	return result;	} catch(...) {} return -1; }	\
	::gpk::err_t	GPK_STDCALL	gpk_moduleRender	(void	* instanceApp)											{ try { const ::gpk::err_t result = draw	(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
	::gpk::err_t	GPK_STDCALL	gpk_moduleTitle		(::gpk::sc_t * out_title, ::gpk::u2_t *maxCount)					{	\
		stxp	::gpk::sc_c			mylmoduleTitle[] = _moduleTitle;													\
		if(0 == out_title) 																								\
			return maxCount ? (*maxCount = ::gpk::size(mylmoduleTitle)) : ::gpk::size(mylmoduleTitle);					\
		memcpy(out_title, mylmoduleTitle, ::gpk::min(::gpk::size(mylmoduleTitle), *maxCount));							\
		return 0;																										\
	} //

#endif // GPK_RUNTIME_MODULE_H_23627


