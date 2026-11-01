#include "gpk_gui.h"
#include "gpk_png.h"

#ifndef DRYER_SCREEN_H_239087923
#define DRYER_SCREEN_H_239087923

namespace dry
{
#pragma pack(push, 1)
	struct SControlParams {
		::gpk::GUI_COLOR_MODE					ColorMode		;
		int32_t									Id				;
		int32_t									IdParent		;
		bool									Design			;
		bool									NoBackground	;
		bool									NoHoverEffect	;
		bool									FrameOut		;
		::gpk::ALIGN							Align			;
		::gpk::rect2<int16_t>					Area			;
		::gpk::SRectLimits<uint8_t>				Border			;
		::gpk::SRectLimits<uint16_t>			Margin			;
		::gpk::vcst_t		Text			;
		::gpk::ALIGN		TextAlign		;
		::gpk::ALIGN		IconAlign		;
		::gpk::vcst_t		IconPathOff		;
		::gpk::vcst_t		IconPathOn		;
		::gpk::n2<bool>		AdjustToText	;
	};
#pragma pack(pop)

	struct SControlImages {
		::gpk::img8bgra		On	;
		::gpk::img8bgra		Off	;
	};

	template<size_t _countControls>
	struct SScreen {
		::gpk::array_static<int32_t, _countControls>				ControlIds;
		::gpk::array_static<::dry::SControlImages, _countControls>	ControlIcons;

		inline														SScreen			()	{ ::gpk::vi32{ControlIds}.fill(-1); }
	};

	template<size_t _countControls>
	static	::gpk::error_t						setupScreenFromControlParams	(::gpk::SGUI & gui, int32_t rootControlIndex, ::dry::SScreen<_countControls>& screen, ::gpk::view_array<const ::dry::SControlParams> controlSetupList)	{
		::gpk::SPNGData									pngCache				= {};
		for(uint32_t iControl = 0; iControl < controlSetupList.size(); ++iControl) {
			const SControlParams							& controlSetup			= controlSetupList[iControl];
			const int32_t									controlIndex
				= screen.ControlIds[controlSetup.Id]	= ::gpk::controlCreate(gui);

			::gpk::SControl									& control				= gui.Controls.Controls		[controlIndex];
			::gpk::SControlText								& controlText			= gui.Controls.Text			[controlIndex];
			::gpk::SControlMode								& controlMode			= gui.Controls.Modes		[controlIndex];
			::gpk::SControlConstraints						& controlConstraints	= gui.Controls.Constraints	[controlIndex];
			control.Align								= controlSetup.Align		;
			control.Area								= controlSetup.Area			;
			control.Border								= controlSetup.Border		;
			control.Margin								= controlSetup.Margin		;
			control.ImageAlign							= controlSetup.IconAlign	;
			controlText.Text							= controlSetup.Text			;
			controlText.Align							= controlSetup.TextAlign	;
			controlConstraints.AttachSizeToText			= controlSetup.AdjustToText	;
			controlMode.Design							= controlSetup.Design		;
			controlMode.ColorMode						= controlSetup.ColorMode	;
			controlMode.NoBackgroundRect				= controlSetup.NoBackground;
			controlMode.NoHoverEffect					= controlSetup.NoHoverEffect;
			controlMode.FrameOut						= controlSetup.FrameOut;
			if(controlSetup.ColorMode == ::gpk::GUI_COLOR_MODE_Flat) {
				controlMode.UseNewPalettes					= true;
			}
			::gpk::memcpy_s(control.Palettes.Storage, gui.DefaultColors.Storage);
			::gpk::controlSetParent(gui, controlIndex, (controlSetup.IdParent == -1) ? rootControlIndex : screen.ControlIds[controlSetup.IdParent]);

			if((controlSetup.IconPathOff == controlSetup.IconPathOn) && controlSetup.IconPathOn.size()) {
				ce_if(errored(::gpk::pngFileLoad(pngCache, controlSetup.IconPathOff, screen.ControlIcons[controlSetup.Id].Off)), "Failed to load icon: '%s'", controlSetup.IconPathOff.begin());
				control.Image								= screen.ControlIcons[controlSetup.Id].Off.View;
				screen.ControlIcons[controlSetup.Id].On.View	= screen.ControlIcons[controlSetup.Id].Off.View;
			}
			else {
				if(controlSetup.IconPathOn.size()) {
					ce_if(errored(::gpk::pngFileLoad(pngCache, controlSetup.IconPathOn , screen.ControlIcons[controlSetup.Id].On)), "Failed to load icon: '%s'", controlSetup.IconPathOn.begin());
					control.Image								= screen.ControlIcons[controlSetup.Id].On.View;
				}
				if(controlSetup.IconPathOff.size()) {
					ce_if(errored(::gpk::pngFileLoad(pngCache, controlSetup.IconPathOff, screen.ControlIcons[controlSetup.Id].Off)), "Failed to load icon: '%s'", controlSetup.IconPathOff.begin());
					control.Image								= screen.ControlIcons[controlSetup.Id].Off.View;
				}
			}
		}
		return 0;
	}

} // namespace

#endif // DRYER_SCREEN_H_239087923
