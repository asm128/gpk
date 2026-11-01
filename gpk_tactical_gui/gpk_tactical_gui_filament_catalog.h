#include "gpk_json.h"

#ifndef GPK_TACTICAL_GUI_FILAMENT_CATALOG_H_8372492
#define GPK_TACTICAL_GUI_FILAMENT_CATALOG_H_8372492

namespace dry
{
#pragma pack(push, 1)
	struct SSpool {
		int64_t								UniqueId				= 0;
		int64_t								TimestampLoaded			= 0;
		int64_t								TimestampUnloaded		= 0;
		int64_t								TimestampDryed			= 0;

		int16_t								Brand					= -1;
		int16_t								Material				= -1;
		int16_t								Color					= -1;
		int16_t								Notes					= -1;
		int16_t								Density					= -1;
		int16_t								Diameter				= -1;
		int16_t								SpoolSize				= -1;
		int16_t								MaxDryingTemperature	= -1;
	};
#pragma pack(pop)

	struct SSpoolData {
		int64_t								UniqueId				= 0;
		::gpk::vcst_t						Brand					= {};
		::gpk::vcst_t						Material				= {};
		::gpk::vcst_t						Color					= {};
		::gpk::vcst_t						Notes					= {};
		double								Density					= {};
		double								Diameter				= {};
		int32_t								SpoolSize				= {};
		int8_t								MaxDryingTemperature	= {};
	};

	struct SSpoolCatalog {
		::gpk::aobj<::gpk::vcst_t>		 Brand					;
		::gpk::aobj<::gpk::vcst_t>		 Material				;
		::gpk::aobj<::gpk::vcst_t>		 Color					;
		::gpk::aobj<::gpk::vcst_t>		 Notes					;
		::gpk::aobj<::gpk::f3_t>			Density					;
		::gpk::aobj<::gpk::f3_t>			Diameter				;
		::gpk::aobj<::gpk::s2_t>			SpoolSize				;
		::gpk::aobj<::gpk::s0_t>			MaxDryingTemperature	;

		::gpk::array_pod<::dry::SSpool>		FilamentLoaded			;
		::gpk::array_pod<::dry::SSpool>		FilamentHistory			;

		::gpk::SJSONFile					JSONData				;

		bool								Exists					(int64_t uniqueId) {
			for(uint32_t iId = 0; iId < FilamentLoaded.size(); ++iId) {
				if(FilamentLoaded[iId].UniqueId == uniqueId)
					return true;
			}
			return false;
		}
	};

	::gpk::error_t						catalogSave		(::dry::SSpoolCatalog & catalog, ::gpk::vcst_t filename);
	::gpk::error_t						catalogLoad		(::dry::SSpoolCatalog & catalog, ::gpk::vcst_t filename);
	::gpk::error_t						catalogRemove	(::dry::SSpoolCatalog & catalog, ::gpk::s2_t index);
	::gpk::error_t						catalogAdd
		( ::dry::SSpoolCatalog	& catalog
		, const SSpoolData		& spoolToAdd
		, int64_t				timeLoaded
		, int64_t				timeUnloaded
		, int64_t				timeDryed
		, bool					history
		);
}

#endif // GPK_TACTICAL_GUI_FILAMENT_CATALOG_H_8372492
