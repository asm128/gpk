#include "gpk_udp_server.h"
#include "gpk_gui_inputbox.h"
#include "gpk_json.h"

#ifndef GPK_UDP_SERVER_APP_H_23701
#define GPK_UDP_SERVER_APP_H_23701

namespace gpk
{
	GDEFINE_ENUM_TYPE (UI_SERVER_INPUT, u0_t);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Root			, 0);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Exit			, 1);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Restart			, 2);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Disconnect		, 3);
	GDEFINE_ENUM_VALUE(UI_SERVER_INPUT, Stop			, 4);

	GDEFINE_ENUM_TYPE (UI_SERVER_INFO, u0_t);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, Root				, 0);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, DateTime			, 1);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, Listening		, 2);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, ClientCount		, 3);
	GDEFINE_ENUM_VALUE(UI_SERVER_INFO, TimeStarted		, 4);

	struct SServerUI {
		cid_t				Root				= -1;
		acid				Info				= {};
		acid				Input				= {};
		string				TextDateTime		= "-";
		string				TextTimeStarted		= "-";
		string				TextClientCount		= "-";
	};

	struct SServer {
		SUDPServer			UDP;

		ao<TUDPQueue>		QueueReceived;
		apobj<TQueueSend>	QueueToSend;	// A queue for each client

		vcst_t				Adapter				= "0";	// default
		vcst_t				Port				= "9865";

		SServerUI					UI;

		f3_t						ListeningSince		= {};
	};

	err_t			loadServerConfig	(const SJSONReader & jsonConfig, s2_t parentNode, vcst_t & port, vcst_t & adapter);
	err_t			serverStart			(SServer & server, SGUI & gui);
	err_t			serverUpdate		(SServer & server, SGUI & gui);
	err_t			setupGUI			(SServerUI & serverUI, SGUI & gui);
	err_t			processGUIEvent		(SServer & server, SGUI & gui, const SEventView<EVENT_GUI_CONTROL> & screenEvent);

	tplt<tpnm _tEvent, tpnm _tState> 
	err_t			processUDPEvents
		( vcpobj<SServer>	servers
		, _tState					& state
		, const ::std::function<err_t(u2_t iServer, u2_t iClient, _tState &, const _tEvent &)>	
										& funcProcessServerRequest
		) {
		for(u2_t iServer = 0; iServer < servers.size(); ++iServer) {
			const ::std::function<err_t(u2_t iClient, _tState &, const _tEvent &)>	
										funcProcessRequest		= [iServer, funcProcessServerRequest](u2_t iClient, _tState & customState, const _tEvent & customEvent) {
				return funcProcessServerRequest(iServer, iClient, customState, customEvent);
			};
			if_fail_fe(processUDPEvents(servers[iServer]->QueueReceived, state, funcProcessRequest));
		}
		return 0;
	}
} // namespace

#endif // GPK_UDP_SERVER_APP_H_23701