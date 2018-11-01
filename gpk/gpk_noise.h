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
}

#endif // GPK_NOISE_H_908237498237928734893