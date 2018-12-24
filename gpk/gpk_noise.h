#include "gpk_typeint.h"

#ifndef GPK_NOISE_H_908237498237928734893
#define GPK_NOISE_H_908237498237928734893

namespace gpk
{

	static constexpr	const uint64_t			NOISE_SEED			= 16381; // 525253

						uint64_t				noise1DBase			(uint64_t x, uint64_t noiseSeed = 16381)																noexcept;
						double					noise1D				(uint64_t x, uint64_t noiseSeed = 16381)																noexcept;
						double					noiseNormal1D		(uint64_t x, uint64_t noiseSeed = 16381)																noexcept;
						double					noise2D				(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed = 16381)	noexcept;
						double					noise3D				(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed = 16381)	noexcept;
						double					noiseNormal2D		(uint32_t x, uint32_t y				, uint32_t nWidth					, uint64_t noiseSeed = 16381)	noexcept;
						double					noiseNormal3D		(uint32_t x, uint32_t y, uint32_t z	, uint32_t nWidth, uint32_t nHeight	, uint64_t noiseSeed = 16381)	noexcept;

#pragma pack(push, 1)
	struct SPRNG {
							uint64_t				Seed				= 16381;
							uint64_t				Position			= 0;
							uint64_t				Value				= 0;

		inline				uint64_t				Next				()																										noexcept	{ return Value = ::gpk::noise1DBase(++Position, Seed); }
							void					Reset				()																										noexcept	{ Reset(Seed); }
							void					Reset				(uint64_t seed)																							noexcept	{
			Seed										= seed;
			Position									= 0;
			Value										= 0;
		}
	};
#pragma pack(pop)
}

#endif // GPK_NOISE_H_908237498237928734893