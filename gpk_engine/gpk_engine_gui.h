#include "gpk_engine.h"

#ifndef GPK_ENGINE_GUI_H_23610
#define GPK_ENGINE_GUI_H_23610

namespace gpk 
{
	GDEFINE_ENUM_TYPE(GUI_ENTITY, uint8_t);
	GDEFINE_ENUM_VALUE(GUI_ENTITY, Label	, 0);
	GDEFINE_ENUM_VALUE(GUI_ENTITY, Button	, 1);
	GDEFINE_ENUM_VALUE(GUI_ENTITY, Menu		, 2);
	GDEFINE_ENUM_VALUE(GUI_ENTITY, Viewport	, 3);
	GDEFINE_ENUM_VALUE(GUI_ENTITY, Palette	, 4);

	struct SEngineGUI	{
		::gpk::SEngine	Engine;	// We're going to use this in order to take advantage of the d3d implementation we already have.
		::gpk::aeid		Entities		= {};
		
		inline			SEngineGUI		() { Reset(); }

		::gpk::error_t	Reset			() {
			Entities.resize(::gpk::get_value_count<GUI_ENTITY>());
			Entities[GUI_ENTITY_Label]	= Engine.CreateGrid({2, 2});
			Entities[GUI_ENTITY_Button]	= Engine.CreateGrid({4, 4});

			return 0;
		}

		::gpk::eid_t	CreateControl	();
		::gpk::eid_t	CreateMenu		();
		::gpk::eid_t	CreateViewport	();
	};
} // namespace

#endif // GPK_ENGINE_GUI_H_23610

//#ifdef CreateDialog
//#	define GPK_CreateDialog_REDEFINE
//#	undef CreateDialog
//#endif
//#ifdef GPK_CreateDialog_REDEFINE
//#	ifdef UNICODE
//#		define CreateDialog  CreateDialogW
//#	else
//	#	define CreateDialog  CreateDialogA
//#	endif // !UNICODE
//#endif

