#include "gpk_udp_server_app.h"
#include "gpk_parse.h"
#include "gpk_gui_control_list.h"
#include "gpk_json_expression.h"

::gpk::error_t			gpk::loadServerConfig	(const ::gpk::SJSONReader & jsonConfig, int32_t parentNode, ::gpk::vcc & port, ::gpk::vcc & adapter) {
	ws_if_failed(::gpk::jsonExpressionResolve(::gpk::vcs{"listen_port"}, jsonConfig, parentNode, port));
	ws_if_failed(::gpk::jsonExpressionResolve(::gpk::vcs{"adapter"}, jsonConfig, parentNode, adapter));
	info_printf("\nListen port: %s.\nAdapter: %s.'", ::gpk::toString(port).begin(), ::gpk::toString(adapter).begin());
	return 0;
}

::gpk::error_t			gpk::setupGUI		(::gpk::SServerUI & serverUI, ::gpk::SGUI & gui) {
	gpk_necs(serverUI.Root = ::gpk::createScreenLayout(gui));

	stacxpr	::gpk::n2u16		CONTROL_SIZE		= {192, 20};
	gpk_necs(::gpk::setupControlListWithRoot<::gpk::UI_SERVER_INPUT>(gui, serverUI.Input, serverUI.Root, CONTROL_SIZE, ::gpk::ALIGN_BOTTOM_RIGHT));
	gpk_necs(::gpk::setupControlListWithRoot<::gpk::UI_SERVER_INFO> (gui, serverUI.Info , serverUI.Root, CONTROL_SIZE, ::gpk::ALIGN_TOP_LEFT));
	return 0;
}

::gpk::error_t			gpk::serverStart	(::gpk::SServer & server, ::gpk::SGUI & gui) {
	gpk_necs(::gpk::tcpipAddress({}, server.Port, server.UDP.Address));
	gpk_necs(::gpk::parseIntegerDecimal(server.Adapter, server.UDP.AdapterIndex));
	gpk_necs(::gpk::serverStart(server.UDP, server.UDP.Address.Port, server.UDP.AdapterIndex)); 
	server.ListeningSince	= ::gpk::timeCurrentInMs() / 1000.0;
	time_t						timeInt				= time(NULL);
	tm							timeptr				= {};
	localtime_s(&timeptr, &timeInt);
	char						buf [64]			= {};
	strftime(buf, 32, "%Y-%m-%d %H:%M:%S", &timeptr);	
	server.UI.TextTimeStarted	= ::gpk::vcs(buf);
	return ::gpk::controlTextSet(gui, server.UI.Info[::gpk::UI_SERVER_INFO_TimeStarted], {server.UI.TextTimeStarted});
}


static	::gpk::error_t	updateGUI		(::gpk::SServer & server, ::gpk::SGUI & gui)		{
	::gpk::SUDPServer			& udpServer		= server.UDP;
	gpk_necs(::gpk::controlTextSet(gui, server.UI.Input[::gpk::UI_SERVER_INPUT_Stop		], udpServer.Listen ? ::gpk::vcs{"Stop"} : ::gpk::vcs{"Start"})); 
	gpk_necs(::gpk::controlTextSet(gui, server.UI.Info [::gpk::UI_SERVER_INFO_Listening	], udpServer.Listen ? ::gpk::vcs{"Listening"} : ::gpk::vcs{"Stopped"})); 
	char						buf [128]		= {};
	{	
		uint32_t					clientCount		= 0;
		for(uint32_t iClient = 0; iClient < udpServer.Clients.size(); ++iClient) 
			if(udpServer.Clients[iClient]->State == ::gpk::UDP_CONNECTION_STATE_IDLE)
				++clientCount;

		sprintf_s(buf, "Clients: %i", clientCount);	
		server.UI.TextClientCount	= ::gpk::vcs(buf);
		gpk_necs(::gpk::controlTextSet(gui, server.UI.Info[::gpk::UI_SERVER_INFO_ClientCount], {server.UI.TextClientCount}));
	}
	{ // update datetime field
		time_t						timeInt			= time(0);
		tm							timeptr			= {};
		localtime_s(&timeptr, &timeInt);
		strftime(buf, 32, "%Y-%m-%d %H:%M:%S", &timeptr);	
		server.UI.TextDateTime	= ::gpk::vcs(buf);
		gpk_necs(::gpk::controlTextSet(gui, server.UI.Info[::gpk::UI_SERVER_INFO_DateTime], {server.UI.TextDateTime})); 
	}

	return 0;
}

::gpk::error_t			gpk::serverUpdate		(::gpk::SServer & server, ::gpk::SGUI & gui) {
	server.QueueReceived.clear();
	es_if_failed(::gpk::serverPayloadCollect(server.UDP, server.QueueReceived));

	server.UDP.Clients.enumerate([&server](uint32_t & iClient, ::gpk::pobj<::gpk::SUDPConnection> & client){
		if(server.QueueToSend.size() <= iClient)
			iClient					= server.UDP.Clients.size();
		else {
			server.QueueToSend[iClient]->for_each([&client](::gpk::pau8 & payload) {
				if(payload && payload->size())
					gpk_necs(::gpk::connectionPushData(*client, client->Queue, *payload));

				payload.clear();
 				return 0;
			});
			server.QueueToSend[iClient].clear();
		}
	});
	return ::updateGUI(server, gui);
}

::gpk::error_t			gpk::processGUIEvent		(::gpk::SServer & server, ::gpk::SGUI & gui, const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & screenEvent) { 
	::gpk::SUDPServer			& udpServer		= server.UDP;
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_GUI_CONTROL_StateChange: {
		const ::gpk::SChangeControlState	stateChange	= *(const ::gpk::SChangeControlState*)screenEvent.Data.begin();
		if(0 == (stateChange.Set & ::gpk::GUI_CONTROL_FLAG_Action))
			return 0;

		for(uint32_t iInput = 0; iInput < server.UI.Input.size(); ++iInput) {
			if(server.UI.Input[iInput] != stateChange.Id) 
				continue;

			switch(iInput) {
			default: break;
			case ::gpk::UI_SERVER_INPUT_Exit			: return 1;
			case ::gpk::UI_SERVER_INPUT_Disconnect	: return 0;
			case ::gpk::UI_SERVER_INPUT_Restart		: 
				if(udpServer.Listen)
					gpk_necs(::gpk::serverStop(udpServer)); 

				gpk_necs(::gpk::serverStart(server, gui));
				return 0;
			case ::gpk::UI_SERVER_INPUT_Stop			: 
				if(udpServer.Listen)
					gpk_necs(::gpk::serverStop(udpServer)); 
				else 
					gpk_necs(::gpk::serverStart(server, gui));

				return 0;
			}
		}
	}
	}
	return 0;
}
