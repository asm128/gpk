#include "gpk_dialog.h"

#include <functional>

#ifndef GPK_DIALOG_CONTROLS_H_209837402937
#define GPK_DIALOG_CONTROLS_H_209837402937

namespace gpk
{
	struct SDialogSlider;
	::gpk::error_t										sliderCreate						(::gpk::SDialog					& dialog);
	::gpk::error_t										sliderSetValue						(::gpk::SDialogSlider			& control, int64_t value);
	::gpk::error_t										sliderUpdate						(::gpk::SDialogSlider			& control);

	struct SDialogTuner;
	::gpk::error_t										tunerCreate							(::gpk::SDialog					& dialog);
	::gpk::error_t										tunerUpdate							(::gpk::SDialogTuner			& control);
	::gpk::error_t										tunerSetValue						(::gpk::SDialogTuner			& control, int64_t value, ::gpk::vcs formatString = {});

	struct SDialogCheckBox;
	::gpk::error_t										checkBoxCreate						(::gpk::SDialog					& dialog);
	::gpk::error_t										checkBoxUpdate						(::gpk::SDialogCheckBox			& control);

	struct SDialogEditBox;
	::gpk::error_t										editBoxCreate						(::gpk::SDialog					& dialog);
	::gpk::error_t										editBoxUpdate						(::gpk::SDialogEditBox			& control);

	struct SDialogViewport;
	::gpk::error_t										viewportCreate						(::gpk::SDialog					& dialog);
	::gpk::error_t										viewportUpdate						(::gpk::SDialogViewport			& control);
	::gpk::error_t										viewportFold						(::gpk::SDialogViewport			& control, bool fold);
	::gpk::error_t										viewportAdjustSize					(::gpk::SCoord2<int16_t>		& sizeViewport, const ::gpk::SCoord2<int16_t>	& sizeClient);

	static inline	::gpk::error_t						sliderCreate						(::gpk::SDialog			& dialog, ::gpk::ptr_nco<SDialogSlider	>& createdControl)	 { int32_t index = -1; gpk_necall(index = sliderCreate		(dialog), "%s", "Out of memory?"); dialog.Controls[index].as(createdControl); return index; }
	static inline	::gpk::error_t						tunerCreate							(::gpk::SDialog			& dialog, ::gpk::ptr_nco<SDialogTuner	>& createdControl)	 { int32_t index = -1; gpk_necall(index = tunerCreate		(dialog), "%s", "Out of memory?"); dialog.Controls[index].as(createdControl); return index; }
	static inline	::gpk::error_t						checkBoxCreate						(::gpk::SDialog			& dialog, ::gpk::ptr_nco<SDialogCheckBox>& createdControl)	 { int32_t index = -1; gpk_necall(index = checkBoxCreate	(dialog), "%s", "Out of memory?"); dialog.Controls[index].as(createdControl); return index; }
	static inline	::gpk::error_t						editBoxCreate						(::gpk::SDialog			& dialog, ::gpk::ptr_nco<SDialogEditBox	>& createdControl)	 { int32_t index = -1; gpk_necall(index = editBoxCreate		(dialog), "%s", "Out of memory?"); dialog.Controls[index].as(createdControl); return index; }
	static inline	::gpk::error_t						viewportCreate						(::gpk::SDialog			& dialog, ::gpk::ptr_nco<SDialogViewport>& createdControl)	 { int32_t index = -1; gpk_necall(index = viewportCreate	(dialog), "%s", "Out of memory?"); dialog.Controls[index].as(createdControl); return index; }

#pragma pack(push, 1)
	struct SDialogViewportSettings {
				bool										Dragging							: 1;
				bool										Unfolded							: 1;
				bool										DisplacementLockX					: 1;
				bool										DisplacementLockY					: 1;
				bool										ScrollBarX							: 1;
				bool										ScrollBarY							: 1;
	};

	struct SDialogViewport : public ::gpk::IDialogControl {
				int32_t										IdClose								= -1;
				int32_t										IdMinimize							= -1;
				int32_t										IdMaximize							= -1;
				int32_t										IdScrollBarX						= -1;
				int32_t										IdScrollBarY						= -1;
				int32_t										IdTitle								= -1;
				int32_t										IdClient							= -1;
				SDialogViewportSettings						Settings							= {};
				SDialogViewportSettings						SettingsOld							= {};

		virtual	::gpk::error_t								Update								()							{ return ::gpk::viewportUpdate(*this); }
	};

	struct SDialogEditBox : public ::gpk::IDialogControl {
				::gpk::SMinMax<uint32_t>					Selection							= {};
				::gpk::apod<char_t>							String								= {};

		virtual	::gpk::error_t								Update								()							{ return ::gpk::editBoxUpdate(*this); }
	};

	struct SDialogCheckBox : public ::gpk::IDialogControl {
				bool										Checked								= false;

		virtual	::gpk::error_t								Update								()							{ return ::gpk::checkBoxUpdate(*this); }
	};

	struct SDialogSlider : public ::gpk::IDialogControl {
				int32_t										IdButton							= -1;

				::gpk::SMinMax<int64_t>						ValueLimits							= {0, 100};// 0x7fFFffFF};
				int64_t										ValueCurrent						= -1;
				bool										Vertical							= false;

		virtual	::gpk::error_t								Update								()							{ return ::gpk::sliderUpdate(*this); }
	};

	struct SDialogTuner : public ::gpk::IDialogControl {
				int32_t										IdDecrease							= -1;
				int32_t										IdIncrease							= -1;

				::gpk::SMinMax<int64_t>						ValueLimits							= {(int32_t)0xC0000001, (int32_t)0x3fFFffFF};
				int64_t										ValueCurrent						= -1;
				::std::function<::gpk::error_t(::gpk::vcc & format, int64_t value)>	
															FuncValueFormat						= {};
				char_t										ValueString	[32]					= {};


		virtual	::gpk::error_t								Update								()							{ return ::gpk::tunerUpdate(*this); }
	};
#pragma pack(pop)
}

#endif // GPK_DIALOG_CONTROLS_H_209837402937
