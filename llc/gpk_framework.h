#include "gpk_gui.h"
#include "gpk_runtime.h"
#include "gpk_window.h"
#include "gpk_input.h"
#include "gpk_timer.h"
#include "gpk_frameinfo.h"
#include "gpk_image.h"
#include "gpk_json_expression.h"
#include "gpk_storage.h"

#ifndef GPK_FRAMEWORK_H_20987347928
#define GPK_FRAMEWORK_H_20987347928

namespace gpk
{

	struct SFrameworkDetail {
#if defined(GPK_WINDOWS)
#elif defined(GPK_XCB)
							xcb_connection_t										* XCBConnection			= {};
							xcb_screen_t											* XCBScreen				= {};

																					SFrameworkDetail		() {
			if(0 == XCBConnection) {
				XCBConnection		= xcb_connect(NULL, NULL);
				gthrow_if(0 == XCBConnection, "%s", "Failed to connect to X Server!");
				XCBScreen			= xcb_setup_roots_iterator(xcb_get_setup(XCBConnection)).data;
			}
		}

																					~SFrameworkDetail		() {
			if(XCBConnection) {
				xcb_disconnect(XCBConnection);
				info_printf("%s.", "XCB disconnected");
			}
		}
#endif
	};

	struct SFrameworkSettings {
							bool													GUIZoom					: 1;
	};

	struct SFramework {
		typedef				::gpk::SColorBGRA										TTexel;
		typedef				::gpk::SRenderTarget<TTexel, uint32_t>					TOffscreen;

							::gpk::SRuntimeValues									& RuntimeValues			;
							::gpk::SFrameworkDetail									PlatformDetail			= {};
							::gpk::SWindow											MainDisplay				= {};
							::gpk::ptr_obj<::gpk::SRenderTarget<TTexel, uint32_t>>	MainDisplayOffscreen	= {};
							::gpk::ptr_obj<::gpk::SInput>							Input					= {};
							::gpk::STimer											Timer					= {};
							::gpk::SFrameInfo										FrameInfo				= {};
							::gpk::ptr_obj<::gpk::SGUI>								GUI						= {};
							::gpk::SFrameworkSettings								Settings				= {1, };
							::gpk::SJSONFile										JSONConfig				= {};
							::gpk::view_const_string								FileNameJSONConfig		= "gpk_config.json";

							::std::mutex											LockGUI;

		inline																		SFramework				(::gpk::SRuntimeValues& runtimeValues, ::gpk::view_const_string	fileNameJSONConfig = "gpk_config.json")			noexcept
			: RuntimeValues(runtimeValues), FileNameJSONConfig(fileNameJSONConfig)
		{
			Input.create();
			MainDisplay.Input = Input;
			GUI.create();
			if(fileNameJSONConfig.size())	// Attempt to load config file.
				rw_if(errored(::gpk::jsonFileRead(JSONConfig, fileNameJSONConfig)), "Failed to load config JSON file! File not found? File name: %s.", fileNameJSONConfig.begin());
		}
	}; // struct

	::gpk::error_t			updateFramework			(::gpk::SFramework& framework);
	::gpk::error_t			mainWindowCreate		(::gpk::SWindow& mainWindow, ::gpk::SRuntimeValuesDetail& runtimeValues, ::gpk::ptr_obj<SInput>& displayInput);
	::gpk::error_t			mainWindowDestroy		(::gpk::SWindow& mainWindow);
} // namespace

#endif // GPK_FRAMEWORK_H_20987347928
