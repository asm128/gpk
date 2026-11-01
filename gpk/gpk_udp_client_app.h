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
		::gpk::cid_t		Root				= ::gpk::CID_INVALID;
		::gpk::acid			Info				= {};
		::gpk::acid			Input				= {};
		::gpk::asc_t		TextDateTime		= {};
		::gpk::asc_t		TextConnected		= {};
		::gpk::asc_t		TextConnectedSince	= {};
		::gpk::SInputBox	InputBox;
	};

	struct SClient {
		::gpk::SUDPClient	UDP;

		::gpk::TUDPQueue	QueueReceived		= {};
		::gpk::TQueueSend	QueueToSend			= {};

		::gpk::vcst_t		RemoteIp			= "192.168.100.59";
		::gpk::vcst_t		RemotePort			= "9865";

		::gpk::SClientUI	UI					= {};

		double				ConnectedSince		= {};

							~SClient			()	{
			if(UDP.State != ::gpk::UDP_CONNECTION_STATE_DISCONNECTED)
				if_fail_w(::gpk::clientDisconnect(UDP));
		}
	};

	::gpk::err_t		loadClientConfig	(const ::gpk::SJSONReader & jsonConfig, s2_t parentNode, ::gpk::view<sc_t> & remote_ip, ::gpk::view<sc_t> & remote_port);
	::gpk::err_t		clientConnect		(::gpk::SClient & client, ::gpk::SGUI & gui);
	::gpk::err_t		clientUpdate		(::gpk::SClient & client, ::gpk::SGUI & gui);
	::gpk::err_t		setupGUI			(::gpk::SClientUI & client, ::gpk::SGUI & gui);
	::gpk::err_t		updateGUI			(::gpk::SClient & client, ::gpk::SGUI & gui);
	::gpk::err_t		processGUIEvent		(::gpk::SClient & client, ::gpk::SGUI & gui, const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & screenEvent);
} // namespace

#endif // GPK_UDP_CLIENT_APP_H_23701