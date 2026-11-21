#include "gpk_gui_control.h"
#include "gpk_array.h"
#include "gpk_png.h"

nmsp dry
{
    GPK_USING_TYPEINT();
    GPK_USING_VIEW();
    GPK_USING_ARRAY();
    GPK_USING_ASTATIC();
    GPK_USING_N2();
    GPK_USING_TRI2();
    GPK_USING_RECT();
    GPK_USING_RECT2();
    GPK_USING_MINMAX2();

    CTTI_ENUM(SCREEN,
    enum SCREEN : u0_t 
        { SCREEN_ROOT           
        , SCREEN_HUD            
        , SCREEN_MENU           
        , SCREEN_MODAL          
        , SCREEN_HOME           
        , SCREEN_LOAD_FILAMENT  
        , SCREEN_INFO           
        , SCREEN_CONFIG         
        , SCREEN_COUNT
        };
    );
    //GDEFINE_ENUM_VALUE(SCREEN, , 4);

    GDEFINE_ENUM_TYPE(SCREEN_CONTROL_MENU, u0_t );
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, DIALOG                    , 0);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, HOME                    , 1);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, LOAD_FILAMENT            , 2);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, INFO                    , 3);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, CONFIG                    , 4);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, COUNT                    , 5);

    GDEFINE_ENUM_TYPE(SCREEN_CONTROL_HUD, u0_t );
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, DIALOG                    , 0);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, CALENDAR                    , 1);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, DOOR                        , 2);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, ETHERNET                    , 3);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, WIFI                        , 4);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, RFID                        , 5);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, SERIAL                    , 6);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, COUNT                    , 7);

    GDEFINE_ENUM_TYPE(SCREEN_CONTROL_MODAL, u0_t );
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, DIALOG                    , 0);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, CLOSE                    , 1);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, YES                    , 2);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, NO                        , 3);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, LOAD                    , 4);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, UNLOAD                    , 5);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, CANCEL                    , 6);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, ICON_START                , 7);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, ICON_STOP                , 8);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, COUNT                    , 9);

    GDEFINE_ENUM_TYPE(SCREEN_CONTROL_LOAD_FILAMENT, u0_t );
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_LOAD_FILAMENT, DIALOG            , 0);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_LOAD_FILAMENT, COUNT            , 1);

    GDEFINE_ENUM_TYPE(SCREEN_CONTROL_SPOOL_INFO, u0_t );
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, DIALOG            ,  0);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, TITLE                ,  1);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_BRAND        ,  2);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_MATERIAL    ,  3);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_COLOR        ,  4);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_NOTES        ,  5);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_DENSITY        ,  6);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_DIAMETER    ,  7);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_SPOOL_SIZE    ,  8);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_BRAND        ,  9);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_MATERIAL    , 10);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_COLOR        , 11);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_NOTES        , 12);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_DENSITY        , 13);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_DIAMETER    , 14);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_SPOOL_SIZE    , 15);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, COUNT                , 16);

    GDEFINE_ENUM_TYPE(SCREEN_CONTROL_INFO, u0_t );
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_INFO, DIALOG                    , 0);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_INFO, COUNT                    , 1);

    GDEFINE_ENUM_TYPE(SCREEN_CONTROL_CONFIG, u0_t );
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_CONFIG, DIALOG                , 0);
    GDEFINE_ENUM_VALUE(SCREEN_CONTROL_CONFIG, COUNT                    , 1);

    GDEFINE_ENUM_TYPE(MODAL, u0_t );
    GDEFINE_ENUM_VALUE(MODAL, CLOSE_DOOR        , 0);
    GDEFINE_ENUM_VALUE(MODAL, SCAN_SPOOL        , 1);
    GDEFINE_ENUM_VALUE(MODAL, DRYING_COMPLETE    , 2);
    GDEFINE_ENUM_VALUE(MODAL, DRYING_STOP        , 3);

    struct SControlParams {
        ::gpk::GUI_COLOR_MODE  ColorMode     ;
        ::gpk::cid_t           Id            ;
        ::gpk::cid_t           IdParent      ;
        b8_t                   Design        ;
        b8_t                   NoBackground  ;
        b8_t                   NoHoverEffect ;
        b8_t                   FrameOut      ;
        ::gpk::ALIGN           Align         ;
        rect2s1_t              Area          ;
        rectu1_t               Border        ;
        rectu1_t               Margin        ;
        vcst_t                 Text          ;
        ::gpk::ALIGN           TextAlign     ;
        ::gpk::ALIGN           IconAlign     ;
        vcst_t                 IconPathOff   ;
        vcst_t                 IconPathOn    ;
        n2b8_t                 AdjustToText  ;
    };
    CTTI_ENUM(PALETTE,
    enum PALETTE : u1_t
        { PALETTE_MENU_NORMAL                   = 400 + (u0_t)::gpk::GUI_CONTROL_PALETTE_NORMAL                                   
        , PALETTE_MENU_DISABLED                 = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_DISABLED             
        , PALETTE_MENU_HOVER                    = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_HOVER                
        , PALETTE_MENU_PRESSED                  = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_PRESSED              
        , PALETTE_MENU_DRAGGING                 = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_DRAGGING             
        , PALETTE_MENU_SELECTED                 = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED             
        , PALETTE_MENU_SELECTED_DISABLED        = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED     
        , PALETTE_MENU_SELECTED_HOVER           = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER       
        , PALETTE_MENU_SELECTED_PRESSED         = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED        
        , PALETTE_MENU_SELECTED_DRAGGING        = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_DRAGGING    
        , PALETTE_MENU_OUTDATED                 = PALETTE_MENU_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_OUTDATED             
        , PALETTE_START_NORMAL                  = 420 + (u0_t)::gpk::GUI_CONTROL_PALETTE_NORMAL                                   
        , PALETTE_START_DISABLED                = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_DISABLED             
        , PALETTE_START_HOVER                   = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_HOVER                 
        , PALETTE_START_PRESSED                 = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_PRESSED              
        , PALETTE_START_DRAGGING                = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_DRAGGING             
        , PALETTE_START_SELECTED                = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED              
        , PALETTE_START_SELECTED_DISABLED       = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED    
        , PALETTE_START_SELECTED_HOVER          = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER       
        , PALETTE_START_SELECTED_PRESSED        = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED     
        , PALETTE_START_SELECTED_DRAGGING       = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_DRAGGING    
        , PALETTE_START_OUTDATED                = PALETTE_START_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_OUTDATED             
        , PALETTE_CANCEL_NORMAL                 = 440 + (u0_t)::gpk::GUI_CONTROL_PALETTE_NORMAL                                   
        , PALETTE_CANCEL_DISABLED               = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_DISABLED                 
        , PALETTE_CANCEL_HOVER                  = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_HOVER                 
        , PALETTE_CANCEL_PRESSED                = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_PRESSED                 
        , PALETTE_CANCEL_DRAGGING               = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_DRAGGING                 
        , PALETTE_CANCEL_SELECTED               = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED              
        , PALETTE_CANCEL_SELECTED_DISABLED      = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED      
        , PALETTE_CANCEL_SELECTED_HOVER         = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER           
        , PALETTE_CANCEL_SELECTED_PRESSED       = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED         
        , PALETTE_CANCEL_SELECTED_DRAGGING      = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_DRAGGING        
        , PALETTE_CANCEL_OUTDATED               = PALETTE_CANCEL_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_OUTDATED                 
        , PALETTE_OK_NORMAL                     = 460 + (u0_t)::gpk::GUI_CONTROL_PALETTE_NORMAL                                   
        , PALETTE_OK_DISABLED                   = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_DISABLED                   
        , PALETTE_OK_HOVER                      = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_HOVER                   
        , PALETTE_OK_PRESSED                    = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_PRESSED                   
        , PALETTE_OK_DRAGGING                   = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_DRAGGING                   
        , PALETTE_OK_SELECTED                   = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED                   
        , PALETTE_OK_SELECTED_DISABLED          = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED       
        , PALETTE_OK_SELECTED_HOVER             = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER           
        , PALETTE_OK_SELECTED_PRESSED           = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED           
        , PALETTE_OK_SELECTED_DRAGGING          = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_SELECTED_DRAGGING                   
        , PALETTE_OK_OUTDATED                   = PALETTE_OK_NORMAL + (u0_t)::gpk::GUI_CONTROL_PALETTE_OUTDATED                   
        , PALETTE_COUNT                         = 1 + PALETTE_OK_OUTDATED                                                       
        };
    );

	::gpk::err_t					setupPalettes	(::gpk::SControlColors	& colors );
	struct SControlImages {
		::gpk::img8bgra		On	;
		::gpk::img8bgra		Off	;
	};
    
	template<size_t _countControls>
	struct SScreen {
		::gpk::astatic<::gpk::cid_t, _countControls>			ControlIds;
		::gpk::astatic<::dry::SControlImages, _countControls>	ControlIcons;
    
		inln											SScreen			()	{ ::gpk::view<::gpk::cid_t>{ControlIds}.fill(-1); }
	};
    
	template<typename _tScreen, size_t _countControls>
	sttc gpk::err_t						setupScreenFromControlParams	(::gpk::SControlTable & controls, ::gpk::cid_t rootControlIndex, _tScreen & screen, cnst ::dry::SControlParams (&controlSetupList)[_countControls], ::gpk::vs1_t defaultColors, ::gpk::b8_t loadImages = true)	{
		::gpk::SPNGData									pngCache				= {};
		for(u2_t  iControl = 0; iControl < _countControls; ++iControl) {
			cnst SControlParams							& controlSetup			= controlSetupList[iControl];
			cnst ::gpk::cid_t									controlIndex
				= screen.ControlIds[controlSetup.Id]	= ::gpk::controlCreate(controls, defaultColors);
    
			::gpk::SControlPlacement						& control				= controls.Placement	[controlIndex];
			::gpk::SControlText								& controlText			= controls.Text			[controlIndex];
			::gpk::SControlDraw								& controlDraw			= controls.Draw			[controlIndex];
			::gpk::SControlImage							& controlImage			= controls.Images		[controlIndex];
			::gpk::SControlState							& controlState			= controls.States		[controlIndex];
			::gpk::SControlConstraints						& controlConstraints	= controls.Constraints	[controlIndex];
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
			::gpk::controlSetParent(controls, controlIndex, (controlSetup.IdParent == -1) ? rootControlIndex : screen.ControlIds[controlSetup.IdParent]);
    
			if(loadImages && (controlSetup.IconPathOff == controlSetup.IconPathOn) && controlSetup.IconPathOn.size()) {
				if_fail_cef(::gpk::pngLoadFile(pngCache, controlSetup.IconPathOff, screen.ControlIcons[controlSetup.Id].Off), "Failed to load icon: '%.*s'", controlSetup.IconPathOff.begin());
				controlImage.Image							= screen.ControlIcons[controlSetup.Id].Off.View;
				screen.ControlIcons[controlSetup.Id].On.View	= screen.ControlIcons[controlSetup.Id].Off.View;
			}
			else if(loadImages) {
				if(controlSetup.IconPathOn.size()) {
					if_fail_cef(::gpk::pngLoadFile(pngCache, controlSetup.IconPathOn , screen.ControlIcons[controlSetup.Id].On), "Failed to load icon: '%.*s'", controlSetup.IconPathOn.begin());
					controlImage.Image							= screen.ControlIcons[controlSetup.Id].On.View;
				}
				if(controlSetup.IconPathOff.size()) {
					if_fail_cef(::gpk::pngLoadFile(pngCache, controlSetup.IconPathOff, screen.ControlIcons[controlSetup.Id].Off), "Failed to load icon: '%.*s'", controlSetup.IconPathOff.begin());
					controlImage.Image							= screen.ControlIcons[controlSetup.Id].Off.View;
				}
			}
		}
		return 0;
	}

} // nmsp

#define USE_3D_CONTROLS

#if defined USE_3D_CONTROLS
#	define DRY_MENU_COLOR_MODE	::gpk::GUI_COLOR_MODE_3D
#	define BACKGROUND_3D_ONLY	false
#else
#	define DRY_MENU_COLOR_MODE ::gpk::GUI_COLOR_MODE_FLAT
#	define BACKGROUND_3D_ONLY	true
#endif

nmsp dry
{
    stxp cnst ::dry::SControlParams            control_list_hud            []    =
        {    {DRY_MENU_COLOR_MODE           , ::dry::SCREEN_CONTROL_HUD_DIALOG        , -1                                ,  true, true, false, false, ::gpk::ALIGN_CENTER_TOP    , {{}, {0, 40}}, {1, 1, 1, 1}, {4, 4, 4, 4}}
        ,    {::gpk::GUI_COLOR_MODE_Flat    , ::dry::SCREEN_CONTROL_HUD_DOOR        , ::dry::SCREEN_CONTROL_HUD_DIALOG    , false, true, false, false, ::gpk::ALIGN_CENTER_RIGHT    , {{ 10, 0}, {48, 40}}, {}, {}, ""                , ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_lock-off.024.png"        , "../smart-data/icons/icon_lock-on.024.png"        , {}}
        ,    {::gpk::GUI_COLOR_MODE_Flat    , ::dry::SCREEN_CONTROL_HUD_ETHERNET    , ::dry::SCREEN_CONTROL_HUD_DIALOG    , false, true, false, false, ::gpk::ALIGN_CENTER_LEFT    , {{  0, 0}, {48, 40}}, {}, {}, ""                , ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_ethernet-off.024.png"    , "../smart-data/icons/icon_ethernet-on.024.png"    , {}}
        ,    {::gpk::GUI_COLOR_MODE_Flat    , ::dry::SCREEN_CONTROL_HUD_WIFI        , ::dry::SCREEN_CONTROL_HUD_DIALOG    , false, true, false, false, ::gpk::ALIGN_CENTER_LEFT    , {{ 48, 0}, {48, 40}}, {}, {}, ""                , ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon-wifi-01.024.png"        , "../smart-data/icons/icon-wifi-01.024.png"        , {}}
        ,    {::gpk::GUI_COLOR_MODE_Flat    , ::dry::SCREEN_CONTROL_HUD_RFID        , ::dry::SCREEN_CONTROL_HUD_DIALOG    , false, true, false, false, ::gpk::ALIGN_CENTER_LEFT    , {{ 96, 0}, {48, 40}}, {}, {}, ""                , ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_rfid.024.png"            , "../smart-data/icons/icon_rfid.024.png"            , {}}
        ,    {::gpk::GUI_COLOR_MODE_Flat    , ::dry::SCREEN_CONTROL_HUD_SERIAL        , ::dry::SCREEN_CONTROL_HUD_DIALOG    , false, true, false, false, ::gpk::ALIGN_CENTER_LEFT    , {{144, 0}, {48, 40}}, {}, {}, "Serial"        , ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "", "", {true,}}
        ,    {::gpk::GUI_COLOR_MODE_Flat    , ::dry::SCREEN_CONTROL_HUD_CALENDAR    , ::dry::SCREEN_CONTROL_HUD_DIALOG    , false, true, false, false, ::gpk::ALIGN_CENTER        , {{  0, 0}, {32, 40}}, {}, {}, "Jan 01 | 23:59", ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "", "", {true,}}
        };
    stxp cnst ::dry::SControlParams            control_list_menu            []    =
        {    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_DIALOG            , -1                                ,  true, true                , false, false, ::gpk::ALIGN_CENTER_BOTTOM    , {{}, {  0, 24}}, {0, 1, 0, 1}, {}}
        //,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_HOME            , ::dry::SCREEN_CONTROL_MENU_DIALOG    , false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT    , {{0, 0}            , {128, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Simplified View", ::gpk::ALIGN_CENTER}
        //,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_HOME            , ::dry::SCREEN_CONTROL_MENU_DIALOG    , false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT    , {{0, 0}            , { 88, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Home"            , ::gpk::ALIGN_CENTER_RIGHT, ::gpk::ALIGN_LEFT, "../smart-data/icons/icon_menu-home.024.png"    }
        //,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_LOAD_FILAMENT    , ::dry::SCREEN_CONTROL_MENU_DIALOG    , false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT    , {{88, 0}            , {168, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Load filament"    , ::gpk::ALIGN_CENTER_RIGHT, ::gpk::ALIGN_LEFT, "../smart-data/icons/icon_menu-filament.024.png"}
        //,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_INFO            , ::dry::SCREEN_CONTROL_MENU_DIALOG    , false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT    , {{88+168, 0}        , { 88, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Info"            , ::gpk::ALIGN_CENTER_RIGHT, ::gpk::ALIGN_LEFT, "../smart-data/icons/icon_menu-info.024.png"    }
        //,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_CONFIG            , ::dry::SCREEN_CONTROL_MENU_DIALOG    , false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT    , {{88+168+88, 0}    , {104, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Config"            , ::gpk::ALIGN_CENTER_RIGHT, ::gpk::ALIGN_LEFT, "../smart-data/icons/icon_menu-config.024.png"    }
        };
    stxp cnst ::dry::SControlParams            control_list_modal            []    =
        {    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_DIALOG        , -1                                , false, false,  true,  true, ::gpk::ALIGN_CENTER        , {{}, {}}, {2, 2, 2, 2}, {}, "Close the door correctly to start drying"    , ::gpk::ALIGN_CENTER}
        ,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_ICON_START    , ::dry::SCREEN_CONTROL_MODAL_DIALOG, true,  true, false, false    , ::gpk::ALIGN_CENTER_TOP    , {{  0, 16}, {48, 48}}, {}, {}, "", ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_play.032.png"}
        ,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_ICON_STOP        , ::dry::SCREEN_CONTROL_MODAL_DIALOG, true,  true, false, false    , ::gpk::ALIGN_CENTER_TOP    , {{  0, 16}, {48, 48}}, {}, {}, "", ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_stop.032.png"}
        ,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_YES            , ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{-64, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "Yes"    , ::gpk::ALIGN_CENTER}
        ,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_NO            , ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{ 64, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "No"        , ::gpk::ALIGN_CENTER}
        ,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_CLOSE            , ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{  0, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "OK"        , ::gpk::ALIGN_CENTER}
        ,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_LOAD            , ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{-96, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "Load"    , ::gpk::ALIGN_CENTER}
        ,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_UNLOAD        , ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{-96, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "Unload"    , ::gpk::ALIGN_CENTER}
        ,    {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_CANCEL        , ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{ 96, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "Cancel"    , ::gpk::ALIGN_CENTER}
        };
    stxp cnst ::dry::SControlParams            control_list_spool_info        []    =
        {     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG            , -1,  true, true, true, true, ::gpk::ALIGN_CENTER_TOP}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_TITLE            , ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, true, false, true, true, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 0        }, { 64, 30}}, {3, 3, 3, 3}, {}, "Spool Scanned!", ::gpk::ALIGN_CENTER}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_BRAND        , ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 1 + 6    }, {256, 24}}, {1, 1, 1, 1}, {2,}, "Brand"        , ::gpk::ALIGN_CENTER_LEFT}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_MATERIAL    , ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 2 + 6    }, {256, 24}}, {1, 1, 1, 1}, {2,}, "Material"    , ::gpk::ALIGN_CENTER_LEFT}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_COLOR        , ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 3 + 6    }, {256, 24}}, {1, 1, 1, 1}, {2,}, "Color"        , ::gpk::ALIGN_CENTER_LEFT}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_SPOOL_SIZE    , ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 4 + 6    }, {256, 24}}, {1, 1, 1, 1}, {2,}, "Spool Size"        , ::gpk::ALIGN_CENTER_LEFT}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DENSITY    , ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 5 + 6    }, {256, 24}}, {1, 1, 1, 1}, {2,}, "Density"    , ::gpk::ALIGN_CENTER_LEFT}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DIAMETER    , ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 6 + 6    }, {256, 24}}, {1, 1, 1, 1}, {2,}, "Diameter"    , ::gpk::ALIGN_CENTER_LEFT}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_NOTES        , ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 7 + 6    }, {256, 24}}, {1, 1, 1, 1}, {2,}, "Notes"    , ::gpk::ALIGN_CENTER_LEFT}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_BRAND        , ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_BRAND        , true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "Kodak"                , ::gpk::ALIGN_CENTER}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_MATERIAL    , ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_MATERIAL    , true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "ABS"                    , ::gpk::ALIGN_CENTER}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_COLOR        , ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_COLOR        , true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "Red"                    , ::gpk::ALIGN_CENTER}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_NOTES        , ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_NOTES        , true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "Filament prototype"    , ::gpk::ALIGN_CENTER}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_DENSITY    , ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DENSITY    , true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "?"                    , ::gpk::ALIGN_CENTER}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_DIAMETER    , ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DIAMETER    , true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "1.75mm"                , ::gpk::ALIGN_CENTER}
        ,     {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_SPOOL_SIZE    , ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_SPOOL_SIZE    , true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "750g"                    , ::gpk::ALIGN_CENTER}
        };
    stxp cnst ::dry::SControlParams            control_list_load_filament    []    =
        {    {::gpk::GUI_COLOR_MODE_Flat, ::dry::SCREEN_CONTROL_LOAD_FILAMENT_DIALOG    , -1,  true, true}
        };
    stxp cnst ::dry::SControlParams            control_list_info            []    =
        {    {::gpk::GUI_COLOR_MODE_Flat, ::dry::SCREEN_CONTROL_INFO_DIALOG    , -1,  true, true}
        };
    stxp cnst ::dry::SControlParams            control_list_config            []    =
        {    {::gpk::GUI_COLOR_MODE_Flat, ::dry::SCREEN_CONTROL_CONFIG_DIALOG, -1,  true, true}
        };

} // nmsp
