#include "gpk_udp_server.h"
#include "gpk_udp_client.h"
#include "gpk_framework.h"
#include "gpk_gui_inputbox.h"
#include "gpk_dialog.h"
#include "gpk_gui_control_list.h"
#include "gpk_label.h"

#include <mutex>

#ifndef GPK_TEST_LOBBY_H_23627
#define GPK_TEST_LOBBY_H_23627

namespace gpk
{

	struct SLobbyRoomStats {
		uint64_t			UsersConnected;
		uint64_t			UsersEverJoined;
	};

	struct SLobbyRoom {
		::gpk::vcc			Name;
		::gpk::ai64			Moderators;
		::gpk::ai64			Users;
	};

	struct SLobbyUser {
		::gpk::vcc			Mail;
		::gpk::vcc			Pass;
	};

	struct SLobbyUserInfo {
		::gpk::vcc			Name;
	};

	struct SLobbyServer {
		typedef	::gpk::apobj<::gpk::SUDPMessage> TMessageQueue;

		::gpk::au64								IdListRoom			= {};
		::gpk::au64								IdListUsers			= {};

		::gpk::SUDPServer						Server				= {};

		::gpk::aobj<::gpk::SLobbyUser		>	Credentials			= {};
		::gpk::aobj<::gpk::SLobbyUserInfo	>	InfoUser			= {};
		::gpk::aobj<::gpk::SLobbyRoom		>	InfoRoom			= {};

		::gpk::SDialog							Dialog				= {};
		::gpk::SVirtualKeyboard					VirtualKeyboard		= {};

		::gpk::aobj<TMessageQueue>				MessagesToProcess	= {};	// A queue per client.

		::gpk::error_t							Stop				()												{ return ::gpk::serverStop(Server); }
		::gpk::error_t							Start				(uint16_t portNumber, int16_t adapterIndex=0)	{ return ::gpk::serverStart(Server, portNumber, adapterIndex); }
		::gpk::error_t							Update				()	{ return 0; }
	};

	GDEFINE_ENUM_TYPE(UI_LOBBY_USER, uint8_t);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Offline	, 0);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Mail		, 1);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Name		, 2);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Room		, 3);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Pass		, 4);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, GUID		, 5);

	GDEFINE_ENUM_TYPE(UI_LOBBY_LIST, uint8_t);
	GDEFINE_ENUM_VALUE(UI_LOBBY_LIST, Create, 0);


	struct SLobbyClient {
		typedef	::gpk::apobj<::gpk::SUDPMessage> TMessageQueue;

		::gpk::SUDPClient					Client				= {};
		::gpk::SDialog						Dialog				= {};
		::gpk::SInputBox					InputBox			= {};
		UI_LOBBY_USER						Field				= UI_LOBBY_USER_Offline;

		uint64_t							IdRoom				= 0;
		uint64_t							IdUser				= 0;

		::gpk::SLobbyUser					Credentials			= {};
		::gpk::SLobbyUserInfo				InfoUser			= {};
		::gpk::SLobbyRoom					Room				= {};

		::gpk::apobj<::gpk::SLobbyUserInfo>	InfoRoomUsers		= {};
		::gpk::apobj<::gpk::SLobbyUserInfo>	InfoRoomModerators	= {};

		TMessageQueue						MessagesToProcess	= {};

		::std::thread						NetworkThread;
		 
											~SLobbyClient		()	{
			NetworkThread.join();
			NetworkThread								= {};
			::gpk::clientDisconnect(Client);
		}

											SLobbyClient		()	{
			::gpk::SGUI								 & gui				= *Dialog.GUI;
			gui.Controls.Draw[Dialog.Root].NoBorder = gui.Controls.Draw[Dialog.Root].NoClient = true;
			::gpk::acid	controls;
			e_if(errored(::gpk::guiCreateControlList<::gpk::UI_LOBBY_USER>(gui, Dialog.Root, {240U, 22U}, {}, ::gpk::ALIGN_TOP_RIGHT, ::gpk::ALIGN_TOP_RIGHT, controls)));
			e_if(errored(::gpk::inputBoxCreate(InputBox, gui, Dialog.Root)));
		}

		::gpk::error_t						Connect				() { 
			Client.State								= ::gpk::UDP_CONNECTION_STATE_HANDSHAKE;
			if(NetworkThread.joinable()) {
				NetworkThread.join();
			}

			if(Client.State == ::gpk::UDP_CONNECTION_STATE_IDLE)
				return 0;
			NetworkThread								= ::std::thread([this] { 
				gpk_necs(::gpk::clientConnect(Client)); 
				return ::gpk::controlTextSet(*Dialog.GUI, Dialog.Root + 1, "Connected");
			});
			return 0;
		}

		::gpk::error_t								Update				(const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> frameEvents) { 
			if(!Dialog.Input) {
				Dialog.Input								= inputState;
			}
			::gpk::SGUI										 & gui				= *Dialog.GUI;
			static const ::gpk::vcs							connectingString	= {"Connecting..."};
			switch(Client.State) {
			case ::gpk::UDP_CONNECTION_STATE_IDLE			: break;
			case ::gpk::UDP_CONNECTION_STATE_DISCONNECTED	: 
				gpk_necs(::gpk::controlTextSet(gui, Dialog.Root + 1, "Disconnected"));
				Connect(); 
				break;
			case ::gpk::UDP_CONNECTION_STATE_HANDSHAKE		: 
				gpk_necs(::gpk::controlTextSet(gui, Dialog.Root + 1, {connectingString.begin(), connectingString.size() - 3 + ((uint32_t)::gpk::timeCurrent() % 4)}));
				break;
			}

			gpk_necs(::gpk::guiProcessInput(gui, *inputState, frameEvents));

			::gpk::acid				controlsToProcess			= {};
			gpk_necs(::gpk::guiGetProcessableControls(gui, controlsToProcess));
			if(const int32_t result = InputBox.Update(gui, InputBox.VirtualKeyboard, frameEvents, controlsToProcess)) {
				if(result == INT_MAX) { 
					::gpk::vcc						trimmed			= ::gpk::vcc{InputBox.Text};
					::gpk::trim(trimmed);
					::gpk::controlTextSet(gui, Dialog.Root + 1 + Field, ::gpk::label{trimmed});
					InputBox.Edit(gui, false);
					gui.Controls.SetHidden(InputBox.IdRoot, true);
				}
			}
			else {
				gpk_necs(::gpk::guiProcessControls(gui, controlsToProcess, [&](int32_t iControl) {
					const ::gpk::SControlPlacement			& control		= gui.Controls.Placement[iControl];
					::gpk::SControlPlacement				& inputBox		= gui.Controls.Placement[InputBox.IdRoot];
					bool							edit			= false;
					switch(iControl - (Dialog.Root + 1)) {
					default								: 
					case ::gpk::UI_LOBBY_USER_GUID		: 
					case ::gpk::UI_LOBBY_USER_Offline	: 
						gui.Controls.SetHidden(InputBox.IdRoot, true);
						break;
					case ::gpk::UI_LOBBY_USER_Mail		: 
					case ::gpk::UI_LOBBY_USER_Name		: 
					case ::gpk::UI_LOBBY_USER_Room		: 
					case ::gpk::UI_LOBBY_USER_Pass		: 
						inputBox.Area.Offset		= control.Area.Offset; 
						inputBox.Align				= control.Align; 
						::gpk::controlMetricsInvalidate(gui, InputBox.IdRoot);
						InputBox.SetText(gui, gui.Controls.Text[iControl].Text);
						edit						= true;
						Field						= (::gpk::UI_LOBBY_USER)(iControl - (Dialog.Root + 1));
						gui.Controls.SetHidden(InputBox.IdRoot, false);
						break;
					}
					InputBox.Edit(gui, edit);
					return 0;
				}));
			}
			{
				::std::lock_guard								lockRecv					(Client.Queue.MutexReceive);
				for(uint32_t iMessage = 0; iMessage < Client.Queue.Received.size(); ++iMessage) {
					gpk_necall(MessagesToProcess.push_back(Client.Queue.Received[iMessage]), "%s", "Out of memory?");
				}
				Client.Queue.Received.clear();
			}
			for(uint32_t iMessage = 0; iMessage < MessagesToProcess.size(); ++iMessage) {
				::gpk::vcu8							viewPayload					= MessagesToProcess[iMessage]->Payload;
				info_printf("Client received: %s.", viewPayload.begin());
			}
			return 0;
		}

		::gpk::error_t								Draw				(::gpk::g8bgra target) { 
			return ::gpk::guiDraw(*Dialog.GUI, target);
		}
	};

} // namespace

#endif // GPK_TEST_LOBBY_H_23627
