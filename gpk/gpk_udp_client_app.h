#include "gpk_udp_client.h"
#include "gpk_gui_inputbox.h"
#include "gpk_json.h"

#ifndef GPK_UDP_CLIENT_APP_H_23701
#define GPK_UDP_CLIENT_APP_H_23701

namespace gpk
{
	GDEFINE_ENUM_TYPE (UI_CLIENT_INPUT, u0_t);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INPUT, Root		, 0);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INPUT, Exit		, 1);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INPUT, Connect		, 2);

	GDEFINE_ENUM_TYPE (UI_CLIENT_INFO, u0_t);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INFO, Root				, 0);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INFO, DateTime			, 1);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INFO, ConnectedSince	, 2);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INFO, Connected		, 3);

	struct SClientUI {
		cid_t		Root				= CID_INVALID;
		acid			Info				= {};
		acid			Input				= {};
		asc_t		TextDateTime		= {};
		asc_t		TextConnected		= {};
		asc_t		TextConnectedSince	= {};
		SInputBox	InputBox;
	};

	struct SClient {
		SUDPClient	UDP;

		TUDPQueue	QueueReceived		= {};
		TQueueSend	QueueToSend			= {};

		vcst_t		RemoteIp			= "192.168.100.59";
		vcst_t		RemotePort			= "9865";

		SClientUI	UI					= {};

		f3_t				ConnectedSince		= {};

							~SClient			()	{
			if(UDP.State != UDP_CONNECTION_STATE_DISCONNECTED)
				if_fail_w(clientDisconnect(UDP));
		}
	};

	err_t		loadClientConfig	(const SJSONReader & jsonConfig, s2_t parentNode, vcst_t & remote_ip, vcst_t & remote_port);
	err_t		clientConnect		(SClient & client, SGUI & gui);
	err_t		clientUpdate		(SClient & client, SGUI & gui);
	err_t		setupGUI			(SClientUI & client, SGUI & gui);
	err_t		updateGUI			(SClient & client, SGUI & gui);
	err_t		processGUIEvent		(SClient & client, SGUI & gui, const SEventView<EVENT_GUI_CONTROL> & screenEvent);
} // namespace

#endif // GPK_UDP_CLIENT_APP_H_23701