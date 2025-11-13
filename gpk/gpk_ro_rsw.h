#include "gpk_array_pod.h"
#include "gpk_n3.h"

#ifndef GPK_RO_RSW_H_23627
#define GPK_RO_RSW_H_23627

namespace gpk
{
#pragma pack(push, 1)
	struct SRSWWorldObject {
		sc_t				part0			[40];
		sc_t				part1			[40];
		sc_t				part2			[40];
		::gpk::n3f2_t		Position		;
		::gpk::n3f2_t		Rotation		;
		::gpk::n3f2_t		Scale			;
		sc_t				u0				;
		sc_t				uf0				[4];
		sc_t				somePath		[16];
		sc_t				unkstr			[12];
		::gpk::n3<int8_t>	unkchars		;
		uint32_t			minusone		;
		sc_t				somevals		[8];
		float				part3			;
		uint8_t				part4			[4];
	};

	struct SLightInfoRSW {
		sc_t				Name			[40];
		sc_t				ToDo			[40];
		::gpk::n3f2_t		Position 		;
		::gpk::n3f2_t		Color			;
		float				ToDo2			;
	};

	struct SEffectInfoRSW {
		sc_t				name			[40];
		sc_t				nameUnk			[40];
		::gpk::n3f2_t		position		;
		int					id				;
		float				loop			;
		float				param1			;
		float				param2			;
		float				param3			;
		float				param4			;
	};

	struct SSoundInfoRSW {
		sc_t				name			[40];
		sc_t				strUnk0			[40];
		sc_t				fileName		[40];
		sc_t				strUnk1			[40];
		::gpk::n3f2_t		position		;
		float				vol				;
		int32_t				width			;
		int32_t				height			;
		float				range			;
		float				cycle			;			// v >= 2.0
	};

	struct SModelInfoRSW {
		sc_t				Name			[40];
		sc_t				Filename		[40];
		sc_t				Str2			[40];
		sc_t				RootRSMNode		[40];
		sc_t				Str4			[40];
		int32_t				AnimType		;
		float				AnimSpeed		;
		int32_t				BlockType		;
		::gpk::n3f2_t		Position		;
		::gpk::n3f2_t		Rotation		;
		::gpk::n3f2_t		Scale			;
	};

	struct SRSWWorldWater {
		float				Height			;
		uint32_t			Type			;
		float				Amplitude		;
		float				Speed			;
		float				Pitch			;
		uint32_t			TexCycling		;
	};

	struct SRSWWorldLight {
		int32_t				Longitude		;
		int32_t				Latitude		;
		::gpk::n3f2_t		Diffuse			;
		::gpk::n3f2_t		Ambient			;
		float				Intensity		;
	};

	struct SRSWFileContents {
		::gpk::apod<SModelInfoRSW	>	RSWModels		;
		::gpk::apod<SLightInfoRSW	>	RSWLights		;
		::gpk::apod<SEffectInfoRSW	>	RSWEffects		;
		::gpk::apod<SSoundInfoRSW	>	RSWSounds		;

		sc_t								INIFilename		[40];
		sc_t								GNDFilename		[40];
		sc_t								GATFilename		[40];
		sc_t								SOMFilename		[40];

		::gpk::SRSWWorldWater				Water			;
		::gpk::SRSWWorldLight				Light			;

	};
#pragma pack(pop)

	::gpk::error_t		rswFileLoad		(::gpk::SRSWFileContents& loaded, ::gpk::vcu0_c	& input);
	::gpk::error_t		rswFileLoad		(::gpk::SRSWFileContents& loaded, FILE			* input);
	::gpk::error_t		rswFileLoad		(::gpk::SRSWFileContents& loaded, ::gpk::vcst_c	& input);
}

#endif // GPK_RO_RSW_H_23627
