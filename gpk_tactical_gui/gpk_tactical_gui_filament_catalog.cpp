#include "gpk_tactical_gui_filament_catalog.h"

#include "gpk_json.h"
#include "gpk_file.h"
#include "gpk_path.h"
#include "gpk_label.h"
#include "gpk_string.h"

#include <ctime>

::gpk::err_t						dry::catalogAdd
		( ::dry::SSpoolCatalog		& catalog
		, const ::dry::SSpoolData	& spoolData
		, s3_t					timeLoaded
		, s3_t					timeUnloaded
		, s3_t					timeDryed
		, bool						history
		) {
	::dry::SSpool							newFilament			= {};
	newFilament.UniqueId				= spoolData.UniqueId;
	newFilament.Brand					= (s1_t)::gpk::find(spoolData.Brand					, ::gpk::view<::gpk::vcst_c>{catalog.Brand		});
	newFilament.Material				= (s1_t)::gpk::find(spoolData.Material				, ::gpk::view<::gpk::vcst_c>{catalog.Material	});
	newFilament.Color					= (s1_t)::gpk::find(spoolData.Color					, ::gpk::view<::gpk::vcst_c>{catalog.Color		});
	newFilament.Notes					= (s1_t)::gpk::find(spoolData.Notes					, ::gpk::view<::gpk::vcst_c>{catalog.Notes		});
	newFilament.Density					= (s1_t)::gpk::find(spoolData.Density				, ::gpk::view<::gpk::f3_c>{catalog.Density	});
	newFilament.Diameter				= (s1_t)::gpk::find(spoolData.Diameter				, ::gpk::view<::gpk::f3_c>{catalog.Diameter	});
	newFilament.SpoolSize				= (s1_t)::gpk::find(spoolData.SpoolSize				, ::gpk::view<::gpk::s2_c>{catalog.SpoolSize	});
	newFilament.MaxDryingTemperature	= (s1_t)::gpk::find(spoolData.MaxDryingTemperature	, ::gpk::view<::gpk::s0_c>{catalog.MaxDryingTemperature	});
	if(-1 == newFilament.Brand					) newFilament.Brand					= (s1_t)catalog.Brand				.push_back(::gpk::label(spoolData.Brand		));
	if(-1 == newFilament.Material				) newFilament.Material				= (s1_t)catalog.Material				.push_back(::gpk::label(spoolData.Material	));
	if(-1 == newFilament.Color					) newFilament.Color					= (s1_t)catalog.Color				.push_back(::gpk::label(spoolData.Color		));
	if(-1 == newFilament.Notes					) newFilament.Notes					= (s1_t)catalog.Notes				.push_back(::gpk::label(spoolData.Notes		));
	if(-1 == newFilament.Density				) newFilament.Density				= (s1_t)catalog.Density				.push_back(spoolData.Density				);
	if(-1 == newFilament.Diameter				) newFilament.Diameter				= (s1_t)catalog.Diameter				.push_back(spoolData.Diameter				);
	if(-1 == newFilament.SpoolSize				) newFilament.SpoolSize				= (s1_t)catalog.SpoolSize			.push_back(spoolData.SpoolSize				);
	if(-1 == newFilament.MaxDryingTemperature	) newFilament.MaxDryingTemperature	= (s1_t)catalog.MaxDryingTemperature	.push_back(spoolData.MaxDryingTemperature	);
	newFilament.TimestampLoaded			= timeLoaded;
	newFilament.TimestampUnloaded		= timeUnloaded;
	newFilament.TimestampDryed			= timeDryed;
	return history
		? catalog.FilamentHistory.push_back(newFilament)
		: catalog.FilamentLoaded .push_back(newFilament)
		;
}

::gpk::err_t						dry::catalogRemove		(::dry::SSpoolCatalog & catalog, s2_t index) {
	::dry::SSpool							oldFilament			= catalog.FilamentLoaded[index];
	oldFilament.TimestampUnloaded		= time(0);
	catalog.FilamentHistory.push_back(oldFilament);
	return catalog.FilamentLoaded.remove(index);
}

static	::gpk::err_t				jsonArrayFromFilamentView	(::dry::SSpoolCatalog & catalog, ::gpk::view<const ::dry::SSpool> filaments, ::gpk::string & output) {
	output.push_back('[');
	::gpk::string				bufferFormat				= {};
	for(u2_t iFilament = 0; iFilament < filaments.size(); ++iFilament) {
		::dry::SSpool							filament					= filaments[iFilament];
		bufferFormat.resize(1024 * 16);
		s2_t									newSize						= sprintf_s(bufferFormat.Data, bufferFormat.size(),
			"\n\t{ \"unique_id\"             : %llu"
			"\n\t, \"brand\"                 : \"%s\""
			"\n\t, \"material\"              : \"%s\""
			"\n\t, \"color\"                 : \"%s\""
			"\n\t, \"density\"               : %f"
			"\n\t, \"diameter\"              : %f"
			"\n\t, \"spool_size\"            : %u"
			"\n\t, \"max_drying_temperature\": %u"
			"\n\t, \"loaded\"                : %llu"
			"\n\t, \"unloaded\"              : %llu"
			"\n\t, \"dried\"                 : %llu"
			"\n\t, \"notes\"                 : \"%s\""
			"\n\t}"
			, filament.UniqueId
			, ::gpk::string(catalog.Brand		[filament.Brand		]).begin()
			, ::gpk::string(catalog.Material	[filament.Material	]).begin()
			, ::gpk::string(catalog.Color		[filament.Color		]).begin()
			, catalog.Density				[filament.Density				]
			, catalog.Diameter				[filament.Diameter				]
			, catalog.SpoolSize				[filament.SpoolSize				]
			, catalog.MaxDryingTemperature	[filament.MaxDryingTemperature	]
			, filament.TimestampLoaded
			, filament.TimestampUnloaded
			, filament.TimestampDryed
			, ::gpk::string(catalog.Notes		[filament.Notes		]).begin()
			);

		bufferFormat.resize(newSize);
		output.append(bufferFormat);

		if(iFilament < filaments.size() - 1)
			output.push_back(',');
	}
	output.push_back(']');
	return 0;
}

::gpk::err_t						dry::catalogSave		(::dry::SSpoolCatalog & catalog, ::gpk::vcst_t filename) {
	::gpk::string							jsonString				= {};
	jsonString.append_string("{ \"loaded\" : ");
	::jsonArrayFromFilamentView(catalog, catalog.FilamentLoaded, jsonString);
	if(catalog.FilamentHistory.size()) {
		jsonString.append_string("\n, \"history\" : ");
		::jsonArrayFromFilamentView(catalog, catalog.FilamentHistory, jsonString);
	}
	jsonString.append_string("\n}");
	return ::gpk::fileFromMemory(filename, jsonString);
}

::gpk::err_t						dry::catalogLoad		(::dry::SSpoolCatalog & catalog, ::gpk::vcst_t filename) {
	{ if_fail_fef(::gpk::jsonFileRead(catalog.JSONData, filename), "Failed to load filament catalog file: '%.*s'.", GPK_VIEW_ARGS(filename)); }
	const ::gpk::SJSONReader				& reader				= catalog.JSONData.Reader;
	const s2_t							indexHistory			= ::gpk::jsonObjectValueGet(reader, 0, "history");
	static const	::gpk::vcst_t				keys	[]				= {"loaded", "history"};
	for(u2_t iKey = 0; iKey < ::gpk::size(keys); ++iKey) {
		const s2_t							indexValue				= ::gpk::jsonObjectValueGet(reader, 0, keys[iKey]);
		if_true_cef(0 > indexValue, "'%.*s' node not found in '%.*s'", GPK_VIEW_ARGS(keys[iKey]), GPK_VIEW_ARGS(filename)) else {}
		for(u2_t iFilament = 0, countFilaments = ::gpk::jsonArraySize(reader, indexValue); iFilament < countFilaments; ++iFilament) {
			const s2_t							jsonIndexFilament			= ::gpk::jsonArrayValueGet(*reader[indexValue], iFilament);
			const s2_t							jsonIndexUniqueId			= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "unique_id"				);
			const s2_t							jsonIndexBrand				= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "brand"					);
			const s2_t							jsonIndexMaterial			= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "material"				);
			const s2_t							jsonIndexColour				= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "color"					);
			const s2_t							jsonIndexNotes				= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "notes"					);
			const s2_t							jsonIndexDensity			= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "density"				);
			const s2_t							jsonIndexDiameter			= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "diameter"				);
			const s2_t							jsonIndexSpoolSize			= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "spool_size"				);
			const s2_t							jsonIndexMaxDryingTemp		= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "max_drying_temperature"	);
			const s2_t							jsonIndexTimestampLoaded	= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "loaded"					);
			const s2_t							jsonIndexTimestampUnloaded	= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "unloaded"				);
			const s2_t							jsonIndexTimestampDried		= ::gpk::jsonObjectValueGet(reader, jsonIndexFilament, "dried"					);
			if(-1 != jsonIndexUniqueId			)	info_printf("Found filament property '%.*s': %s"	, "unique_id"	, GPK_VIEW_ARGS(reader.View	[jsonIndexBrand		]));
			if(-1 != jsonIndexBrand				)	info_printf("Found filament property '%.*s': %s"	, "brand"		, GPK_VIEW_ARGS(reader.View	[jsonIndexBrand		]));
			if(-1 != jsonIndexMaterial			)	info_printf("Found filament property '%.*s': %s"	, "material"	, GPK_VIEW_ARGS(reader.View	[jsonIndexMaterial	]));
			if(-1 != jsonIndexColour			)	info_printf("Found filament property '%.*s': %s"	, "color"		, GPK_VIEW_ARGS(reader.View	[jsonIndexColour	]));
			if(-1 != jsonIndexNotes				)	info_printf("Found filament property '%.*s': %s"	, "notes"		, GPK_VIEW_ARGS(reader.View	[jsonIndexNotes		]));
			if(-1 != jsonIndexDensity			)	info_printf("Found filament property '%s': %f"	, "density"					, reader.Token	[jsonIndexDensity			].Value);
			if(-1 != jsonIndexDiameter			)	info_printf("Found filament property '%s': %f"	, "diameter"				, reader.Token	[jsonIndexDiameter			].Value);
			if(-1 != jsonIndexSpoolSize			)	info_printf("Found filament property '%s': %u"	, "spool_size"				, reader.Token	[jsonIndexSpoolSize			].Value);
			if(-1 != jsonIndexMaxDryingTemp		)	info_printf("Found filament property '%s': %u"	, "max_drying_temperature"	, reader.Token	[jsonIndexMaxDryingTemp		].Value);
			if(-1 != jsonIndexTimestampLoaded	)	info_printf("Found filament property '%s': %llu", "loaded"					, reader.Token	[jsonIndexTimestampLoaded	].Value);
			if(-1 != jsonIndexTimestampUnloaded	)	info_printf("Found filament property '%s': %llu", "unloaded"				, reader.Token	[jsonIndexTimestampUnloaded	].Value);
			if(-1 != jsonIndexTimestampDried	)	info_printf("Found filament property '%s': %llu", "dried"					, reader.Token	[jsonIndexTimestampDried	].Value);
			::dry::SSpoolData						spoolData					= {};
			spoolData.Brand						= (-1 == jsonIndexBrand		) ? ::gpk::vcst_t{}	: reader.View[jsonIndexBrand	];
			spoolData.Material					= (-1 == jsonIndexMaterial	) ? ::gpk::vcst_t{}	: reader.View[jsonIndexMaterial	];
			spoolData.Color						= (-1 == jsonIndexColour	) ? ::gpk::vcst_t{}	: reader.View[jsonIndexColour	];
			spoolData.Notes						= (-1 == jsonIndexNotes		) ? ::gpk::vcst_t{}	: reader.View[jsonIndexNotes	];
			if(-1 != jsonIndexDensity	)
				memcpy(&spoolData.Density, &reader.Token[jsonIndexDensity	].Value, sizeof(u3_t));
			if(-1 != jsonIndexDiameter	)
				memcpy(&spoolData.Diameter, &reader.Token[jsonIndexDiameter	].Value, sizeof(u3_t));
			spoolData.UniqueId					= (-1 == jsonIndexUniqueId			) ? 0 : (s2_t)reader.Token[jsonIndexUniqueId		].Value;
			spoolData.SpoolSize					= (-1 == jsonIndexSpoolSize			) ? 0 : (s2_t)reader.Token[jsonIndexSpoolSize	].Value;
			spoolData.MaxDryingTemperature		= (-1 == jsonIndexMaxDryingTemp		) ? 0 : (s0_t	)reader.Token[jsonIndexMaxDryingTemp].Value;
			const s2_t							indexFilament				= ::dry::catalogAdd(catalog
				, spoolData
				, (-1 == jsonIndexTimestampLoaded	) ? 0 : reader.Token[jsonIndexTimestampLoaded	].Value
				, (-1 == jsonIndexTimestampUnloaded	) ? 0 : reader.Token[jsonIndexTimestampUnloaded	].Value
				, (-1 == jsonIndexTimestampDried	) ? 0 : reader.Token[jsonIndexTimestampDried	].Value
				, indexValue == indexHistory
				);
			if_fail_cef(indexFilament, "Failed to add spool record to '%s' catalog.", (indexValue == indexHistory) ? "history" : "loaded");
		}
	}
	return 0;
}
