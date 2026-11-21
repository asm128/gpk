#include "gpk_json.h"

#ifndef CATALOG_H_8372492
#define CATALOG_H_8372492

namespace dry
{
#pragma pack(push, 1)
	struct SSpool {
		::gpk::s3_t								UniqueId				= 0;
		::gpk::s3_t								TimestampLoaded			= 0;
		::gpk::s3_t								TimestampUnloaded		= 0;
		::gpk::s3_t								TimestampDryed			= 0;

		::gpk::s1_t								Brand					= -1;
		::gpk::s1_t								Material				= -1;
		::gpk::s1_t								Color					= -1;
		::gpk::s1_t								Notes					= -1;
		::gpk::s1_t								Density					= -1;
		::gpk::s1_t								Diameter				= -1;
		::gpk::s1_t								SpoolSize				= -1;
		::gpk::s1_t								MaxDryingTemperature	= -1;
	};
#pragma pack(pop)

	struct SSpoolData {
		::gpk::s3_t								UniqueId				= 0;
		::gpk::vcst_t						Brand					= {};
		::gpk::vcst_t						Material				= {};
		::gpk::vcst_t						Color					= {};
		::gpk::vcst_t						Notes					= {};
		double								Density					= {};
		double								Diameter				= {};
		::gpk::s2_t								SpoolSize				= {};
		::gpk::s0_t								MaxDryingTemperature	= {};
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

		bool								Exists					(::gpk::s3_t uniqueId) {
			for(::gpk::u2_t iId = 0; iId < FilamentLoaded.size(); ++iId) {
				if(FilamentLoaded[iId].UniqueId == uniqueId)
					return true;
			}
			return false;
		}
	};

	gpk::err_t						catalogSave		(::dry::SSpoolCatalog & catalog, ::gpk::vcst_t filename);
	gpk::err_t						catalogLoad		(::dry::SSpoolCatalog & catalog, ::gpk::vcst_t filename);
	gpk::err_t						catalogRemove	(::dry::SSpoolCatalog & catalog, ::gpk::s2_t index);
	gpk::err_t						catalogAdd
		( ::dry::SSpoolCatalog	& catalog
		, const SSpoolData		& spoolToAdd
		, ::gpk::s3_t				timeLoaded
		, ::gpk::s3_t				timeUnloaded
		, ::gpk::s3_t				timeDryed
		, bool					history
		);
}

#endif //CATALOG_H_8372492
