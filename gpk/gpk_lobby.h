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
		vcst_t			Name;
		as3_t			Moderators;
		as3_t			Users;
	};

	struct SLobbyUser {
		vcst_t			Mail;
		vcst_t			Pass;
	};

	struct SLobbyUserInfo {
		vcst_t			Name;
	};

	struct SLobbyServer {
		typedef	apobj<SUDPMessage> TMessageQueue;

		au3_t								IdListRoom			= {};
		au3_t								IdListUsers			= {};

		SUDPServer						Server				= {};

		aobj<SLobbyUser		>	Credentials			= {};
		aobj<SLobbyUserInfo	>	InfoUser			= {};
		aobj<SLobbyRoom		>	InfoRoom			= {};

		SDialog							Dialog				= {};
		SVirtualKeyboard					VirtualKeyboard		= {};

		aobj<TMessageQueue>				MessagesToProcess	= {};	// A queue per client.

		gpk::err_t 						Stop				()												{ return serverStop(Server); }
		gpk::err_t 						Start				(u1_t portNumber, s1_t adapterIndex=0)	{ return serverStart(Server, portNumber, adapterIndex); }
		gpk::err_t 						Update				()	{ return 0; }
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
		typedef	apobj<SUDPMessage> TMessageQueue;

		SUDPClient					Client				= {};
		SDialog						Dialog				= {};
		SInputBox					InputBox			= {};
		UI_LOBBY_USER						Field				= UI_LOBBY_USER_Offline;

		u3_t							IdRoom				= 0;
		u3_t							IdUser				= 0;

		SLobbyUser					Credentials			= {};
		SLobbyUserInfo				InfoUser			= {};
		SLobbyRoom					Room				= {};

		apobj<SLobbyUserInfo>	InfoRoomUsers		= {};
		apobj<SLobbyUserInfo>	InfoRoomModerators	= {};

		TMessageQueue						MessagesToProcess	= {};

		::std::thread						NetworkThread;
		 
											~SLobbyClient		()	{
			NetworkThread.join();
			NetworkThread								= {};
			clientDisconnect(Client);
		}

											SLobbyClient		()	{
			SGUI								 & gui				= *Dialog.GUI;
			gui.Controls.Draw[Dialog.Root].NoBorder = gui.Controls.Draw[Dialog.Root].NoClient = true;
			acid	controls;
			if_true_e(failed(guiCreateControlList<UI_LOBBY_USER>(gui, Dialog.Root, {240U, 22U}, {}, ALIGN_TOP_RIGHT, ALIGN_TOP_RIGHT, controls)));
			if_true_e(failed(inputBoxCreate(InputBox, gui, Dialog.Root)));
		}

		gpk::err_t 					Connect				() { 
			Client.State								= UDP_CONNECTION_STATE_HANDSHAKE;
			if(NetworkThread.joinable()) {
				NetworkThread.join();
			}

			if(Client.State == UDP_CONNECTION_STATE_IDLE)
				return 0;
			NetworkThread								= ::std::thread([this] { 
				if_fail_fe(gpk::clientConnect(Client)); 
				return controlTextSet(*Dialog.GUI, Dialog.Root + 1, "Connected");
			});
			return 0;
		}

		gpk::err_t 							Update				(const pobj<SInput> & inputState, vpobj<SEventSystem> frameEvents) { 
			if(!Dialog.Input) {
				Dialog.Input								= inputState;
			}
			SGUI										 & gui				= *Dialog.GUI;
			static const vcst_t							connectingString	= {"Connecting..."};
			switch(Client.State) {
			case UDP_CONNECTION_STATE_IDLE			: break;
			case UDP_CONNECTION_STATE_DISCONNECTED	: 
				if_fail_fe(gpk::controlTextSet(gui, Dialog.Root + 1, "Disconnected"));
				Connect(); 
				break;
			case UDP_CONNECTION_STATE_HANDSHAKE		: 
				if_fail_fe(gpk::controlTextSet(gui, Dialog.Root + 1, {connectingString.begin(), connectingString.size() - 3 + ((u2_t)timeCurrent() % 4)}));
				break;
			}

			if_fail_fe(gpk::guiProcessInput(gui, *inputState, frameEvents));

			acid				controlsToProcess			= {};
			if_fail_fe(gpk::guiGetProcessableControls(gui, controlsToProcess));
			if(const s2_t result = InputBox.Update(gui, InputBox.VirtualKeyboard, frameEvents, controlsToProcess)) {
				if(result == INT_MAX) { 
					vcst_t						trimmed			= vcst_t{InputBox.Text};
					trim(trimmed);
					controlTextSet(gui, Dialog.Root + 1 + Field, label{trimmed});
					InputBox.Edit(gui, false);
					gui.Controls.SetHidden(InputBox.IdRoot, true);
				}
			}
			else {
				if_fail_fe(gpk::guiProcessControls(gui, controlsToProcess, [&](s2_t iControl) {
					const SControlPlacement			& control		= gui.Controls.Placement[iControl];
					SControlPlacement				& inputBox		= gui.Controls.Placement[InputBox.IdRoot];
					bool							edit			= false;
					switch(iControl - (Dialog.Root + 1)) {
					default								: 
					case UI_LOBBY_USER_GUID		: 
					case UI_LOBBY_USER_Offline	: 
						gui.Controls.SetHidden(InputBox.IdRoot, true);
						break;
					case UI_LOBBY_USER_Mail		: 
					case UI_LOBBY_USER_Name		: 
					case UI_LOBBY_USER_Room		: 
					case UI_LOBBY_USER_Pass		: 
						inputBox.Area.Offset		= control.Area.Offset; 
						inputBox.Align				= control.Align; 
						controlMetricsInvalidate(gui, InputBox.IdRoot);
						InputBox.SetText(gui, gui.Controls.Text[iControl].Text);
						edit						= true;
						Field						= (UI_LOBBY_USER)(iControl - (Dialog.Root + 1));
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
				vcu0_t							viewPayload					= MessagesToProcess[iMessage]->Payload;
				info_printf("Client received: %s.", viewPayload.begin());
			}
			return 0;
		}

		gpk::err_t 							Draw				(g8bgra target) { 
			return guiDraw(*Dialog.GUI, target);
		}
	};

} // namespace

#endif // GPK_TEST_LOBBY_H_23627
