#include "dryer_app.h"

static	::gpk::error_t				processStatusArg		(::dry::SApplication & app, ::gpk::SJSONReader & jsonReader, ::gpk::vcsc_t arg) {
	ree_if(arg.size() <= 2, "Invalid string received: %s.", ::gpk::toString(arg).begin());
	switch(arg[0]) {
	case 'R':
		arg.slice(arg, 2);
		info_printf("Received reset value: '%s'", ::gpk::toString(arg).begin());
		if(arg[0] == '1') {
			if_fail_be(::gpk::failed(::s3d::serialWrite(app.Serial, ::gpk::vcs{"<ACK>"}.u8())));
			const uint8_t							temperature						= app.Dryer.State.Running
				? (uint8_t)app.Dryer.CurrentProgram.Temperature
				: (uint8_t)::dry::DRYER_MIN_TEMPERATURE
				;
			char									buffer[32]						= {};
			snprintf(buffer, ::gpk::size(buffer)-2, "<TEMP %u>", temperature);
			always_printf("Recovering to temperature: %u.", temperature);
			if_fail_bef(::gpk::failed(::s3d::serialWrite(app.Serial, ::gpk::vcs{buffer}.u8())), "Failed to send serial command to set temperature to: %u.", temperature);
			::gpk::vcst_t							command							= app.Dryer.State.Pump ? ::gpk::vcs{"<ON>"} : ::gpk::vcs{"<OFF>"};
			always_printf("Recovering to pump state: %s.", command.begin());
			if_fail_bef(::gpk::failed(::s3d::serialWrite(app.Serial, command.u8())), "Failed to send serial command to start pump: %s.", command.begin());
			app.Dryer.State.Dryer				= true;
		}
		break;
	case 'E':
		arg.slice(arg, 2);
		info_printf("Received error value: '%s'", ::gpk::toString(arg).begin());
		break;
	case 'C':
		arg.slice(arg, 2);
		info_printf("Received cycle value: '%s'", ::gpk::toString(arg).begin());
		if(arg[0] == '1') {
			if(false == app.Dryer.State.Pump)
				::s3d::serialWrite(app.Serial, ::gpk::vcs{"<OFF>"}.u8());
		}
		else if(app.Dryer.State.Pump)
			::s3d::serialWrite(app.Serial, ::gpk::vcs{"<ON>"}.u8());

		break;
	case 'D':
		arg.slice(arg, 2);
		info_printf("Received door value: '%s'", ::gpk::toString(arg).begin());
		app.Dryer.State.DoorClosed			= arg[0] == '1';
		break;
	case 'T': {
		arg.slice(arg, 2);
		info_printf("Received temperature value: '%s'", ::gpk::toString(arg).begin());
		::gpk::array_obj<::gpk::vcsc_t>	values;
		::gpk::split(arg, '/', values);
		jsonReader.Reset();
		if_fail_bef(::gpk::failed(::gpk::jsonParse(jsonReader, values[0])), "Failed to parse sensor temperature value: %s", ::gpk::toString(values[0]).begin());
		if(::gpk::JSON_TYPE_DECIMAL != jsonReader[1]->Token->Type)
			app.Dryer.State.Temperature			= (float)jsonReader[1]->Token->Value;
		else {
			double									value			= 0;
			memcpy(&value, &jsonReader[1]->Token->Value, sizeof(double));
			app.Dryer.State.Temperature			= (float)value;
		}
		jsonReader.Reset();
		if_fail_bef(::gpk::failed(::gpk::jsonParse(jsonReader, values[1])), "Failed to parse target temperature value: %s", ::gpk::toString(values[1]).begin());
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
			if_fail_bef(::gpk::failed(::s3d::serialWrite(app.Serial, ::gpk::vcs{buffer}.u8())), "Failed to send serial command to set temperature to: %u.", app.Dryer.CurrentProgram.Temperature);
		}
		break;
	}
	case 'P':
		arg.slice(arg, 2);
		info_printf("Received pressure value: '%s'", ::gpk::toString(arg).begin());
		jsonReader							= {};
		if_fail_bef(::gpk::failed(::gpk::jsonParse(jsonReader, arg)), "Failed to parse sensor pressure value: %s", ::gpk::toString(arg).begin());
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
		info_printf("Received humidity value: '%s'", ::gpk::toString(arg).begin());
		jsonReader							= {};
		if_fail_bef(::gpk::failed(::gpk::jsonParse(jsonReader, arg)), "Failed to parse sensor humidity value: %s", ::gpk::toString(arg).begin());
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

::gpk::error_t						dry::updateSerial		(::dry::SApplication & app) {
	::gpk::apod<::gpk::u0_t>				bufferRead;
#if defined(GPK_WINDOWS)
	const ::gpk::vcs						portName				= "COM6";
#else
	//const ::gpk::vcs						portName				= "/dev/ttyAMA0";
	const ::gpk::vcs						portName				= "/dev/ttyACM0";

#endif
	const int32_t							baud					= 115200;
	gpk_sync_increment(app.ThreadClients);
	::gpk::SJSONReader						jsonReader				= {};
	while(gpk_sync_compare_exchange(app.ThreadClients, 0, 1) && (false == app.Framework.RootWindow.Closed)) {
		app.TimeLastSerial					= time(0);
		if(false == app.Serial.Connected) {
			app.Dryer.State.Dryer				= false;
			if(::gpk::failed(::s3d::serialOpen(app.Serial, portName, baud))) {
				error_printf("Failed to open serial port '%s'.", ::gpk::toString(portName).begin());
				::gpk::sleep(1000);
				continue;
			}
			::gpk::sleep(5000);
		}
		if(::gpk::failed(::s3d::serialWrite(app.Serial, ::gpk::vcs{"<STATUS>"}.u8()))) {
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
		info_printf("read %d bytes: '%s'", nReadBytes, ::gpk::toString(messageText).begin());
		::gpk::vcsc_t							trimmed			= messageText;
		::gpk::trim(trimmed, trimmed, "<> \t\b\n\r");
		info_printf("trimmed: '%s'", ::gpk::toString(trimmed).begin());
		::gpk::array_obj<::gpk::vcsc_t>		splitMessage;
		::gpk::split(trimmed, ',', splitMessage);
		for(uint32_t iArg = 0; iArg < splitMessage.size(); ++iArg)
			if_fail_ef(::gpk::failed(::processStatusArg(app, jsonReader, splitMessage[iArg])), "Failed to process message parameter: %s", ::gpk::toString(splitMessage[iArg]).begin());
		app.Dryer.State.Dryer				= true;
		::gpk::sleep(1000);
	}
	if_fail_ef(::gpk::failed(::s3d::serialClose(app.Serial)), "%s", "Failed to close serial port.");
	return 0;
}
