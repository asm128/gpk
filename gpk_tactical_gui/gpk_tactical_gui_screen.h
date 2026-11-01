#include "gpk_gui.h"
#include "gpk_png.h"

#ifndef GPK_TACTICAL_GUI_SCREEN_H_239087923
#define GPK_TACTICAL_GUI_SCREEN_H_239087923

namespace dry
{
#pragma pack(push, 1)
	struct SControlParams {
		::gpk::GUI_COLOR_MODE					ColorMode		;
		::gpk::cid_t							Id				;
		::gpk::cid_t							IdParent		;
		bool									Design			;
		bool									NoBackground	;
		bool									NoHoverEffect	;
		bool									FrameOut		;
		::gpk::ALIGN							Align			;
		::gpk::rect2i16						Area			;
		::gpk::rectu16							Border			;
		::gpk::rectu16							Margin			;
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
		::gpk::astatic<::gpk::cid_t, _countControls>				ControlIds;
		::gpk::astatic<::dry::SControlImages, _countControls>		ControlIcons;

		inline														SScreen			()	{ ::gpk::view<::gpk::cid_t>{ControlIds}.fill(-1); }
	};

	template<typename _tScreen, size_t _countControls>
	static	::gpk::error_t						setupScreenFromControlParams	(::gpk::SGUI & gui, ::gpk::cid_t rootControlIndex, _tScreen & screen, const ::dry::SControlParams (&controlSetupList)[_countControls])	{
		::gpk::SPNGData									pngCache				= {};
		for(uint32_t iControl = 0; iControl < _countControls; ++iControl) {
			const SControlParams							& controlSetup			= controlSetupList[iControl];
			const ::gpk::cid_t									controlIndex
				= screen.ControlIds[controlSetup.Id]	= ::gpk::controlCreate(gui);

			::gpk::SControlPlacement						& control				= gui.Controls.Placement	[controlIndex];
			::gpk::SControlText								& controlText			= gui.Controls.Text			[controlIndex];
			::gpk::SControlDraw								& controlDraw			= gui.Controls.Draw			[controlIndex];
			::gpk::SControlImage							& controlImage			= gui.Controls.Images		[controlIndex];
			::gpk::SControlState							& controlState			= gui.Controls.States		[controlIndex];
			::gpk::SControlConstraints						& controlConstraints	= gui.Controls.Constraints	[controlIndex];
			control.Align								= controlSetup.Align		;
			control.Area								= controlSetup.Area			;
			control.Border								= controlSetup.Border		;
			control.Margin								= controlSetup.Margin		;
			controlImage.ImageAlign						= controlSetup.IconAlign	;
			controlText.Text							= controlSetup.Text			;
			controlText.Align							= controlSetup.TextAlign	;
			controlConstraints.AttachSizeToText			= controlSetup.AdjustToText	;
			controlDraw.NoBorder							= controlSetup.Design		;
			controlDraw.ColorMode						= controlSetup.ColorMode	;
			controlDraw.NoClient							= controlSetup.NoBackground;
			controlDraw.FrameOut							= controlSetup.FrameOut;
			if(controlSetup.NoHoverEffect)
				controlState.Mask							= ::gpk::bit_clear(controlState.Mask, ::gpk::GUI_CONTROL_FLAG_Hovered);
			::gpk::controlSetParent(gui, controlIndex, (controlSetup.IdParent == -1) ? rootControlIndex : screen.ControlIds[controlSetup.IdParent]);

			if((controlSetup.IconPathOff == controlSetup.IconPathOn) && controlSetup.IconPathOn.size()) {
				if_fail_cef(::gpk::failed(::gpk::pngFileLoad(pngCache, controlSetup.IconPathOff, screen.ControlIcons[controlSetup.Id].Off)), "Failed to load icon: '%s'", controlSetup.IconPathOff.begin());
				controlImage.Image							= screen.ControlIcons[controlSetup.Id].Off.View;
				screen.ControlIcons[controlSetup.Id].On.View	= screen.ControlIcons[controlSetup.Id].Off.View;
			}
			else {
				if(controlSetup.IconPathOn.size()) {
					if_fail_cef(::gpk::failed(::gpk::pngFileLoad(pngCache, controlSetup.IconPathOn , screen.ControlIcons[controlSetup.Id].On)), "Failed to load icon: '%s'", controlSetup.IconPathOn.begin());
					controlImage.Image							= screen.ControlIcons[controlSetup.Id].On.View;
				}
				if(controlSetup.IconPathOff.size()) {
					if_fail_cef(::gpk::failed(::gpk::pngFileLoad(pngCache, controlSetup.IconPathOff, screen.ControlIcons[controlSetup.Id].Off)), "Failed to load icon: '%s'", controlSetup.IconPathOff.begin());
					controlImage.Image							= screen.ControlIcons[controlSetup.Id].Off.View;
				}
			}
		}
		return 0;
	}

} // namespace

#endif // GPK_TACTICAL_GUI_SCREEN_H_239087923
