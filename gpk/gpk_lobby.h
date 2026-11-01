#include "gpk_udp_server.h"
#include "gpk_udp_client.h"
#include "gpk_framework.h"
#include "gpk_gui_inputbox.h"
#include "gpk_dialog.h"
#include "gpk_gui_control_list.h"
#include "gpk_label.h"
#include "gpk_chrono.h"

#include <mutex>

#ifndef GPK_TEST_LOBBY_H_23627
#define GPK_TEST_LOBBY_H_23627

namespace gpk
{

	struct SLobbyRoomStats {
		u3_t			UsersConnected;
		u3_t			UsersEverJoined;
	};

	struct SLobbyRoom {
		::gpk::view<sc_t>			Name;
		::gpk::as3_t			Moderators;
		::gpk::as3_t			Users;
	};

	struct SLobbyUser {
		::gpk::view<sc_t>			Mail;
		::gpk::view<sc_t>			Pass;
	};

	struct SLobbyUserInfo {
		::gpk::view<sc_t>			Name;
	};

	struct SLobbyServer {
		typedef	::gpk::apobj<::gpk::SUDPMessage> TMessageQueue;

		::gpk::au3_t								IdListRoom			= {};
		::gpk::au3_t								IdListUsers			= {};

		::gpk::SUDPServer						Server				= {};

		::gpk::aobj<::gpk::SLobbyUser		>	Credentials			= {};
		::gpk::aobj<::gpk::SLobbyUserInfo	>	InfoUser			= {};
		::gpk::aobj<::gpk::SLobbyRoom		>	InfoRoom			= {};

		::gpk::SDialog							Dialog				= {};
		::gpk::SVirtualKeyboard					VirtualKeyboard		= {};

		::gpk::aobj<TMessageQueue>				MessagesToProcess	= {};	// A queue per client.

		::gpk::err_t							Stop				()												{ return ::gpk::serverStop(Server); }
		::gpk::err_t							Start				(u1_t portNumber, s1_t adapterIndex=0)	{ return ::gpk::serverStart(Server, portNumber, adapterIndex); }
		::gpk::err_t							Update				()	{ return 0; }
	};

	GDEFINE_ENUM_TYPE(UI_LOBBY_USER, u0_t);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Offline	, 0);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Mail		, 1);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Name		, 2);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Room		, 3);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, Pass		, 4);
	GDEFINE_ENUM_VALUE(UI_LOBBY_USER, GUID		, 5);

	GDEFINE_ENUM_TYPE(UI_LOBBY_LIST, u0_t);
	GDEFINE_ENUM_VALUE(UI_LOBBY_LIST, Create, 0);


	struct SLobbyClient {
		typedef	::gpk::apobj<::gpk::SUDPMessage> TMessageQueue;

		::gpk::SUDPClient					Client				= {};
		::gpk::SDialog						Dialog				= {};
		::gpk::SInputBox					InputBox			= {};
		UI_LOBBY_USER						Field				= UI_LOBBY_USER_Offline;

		u3_t							IdRoom				= 0;
		u3_t							IdUser				= 0;

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
			if_true_e(failed(::gpk::guiCreateControlList<::gpk::UI_LOBBY_USER>(gui, Dialog.Root, {240U, 22U}, {}, ::gpk::ALIGN_TOP_RIGHT, ::gpk::ALIGN_TOP_RIGHT, controls)));
			if_true_e(failed(::gpk::inputBoxCreate(InputBox, gui, Dialog.Root)));
		}

		::gpk::err_t						Connect				() { 
			Client.State								= ::gpk::UDP_CONNECTION_STATE_HANDSHAKE;
			if(NetworkThread.joinable()) {
				NetworkThread.join();
			}

			if(Client.State == ::gpk::UDP_CONNECTION_STATE_IDLE)
				return 0;
			NetworkThread								= ::std::thread([this] { 
				if_fail_fe(gpk::clientConnect(Client)); 
				return ::gpk::controlTextSet(*Dialog.GUI, Dialog.Root + 1, "Connected");
			});
			return 0;
		}

		::gpk::err_t								Update				(const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> frameEvents) { 
			if(!Dialog.Input) {
				Dialog.Input								= inputState;
			}
			::gpk::SGUI										 & gui				= *Dialog.GUI;
			static const ::gpk::vcst_t							connectingString	= {"Connecting..."};
			switch(Client.State) {
			case ::gpk::UDP_CONNECTION_STATE_IDLE			: break;
			case ::gpk::UDP_CONNECTION_STATE_DISCONNECTED	: 
				if_fail_fe(gpk::controlTextSet(gui, Dialog.Root + 1, "Disconnected"));
				Connect(); 
				break;
			case ::gpk::UDP_CONNECTION_STATE_HANDSHAKE		: 
				if_fail_fe(gpk::controlTextSet(gui, Dialog.Root + 1, {connectingString.begin(), connectingString.size() - 3 + ((u2_t)::gpk::timeCurrent() % 4)}));
				break;
			}

			if_fail_fe(gpk::guiProcessInput(gui, *inputState, frameEvents));

			::gpk::acid				controlsToProcess			= {};
			if_fail_fe(gpk::guiGetProcessableControls(gui, controlsToProcess));
			if(const s2_t result = InputBox.Update(gui, InputBox.VirtualKeyboard, frameEvents, controlsToProcess)) {
				if(result == INT_MAX) { 
					::gpk::view<sc_t>						trimmed			= ::gpk::view<sc_t>{InputBox.Text};
					::gpk::trim(trimmed);
					::gpk::controlTextSet(gui, Dialog.Root + 1 + Field, ::gpk::label{trimmed});
					InputBox.Edit(gui, false);
					gui.Controls.SetHidden(InputBox.IdRoot, true);
				}
			}
			else {
				if_fail_fe(gpk::guiProcessControls(gui, controlsToProcess, [&](s2_t iControl) {
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
				for(u2_t iMessage = 0; iMessage < Client.Queue.Received.size(); ++iMessage) {
					if_fail_fef(MessagesToProcess.push_back(Client.Queue.Received[iMessage]), "%s", "Out of memory?");
				}
				Client.Queue.Received.clear();
			}
			for(u2_t iMessage = 0; iMessage < MessagesToProcess.size(); ++iMessage) {
				::gpk::vcu0_t							viewPayload					= MessagesToProcess[iMessage]->Payload;
				info_printf("Client received: %s.", viewPayload.begin());
			}
			return 0;
		}

		::gpk::err_t								Draw				(::gpk::g8bgra target) { 
			return ::gpk::guiDraw(*Dialog.GUI, target);
		}
	};

} // namespace

#endif // GPK_TEST_LOBBY_H_23627
