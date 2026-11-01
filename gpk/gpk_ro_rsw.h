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
		::gpk::n3<s0_t>	unkchars		;
		u2_t			minusone		;
		sc_t				somevals		[8];
		f2_t				part3			;
		u0_t				part4			[4];
	};

	struct SLightInfoRSW {
		sc_t				Name			[40];
		sc_t				ToDo			[40];
		::gpk::n3f2_t		Position 		;
		::gpk::n3f2_t		Color			;
		f2_t				ToDo2			;
	};

	struct SEffectInfoRSW {
		sc_t				name			[40];
		sc_t				nameUnk			[40];
		::gpk::n3f2_t		position		;
		int					id				;
		f2_t				loop			;
		f2_t				param1			;
		f2_t				param2			;
		f2_t				param3			;
		f2_t				param4			;
	};

	struct SSoundInfoRSW {
		sc_t				name			[40];
		sc_t				strUnk0			[40];
		sc_t				fileName		[40];
		sc_t				strUnk1			[40];
		::gpk::n3f2_t		position		;
		f2_t				vol				;
		s2_t				width			;
		s2_t				height			;
		f2_t				range			;
		f2_t				cycle			;			// v >= 2.0
	};

	struct SModelInfoRSW {
		sc_t				Name			[40];
		sc_t				Filename		[40];
		sc_t				Str2			[40];
		sc_t				RootRSMNode		[40];
		sc_t				Str4			[40];
		s2_t				AnimType		;
		f2_t				AnimSpeed		;
		s2_t				BlockType		;
		::gpk::n3f2_t		Position		;
		::gpk::n3f2_t		Rotation		;
		::gpk::n3f2_t		Scale			;
	};

	struct SRSWWorldWater {
		f2_t				Height			;
		u2_t			Type			;
		f2_t				Amplitude		;
		f2_t				Speed			;
		f2_t				Pitch			;
		u2_t			TexCycling		;
	};

	struct SRSWWorldLight {
		s2_t				Longitude		;
		s2_t				Latitude		;
		::gpk::n3f2_t		Diffuse			;
		::gpk::n3f2_t		Ambient			;
		f2_t				Intensity		;
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

	gpk::err_t 	rswFileLoad		(::gpk::SRSWFileContents& loaded, ::gpk::vcu0_c	& input);
	gpk::err_t 	rswFileLoad		(::gpk::SRSWFileContents& loaded, FILE			* input);
	gpk::err_t 	rswFileLoad		(::gpk::SRSWFileContents& loaded, ::gpk::vcst_c	& input);
}

#endif // GPK_RO_RSW_H_23627
