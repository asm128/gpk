#include "gpk_framework.h"
#include "gpk_gui.h"

#include <mutex>

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework				Framework							;
		::gpk::img8bgra					TextureFont							= {};
		::gpk::img8bgra					VerticalAtlas						;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen							= {};

		int16_t							Palettes[::gpk::GUI_CONTROL_PALETTE_COUNT]	= {};

		::gpk::cid_t					IdExit								= -1;
		::gpk::cid_t					IdMode								= -1;
		::gpk::cid_t					IdTheme								= -1;
		::gpk::cid_t					IdNewPalette						= -1;
		::gpk::cid_t					PaletteRow							= 10;
		::gpk::cid_t					PaletteColumn						= 16;
		char							RowText	[64][3]						=
			{ "00"
			, "01"
			, "02"
			, "03"
			, "04"
			, "05"
			, "06"
			, "07"
			, "08"
			, "09"
			, "10"
			, "11"
			, "12"
			, "13"
			, "14"
			, "15"
			, "16"
			, "17"
			, "18"
			, "19"
			, "20"
			, "21"
			, "22"
			, "23"
			, "24"
			, "25"
			, "26"
			, "27"
			, "28"
			, "29"
			, "30"
			, "31"
			, "32"
			, "33"
			, "34"
			, "35"
			, "36"
			, "37"
			, "38"
			, "39"
			, "40"
			, "41"
			, "42"
			, "43"
			, "44"
			, "45"
			, "46"
			, "47"
			, "48"
			, "49"
			, "50"
			, "51"
			, "52"
			, "53"
			, "54"
			, "55"
			, "56"
			, "57"
			, "58"
			, "59"
			, "60"
			, "61"
			, "62"
			, "63"
			//, "64"
			//, "65"
			//, "66"
			//, "67"
			//, "68"
			//, "69"
			//, "70"
			//, "71"
			//, "72"
			//, "73"
			//, "74"
			//, "75"
			//, "76"
			//, "77"
			//, "78"
			//, "79"
			};


		::std::mutex	LockGUI;
		::std::mutex	LockRender;

						SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_23627
