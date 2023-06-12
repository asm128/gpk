#include "gpk_engine.h"
#include "gpk_rect_align.h"

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

	GDEFINE_ENUM_TYPE(UI3D_CONTROL_AREA, uint8_t);
	GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, Total			, 0);
	GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, Client		, 1);
	GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, Left			, 0x80 + ::gpk::ALIGN_LEFT		);
	GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, Top			, 0x80 + ::gpk::ALIGN_TOP		);
	GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, Right			, 0x80 + ::gpk::ALIGN_RIGHT		);
	GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, Bottom		, 0x80 + ::gpk::ALIGN_BOTTOM	);
	GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, COUNT			, 6);
	//GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, LeftBottom	, 6);
	//GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, TopLeft		, 7);
	//GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, RightTop	, 8);
	//GDEFINE_ENUM_VALUE(UI3D_CONTROL_AREA, BottomRight	, 9);

#pragma pack(push, 1)

	struct S3DControlTheme {
		int16_t					ColorTheme		= 0;
		::gpk::asti32<::gpk::GUI_CONTROL_PALETTE_COUNT>	
								Palettes		= {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9,}};
	};

	struct S3DControl {
		::gpk::rect2i16		Area		;
		::gpk::recti16		Border		;
		::gpk::ALIGN		Align		;
		::gpk::SRelativeBit	RelativeUnit;
	};

	struct S3DControlImage {
		::gpk::n2i16		ImageOffset	;
		::gpk::ALIGN		ImageAlign	;
		bool				Invert		: 1;
	};
#pragma pack(pop)

	struct SEngineGUI	{
		::gpk::SEngine		Engine			= {};	// We're going to use this in order to take advantage of the d3d implementation we already have.
		::gpk::SGUI			GUI				= {};
		::gpk::aeid			Entities		= {};
		
		inline				SEngineGUI		()	{ Reset(); }

		::gpk::error_t		Reset			()	{
			Engine				= {};
			GUI					= {};
			Entities.clear();

			gpk_necs(Entities.resize(::gpk::get_value_count<GUI_ENTITY>()));
			gpk_necs(Entities[GUI_ENTITY_Label]		= Engine.CreateGrid({4, 4}));
			gpk_necs(Entities[GUI_ENTITY_Button]	= Engine.CreateGrid({4, 4}));

			gpk_necs(Entities[GUI_ENTITY_Menu]		= Engine.Clone(Entities[GUI_ENTITY_Button], false, false, false));
			gpk_necs(Entities[GUI_ENTITY_Viewport]	= Engine.Clone(Entities[GUI_ENTITY_Button], false, false, false));
			gpk_necs(Entities[GUI_ENTITY_Palette]	= Engine.Clone(Entities[GUI_ENTITY_Button], false, false, false));

			rees_if(0 != ::gpk::controlCreate(GUI));
			return 0;
		}

		::gpk::eid_t	CreateControl	()	{ return 0; }
		::gpk::eid_t	CreateMenu		()	{ return 0; }
		::gpk::eid_t	CreateViewport	()	{ return 0; }
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

