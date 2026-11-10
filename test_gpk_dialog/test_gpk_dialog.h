#include "gpk_framework.h"
#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control_list.h"
#include "gpk_gui_inputbox.h"
#include "gpk_index_path.h"

#include <mutex>
#include <atomic>
#include <thread>

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627

namespace gme
{
	struct SExplorerFolder {
		::gpk::string					Path					= {};
		::gpk::SPathTree				Tree					= {};
		::gpk::SControlList			Controls				= {};
		::gpk::aobj<::gpk::string>	Labels					= {};
		::gpk::au2_t					ChildFolder			= {};
		::gpk::u2_t					Depth					= 0;
		bool							Loaded					= false;
		bool							Loading					= false;
		bool							Expanded				= false;
		::std::atomic<::gpk::err_t>		ScanResult				= INT_MAX;
		::std::thread					Worker					= {};
	};

	struct SExplorer {
		::gpk::apobj<SExplorerFolder>	Folder					= {};
		::gpk::acid					RowControl				= {};
		::gpk::au2_t					RowFolder				= {};
		::gpk::au2_t					RowEntry					= {};
		::gpk::au2_t					VisibleFolder			= {};
		::gpk::au2_t					VisibleEntry				= {};
	};

	struct SSearch {
		::gpk::SPathTree				Tree					= {};
		::gpk::SPathIndexData			Data					= {};
		::gpk::aobj<::gpk::SWordIndices>	Word				= {};
		::gpk::aobj<::gpk::vcst_t>		Reverse					= {};
		::gpk::au2_t					Result					= {};
		::gpk::SControlList			Controls				= {};
		::gpk::apobj<::gpk::string>		Labels					= {};
		::gpk::string					Query					= {};
		bool							Ready					= false;
		bool							Published				= false;
		::std::atomic<::gpk::err_t>		BuildResult				= INT_MAX;
		::std::thread					Worker					= {};
	};

	struct SApplication {
		::gpk::SFramework				Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen				= {};
		::std::mutex					LockRender;

		::gpk::SDialog					DialogMain				= {};
		::gpk::SInputBox				InputBox				= {};
		::gpk::pobj<::gpk::SDialogSlider>	ScrollY				= {};
		::gpk::cid_t					Viewport				= -1;
		::gpk::cid_t					IdExit					= -1;

		SExplorer						Explorer				= {};
		SSearch						Search					= {};
		::gpk::string					RootPath				= {};
		::gpk::string					ViewportTitle			= {};
		::gpk::s3_t					ScrollOffset			= 0;
		::gpk::u2_t					PageSize				= 1;

										SApplication				(::gpk::SRuntimeValues & runtimeValues) : Framework(runtimeValues) {}
	};
}

#endif // APPLICATION_H_23627
