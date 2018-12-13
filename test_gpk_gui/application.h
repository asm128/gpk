#include "gpk_framework.h"
#include "gpk_gui.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
					::gpk::SFramework														Framework							;
					::gpk::SImage<::gpk::SColorBGRA>										TextureFont							= {};
					::gpk::SImage<::gpk::SColorBGRA>										VerticalAtlas						;
					::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		Offscreen							= {};

					int32_t																	Palettes[::gpk::GUI_CONTROL_PALETTE_COUNT]	= {};

					int32_t																	IdExit								= -1;
					int32_t																	IdMode								= -1;
					int32_t																	IdTheme								= -1;
					int32_t																	IdNewPalette						= -1;
					int32_t																	PaletteRow							= 10;
					int32_t																	PaletteColumn						= 16;
					char																	RowText	[64][3]						=
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


					::std::mutex															LockGUI;
					::std::mutex															LockRender;

																							SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
