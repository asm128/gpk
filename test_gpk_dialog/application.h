#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734



namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
					::gpk::SFramework													Framework;
					::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen						= {};
					::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>					PNGImages						= {};
					
					int32_t																IdExit							= -1;

					::std::mutex														LockGUI;
					::std::mutex														LockRender;

					::gpk::SDialog														DialogMain;
					int32_t																Slider;
					int32_t																NumericTuner;
					int32_t																CheckBox;

																						SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

static		const ::gpk::view_const_string										filenames	[]				=
	 { "..\\gpk_data\\pngsuite\\basi0g01.png"
	 //, "png_test_generated.png"
	 , "..\\gpk_data\\pngsuite\\basi3p01.png"
	 , "..\\gpk_data\\pngsuite\\basn0g01.png"
	 , "..\\gpk_data\\pngsuite\\basn3p01.png"
	 , "..\\gpk_data\\pngsuite\\s01i3p01.png"
	 , "..\\gpk_data\\pngsuite\\s01n3p01.png"
	 , "..\\gpk_data\\pngsuite\\s02i3p01.png"
	 , "..\\gpk_data\\pngsuite\\s02n3p01.png"
	 , "..\\gpk_data\\pngsuite\\s03i3p01.png"
	 , "..\\gpk_data\\pngsuite\\s03n3p01.png"
	 , "..\\gpk_data\\pngsuite\\s04i3p01.png"
	 , "..\\gpk_data\\pngsuite\\s04n3p01.png"
	 , "..\\gpk_data\\pngsuite\\basi0g02.png"
	 , "..\\gpk_data\\pngsuite\\basn0g02.png"
	 , "..\\gpk_data\\pngsuite\\basi3p02.png"
	 , "..\\gpk_data\\pngsuite\\basn3p02.png"
	 , "..\\gpk_data\\pngsuite\\s05i3p02.png"
	 , "..\\gpk_data\\pngsuite\\s05n3p02.png"
	 , "..\\gpk_data\\pngsuite\\s06i3p02.png"
	 , "..\\gpk_data\\pngsuite\\s06n3p02.png"
	 , "..\\gpk_data\\pngsuite\\s07i3p02.png"
	 , "..\\gpk_data\\pngsuite\\s07n3p02.png"
	 , "..\\gpk_data\\pngsuite\\s08i3p02.png"
	 , "..\\gpk_data\\pngsuite\\s08n3p02.png"
	 , "..\\gpk_data\\pngsuite\\s09i3p02.png"
	 , "..\\gpk_data\\pngsuite\\s09n3p02.png"
	 , "..\\gpk_data\\pngsuite\\tm3n3p02.png"
	 , "..\\gpk_data\\pngsuite\\basi0g04.png"
	 , "..\\gpk_data\\pngsuite\\basn0g04.png"
	 , "..\\gpk_data\\pngsuite\\basi3p04.png"
	 , "..\\gpk_data\\pngsuite\\basn3p04.png"
	 , "..\\gpk_data\\pngsuite\\ch1n3p04.png"
	 , "..\\gpk_data\\pngsuite\\cm0n0g04.png"
	 , "..\\gpk_data\\pngsuite\\cm7n0g04.png"
	 , "..\\gpk_data\\pngsuite\\cm9n0g04.png"
	 , "..\\gpk_data\\pngsuite\\ct0n0g04.png"
	 , "..\\gpk_data\\pngsuite\\ct1n0g04.png"
	 , "..\\gpk_data\\pngsuite\\cten0g04.png"
	 , "..\\gpk_data\\pngsuite\\ctfn0g04.png"
	 , "..\\gpk_data\\pngsuite\\ctgn0g04.png"
	 , "..\\gpk_data\\pngsuite\\cthn0g04.png"
	 , "..\\gpk_data\\pngsuite\\ctjn0g04.png"
	 , "..\\gpk_data\\pngsuite\\ctzn0g04.png"
	 , "..\\gpk_data\\pngsuite\\f99n0g04.png"
	 , "..\\gpk_data\\pngsuite\\g03n3p04.png"
	 , "..\\gpk_data\\pngsuite\\g04n3p04.png"
	 , "..\\gpk_data\\pngsuite\\g05n3p04.png"
	 , "..\\gpk_data\\pngsuite\\g07n3p04.png"
	 , "..\\gpk_data\\pngsuite\\g10n3p04.png"
	 , "..\\gpk_data\\pngsuite\\g25n3p04.png"
	 , "..\\gpk_data\\pngsuite\\s32i3p04.png"
	 , "..\\gpk_data\\pngsuite\\s32n3p04.png"
	 , "..\\gpk_data\\pngsuite\\s33i3p04.png"
	 , "..\\gpk_data\\pngsuite\\s33n3p04.png"
	 , "..\\gpk_data\\pngsuite\\s34i3p04.png"
	 , "..\\gpk_data\\pngsuite\\s34n3p04.png"
	 , "..\\gpk_data\\pngsuite\\s35i3p04.png"
	 , "..\\gpk_data\\pngsuite\\s35n3p04.png"
	 , "..\\gpk_data\\pngsuite\\s36i3p04.png"
	 , "..\\gpk_data\\pngsuite\\s36n3p04.png"
	 , "..\\gpk_data\\pngsuite\\s37i3p04.png"
	 , "..\\gpk_data\\pngsuite\\s37n3p04.png"
	 , "..\\gpk_data\\pngsuite\\s38i3p04.png"
	 , "..\\gpk_data\\pngsuite\\s38n3p04.png"
	 , "..\\gpk_data\\pngsuite\\s39i3p04.png"
	 , "..\\gpk_data\\pngsuite\\s39n3p04.png"
	 , "..\\gpk_data\\pngsuite\\s40i3p04.png"
	 , "..\\gpk_data\\pngsuite\\s40n3p04.png"
	 , "..\\gpk_data\\pngsuite\\tbbn0g04.png"
	 , "..\\gpk_data\\pngsuite\\basi0g08.png"
	 , "..\\gpk_data\\pngsuite\\basi2c08.png"
	 , "..\\gpk_data\\pngsuite\\basi3p08.png"
	 , "..\\gpk_data\\pngsuite\\basi4a08.png"
	 , "..\\gpk_data\\pngsuite\\basi6a08.png"
	 , "..\\gpk_data\\pngsuite\\basn0g08.png"
	 , "..\\gpk_data\\pngsuite\\basn2c08.png"
	 , "..\\gpk_data\\pngsuite\\basn3p08.png"
	 , "..\\gpk_data\\pngsuite\\basn4a08.png"
	 , "..\\gpk_data\\pngsuite\\basn6a08.png"
	 , "..\\gpk_data\\pngsuite\\bgai4a08.png"
	 , "..\\gpk_data\\pngsuite\\bgan6a08.png"
	 , "..\\gpk_data\\pngsuite\\bgbn4a08.png"
	 , "..\\gpk_data\\pngsuite\\bgwn6a08.png"
	 , "..\\gpk_data\\pngsuite\\ccwn2c08.png"
	 , "..\\gpk_data\\pngsuite\\ccwn3p08.png"
	 , "..\\gpk_data\\pngsuite\\cdfn2c08.png"
	 , "..\\gpk_data\\pngsuite\\cdhn2c08.png"
	 , "..\\gpk_data\\pngsuite\\cdsn2c08.png"
	 , "..\\gpk_data\\pngsuite\\cdun2c08.png"
	 , "..\\gpk_data\\pngsuite\\ch2n3p08.png"
	 , "..\\gpk_data\\pngsuite\\cs3n3p08.png"
	 , "..\\gpk_data\\pngsuite\\cs5n2c08.png"
	 , "..\\gpk_data\\pngsuite\\cs5n3p08.png"
	 , "..\\gpk_data\\pngsuite\\cs8n2c08.png"
	 , "..\\gpk_data\\pngsuite\\cs8n3p08.png"
	 , "..\\gpk_data\\pngsuite\\exif2c08.png"
	 , "..\\gpk_data\\pngsuite\\f00n0g08.png"
	 , "..\\gpk_data\\pngsuite\\f00n2c08.png"
	 , "..\\gpk_data\\pngsuite\\f01n0g08.png"
	 , "..\\gpk_data\\pngsuite\\f01n2c08.png"
	 , "..\\gpk_data\\pngsuite\\f02n0g08.png"
	 , "..\\gpk_data\\pngsuite\\f02n2c08.png"
	 , "..\\gpk_data\\pngsuite\\f03n0g08.png"
	 , "..\\gpk_data\\pngsuite\\f03n2c08.png"
	 , "..\\gpk_data\\pngsuite\\f04n0g08.png"
	 , "..\\gpk_data\\pngsuite\\f04n2c08.png"
	 , "..\\gpk_data\\pngsuite\\g03n2c08.png"
	 , "..\\gpk_data\\pngsuite\\g04n2c08.png"
	 , "..\\gpk_data\\pngsuite\\g05n2c08.png"
	 , "..\\gpk_data\\pngsuite\\g07n2c08.png"
	 , "..\\gpk_data\\pngsuite\\g10n2c08.png"
	 , "..\\gpk_data\\pngsuite\\g25n2c08.png"
	 , "..\\gpk_data\\pngsuite\\pp0n6a08.png"
	 , "..\\gpk_data\\pngsuite\\ps1n0g08.png"
	 , "..\\gpk_data\\pngsuite\\ps2n0g08.png"
	 , "..\\gpk_data\\pngsuite\\tbbn3p08.png"
	 , "..\\gpk_data\\pngsuite\\tbgn3p08.png"
	 , "..\\gpk_data\\pngsuite\\tbrn2c08.png"
	 , "..\\gpk_data\\pngsuite\\tbwn3p08.png"
	 , "..\\gpk_data\\pngsuite\\tbyn3p08.png"
	 , "..\\gpk_data\\pngsuite\\tp0n0g08.png"
	 , "..\\gpk_data\\pngsuite\\tp0n2c08.png"
	 , "..\\gpk_data\\pngsuite\\tp0n3p08.png"
	 , "..\\gpk_data\\pngsuite\\tp1n3p08.png"
	 , "..\\gpk_data\\pngsuite\\z00n2c08.png"
	 , "..\\gpk_data\\pngsuite\\z03n2c08.png"
	 , "..\\gpk_data\\pngsuite\\z06n2c08.png"
	 , "..\\gpk_data\\pngsuite\\z09n2c08.png"
	 , "..\\gpk_data\\pngsuite\\basi0g16.png"
	 , "..\\gpk_data\\pngsuite\\basi2c16.png"
	 , "..\\gpk_data\\pngsuite\\basi4a16.png"
	 , "..\\gpk_data\\pngsuite\\basi6a16.png"
	 , "..\\gpk_data\\pngsuite\\basn0g16.png"
	 , "..\\gpk_data\\pngsuite\\basn2c16.png"
	 , "..\\gpk_data\\pngsuite\\basn4a16.png"
	 , "..\\gpk_data\\pngsuite\\basn6a16.png"
	 , "..\\gpk_data\\pngsuite\\bgai4a16.png"
	 , "..\\gpk_data\\pngsuite\\bgan6a16.png"
	 , "..\\gpk_data\\pngsuite\\bggn4a16.png"
	 , "..\\gpk_data\\pngsuite\\bgyn6a16.png"
	 , "..\\gpk_data\\pngsuite\\cs3n2c16.png"
	 , "..\\gpk_data\\pngsuite\\g03n0g16.png"
	 , "..\\gpk_data\\pngsuite\\g04n0g16.png"
	 , "..\\gpk_data\\pngsuite\\g05n0g16.png"
	 , "..\\gpk_data\\pngsuite\\g07n0g16.png"
	 , "..\\gpk_data\\pngsuite\\g10n0g16.png"
	 , "..\\gpk_data\\pngsuite\\g25n0g16.png"
	 , "..\\gpk_data\\pngsuite\\oi1n0g16.png"
	 , "..\\gpk_data\\pngsuite\\oi1n2c16.png"
	 , "..\\gpk_data\\pngsuite\\oi2n0g16.png"
	 , "..\\gpk_data\\pngsuite\\oi2n2c16.png"
	 , "..\\gpk_data\\pngsuite\\oi4n0g16.png"
	 , "..\\gpk_data\\pngsuite\\oi4n2c16.png"
	 , "..\\gpk_data\\pngsuite\\oi9n0g16.png"
	 , "..\\gpk_data\\pngsuite\\oi9n2c16.png"
	 , "..\\gpk_data\\pngsuite\\pp0n2c16.png"
	 , "..\\gpk_data\\pngsuite\\ps1n2c16.png"
	 , "..\\gpk_data\\pngsuite\\ps2n2c16.png"
	 , "..\\gpk_data\\pngsuite\\tbbn2c16.png"
	 , "..\\gpk_data\\pngsuite\\tbgn2c16.png"
	 , "..\\gpk_data\\pngsuite\\tbwn0g16.png"
	 //, "..\\gpk_data\\pngsuite\\xcsn0g01.png"
	 //, "..\\gpk_data\\pngsuite\\xdtn0g01.png"
	 //, "..\\gpk_data\\pngsuite\\xs1n0g01.png"
	 //, "..\\gpk_data\\pngsuite\\xs2n0g01.png"
	 //, "..\\gpk_data\\pngsuite\\xs4n0g01.png"
	 //, "..\\gpk_data\\pngsuite\\xs7n0g01.png"
	 //, "..\\gpk_data\\pngsuite\\xcrn0g04.png"
	 //, "..\\gpk_data\\pngsuite\\xlfn0g04.png"
	 //, "..\\gpk_data\\pngsuite\\xc1n0g08.png"
	 //, "..\\gpk_data\\pngsuite\\xc9n2c08.png"
	 //, "..\\gpk_data\\pngsuite\\xd0n2c08.png"
	 //, "..\\gpk_data\\pngsuite\\xd3n2c08.png"
	 //, "..\\gpk_data\\pngsuite\\xd9n2c08.png"
	 //, "..\\gpk_data\\pngsuite\\xhdn0g08.png"
	 , "..\\gpk_data\\pngsuite\\PngSuite.png"
	};
 
#endif // APPLICATION_H_2078934982734
