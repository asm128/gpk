#include "gpk_array_pod.h"

#ifndef GPK_CRC_H_261024
#define GPK_CRC_H_261024

namespace gpk
{
	err_t			crcGenerate				(vcu0_t bytes, u3_t & crc);
	err_t			crcVerifyAndRemove		(au0_t & bytes);
	err_t			crcGenerateAndAppend	(au0_t & bytes);
} // namespace

#endif // GPK_DEFLATE_H_23627
