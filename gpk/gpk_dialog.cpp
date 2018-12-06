#include "gpk_dialog.h"
#include "gpk_png.h"

												::gpk::IDialogControl::~IDialogControl		()									{ if(-1 != IdGUIControl) ::gpk::controlDelete(Dialog->GUI, IdGUIControl, true); }

												::gpk::SDialog::SDialog						()				{
	Root												= ::gpk::controlCreate(GUI);													
	throw_if(-1 == Root, "%s", "Out of memory?");
	GUI.Controls.Constraints[Root].AttachSizeToControl	= {Root, Root};
	GUI.Controls.Controls[Root].Margin	= GUI.Controls.Controls[Root].Border	= {};
	GUI.Controls.States[Root].Design					= true;
	GUI.ColorModeDefault								= ::gpk::GUI_COLOR_MODE_FLAT;
	GUI.ThemeDefault									= ::gpk::ASCII_COLOR_CYAN * 16 + 8;
	
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_NORMAL				]	= GUI.Palettes.push_back({{::gpk::DARKGRAY			, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE		* .85,}});
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_DISABLED			]	= GUI.Palettes.push_back({{::gpk::LIGHTGRAY * 1.3	, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY	* 1.2,}});
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_HOVER				]	= GUI.Palettes.push_back({{::gpk::GRAY	 			, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE			,}});
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_PRESSED				]	= GUI.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, {}, ::gpk::WHITE			,}});
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_SELECTED			]	= ColorsControl[::gpk::GUI_CONTROL_PALETTE_NORMAL	];
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED	]	= ColorsControl[::gpk::GUI_CONTROL_PALETTE_DISABLED	];
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER		]	= ColorsControl[::gpk::GUI_CONTROL_PALETTE_HOVER	];
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED	]	= ColorsControl[::gpk::GUI_CONTROL_PALETTE_PRESSED	];
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_EXECUTE				]	= GUI.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE			,}});
	ColorsControl	[::gpk::GUI_CONTROL_PALETTE_OUTDATED			]	= GUI.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE			,}});

	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_NORMAL				]	= GUI.Palettes.push_back({{::gpk::GRAY				, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE			,}});
	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_DISABLED			]	= GUI.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_HOVER				]	= GUI.Palettes.push_back({{::gpk::GRAY	 	* 1.1	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE			,}});
	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_PRESSED				]	= GUI.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::ORANGE		, ::gpk::ORANGE		, ::gpk::ORANGE		, {}, ::gpk::BLUE			,}});
	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_SELECTED			]	= ColorsButton[::gpk::GUI_CONTROL_PALETTE_NORMAL	];
	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED	]	= ColorsButton[::gpk::GUI_CONTROL_PALETTE_DISABLED	];
	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER		]	= ColorsButton[::gpk::GUI_CONTROL_PALETTE_HOVER		];
	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED	]	= ColorsButton[::gpk::GUI_CONTROL_PALETTE_PRESSED	];
	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_EXECUTE				]	= GUI.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE			,}});
	ColorsButton	[::gpk::GUI_CONTROL_PALETTE_OUTDATED			]	= GUI.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE			,}});

	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_NORMAL				]	= GUI.Palettes.push_back({{::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE			,}});
	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_DISABLED			]	= GUI.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_HOVER				]	= GUI.Palettes.push_back({{::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE			,}});
	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_PRESSED				]	= GUI.Palettes.push_back({{::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE			,}});
	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_SELECTED			]	= ColorsCheckBox[::gpk::GUI_CONTROL_PALETTE_NORMAL		];
	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED	]	= ColorsCheckBox[::gpk::GUI_CONTROL_PALETTE_DISABLED	];
	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER		]	= ColorsCheckBox[::gpk::GUI_CONTROL_PALETTE_HOVER		];
	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED	]	= ColorsCheckBox[::gpk::GUI_CONTROL_PALETTE_PRESSED		];
	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_EXECUTE				]	= GUI.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE			,}});
	ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_OUTDATED			]	= GUI.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE			,}});
}

::gpk::error_t									gpk::checkBoxCreate							(::gpk::SDialog & dialog)			{
	int32_t												index										= -1;
	::gpk::ptr_obj<::gpk::SDialogCheckBox>				checkBox;
	gpk_necall(index = dialog.Create(checkBox), "%s", "Out of memory?");
	dialog.GUI.Controls.Modes[checkBox->IdGUIControl].UseNewPalettes	= true;
	::gpk::memcpy_s(dialog.GUI.Controls.Controls[checkBox->IdGUIControl].Palettes, dialog.ColorsCheckBox);
	return index;
}

::gpk::error_t									gpk::checkBoxUpdate							(::gpk::SDialogCheckBox & checkbox)		{
	::gpk::SImage<::gpk::SColorBGRA>					& imageCross								= checkbox.Dialog->ImageCrossBGRA;
	if(checkbox.Dialog->GUI.Controls.States[checkbox.IdGUIControl].Execute) {
		if(checkbox.Dialog->ImageCross.Texels.size() < 4) {
			::gpk::pngFileLoad("..\\gpk_data\\images\\cross.png", imageCross);

			checkbox.Dialog->ImageCross.resize(imageCross.metrics());
			for(uint32_t iTexel = 0; iTexel < imageCross.Texels.size(); ++iTexel) {
				if(imageCross.Texels[iTexel] == ::gpk::BLACK)
					checkbox.Dialog->ImageCross.View[iTexel]		=  true;
				else {
					imageCross.Texels[iTexel].a						= 0;
					checkbox.Dialog->ImageCross.View[iTexel]		= false;
				}
			}
		}
		checkbox.Checked								= !checkbox.Checked;
		checkbox.Dialog->GUI.Controls.Controls[checkbox.IdGUIControl].Image = (false == checkbox.Checked) ? ::gpk::view_grid<::gpk::SColorBGRA>{} : imageCross.View;
	}
	return 0;
}

::gpk::error_t									gpk::tunerCreate							(::gpk::SDialog & dialog)			{
	int32_t												index										= -1;
	::gpk::ptr_obj<::gpk::SDialogTuner>					tuner;
	gpk_necall(index = dialog.Create(tuner), "%s", "Out of memory?");
	tuner->ValueCurrent								= tuner->ValueLimits.Min;
	gpk_necall(tuner->IdDecrease = ::gpk::controlCreateChild(dialog.GUI, tuner->IdGUIControl), "%s", "Out of memory?");
	gpk_necall(tuner->IdIncrease = ::gpk::controlCreateChild(dialog.GUI, tuner->IdGUIControl), "%s", "Out of memory?");
	dialog.GUI.Controls.Controls[tuner->IdGUIControl].Margin	= {};
	for(int32_t iControl = tuner->IdDecrease; iControl < tuner->IdIncrease + 1; ++iControl) {
		dialog.GUI.Controls.Controls[iControl].Align				= (iControl == tuner->IdDecrease) ? ::gpk::ALIGN_CENTER_LEFT : ::gpk::ALIGN_CENTER_RIGHT;
		dialog.GUI.Controls.Text	[iControl].Text					= (iControl == tuner->IdDecrease) ? "-" : "+";
		dialog.GUI.Controls.Modes	[iControl].UseNewPalettes		= true;
		dialog.GUI.Controls.Modes	[iControl].ColorMode			= ::gpk::GUI_COLOR_MODE_3D;
		::gpk::memcpy_s(dialog.GUI.Controls.Controls[iControl].Palettes, dialog.ColorsButton);
		dialog.GUI.Controls.Constraints[iControl].AttachSizeToControl.y	= iControl;
	}
	::gpk::SControlText									& tunerText				= dialog.GUI.Controls.Text[tuner->IdGUIControl];
	tunerText.Text									= {tuner->ValueString, (uint32_t)sprintf_s(tuner->ValueString, "%lli", tuner->ValueCurrent)};
	tunerText.Align									= ::gpk::ALIGN_CENTER;
	return index;
}

::gpk::error_t									gpk::tunerUpdate							(::gpk::SDialogTuner & tuner)			{
	if(tuner.Dialog->GUI.Controls.States[tuner.IdDecrease].Execute || tuner.Dialog->GUI.Controls.States[tuner.IdIncrease].Execute) {
			 if(tuner.Dialog->GUI.Controls.States[tuner.IdDecrease].Execute)
			--tuner.ValueCurrent;
		else if(tuner.Dialog->GUI.Controls.States[tuner.IdIncrease].Execute)
			++tuner.ValueCurrent;
		if (tuner.ValueCurrent < tuner.ValueLimits.Min)
			tuner.ValueCurrent = tuner.ValueLimits.Min;
		if (tuner.ValueCurrent > tuner.ValueLimits.Max)
			tuner.ValueCurrent = tuner.ValueLimits.Max;
		::gpk::SControlText									& tunerText									= tuner.Dialog->GUI.Controls.Text[tuner.IdGUIControl];
		tunerText.Text									= {tuner.ValueString, (uint32_t)sprintf_s(tuner.ValueString, "%lli", tuner.ValueCurrent)};
		::gpk::controlMetricsInvalidate(tuner.Dialog->GUI, tuner.IdGUIControl);
	}
	return 0;
}

::gpk::error_t									gpk::sliderCreate							(::gpk::SDialog & dialog)			{
	int32_t												index										= -1;
	::gpk::ptr_obj<::gpk::SDialogSlider>				slider;
	gpk_necall(index = dialog.Create(slider), "%s", "Out of memory?");
	gpk_necall(slider->IdButton = ::gpk::controlCreateChild(dialog.GUI, slider->IdGUIControl), "%s", "Out of memory?");
	dialog.GUI.Controls.Controls[slider->IdGUIControl].Margin	= {};
	dialog.GUI.Controls.Controls[slider->IdButton].Area.Size	= {16, 16};
	dialog.GUI.Controls.Text	[slider->IdButton].Text			= "-";
	dialog.GUI.Controls.Constraints[slider->IdButton].AttachSizeToControl.y	= slider->IdButton;
	dialog.GUI.Controls.Modes[slider->IdButton].UseNewPalettes	= true;
	dialog.GUI.Controls.Modes[slider->IdButton].ColorMode		= ::gpk::GUI_COLOR_MODE_3D;
	::gpk::memcpy_s(dialog.GUI.Controls.Controls[slider->IdButton		].Palettes, dialog.ColorsButton);
	::gpk::memcpy_s(dialog.GUI.Controls.Controls[slider->IdGUIControl	].Palettes, dialog.ColorsCheckBox);
	return index;
}

::gpk::error_t									gpk::sliderSetValue							(::gpk::SDialogSlider	& slider, int64_t value)	{
	slider.ValueCurrent								= ::gpk::max(::gpk::min(value, slider.ValueLimits.Max), slider.ValueLimits.Min);
	const double										proportion									= (slider.ValueCurrent - slider.ValueLimits.Min) * (1.0 / (slider.ValueLimits.Max - slider.ValueLimits.Min)); 

	::gpk::SControl										& controlButton								= slider.Dialog->GUI.Controls.Controls[slider.IdButton];
	::gpk::SControl										& controlSlider								= slider.Dialog->GUI.Controls.Controls[slider.IdGUIControl];
	::gpk::SCoord2<double>								ncSpacing									= ::gpk::controlNCSpacing(controlSlider).Cast<double>();
	if(slider.Vertical) 
		controlButton.Area.Offset.y						= (int16_t)int32_t((controlSlider.Area.Size.y - 16 - ncSpacing.y) * proportion); 
	else
		controlButton.Area.Offset.x						= (int16_t)int32_t((controlSlider.Area.Size.x - 16 - ncSpacing.x) * proportion); 

	::gpk::SControlConstraints							& buttonConstraints							= slider.Dialog->GUI.Controls.Constraints[slider.IdButton];
	if(slider.Vertical) 
		buttonConstraints.AttachSizeToControl.x			= slider.IdButton;
	else
		buttonConstraints.AttachSizeToControl.y			= slider.IdButton;
	::gpk::controlMetricsInvalidate(slider.Dialog->GUI, slider.IdGUIControl);
	return 0;
}

::gpk::error_t									gpk::sliderUpdate							(::gpk::SDialogSlider	& slider)	{
	const int64_t										valueRange									= slider.ValueLimits.Max - slider.ValueLimits.Min;
	if(slider.Dialog->GUI.Controls.States[slider.IdButton].Pressed) {
		if(slider.Dialog->Input->MouseCurrent.Position != slider.Dialog->Input->MousePrevious.Position) {
			::gpk::SInput										& input										= *slider.Dialog->Input;
			const ::gpk::SGUIZoom								& zoom										= slider.Dialog->GUI.Zoom;
			::gpk::SCoord2<double>								scale										= 
				{ 1.0 / (zoom.ZoomLevel * zoom.DPI.x)
				, 1.0 / (zoom.ZoomLevel * zoom.DPI.y)
				};
			const ::gpk::SCoord2<int16_t>						& controlSliderSize							= slider.Dialog->GUI.Controls.Controls[slider.IdGUIControl].Area.Size;

			::gpk::SCoord2<double>								valueDisplacement							= 
				{ input.MouseCurrent.Deltas.x * scale.x * (valueRange * (1.0 / controlSliderSize.x))
				, input.MouseCurrent.Deltas.y * scale.y * (valueRange * (1.0 / controlSliderSize.y))
				};

			if (valueDisplacement.y > 0 && valueDisplacement.y <  1) valueDisplacement.y =  1;
			if (valueDisplacement.y < 0 && valueDisplacement.y > -1) valueDisplacement.y = -1;
			if (valueDisplacement.x > 0 && valueDisplacement.x <  1) valueDisplacement.x =  1;
			if (valueDisplacement.x < 0 && valueDisplacement.x > -1) valueDisplacement.x = -1;

			::gpk::sliderSetValue(slider, slider.ValueCurrent + (int64_t)(slider.Vertical ? valueDisplacement.y : valueDisplacement.x));
		}
	}
	else
		if(slider.Dialog->Input->ButtonDown(0) && slider.Dialog->GUI.Controls.States[slider.IdGUIControl].Pressed) {	// 
			::gpk::SInput										& input										= *slider.Dialog->Input;
			const int64_t										valuePage									= ::gpk::max(1LL, (valueRange > 10) ? (int64_t)(valueRange * .1) : valueRange / 2);
			const ::gpk::SCoord2<int32_t>						& controlButtonPosition						= slider.Dialog->GUI.Controls.Metrics[slider.IdButton].Total.Global.Offset;
			if(slider.Vertical) {
				const int64_t										finalValue									= slider.ValueCurrent + ((input.MouseCurrent.Position.y > controlButtonPosition.y) ? valuePage : -valuePage);
				::gpk::sliderSetValue(slider, finalValue);
			}
			else {
				const int64_t										finalValue									= slider.ValueCurrent + ((input.MouseCurrent.Position.x > controlButtonPosition.x) ? valuePage : -valuePage);
				::gpk::sliderSetValue(slider, finalValue);
			}
		}
	return 0;
}

::gpk::error_t									gpk::editBoxCreate							(::gpk::SDialog			& dialog )	{ 
	int32_t												index										= -1;
	::gpk::ptr_obj<::gpk::SDialogEditBox>				editBox;
	gpk_necall(index = dialog.Create(editBox), "%s", "Out of memory?");
	dialog.GUI.Controls.Modes[editBox->IdGUIControl].UseNewPalettes	= true;
	dialog.GUI.Controls.Modes[editBox->IdGUIControl].ColorMode		= ::gpk::GUI_COLOR_MODE_FLAT;
	::gpk::memcpy_s(dialog.GUI.Controls.Controls[editBox->IdGUIControl].Palettes, dialog.ColorsCheckBox);
	return index; 
}

::gpk::error_t									gpk::editBoxUpdate							(::gpk::SDialogEditBox	& control)	{ 
	if(control.Dialog->Input->ButtonDown(0)) {
	}
	return 0; 
}
