#include "gpk_udp_server_app.h"
#include "gpk_parse.h"
#include "gpk_gui_control_list.h"
#include "gpk_json_expression.h"
#include "gpk_chrono.h"

GPK_USING_TYPEINT();

::gpk::err_t			gpk::loadServerConfig	(const ::gpk::SJSONReader & jsonConfig, s2_t parentNode, ::gpk::view<sc_t> & port, ::gpk::view<sc_t> & adapter) {
	if_fail_w(::gpk::jsonExpressionResolve("listen_port", jsonConfig, parentNode, port));
	if_fail_w(::gpk::jsonExpressionResolve("adapter", jsonConfig, parentNode, adapter));
	info_printf("\nListen port: %s.\nAdapter: %s.'", GPK_VIEW_ARGS(port).begin(), GPK_VIEW_ARGS(adapter).begin());
	return 0;
}

::gpk::err_t			gpk::setupGUI		(::gpk::SServerUI & serverUI, ::gpk::SGUI & gui) {
	if_fail_fe(serverUI.Root = ::gpk::createScreenLayout(gui));

	stxp	::gpk::n2u1_t		CONTROL_SIZE		= {192, 20};
	if_fail_fe(gpk::setupControlListWithRoot<::gpk::UI_SERVER_INPUT>(gui, serverUI.Input, serverUI.Root, CONTROL_SIZE, ::gpk::ALIGN_BOTTOM_RIGHT));
	if_fail_fe(gpk::setupControlListWithRoot<::gpk::UI_SERVER_INFO> (gui, serverUI.Info , serverUI.Root, CONTROL_SIZE, ::gpk::ALIGN_TOP_LEFT));
	return 0;
}

::gpk::err_t			gpk::serverStart	(::gpk::SServer & server, ::gpk::SGUI & gui) {
	if_fail_fe(gpk::tcpipAddress({}, server.Port, server.UDP.Address));
	if_fail_fe(gpk::parseIntegerDecimal(server.Adapter, server.UDP.AdapterIndex));
	if_fail_fe(gpk::serverStart(server.UDP, server.UDP.Address.Port, server.UDP.AdapterIndex)); 
	server.ListeningSince	= ::gpk::timeCurrentInMs() / 1000.0;
	time_t						timeInt				= time(NULL);
	tm							timeptr				= {};
	localtime_s(&timeptr, &timeInt);
	sc_t						buf [64]			= {};
	strftime(buf, 32, "%Y-%m-%d %H:%M:%S", &timeptr);	
	server.UI.TextTimeStarted	= ::gpk::vcst_t(buf);
	return ::gpk::controlTextSet(gui, server.UI.Info[::gpk::UI_SERVER_INFO_TimeStarted], {server.UI.TextTimeStarted});
}


static	::gpk::err_t	updateGUI		(::gpk::SServer & server, ::gpk::SGUI & gui)		{
	::gpk::SUDPServer			& udpServer		= server.UDP;
	if_fail_fe(gpk::controlTextSet(gui, server.UI.Input[::gpk::UI_SERVER_INPUT_Stop		], udpServer.Listen ? "Stop" : "Start")); 
	if_fail_fe(gpk::controlTextSet(gui, server.UI.Info [::gpk::UI_SERVER_INFO_Listening	], udpServer.Listen ? "Listening" : "Stopped")); 
	sc_t						buf [128]		= {};
	{	
		u2_t					clientCount		= 0;
		for(u2_t iClient = 0; iClient < udpServer.Clients.size(); ++iClient) 
			if(udpServer.Clients[iClient]->State == ::gpk::UDP_CONNECTION_STATE_IDLE)
				++clientCount;

		sprintf_s(buf, "Clients: %i", clientCount);	
		server.UI.TextClientCount	= ::gpk::vcst_t(buf);
		if_fail_fe(gpk::controlTextSet(gui, server.UI.Info[::gpk::UI_SERVER_INFO_ClientCount], {server.UI.TextClientCount}));
	}
	{ // update datetime field
		time_t						timeInt			= time(0);
		tm							timeptr			= {};
		localtime_s(&timeptr, &timeInt);
		strftime(buf, 32, "%Y-%m-%d %H:%M:%S", &timeptr);	
		server.UI.TextDateTime	= ::gpk::vcst_t(buf);
		if_fail_fe(gpk::controlTextSet(gui, server.UI.Info[::gpk::UI_SERVER_INFO_DateTime], {server.UI.TextDateTime})); 
	}

	return 0;
}

::gpk::err_t			gpk::serverUpdate		(::gpk::SServer & server, ::gpk::SGUI & gui) {
	server.QueueReceived.clear();
	if_fail_e(::gpk::serverPayloadCollect(server.UDP, server.QueueReceived));

	server.UDP.Clients.enumerate([&server](u2_t & iClient, ::gpk::pobj<::gpk::SUDPConnection> & client){
		if(server.QueueToSend.size() <= iClient)
			iClient					= server.UDP.Clients.size();
		else {
			if_fail_fe(server.QueueToSend[iClient]->for_each([&client](::gpk::pau8 & payload) {
				if(payload && payload->size())
					if_fail_fe(gpk::connectionPushData(*client, client->Queue, *payload));

				payload.clear();
 				return 0;
			}));
			server.QueueToSend[iClient].clear();
		}
		return 0;
	});
	return ::updateGUI(server, gui);
}

::gpk::err_t			gpk::processGUIEvent	(::gpk::SServer & server, ::gpk::SGUI & gui, const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & screenEvent) { 
	::gpk::SUDPServer			& udpServer				= server.UDP;
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_GUI_CONTROL_StateChange: {
		const ::gpk::SChangeControlState	stateChange		= *(const ::gpk::SChangeControlState*)screenEvent.Data.begin();
		if(0 == (stateChange.Set & ::gpk::GUI_CONTROL_FLAG_Action))
			return 0;

		for(u2_t iInput = 0; iInput < server.UI.Input.size(); ++iInput) {
			if(server.UI.Input[iInput] != stateChange.Id) 
				continue;

			switch(iInput) {
			default: break;
			case ::gpk::UI_SERVER_INPUT_Exit		: return 1;
			case ::gpk::UI_SERVER_INPUT_Disconnect	: return 0;
			case ::gpk::UI_SERVER_INPUT_Restart		: 
				if(udpServer.Listen)
					if_fail_fe(gpk::serverStop(udpServer)); 

				if_fail_fe(gpk::serverStart(server, gui));
				return 0;
			case ::gpk::UI_SERVER_INPUT_Stop			: 
				if(udpServer.Listen)
					if_fail_fe(gpk::serverStop(udpServer)); 
				else 
					if_fail_fe(gpk::serverStart(server, gui));

				return 0;
			}
		}
	}
	}
	return 0;
}
