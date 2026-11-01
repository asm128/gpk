#include "dryer.h"
#include "dryer_gauge.h"
#include "dryer_screen_home.h"
#include "filament_catalog.h"
#include "s3d_serial.h"

#if defined(GPK_WINDOWS)
#	include "gpk_framework.h"
#else
#	include "s3d_xcb_framework.h"
#endif

#include <thread>

#ifndef DRYER_APP_H_092743982
#define DRYER_APP_H_092743982

namespace dry
{
	GDEFINE_ENUM_TYPE(SCREEN, uint8_t);
	GDEFINE_ENUM_VALUE(SCREEN, ROOT									, 0);
	GDEFINE_ENUM_VALUE(SCREEN, HUD									, 1);
	GDEFINE_ENUM_VALUE(SCREEN, MENU									, 2);
	GDEFINE_ENUM_VALUE(SCREEN, MODAL								, 3);
	//GDEFINE_ENUM_VALUE(SCREEN, , 4);
	GDEFINE_ENUM_VALUE(SCREEN, HOME									, 5);
	GDEFINE_ENUM_VALUE(SCREEN, LOAD_FILAMENT						, 6);
	GDEFINE_ENUM_VALUE(SCREEN, INFO									, 7);
	GDEFINE_ENUM_VALUE(SCREEN, CONFIG								, 8);
	GDEFINE_ENUM_VALUE(SCREEN, COUNT								, 9);

	GDEFINE_ENUM_TYPE(SCREEN_CONTROL_MENU, uint8_t);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, DIALOG					, 0);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, HOME					, 1);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, LOAD_FILAMENT			, 2);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, INFO					, 3);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, CONFIG					, 4);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MENU, COUNT					, 5);

	GDEFINE_ENUM_TYPE(SCREEN_CONTROL_HUD, uint8_t);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, DIALOG					, 0);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, CALENDAR					, 1);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, DOOR						, 2);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, ETHERNET					, 3);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, WIFI						, 4);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, RFID						, 5);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, SERIAL					, 6);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_HUD, COUNT					, 7);

	GDEFINE_ENUM_TYPE(SCREEN_CONTROL_MODAL, uint8_t);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, DIALOG					, 0);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, CLOSE					, 1);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, YES					, 2);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, NO						, 3);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, LOAD					, 4);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, UNLOAD					, 5);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, CANCEL					, 6);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, ICON_START				, 7);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, ICON_STOP				, 8);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_MODAL, COUNT					, 9);

	GDEFINE_ENUM_TYPE(SCREEN_CONTROL_LOAD_FILAMENT, uint8_t);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_LOAD_FILAMENT, DIALOG			, 0);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_LOAD_FILAMENT, COUNT			, 1);

	GDEFINE_ENUM_TYPE(SCREEN_CONTROL_SPOOL_INFO, uint8_t);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, DIALOG			,  0);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, TITLE				,  1);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_BRAND		,  2);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_MATERIAL	,  3);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_COLOR		,  4);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_NOTES		,  5);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_DENSITY		,  6);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_DIAMETER	,  7);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, LABEL_SPOOL_SIZE	,  8);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_BRAND		,  9);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_MATERIAL	, 10);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_COLOR		, 11);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_NOTES		, 12);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_DENSITY		, 13);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_DIAMETER	, 14);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, VALUE_SPOOL_SIZE	, 15);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_SPOOL_INFO, COUNT				, 16);

	GDEFINE_ENUM_TYPE(SCREEN_CONTROL_INFO, uint8_t);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_INFO, DIALOG					, 0);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_INFO, COUNT					, 1);

	GDEFINE_ENUM_TYPE(SCREEN_CONTROL_CONFIG, uint8_t);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_CONFIG, DIALOG				, 0);
	GDEFINE_ENUM_VALUE(SCREEN_CONTROL_CONFIG, COUNT					, 1);

	struct SScreenMenu				: public ::dry::SScreen<::dry::SCREEN_CONTROL_MENU_COUNT			> {};
	struct SScreenHUD				: public ::dry::SScreen<::dry::SCREEN_CONTROL_HUD_COUNT				> {};
	struct SScreenModal				: public ::dry::SScreen<::dry::SCREEN_CONTROL_MODAL_COUNT			> {};
	struct SScreenSpoolInfo			: public ::dry::SScreen<::dry::SCREEN_CONTROL_SPOOL_INFO_COUNT		> {};
	struct SScreenLoadFilament		: public ::dry::SScreen<::dry::SCREEN_CONTROL_LOAD_FILAMENT_COUNT	> {};
	struct SScreenInfo				: public ::dry::SScreen<::dry::SCREEN_CONTROL_INFO_COUNT			> {};
	struct SScreenConfig			: public ::dry::SScreen<::dry::SCREEN_CONTROL_CONFIG_COUNT			> {};

	GDEFINE_ENUM_TYPE(MODAL, uint8_t);
	GDEFINE_ENUM_VALUE(MODAL, CLOSE_DOOR		, 0);
	GDEFINE_ENUM_VALUE(MODAL, SCAN_SPOOL		, 1);
	GDEFINE_ENUM_VALUE(MODAL, DRYING_COMPLETE	, 2);
	GDEFINE_ENUM_VALUE(MODAL, DRYING_STOP		, 3);

	struct SScreens {
		::dry::SScreenMenu					Menu;
		::dry::SScreenHUD					HUD;
		::dry::SScreenModal					Modal;
		::dry::SScreenHome					Home;
		::dry::SScreenSpoolInfo				SpoolInfo;

		::dry::SScreenInfo					Info;
		::dry::SScreenLoadFilament			LoadFilament;
		::dry::SScreenConfig				Config;

		bool								ModalEnabled			= false;
		::dry::MODAL						ModalSelected			= ::dry::MODAL_CLOSE_DOOR;
		SCREEN								ScreenSelected			= ::dry::SCREEN_HOME;
		::gpk::cid_t						RootControlIndex		= -1;
	};
#pragma pack(push, 1)
	struct SCommState {
		bool								Ethernet				: 1;
		bool								WiFi					: 1;
		bool								RFID					: 1;
	};
#pragma pack(pop)

	struct SSerialClientQueues {
		::std::mutex								LockSend;
		::std::mutex								LockReceive;
		::gpk::array_obj<::gpk::array_pod<::gpk::sc_t>>	Send;
		::gpk::array_obj<::gpk::array_pod<::gpk::sc_t>>	Received;
	};

	struct SApplication {
#ifndef GPK_WINDOWS
		::s3d::SXCBFramework				Framework;
#else
		::gpk::SFramework					Framework;
#endif
		::dry::SDryer						Dryer					= {};
		::dry::SSpoolCatalog				Catalog					= {};
		::gpk::array_static<::gpk::sc_t, 64>	TimeString				= {};

		::dry::SScreens						Screens					= {};
		::dry::SCommState					Comms					= {};
		::s3d::SSerial						Serial					= {};
		::dry::SSerialClientQueues			Queues;
		::gpk::array_obj<::dry::SSpoolData>	Scanned					= {};
		::std::thread						SerialThread			= {};
		//
		::gpk::refcount_t					ThreadClients			= 0;
		uint64_t							TimeLastSerial			= {}; // these work as watchdog in case serial driver hangs and the thread gets blocked by an io call forever.
		static constexpr	uint64_t		SERIAL_TIMEOUT			= 10; // these work as watchdog in case serial driver hangs and the thread gets blocked by an io call forever.

		static constexpr	uint32_t		MAX_HUMIDITY			= 100;

											SApplication			(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues) {}
	};

	// ----
	::gpk::error_t						setupGUI					(::dry::SApplication & app);
	::gpk::error_t						setupScreenHome				(::dry::SApplication & app);
	// TODO:
	::gpk::error_t						setupScreenLoadFilament		(::dry::SApplication & app);
	::gpk::error_t						setupScreenInfo				(::dry::SApplication & app);
	::gpk::error_t						setupScreenConfig			(::dry::SApplication & app);

	::gpk::error_t						updateApp					(::dry::SApplication & app);
	::gpk::error_t						updateSerial				(::dry::SApplication & app);

	// ----
	::gpk::error_t						updateScreenHUD				(::dry::SApplication & app);
	::gpk::error_t						updateScreenHome			(::dry::SApplication & app);
	// TODO:
	static inline ::gpk::error_t		updateScreenInfo			(::dry::SApplication & /*app*/) { return 0; }
	static inline ::gpk::error_t		updateScreenLoadFilament	(::dry::SApplication & /*app*/) { return 0; }
	static inline ::gpk::error_t		updateScreenConfig			(::dry::SApplication & /*app*/) { return 0; }

	// ----
	::gpk::error_t						handleControlHome			(::dry::SApplication & app, int32_t idControl);
	// TODO: Nothing to do here yet
	static inline ::gpk::error_t		handleControlInfo			(::dry::SApplication & /*app*/, int32_t /*idControl*/) { return 0; }
	static inline ::gpk::error_t		handleControlLoadFilament	(::dry::SApplication & /*app*/, int32_t /*idControl*/) { return 0; }
	static inline ::gpk::error_t		handleControlConfig			(::dry::SApplication & /*app*/, int32_t /*idControl*/) { return 0; }

	// ---- Resize handlers
	::gpk::error_t						handleResizeScreenHUD		(::dry::SApplication & app, ::gpk::n2s1_t newSIze);
	::gpk::error_t						handleResizeScreenMenu		(::dry::SApplication & app, ::gpk::n2s1_t newSIze);
	::gpk::error_t						handleResizeScreenModal		(::dry::SApplication & app, ::gpk::n2s1_t newSIze);
	::gpk::error_t						handleResizeScreenHome		(::dry::SApplication & app, ::gpk::n2s1_t newSIze);
	::gpk::error_t						handleResizeScreenSpoolInfo	(::dry::SApplication & app, ::gpk::n2s1_t newSIze);
	::gpk::error_t						handleResize				(::dry::SApplication & app);

	// ---- Modals
	::gpk::error_t						modalClose					(::dry::SApplication & app);
	::gpk::error_t						modalDisplay				(::dry::SApplication & app, MODAL modal);

	::gpk::error_t						updateProgramDisplay		(::dry::SApplication & app);
	::gpk::error_t						updateCatalogDisplay		(::dry::SApplication & app);

#define USE_3D_CONTROLS

#if defined USE_3D_CONTROLS
#	define DRY_MENU_COLOR_MODE	::gpk::GUI_COLOR_MODE_3D
#	define BACKGROUND_3D_ONLY	false
#else
#	define DRY_MENU_COLOR_MODE ::gpk::GUI_COLOR_MODE_FLAT
#	define BACKGROUND_3D_ONLY	true
#endif
}

//#define SINGLE_GAUGE

#endif // DRYER_APP_H_092743982
