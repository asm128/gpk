#include "gpk_error.h"

#ifndef GPK_PLATFORM_ERROR_H_9237849823
#define GPK_PLATFORM_ERROR_H_9237849823

namespace gpk
{
#if defined(GPK_ST)
	::gpk::error_t error_t_from_st_hal_status(HAL_StatusTypeDef halStatus);
#else 
	::gpk::error_t error_t_from_st_hal_status(int32_t status);
#endif
} // namespace

#endif // GPK_ERROR_HELPER_H_9237849823
