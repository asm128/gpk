#include "gpk_gui.h"
#include "gpk_runtime.h"
#include "gpk_window.h"
#include "gpk_timer.h"
#include "gpk_frameinfo.h"
#include "gpk_json_expression.h"
#include "gpk_file.h"
#include "gpk_parse.h"

#ifndef GPK_ATMEL
#	include <mutex>
#endif

#ifndef GPK_FRAMEWORK_H_23627
#define GPK_FRAMEWORK_H_23627

namespace gpk
{
	struct SFrameworkDetail {
#if defined(GPK_WINDOWS)
#elif defined(GPK_XCB)
		xcb_connection_t			* XCBConnection			= {};
		xcb_screen_t				* XCBScreen				= {};

									~SFrameworkDetail		() {
			if(XCBConnection) {
				xcb_disconnect(XCBConnection);
				info_printf("%s.", "XCB disconnected");
			}
		}

									SFrameworkDetail		() {
			if(0 == XCBConnection) {
				XCBConnection				= xcb_connect(NULL, NULL);
				gthrow_if(0 == XCBConnection, "%s", "Failed to connect to X Server!");
				XCBScreen					= xcb_setup_roots_iterator(xcb_get_setup(XCBConnection)).data;
			}
		}

									
#endif
	};

	struct SFrameworkSettings {
		bool						GUIZoom					: 1;
	};

	struct SFramework {
		::gpk::SRuntimeValues		& RuntimeValues			;
		::gpk::vcs					FileNameJSONConfig		= "gpk_config.json";
		::gpk::pobj<::gpk::SGUI>	GUI						= {};
		::gpk::SFrameworkDetail		PlatformDetail			= {};
		::gpk::SWindow				RootWindow				= {};
		::gpk::STimer				Timer					= {};
		::gpk::SFrameInfo			FrameInfo				= {};
		::gpk::SFrameworkSettings	Settings				= {};
		::gpk::SJSONFile			JSONConfig				= {};

#ifndef GPK_ATMEL
		::std::mutex				LockGUI;
#endif

		inline						SFramework				(::gpk::SRuntimeValues & runtimeValues, ::gpk::vcs fileNameJSONConfig = "gpk_config.json")
			: RuntimeValues(runtimeValues), FileNameJSONConfig(fileNameJSONConfig) {
			RootWindow.Input.create();
			GUI.create();
			if(fileNameJSONConfig.size())	// Attempt to load config file.
				if_if(errored(::gpk::jsonFileRead(JSONConfig, fileNameJSONConfig)), "File name: '%s'.", fileNameJSONConfig.begin());
		}
	}; // struct

	::gpk::error_t				updateFramework			(::gpk::SFramework & framework);
	::gpk::error_t				mainWindowCreate		(::gpk::SWindow & mainWindow, ::gpk::SRuntimeValuesDetail & runtimeValues, ::gpk::pobj<SInput> & displayInput);
	::gpk::error_t				mainWindowDestroy		(::gpk::SWindow & mainWindow);
} // namespace

#endif // GPK_FRAMEWORK_H_23627
