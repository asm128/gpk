#include "gpk_typeint.h"

#ifndef GPK_NOISE_H_908237498237928734893
#define GPK_NOISE_H_908237498237928734893

namespace gpk
{

	static constexpr	const uint64_t	NOISE_SEED			= 16381; // 525253

	static constexpr	uint32_t		noise1DBase32		(uint32_t x, uint32_t noiseSeed = ::gpk::NOISE_SEED)	noexcept	{ x = (x << 13) ^ x; return ( x * (x * x * noiseSeed + 715827883UL)  + 1500450271UL); }	// 1073741824.0
	static constexpr	uint64_t		noise1DBase			(uint64_t x, uint64_t noiseSeed = ::gpk::NOISE_SEED)	noexcept	{ x = (x << 13) ^ x; return ( x * (x * x * noiseSeed + 715827883ULL)  + 10657331232548839ULL); }
	static constexpr	double			noise1D				(uint64_t x, uint64_t noiseSeed = ::gpk::NOISE_SEED)	noexcept	{ return ( 1.0 - (::gpk::noise1DBase(x, noiseSeed)  & 0x7fFFffFFffFFffFFULL) / 4611686018427387904.0); }
	static constexpr	double			noiseNormal1D		(uint64_t x, uint64_t noiseSeed = ::gpk::NOISE_SEED)	noexcept	{ return ::gpk::noise1D(x, noiseSeed) *.5 + .5f; }
		   
	static constexpr	double			noise2D				(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed = ::gpk::NOISE_SEED)	noexcept	{ x += (y * nWidth);							return ::gpk::noise1D(x, noiseSeed);		}
	static constexpr	double			noise3D				(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed = ::gpk::NOISE_SEED)	noexcept	{ x += (y * nWidth + (z * nHeight * nWidth));	return ::gpk::noise1D(x, noiseSeed);		}
	static constexpr	double			noiseNormal2D		(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed = ::gpk::NOISE_SEED)	noexcept	{ x += (y * nWidth);							return ::gpk::noiseNormal1D(x, noiseSeed);	}
	static constexpr	double			noiseNormal3D		(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed = ::gpk::NOISE_SEED)	noexcept	{ x += (y * nWidth + (z * nHeight * nWidth));	return ::gpk::noiseNormal1D(x, noiseSeed);	}

#pragma pack(push, 1)
	struct SPRNG {
		uint64_t				Seed				= NOISE_SEED;
		uint64_t				Position			= 0;
		uint64_t				Value				= 0;

		inline	uint64_t		Next				()				noexcept	{ return Value = ::gpk::noise1DBase(++Position, Seed); }

		inline	void			Reset				()				noexcept	{ Reset(Seed); }
		void					Reset				(uint64_t seed)	noexcept	{
			Seed					= seed;
			Position				= 0;
			Value					= 0;
		}
	};
#pragma pack(pop)

// --- Some primes that may come in handy
	static constexpr	uint16_t		primes16bit []		=
	{ 14951,  14957,  14969,  14983,  15013,  15017,  15031,  15053,  15061, 15073
	, 15077,  15083,  15091,  15101,  15107,  15121,  15131,  15137,  15139, 15149
	, 15161,  15173,  15187,  15193,  15199,  15217,  15227,  15233,  15241, 15259
	, 15263,  15269,  15271,  15277,  15287,  15289,  15299,  15307,  15313, 15319
	, 15329,  15331,  15349,  15359,  15361,  15373,  15377,  15383,  15391, 15401
	, 15413,  15427,  15439,  15443,  15451,  15461,  15467,  15473,  15493, 15497
	, 15511,  15527,  15541,  15551,  15559,  15569,  15581,  15583,  15601, 15607
	, 15619,  15629,  15641,  15643,  15647,  15649,  15661,  15667,  15671, 15679
	, 15683,  15727,  15731,  15733,  15737,  15739,  15749,  15761,  15767, 15773
	, 15787,  15791,  15797,  15803,  15809,  15817,  15823,  15859,  15877, 15881
	, 15887,  15889,  15901,  15907,  15913,  15919,  15923,  15937,  15959, 15971
	, 15973,  15991,  16001,  16007,  16033,  16057,  16061,  16063,  16067, 16069
	, 16073,  16087,  16091,  16097,  16103,  16111,  16127,  16139,  16141, 16183
	, 16187,  16189,  16193,  16217,  16223,  16229,  16231,  16249,  16253, 16267
	, 16273,  16301,  16319,  16333,  16339,  16349,  16361,  16363,  16369, 16381
	, 16411,  16417,  16421,  16427,  16433,  16447,  16451,  16453,  16477, 16481
	, 16487,  16493,  16519,  16529,  16547,  16553,  16561,  16567,  16573, 16603
	, 16607,  16619,  16631,  16633,  16649,  16651,  16657,  16661,  16673, 16691
	, 16693,  16699,  16703,  16729,  16741,  16747,  16759,  16763,  16787, 16811
	, 16823,  16829,  16831,  16843,  16871,  16879,  16883,  16889,  16901, 16903
	, 16921,  16927,  16931,  16937,  16943,  16963,  16979,  16981,  16987, 16993
	, 17011,  17021,  17027,  17029,  17033,  17041,  17047,  17053,  17077, 17093
	, 17099,  17107,  17117,  17123,  17137,  17159,  17167,  17183,  17189, 17191
	, 17203,  17207,  17209,  17231,  17239,  17257,  17291,  17293,  17299, 17317
	, 17321,  17327,  17333,  17341,  17351,  17359,  17377,  17383,  17387, 17389
	, 17393,  17401,  17417,  17419,  17431,  17443,  17449,  17467,  17471, 17477
	, 17483,  17489,  17491,  17497,  17509,  17519,  17539,  17551,  17569, 17573
	, 17579,  17581,  17597,  17599,  17609,  17623,  17627,  17657,  17659, 17669
	, 17681,  17683,  17707,  17713,  17729,  17737,  17747,  17749,  17761, 17783
	, 17789,  17791,  17807,  17827,  17837,  17839,  17851,  17863,  17881, 17891
	, 17903,  17909,  17911,  17921,  17923,  17929,  17939,  17957,  17959, 17971
	, 17977,  17981,  17987,  17989,  18013,  18041,  18043,  18047,  18049, 18059
	, 18061,  18077,  18089,  18097,  18119,  18121,  18127,  18131,  18133, 18143
	, 18149,  18169,  18181,  18191,  18199,  18211,  18217,  18223,  18229, 18233
	, 18251,  18253,  18257,  18269,  18287,  18289,  18301,  18307,  18311, 18313
	, 18329,  18341,  18353,  18367,  18371,  18379,  18397,  18401,  18413, 18427
	, 18433,  18439,  18443,  18451,  18457,  18461,  18481,  18493,  18503, 18517
	, 18521,  18523,  18539,  18541,  18553,  18583,  18587,  18593,  18617, 18637
	, 18661,  18671,  18679,  18691,  18701,  18713,  18719,  18731,  18743, 18749
	, 18757,  18773,  18787,  18793,  18797,  18803,  18839,  18859,  18869, 18899
	, 18911,  18913,  18917,  18919,  18947,  18959,  18973,  18979,  19001, 19009
	};

}

#endif // GPK_NOISE_H_908237498237928734893
