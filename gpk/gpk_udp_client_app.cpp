#include "gpk_udp_client_app.h"
#include "gpk_gui_control_list.h"
#include "gpk_json_expression.h"

::gpk::error_t	gpk::loadClientConfig	(const ::gpk::SJSONReader & jsonConfig, int32_t parentNode, ::gpk::vcc & remote_ip, ::gpk::vcc & remote_port) {
	ws_if_failed(::gpk::jsonExpressionResolve(::gpk::vcs{"remote_ip"  }, jsonConfig, parentNode, remote_ip));
	ws_if_failed(::gpk::jsonExpressionResolve(::gpk::vcs{"remote_port"}, jsonConfig, parentNode, remote_port));
	info_printf("Remote server address '%s:%s'", ::gpk::toString(remote_ip).begin(), ::gpk::toString(remote_port).begin());
	return 0;
}

::gpk::error_t		gpk::setupGUI			(::gpk::SClientUI & clientUI, ::gpk::SGUI & gui)		{
	gpk_necs(clientUI.Root = ::gpk::createScreenLayout(gui));

	stacxpr	::gpk::n2u16	CONTROL_SIZE			= {192, 20};
	gpk_necs(gpk::setupControlListWithRoot<::gpk::UI_CLIENT_INPUT>(gui, clientUI.Input, clientUI.Root, CONTROL_SIZE, ::gpk::ALIGN_BOTTOM_RIGHT));
	gpk_necs(gpk::setupControlListWithRoot<::gpk::UI_CLIENT_INFO> (gui, clientUI.Info , clientUI.Root, CONTROL_SIZE, ::gpk::ALIGN_TOP_LEFT));
	return 0;
}

::gpk::error_t		gpk::clientConnect		(::gpk::SClient & client, ::gpk::SGUI & gui) {
	gpk_necs(gpk::tcpipAddress(client.RemoteIp, client.RemotePort, client.UDP.AddressConnect));
	gpk_necs(gpk::clientConnect(client.UDP)); 

	client.ConnectedSince	= ::gpk::timeCurrentInMs() / 1000.0;
	{
		time_t					timeInt					= time(0);
		tm						timeptr					= {};
		localtime_s(&timeptr, &timeInt);
		char					buf[64]					= {};
		strftime(buf, 32, "%Y-%m-%d %H:%M:%S", &timeptr);	
		client.UI.TextConnectedSince	= ::gpk::vcs(buf);
	}
	return ::gpk::controlTextSet(gui, client.UI.Info[::gpk::UI_CLIENT_INFO_ConnectedSince], {client.UI.TextConnectedSince});
}

::gpk::error_t		gpk::processGUIEvent	(::gpk::SClient & client, ::gpk::SGUI & gui, const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_GUI_CONTROL_StateChange: {
		const ::gpk::SChangeControlState	stateChange	= *(const ::gpk::SChangeControlState*)screenEvent.Data.begin();
		if(0 == (stateChange.Set & ::gpk::GUI_CONTROL_FLAG_Action))
			return 0;

		for(uint32_t iInput = 0; iInput < client.UI.Input.size(); ++iInput) {
			if(client.UI.Input[iInput] != stateChange.Id) 
				continue;

			switch(iInput) {
			default: break;
			case ::gpk::UI_CLIENT_INPUT_Exit	:
				return 1;
			case ::gpk::UI_CLIENT_INPUT_Connect	: 
				if(client.UDP.State == ::gpk::UDP_CONNECTION_STATE_DISCONNECTED)
					gpk_necs(gpk::clientConnect(client, gui));
				else if(client.UDP.State == ::gpk::UDP_CONNECTION_STATE_IDLE)
					gpk_necs(gpk::clientDisconnect(client.UDP));

				return 0;
			}
		}
	}
	}
	return 0;
}

::gpk::error_t			gpk::updateGUI		(::gpk::SClient & client, ::gpk::SGUI & gui) {
	char						buf [128]			= {};
	uint32_t					dots				= ::gpk::timeCurrent() % 4;
	::gpk::achar				textConnecting		= ::gpk::vcs{"Connecting"};
	textConnecting.append(gpk::vcs{"...", dots});
	gpk_necs(gpk::controlTextSet(gui, client.UI.Input[::gpk::UI_CLIENT_INPUT_Connect]
		, (client.UDP.State == ::gpk::UDP_CONNECTION_STATE_IDLE		) ? ::gpk::vcs{"Disconnect"} 
		: (client.UDP.State == ::gpk::UDP_CONNECTION_STATE_HANDSHAKE) ? ::gpk::vcc{textConnecting}
		: ::gpk::vcs{"Connect"}
		)); 
	if(client.UDP.State == ::gpk::UDP_CONNECTION_STATE_IDLE) {
		const double				seconds				= ::gpk::timeCurrentInMs() / 1000.0 - client.ConnectedSince;
		const int					minutes				= int(seconds / 60);
		const int					hours				= int(minutes / 60);
		const int					days				= hours / 24;
		if(days > 0)
			sprintf_s(buf, "%u day%s %.2i:%.2i:%05.2f", days, days == 1 ? "s" : "", hours % 24, minutes % 60, (int(seconds) % 60) + (seconds - int(seconds)));
		else
			sprintf_s(buf, "%.2i:%.2i:%05.2f", hours % 24, minutes % 60, (int(seconds) % 60) + (seconds - int(seconds)));

		client.UI.TextConnected	= ::gpk::vcs(buf);
		gpk_necs(gpk::controlTextSet(gui, client.UI.Info[::gpk::UI_CLIENT_INFO_Connected], {client.UI.TextConnected}));
	}
	{
		time_t						timeInt				= time(NULL);
		tm							timeptr				= {};
		localtime_s(&timeptr, &timeInt);
		strftime(buf, 32, "%Y-%m-%d %H:%M:%S", &timeptr);	

		client.UI.TextDateTime	= ::gpk::vcs(buf);
		gpk_necs(gpk::controlTextSet(gui, client.UI.Info[::gpk::UI_CLIENT_INFO_DateTime], {client.UI.TextDateTime}));
	}
	return 0;
}

::gpk::error_t			gpk::clientUpdate		(::gpk::SClient & client, ::gpk::SGUI & gui) {
	if(client.UDP.State == gpk::UDP_CONNECTION_STATE_IDLE) {
		client.QueueReceived.clear();
		gpk_necs(gpk::connectionPayloadCollect(client.UDP, client.QueueReceived));

		client.QueueToSend.for_each([&client](const ::gpk::pau8 & ev) {
			gpk_necs(gpk::connectionPushData(client.UDP, client.UDP.Queue, *ev));
 			return 0;
		});

		client.QueueToSend.clear();
 		gpk_necs(gpk::clientUpdate(client.UDP));
	}

	return ::gpk::updateGUI(client, gui);
}
