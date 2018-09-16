#include "gpk_noise.h"
																																																												// 4611686018427387904
uint64_t					gpk::noise1DBase			(uint64_t x, uint64_t noiseSeed)																noexcept	{ x = (x << 13) ^ x; return ( x * (x * x * noiseSeed + 715827883ULL) + 9876534021204356789ULL); }
double						gpk::noise1D				(uint64_t x, uint64_t noiseSeed)																noexcept	{ return ( 1.0 - (noise1DBase(x, noiseSeed)  & 0x7fFFffFFffFFffFFULL) / 4611686018427387904.0); }
double						gpk::noiseNormal1D			(uint64_t x, uint64_t noiseSeed)																noexcept	{ return ::gpk::noise1D(x, noiseSeed) *.5 + .5f; }																							  
	//----------------------------
double						gpk::noise2D				(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed)	noexcept	{ x += (y * nWidth);							return ::gpk::noise1D(x, noiseSeed);		}
double						gpk::noise3D				(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed)	noexcept	{ x += (y * nWidth + (z * nHeight * nWidth));	return ::gpk::noise1D(x, noiseSeed);		}
double						gpk::noiseNormal2D			(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed)	noexcept	{ x += (y * nWidth);							return ::gpk::noiseNormal1D(x, noiseSeed);	}
double						gpk::noiseNormal3D			(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed)	noexcept	{ x += (y * nWidth + (z * nHeight * nWidth));	return ::gpk::noiseNormal1D(x, noiseSeed);	}
// Based on noiseNormal1D() returns a value between 0 and 1.
//double						::gpk::randNoise			(uint64_t seed)																					noexcept	{
//		static	::gpk::SRandomGenerator		generator;
//		if( generator.Seed != seed )
//			generator.Reset(seed);
//		return generator.Next();
//	}
