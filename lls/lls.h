#include "lls_game.h"

#include "gpk_img_serialize.h"
#include "gpk_engine.h"
#include "gpk_gui.h"
#include "gpk_gui_inputbox.h"
#include "gpk_dialog.h"
#include "gpk_deflate.h"
#include "gpk_array_ptr.h"
#include "gpk_noise.h"

#include <ctime>

#ifndef SSIEGE_H_23701
#define SSIEGE_H_23701

namespace lls
{
	GDEFINE_ENUM_TYPE (UI_PROFILE, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Name , 0);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Score, 1);

	struct SUIPlayer {
		::gpk::SDialog			DialogHome			= {};
		::gpk::acid				ControlsDialogHome	= {};
		::gpk::SInputBox		InputBox			= {};
		::gpk::astsc_t<128>		TextScore			= {};
	};

	struct SUIPlay {
		::gpk::aobj<::lls::SUIPlayer>	PlayerUI		= {};
	};

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_QUICK, SAVE_MODE_STAGE, SAVE_MODE_USER };

	struct SFileStrings {
		::gpk::vcst_t				SavegameFolder				= ".";
		::gpk::vcst_t				ExtensionImages				= ".png";
		::gpk::vcst_t				ExtensionProfile			= ".lls";
		::gpk::vcst_t				ExtensionCredentials		= ".llsc";
		::gpk::vcst_t				ExtensionSave				= ".llsz";
		::gpk::vcst_t				ExtensionSaveUser			= ".user.llsz";
		::gpk::vcst_t				ExtensionSaveAuto			= ".auto.llsz";
		::gpk::vcst_t				ExtensionSaveStage			= ".stage.llsz";
		::gpk::vcst_t				ExtensionSaveQuick			= ".quick.llsz";

		::gpk::err_t			GetSaveGameExtension		(SAVE_MODE saveMode, ::gpk::vcst_t & extension)	const	{
			switch(saveMode) {
			case SAVE_MODE_USER		: extension = ExtensionSaveUser	; break;
			case SAVE_MODE_STAGE	: extension = ExtensionSaveStage; break;
			case SAVE_MODE_QUICK	: extension = ExtensionSaveQuick; break;
			case SAVE_MODE_AUTO		: extension = ExtensionSaveAuto	; break;
			}
			return 0;
		}

		::gpk::err_t			GetPlayerFilePath			(const uint64_t time, const ::gpk::vcst_t & playerName, ::gpk::asc_t & playerFilePath)	const	{
			::gpk::au0_t					b64PlayerName				= {};
			do { if_fail_fe(::gpk::base64Encode(playerName, b64PlayerName)); } while(0);
			do { if_fail_fe(playerFilePath.append(SavegameFolder)); } while(0);
			do { if_fail_fe(playerFilePath.append_string("/")); } while(0);
			do { if_fail_fe(playerFilePath.append(b64PlayerName.cc())); } while(0);

			sc_t						postfix[64]					= {};
			sprintf_s(postfix, "%llu%s", time, ExtensionProfile.begin());
			return playerFilePath.append_string(postfix);
		}
	};

	struct LLSUI {
		::gpk::cid_t			Root				= ::gpk::CID_INVALID;
		::gpk::SInputBox		UserInput			= {};
		::gpk::acid				RootPerState		= {};
		::gpk::apachar			InputHistory		= {};
		::gpk::apachar			InputQueue			= {};
	};

	::gpk::err_t			setupLLSUI			(::gpk::SGUI & gui, ::lls::LLSUI & llsUI);


	GDEFINE_ENUM_TYPE(APP_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(APP_STATE, Init		,  0);
	GDEFINE_ENUM_VALUE(APP_STATE, Welcome	,  1);
	GDEFINE_ENUM_VALUE(APP_STATE, Home		,  2);
	GDEFINE_ENUM_VALUE(APP_STATE, Profile	,  3);
	GDEFINE_ENUM_VALUE(APP_STATE, Shop		,  4);
	GDEFINE_ENUM_VALUE(APP_STATE, Play		,  5);
	GDEFINE_ENUM_VALUE(APP_STATE, Brief		,  6);
	GDEFINE_ENUM_VALUE(APP_STATE, Stage		,  7);
	GDEFINE_ENUM_VALUE(APP_STATE, Stats		,  8);
	GDEFINE_ENUM_VALUE(APP_STATE, Store		,  9);
	GDEFINE_ENUM_VALUE(APP_STATE, Score		, 10);
	GDEFINE_ENUM_VALUE(APP_STATE, About		, 11);
	GDEFINE_ENUM_VALUE(APP_STATE, Settings	, 12);
	GDEFINE_ENUM_VALUE(APP_STATE, Quit		, 13);

	struct SPlayer {
		llsid_t					Id					= SSIEGEID_INVALID;
		::gpk::vcst_t				User				= {};
		::gpk::vcst_t				Name				= {};
	};

	struct SPlayerState {
		uint32_t				Stage				= 0;
	};

	struct LLSApp {
		APP_STATE				ActiveState			= {};

		::lls::LLSGame		Game;

		//SWorldView				World				= {};

		LLSUI				UI					= {};
		gpk::apobj<EventLLS>	EventQueue			= {};

		SFileStrings			FileStrings			= {};


		::gpk::pobj<gpk::SGUI>	GUI;
		::gpk::prtbgra8d32		RenderTargetWorld;
		::gpk::prtbgra8d32		RenderTargetUI;

		::gpk::err_t			StateSwitch			(APP_STATE newState);

		::gpk::err_t			Save				()	const	{
			rni_if(Game.Controlled >= Game.Engine.Entities.size(), "No active player character! %i >= %i", Game.Controlled, Game.Engine.Entities.size());
				
			::gpk::asc_t				playerPath;
			do { if_fail_fe(FileStrings.GetPlayerFilePath(::gpk::timeCurrent(), Game.Players[0].Name, playerPath)); } while(0);
			return Save(playerPath);
		}

		::gpk::err_t			Save				(::gpk::vcst_t fileName)	const	{
			::gpk::au0_t					serialized;
			do { if_fail_fe(Game.World.Save(serialized)); } while(0);
			return ::gpk::deflateFromMemory(fileName, serialized);
		}

		::gpk::err_t			Load				(::gpk::vcst_t filename)			{
			::gpk::au0_t					serialized;
			do { if_fail_fe(::gpk::inflateToMemory(filename, serialized)); } while(0);
			::gpk::vcu0_t					viewSerialized		= serialized;
			return Game.World.Load(viewSerialized);
		}
	};

	::gpk::err_t			llsUpdate		(::lls::LLSApp & lls, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents
		, ::gpk::FBool<::gpk::pobj<::lls::EventLLS> &, ::gpk::apobj<::lls::EventLLS> &> funcHandleEvent
		);
	::gpk::err_t			llsDraw			(::lls::LLSApp & lls, ::gpk::rtbgra8d32 & backBuffer, bool onlyGUI);
} // namespace lls

#endif // SSIEGE_H_23701
