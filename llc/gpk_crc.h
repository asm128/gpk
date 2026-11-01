#include "llc_array_pod.h"

#ifndef LLC_CRC_H_261024
#define LLC_CRC_H_261024

namespace llc
{
	err_t			crcGenerate				(vcu0_t bytes, u3_t & crc);
	err_t			crcVerifyAndRemove		(au0_t & bytes);
	err_t			crcGenerateAndAppend	(au0_t & bytes);
} // namespace

#endif // LLC_DEFLATE_H_23627
