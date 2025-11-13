#include "gpk_udp_server.h"
#include "gpk_gui_inputbox.h"
#include "gpk_json.h"

#ifndef GPK_UDP_SERVER_APP_H_23701
#define GPK_UDP_SERVER_APP_H_23701

namespace gpk
{
	GDEFINE_ENUM_TYPE (UI_SERVER_INPUT, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Root			, 0);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Exit			, 1);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Restart			, 2);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Disconnect		, 3);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Stop			, 4);

	GDEFINE_ENUM_TYPE (UI_SERVER_INFO, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, Root				, 0);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, DateTime			, 1);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, Listening		, 2);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, ClientCount		, 3);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, TimeStarted		, 4);

	struct SServerUI {
		::gpk::cid_t				Root				= -1;
		::gpk::acid					Info				= {};
		::gpk::acid					Input				= {};
		::gpk::asc_t				TextDateTime		= ::gpk::vcsc_t{1, "-"};
		::gpk::asc_t				TextTimeStarted		= ::gpk::vcsc_t{1, "-"};
		::gpk::asc_t				TextClientCount		= ::gpk::vcsc_t{1, "-"};
	};

	struct SServer {
		gpk::SUDPServer				UDP;

		gpk::ao<gpk::TUDPQueue>		QueueReceived;
		gpk::apobj<gpk::TQueueSend>	QueueToSend;	// A queue for each client

		::gpk::vcst_t				Adapter				= "0";	// default
		::gpk::vcst_t				Port				= "9865";

		SServerUI					UI;

		double						ListeningSince		= {};
	};

	::gpk::error_t			loadServerConfig	(const ::gpk::SJSONReader & jsonConfig, int32_t parentNode, ::gpk::vcsc_t & port, ::gpk::vcsc_t & adapter);
	::gpk::error_t			serverStart			(::gpk::SServer & server, ::gpk::SGUI & gui);
	::gpk::error_t			serverUpdate		(::gpk::SServer & server, ::gpk::SGUI & gui);
	::gpk::error_t			setupGUI			(::gpk::SServerUI & serverUI, ::gpk::SGUI & gui);
	::gpk::error_t			processGUIEvent		(::gpk::SServer & server, ::gpk::SGUI & gui, const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & screenEvent);

	tplt<tpnm _tEvent, tpnm _tState> 
	::gpk::error_t			processUDPEvents
		( ::gpk::vcpobj<SServer>	servers
		, _tState					& state
		, const ::std::function<::gpk::error_t(uint32_t iServer, uint32_t iClient, _tState &, const _tEvent &)>	
										& funcProcessServerRequest
		) {
		for(uint32_t iServer = 0; iServer < servers.size(); ++iServer) {
			const ::std::function<::gpk::error_t(uint32_t iClient, _tState &, const _tEvent &)>	
										funcProcessRequest		= [iServer, funcProcessServerRequest](uint32_t iClient, _tState & customState, const _tEvent & customEvent) {
				return funcProcessServerRequest(iServer, iClient, customState, customEvent);
			};
			gpk_necs(gpk::processUDPEvents(servers[iServer]->QueueReceived, state, funcProcessRequest));
		}
		return 0;
	}
} // namespace

#endif // GPK_UDP_SERVER_APP_H_23701