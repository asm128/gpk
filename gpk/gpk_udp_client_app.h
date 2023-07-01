#include "gpk_udp_client.h"
#include "gpk_gui_inputbox.h"
#include "gpk_json.h"

#ifndef GPK_UDP_CLIENT_APP_H_23701
#define GPK_UDP_CLIENT_APP_H_23701

namespace gpk
{
	GDEFINE_ENUM_TYPE (UI_CLIENT_INPUT, uint8_t);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INPUT, Root		, 0);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INPUT, Exit		, 1);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INPUT, Connect		, 2);

	GDEFINE_ENUM_TYPE (UI_CLIENT_INFO, uint8_t);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INFO, Root				, 0);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INFO, DateTime			, 1);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INFO, ConnectedSince	, 2);
	GDEFINE_ENUM_VALUE(UI_CLIENT_INFO, Connected		, 3);

	struct SClientUI {
		::gpk::cid_t		Root				= ::gpk::CID_INVALID;
		::gpk::acid			Info				= {};
		::gpk::acid			Input				= {};
		::gpk::achar		TextDateTime		= {};
		::gpk::achar		TextConnected		= {};
		::gpk::achar		TextConnectedSince	= {};
		::gpk::SInputBox	InputBox;
	};

	struct SClient {
		::gpk::SUDPClient	UDP;

		::gpk::TUDPQueue	QueueReceived		= {};
		::gpk::TQueueSend	QueueToSend			= {};

		::gpk::vcs			RemoteIp			= "192.168.0.188";
		::gpk::vcs			RemotePort			= "9865";

		::gpk::SClientUI	UI					= {};

		double				ConnectedSince		= {};

							~SClient			()	{
			if(UDP.State != ::gpk::UDP_CONNECTION_STATE_DISCONNECTED)
				ws_if_failed(::gpk::clientDisconnect(UDP));
		}
	};

	::gpk::error_t		loadClientConfig	(const ::gpk::SJSONReader & jsonConfig, int32_t parentNode, ::gpk::vcc & remote_ip, ::gpk::vcc & remote_port);
	::gpk::error_t		clientConnect		(::gpk::SClient & client, ::gpk::SGUI & gui);
	::gpk::error_t		clientUpdate		(::gpk::SClient & client, ::gpk::SGUI & gui);
	::gpk::error_t		setupGUI			(::gpk::SClientUI & client, ::gpk::SGUI & gui);
	::gpk::error_t		updateGUI			(::gpk::SClient & client, ::gpk::SGUI & gui);
	::gpk::error_t		processGUIEvent		(::gpk::SClient & client, ::gpk::SGUI & gui, const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & screenEvent);
} // namespace

#endif // GPK_UDP_CLIENT_APP_H_23701