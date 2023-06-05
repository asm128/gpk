#include "gpk_array_pod.h"
#include "gpk_n3.h"

#ifndef GPK_RO_RSW_H_9283742893742893
#define GPK_RO_RSW_H_9283742893742893

namespace gpk
{
#pragma pack(push, 1)
	struct SRSWWorldObject {
					char								part0			[40];
					char								part1			[40];
					char								part2			[40];
					::gpk::n3f32				Position		;
					::gpk::n3f32				Rotation		;
					::gpk::n3f32				Scale			;
					char								u0				;
					char								uf0				[4];
					char								somePath		[16];
					char								unkstr			[12];
					::gpk::n3<int8_t>				unkchars		;
					uint32_t							minusone		;
					char								somevals		[8];
					float								part3			;
					uint8_t								part4			[4];
	};

	struct SLightInfoRSW {
					char								Name			[40];
					char								ToDo			[40];
					::gpk::n3f32				Position 		;
					::gpk::n3f32				Color			;
					float								ToDo2			;
	};

	struct SEffectInfoRSW {
					char								name			[40];
					char								nameUnk			[40];
					::gpk::n3f32				position		;
					int									id				;
					float								loop			;
					float								param1			;
					float								param2			;
					float								param3			;
					float								param4			;
	};

	struct SSoundInfoRSW {
					char								name			[40];
					char								strUnk0			[40];
					char								fileName		[40];
					char								strUnk1			[40];
					::gpk::n3f32				position		;
					float								vol				;
					int32_t								width			;
					int32_t								height			;
					float								range			;
					float								cycle			;			// v >= 2.0
	};

	struct SModelInfoRSW {
					char								Name			[40];
					char								Filename		[40];
					char								Str2			[40];
					char								RootRSMNode		[40];
					char								Str4			[40];
					int32_t								AnimType		;
					float								AnimSpeed		;
					int32_t								BlockType		;
					::gpk::n3f32				Position		;
					::gpk::n3f32				Rotation		;
					::gpk::n3f32				Scale			;
	};

	struct SRSWWorldWater {
					float								Height			;
					uint32_t							Type			;
					float								Amplitude		;
					float								Speed			;
					float								Pitch			;
					uint32_t							TexCycling		;
	};

	struct SRSWWorldLight {
					int32_t								Longitude		;
					int32_t								Latitude		;
					::gpk::n3f32				Diffuse			;
					::gpk::n3f32				Ambient			;
					float								Intensity		;
	};

	struct SRSWFileContents {
					::gpk::apod<SModelInfoRSW	>	RSWModels		;
					::gpk::apod<SLightInfoRSW	>	RSWLights		;
					::gpk::apod<SEffectInfoRSW	>	RSWEffects		;
					::gpk::apod<SSoundInfoRSW	>	RSWSounds		;

					char								INIFilename		[40];
					char								GNDFilename		[40];
					char								GATFilename		[40];
					char								SOMFilename		[40];

					::gpk::SRSWWorldWater				Water			;
					::gpk::SRSWWorldLight				Light			;

	};
#pragma pack(pop)

	::gpk::error_t		rswFileLoad		(::gpk::SRSWFileContents& loaded, const ::gpk::vcu8	& input);
	::gpk::error_t		rswFileLoad		(::gpk::SRSWFileContents& loaded, FILE				* input);
	::gpk::error_t		rswFileLoad		(::gpk::SRSWFileContents& loaded, const ::gpk::vcs	& input);
}

#endif // GPK_RO_RSW_H_9283742893742893
