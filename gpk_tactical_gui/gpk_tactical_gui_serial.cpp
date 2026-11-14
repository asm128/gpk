#include "gpk_tactical_gui_app.h"

static	::gpk::err_t				processStatusArg		(::dry::SApplication & app, ::gpk::SJSONReader & jsonReader, ::gpk::vcsc_t arg) {
	if_true_fef(arg.size() <= 2, "Invalid string received: %.*s.", GPK_VIEW_ARGS(arg));
	switch(arg[0]) {
	case 'R':
		arg.slice(arg, 2);
		info_printf("Received reset value: '%.*s'", GPK_VIEW_ARGS(arg));
		if(arg[0] == '1') {
			if_fail_be(::s3d::serialWrite(app.Serial, ::gpk::vcst_t{"<ACK>"}.u8())));
			const u0_t							temperature						= app.Dryer.State.Running
				? (u0_t)app.Dryer.CurrentProgram.Temperature
				: (u0_t)::dry::DRYER_MIN_TEMPERATURE
				;
			char									buffer[32]						= {};
			snprintf(buffer, ::gpk::size(buffer)-2, "<TEMP %u>", temperature);
			always_printf("Recovering to temperature: %u.", temperature);
			if_fail_bef(::gpk::failed(::s3d::serialWrite(app.Serial, ::gpk::vcst_t{buffer}.u8())), "Failed to send serial command to set temperature to: %u.", temperature);
			::gpk::vcst_t							command							= app.Dryer.State.Pump ? ::gpk::vcst_t{"<ON>"} : ::gpk::vcst_t{"<OFF>"};
			always_printf("Recovering to pump state: %.*s.", GPK_VIEW_ARGS(command));
			if_fail_bef(::gpk::failed(::s3d::serialWrite(app.Serial, command.u8())), "Failed to send serial command to start pump: %.*s.", GPK_VIEW_ARGS(command));
			app.Dryer.State.Dryer				= true;
		}
		break;
	case 'E':
		arg.slice(arg, 2);
		info_printf("Received error value: '%.*s'", GPK_VIEW_ARGS(arg));
		break;
	case 'C':
		arg.slice(arg, 2);
		info_printf("Received cycle value: '%.*s'", GPK_VIEW_ARGS(arg));
		if(arg[0] == '1') {
			if(false == app.Dryer.State.Pump)
				::s3d::serialWrite(app.Serial, ::gpk::vcst_t{"<OFF>"}.u8());
		}
		else if(app.Dryer.State.Pump)
			::s3d::serialWrite(app.Serial, ::gpk::vcst_t{"<ON>"}.u8());

		break;
	case 'D':
		arg.slice(arg, 2);
		info_printf("Received door value: '%.*s'", GPK_VIEW_ARGS(arg));
		app.Dryer.State.DoorClosed			= arg[0] == '1';
		break;
	case 'T': {
		arg.slice(arg, 2);
		info_printf("Received temperature value: '%.*s'", GPK_VIEW_ARGS(arg));
		::gpk::array_obj<::gpk::vcsc_t>	values;
		::gpk::split(arg, '/', values);
		jsonReader.Reset();
		if_fail_bef(::gpk::failed(::gpk::jsonParse(jsonReader, values[0])), "Failed to parse sensor temperature value: %.*s", GPK_VIEW_ARGS(values[0]));
		if(::gpk::JSON_TYPE_DECIMAL != jsonReader[1]->Token->Type)
			app.Dryer.State.Temperature			= (float)jsonReader[1]->Token->Value;
		else {
			double									value			= 0;
			memcpy(&value, &jsonReader[1]->Token->Value, sizeof(double));
			app.Dryer.State.Temperature			= (float)value;
		}
		jsonReader.Reset();
		if_fail_bef(::gpk::failed(::gpk::jsonParse(jsonReader, values[1])), "Failed to parse target temperature value: %.*s", GPK_VIEW_ARGS(values[1]));
		double targetTemp	 = 0;
		if(::gpk::JSON_TYPE_DECIMAL != jsonReader[1]->Token->Type)
			targetTemp			= (float)jsonReader[1]->Token->Value;
		else {
			double									value			= 0;
			memcpy(&value, &jsonReader[1]->Token->Value, sizeof(double));
			targetTemp			= (float)value;
		}
		if(app.Dryer.State.Running && targetTemp != app.Dryer.CurrentProgram.Temperature) {
			char									buffer[32]						= {};
			snprintf(buffer, ::gpk::size(buffer)-2, "<TEMP %u>", app.Dryer.CurrentProgram.Temperature);
			always_printf("Setting target temperature: %u.", app.Dryer.CurrentProgram.Temperature);
			if_fail_bef(::gpk::failed(::s3d::serialWrite(app.Serial, ::gpk::vcst_t{buffer}.u8())), "Failed to send serial command to set temperature to: %u.", app.Dryer.CurrentProgram.Temperature);
		}
		break;
	}
	case 'P':
		arg.slice(arg, 2);
		info_printf("Received pressure value: '%.*s'", GPK_VIEW_ARGS(arg));
		jsonReader							= {};
		if_fail_bef(::gpk::failed(::gpk::jsonParse(jsonReader, arg)), "Failed to parse sensor pressure value: %.*s", GPK_VIEW_ARGS(arg));
		if(::gpk::JSON_TYPE_DECIMAL != jsonReader[1]->Token->Type)
			app.Dryer.State.Vacuum				= (float)jsonReader[1]->Token->Value;
		else {
			double									value			= 0;
			memcpy(&value, &jsonReader[1]->Token->Value, sizeof(double));
			app.Dryer.State.Vacuum				= (float)fabs(value);
		}
		break;
	case 'H':
		arg.slice(arg, 2);
		info_printf("Received humidity value: '%.*s'", GPK_VIEW_ARGS(arg));
		jsonReader							= {};
		if_fail_bef(::gpk::failed(::gpk::jsonParse(jsonReader, arg)), "Failed to parse sensor humidity value: %.*s", GPK_VIEW_ARGS(arg));
		if(::gpk::JSON_TYPE_DECIMAL != jsonReader[1]->Token->Type)
			app.Dryer.State.Humidity			= (float)jsonReader[1]->Token->Value;
		else {
			double									value			= 0;
			memcpy(&value, &jsonReader[1]->Token->Value, sizeof(double));
			app.Dryer.State.Humidity			= (float)value;
		}
		break;
	}

	return 0;
}

::gpk::err_t						dry::updateSerial		(::dry::SApplication & app) {
	::gpk::apod<::gpk::u0_t>				bufferRead;
#if defined(GPK_WINDOWS)
	const ::gpk::vcst_t						portName				= "COM6";
#else
	//const ::gpk::vcst_t						portName				= "/dev/ttyAMA0";
	const ::gpk::vcst_t						portName				= "/dev/ttyACM0";

#endif
	const s2_t							baud					= 115200;
	gpk_sync_increment(app.ThreadClients);
	::gpk::SJSONReader						jsonReader				= {};
	while(gpk_sync_compare_exchange(app.ThreadClients, 0, 1) && (false == app.Framework.RootWindow.Closed)) {
		app.TimeLastSerial					= time(0);
		if(false == app.Serial.Connected) {
			app.Dryer.State.Dryer				= false;
			if(::gpk::failed(::s3d::serialOpen(app.Serial, portName, baud))) {
				error_printf("Failed to open serial port '%.*s'.", GPK_VIEW_ARGS(portName));
				::gpk::sleep(1000);
				continue;
			}
			::gpk::sleep(5000);
		}
		if(::gpk::failed(::s3d::serialWrite(app.Serial, ::gpk::vcst_t{"<STATUS>"}.u8()))) {
			error_printf("%s", "Failed to send <STATUS> command.");
			::s3d::serialClose(app.Serial);
			continue;
		}
		if(app.Framework.RootWindow.Closed)
			break;
		bufferRead.clear();
		int							nReadBytes		= ::s3d::serialReadUntil(app.Serial, bufferRead, '>', 256, 100);
		if(::gpk::failed(nReadBytes)) {
			error_printf("%s", "Failed to read from serial port.");
			::s3d::serialClose(app.Serial);
			continue;
		}
		const ::gpk::vcsc_t					messageText		= {(const ::gpk::sc_t*)bufferRead.begin(), bufferRead.size()};
		info_printf("read %d bytes: '%.*s'", nReadBytes, GPK_VIEW_ARGS(messageText));
		::gpk::vcsc_t							trimmed			= messageText;
		::gpk::trim(trimmed, trimmed, "<> \t\b\n\r");
		info_printf("trimmed: '%.*s'", GPK_VIEW_ARGS(trimmed));
		::gpk::array_obj<::gpk::vcsc_t>		splitMessage;
		::gpk::split(trimmed, ',', splitMessage);
		for(u2_t iArg = 0; iArg < splitMessage.size(); ++iArg)
			if_fail_ef(::gpk::failed(::processStatusArg(app, jsonReader, splitMessage[iArg])), "Failed to process message parameter: %.*s", GPK_VIEW_ARGS(splitMessage[iArg]));
		app.Dryer.State.Dryer				= true;
		::gpk::sleep(1000);
	}
	if_fail_ef(::gpk::failed(::s3d::serialClose(app.Serial)), "%s", "Failed to close serial port.");
	return 0;
}
