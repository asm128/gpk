#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_view_layered.h"

#include "wakawaka.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
#pragma pack(push, 1)
	enum MENU_ORIENTATION : int32_t
		{	MENU_ORIENTATION_HORIZONTAL	= 1
		,	MENU_ORIENTATION_VERTICAL	= 2
		};

	struct SOptionList {
		int32_t																IdControl								= -1;
		MENU_ORIENTATION													Orientation								= ::gme::MENU_ORIENTATION_HORIZONTAL;
		::gpk::array_pod<int32_t>											IdControls								= {};
	};

	::gpk::error_t														optionListPush							(::gpk::SGUI& gui, ::gme::SOptionList& menu, const ::gpk::view_const_string& text);
#pragma pack(pop)

	struct SApplication {
		::gpk::SFramework													Framework;
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen							= {};

		int32_t																IdExit								= -1;
		::gme::SOptionList													OptionListMain						= {};
		::gme::SOptionList													OptionListFile						= {};

		::std::mutex														LockGUI;
		::std::mutex														LockRender;
		::std::mutex														LockGame;
		::std::mutex														LockDraw;
		::wak::SGame														GameInstance;


		::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>					TexturesTest;

		::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>					CharacterAnimationImages;
		::gpk::array_obj<::gpk::view_layered<::gpk::SColorBGRA>>			CharacterAnimationLayers;


																			SApplication						(::gpk::SRuntimeValues & runtimeValues)		: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
