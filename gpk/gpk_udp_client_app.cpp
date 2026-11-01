#include "gpk_udp_client_app.h"
#include "gpk_gui_control_list.h"
#include "gpk_json_expression.h"
#include "gpk_chrono.h"

gpk::err_t		gpk::loadClientConfig	(const SJSONReader & jsonConfig, s2_t parentNode, vcst_t & remote_ip, vcst_t & remote_port) {
	if_fail_w(jsonExpressionResolve("remote_ip", jsonConfig, parentNode, remote_ip));
	if_fail_w(jsonExpressionResolve("remote_port", jsonConfig, parentNode, remote_port));
	info_printf("Remote server address '%.*s:%.*s'", GPK_VIEW_ARGS(remote_ip), GPK_VIEW_ARGS(remote_port));
	return 0;
}

gpk::err_t		gpk::setupGUI			(SClientUI & clientUI, SGUI & gui)		{
	if_fail_fe(clientUI.Root = createScreenLayout(gui));

	stxp	n2u1_t	CONTROL_SIZE			= {192, 20};
	if_fail_fe(gpk::setupControlListWithRoot<UI_CLIENT_INPUT>(gui, clientUI.Input, clientUI.Root, CONTROL_SIZE, ALIGN_BOTTOM_RIGHT));
	if_fail_fe(gpk::setupControlListWithRoot<UI_CLIENT_INFO> (gui, clientUI.Info , clientUI.Root, CONTROL_SIZE, ALIGN_TOP_LEFT));
	return 0;
}

gpk::err_t		gpk::clientConnect		(SClient & client, SGUI & gui) {
	if_fail_fe(gpk::tcpipAddress(client.RemoteIp, client.RemotePort, client.UDP.AddressConnect));
	if_fail_fe(gpk::clientConnect(client.UDP)); 

	client.ConnectedSince	= timeCurrentInMs() / 1000.0;
	{
		time_t					timeInt					= time(0);
		tm						timeptr					= {};
		localtime_s(&timeptr, &timeInt);
		sc_t					buf[64]					= {};
		strftime(buf, 32, "%Y-%m-%d %H:%M:%S", &timeptr);	
		client.UI.TextConnectedSince	= vcst_t(buf);
	}
	return controlTextSet(gui, client.UI.Info[UI_CLIENT_INFO_ConnectedSince], {client.UI.TextConnectedSince});
}

gpk::err_t		gpk::processGUIEvent	(SClient & client, SGUI & gui, const SEventView<EVENT_GUI_CONTROL> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case EVENT_GUI_CONTROL_StateChange: {
		const SChangeControlState	stateChange	= *(const SChangeControlState*)screenEvent.Data.begin();
		if(0 == (stateChange.Set & GUI_CONTROL_FLAG_Action))
			return 0;

		for(u2_t iInput = 0; iInput < client.UI.Input.size(); ++iInput) {
			if(client.UI.Input[iInput] != stateChange.Id) 
				continue;

			switch(iInput) {
			default: break;
			case UI_CLIENT_INPUT_Exit	:
				return 1;
			case UI_CLIENT_INPUT_Connect	: 
				if(client.UDP.State == UDP_CONNECTION_STATE_DISCONNECTED) {
					if_fail_fe(gpk::clientConnect(client, gui));
				}
				else if(client.UDP.State == UDP_CONNECTION_STATE_IDLE)
					if_fail_fe(gpk::clientDisconnect(client.UDP));

				return 0;
			}
		}
	}
	}
	return 0;
}

gpk::err_t		gpk::updateGUI		(SClient & client, SGUI & gui) {
	sc_t					buf [128]			= {};
	u2_t					dots				= timeCurrent() % 4;
	string					textConnecting		= "Connecting";
	textConnecting.append(gpk::vcst_t{"...", dots});
	if_fail_fe(gpk::controlTextSet(gui, client.UI.Input[UI_CLIENT_INPUT_Connect]
		, (client.UDP.State == UDP_CONNECTION_STATE_IDLE		) ? vcst_t("Disconnect")
		: (client.UDP.State == UDP_CONNECTION_STATE_HANDSHAKE) ? vcst_t(textConnecting)
		: vcst_t("Connect")
		)); 
	if(client.UDP.State == UDP_CONNECTION_STATE_IDLE) {
		const f3_t				seconds				= timeCurrentInMs() / 1000.0 - client.ConnectedSince;
		const int					minutes				= int(seconds / 60);
		const int					hours				= int(minutes / 60);
		const int					days				= hours / 24;
		if(days > 0)
			sprintf_s(buf, "%u day%s %.2i:%.2i:%05.2f", days, days == 1 ? "s" : "", hours % 24, minutes % 60, (int(seconds) % 60) + (seconds - int(seconds)));
		else
			sprintf_s(buf, "%.2i:%.2i:%05.2f", hours % 24, minutes % 60, (int(seconds) % 60) + (seconds - int(seconds)));

		client.UI.TextConnected	= vcst_t(buf);
		if_fail_fe(gpk::controlTextSet(gui, client.UI.Info[UI_CLIENT_INFO_Connected], {client.UI.TextConnected}));
	}
	{
		time_t						timeInt				= time(NULL);
		tm							timeptr				= {};
		localtime_s(&timeptr, &timeInt);
		strftime(buf, 32, "%Y-%m-%d %H:%M:%S", &timeptr);	

		client.UI.TextDateTime	= vcst_t(buf);
		if_fail_fe(gpk::controlTextSet(gui, client.UI.Info[UI_CLIENT_INFO_DateTime], {client.UI.TextDateTime}));
	}
	return 0;
}

gpk::err_t		gpk::clientUpdate		(SClient & client, SGUI & gui) {
	if(client.UDP.State == gpk::UDP_CONNECTION_STATE_IDLE) {
		client.QueueReceived.clear();
		if_fail_fe(gpk::connectionPayloadCollect(client.UDP, client.QueueReceived));

		client.QueueToSend.for_each([&client](const pau0_t & ev) {
			if_fail_fe(gpk::connectionPushData(client.UDP, client.UDP.Queue, *ev));
 			return 0;
		});

		client.QueueToSend.clear();
 		if_fail_fe(gpk::clientUpdate(client.UDP));
	}

	return updateGUI(client, gui);
}
