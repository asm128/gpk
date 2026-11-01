#include "dryer_app.h"

static void							timeFormat				(::gpk::vc output)		{
	static constexpr const char				* month_names[]			= {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	time_t									rawtime					= {};
    time ( &rawtime );
#ifndef GPK_WINDOWS
    tm										timeInfo				= *localtime(&rawtime);
#else
	tm										timeInfo				= {};
	localtime_s(&timeInfo, &rawtime);
#endif
	memset(output.begin(), 0, output.size());
	snprintf(output.begin(), output.size() - 1, "%s %i | %i:%.2i", month_names[timeInfo.tm_mon], timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min);
}

template <size_t _countControls>
static	::gpk::error_t				updateIcon				(::gpk::SGUI & gui, ::dry::SScreen<_countControls> & screen, int32_t idControl, bool isOn) {
	const int32_t							controlIndex			= screen.ControlIds[idControl];
	::dry::SControlImages					& icons					= screen.ControlIcons[idControl];
	if(-1 != controlIndex)
		gui.Controls.Controls[controlIndex].Image = isOn ? icons.On.View : icons.Off.View;
	return 0;
}

::gpk::error_t						dry::updateScreenHUD	(::dry::SApplication & app) {
	::gpk::SGUI								& gui					= *app.Framework.GUI;
	char									bufferTime	[64]		= {};
	::timeFormat(bufferTime);
	if(::gpk::vcs{bufferTime} != ::gpk::vcs{app.TimeString.Storage}) {
		::gpk::memcpy_s(app.TimeString.Storage, bufferTime);
		e_if(errored(::gpk::controlTextSet(gui, app.Screens.HUD.ControlIds[::dry::SCREEN_CONTROL_HUD_CALENDAR], app.TimeString.Storage)), "%s", "");
	}
	e_if(errored(::updateIcon(gui, app.Screens.HUD , ::dry::SCREEN_CONTROL_HUD_DOOR		, app.Dryer.State.DoorClosed)), "%s", "");
	e_if(errored(::updateIcon(gui, app.Screens.HUD , ::dry::SCREEN_CONTROL_HUD_ETHERNET	, app.Comms.Ethernet		)), "%s", "");
	e_if(errored(::updateIcon(gui, app.Screens.HUD , ::dry::SCREEN_CONTROL_HUD_WIFI		, app.Comms.WiFi			)), "%s", "");
	e_if(errored(::updateIcon(gui, app.Screens.HUD , ::dry::SCREEN_CONTROL_HUD_RFID		, app.Comms.RFID			)), "%s", "");
	e_if(errored(::updateIcon(gui, app.Screens.HUD , ::dry::SCREEN_CONTROL_HUD_SERIAL	, app.Serial.Connected		)), "%s", "");
	return 0;
}
