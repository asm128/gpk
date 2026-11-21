#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control_list.h"
#include "gpk_path.h"

#include <mutex>

#ifndef GPK_FILE_EXPLORER_H_2078934982734
#define GPK_FILE_EXPLORER_H_2078934982734

namespace gfe
{
	stxp ::gpk::u2_t JSON_ROW_COUNT = 18;

	struct SJSONRow {
		::gpk::string					Type								= {};
		::gpk::string					Name								= {};
		::gpk::string					Value							= {};
	};

	struct SApplication {
		::gpk::SFramework				Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen							= {};
		::gpk::SDialog					DialogMain							= {};

		::gpk::aobj<::gpk::vcst_t>		SourceLabels						= {};
		::gpk::aobj<::gpk::vcst_t>		SourcePaths							= {};
		::gpk::aobj<SJSONRow>			Rows								= {};
		::gpk::string					Status							= {};

		::gpk::acid						IdSources							= {};
		::gpk::acid						IdTypes								= {};
		::gpk::acid						IdNames								= {};
		::gpk::acid						IdValues							= {};

		::gpk::cid_t					IdRoot								= -1;
		::gpk::cid_t					IdTitle							= -1;
		::gpk::cid_t					IdSourcesPanel						= -1;
		::gpk::cid_t					IdTypePanel							= -1;
		::gpk::cid_t					IdNamePanel							= -1;
		::gpk::cid_t					IdValuePanel						= -1;
		::gpk::cid_t					IdStatus							= -1;
		::gpk::cid_t					IdPrevious							= -1;
		::gpk::cid_t					IdNext								= -1;
		::gpk::cid_t					IdExit								= -1;
		::gpk::u2_t					SelectedSource						= 0;
		::gpk::u2_t					RowOffset							= 0;

		::gpk::mutex					LockGUI;
		::gpk::mutex					LockRender;

										SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues, "gpk_file_explorer.json"), DialogMain(Framework.GUI)	{}
	};
} // namespace gpk_file_explorer

#endif // GPK_FILE_EXPLORER_H_2078934982734
