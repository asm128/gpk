#include "gpk_platform_error.h"

#if defined(GPK_ST)
::gpk::error_t gpk::error_t_from_st_hal_status(HAL_StatusTypeDef status) {
  ::gpk::error_t value = gpk::RESULT_HAL_ERROR;
  switch(status) {
  case HAL_OK     : value = ::gpk::OS_OK     ; break;
  case HAL_ERROR  : value = ::gpk::OS_ERROR  ; break;
  case HAL_BUSY   : value = ::gpk::OS_BUSY   ; break;
  case HAL_TIMEOUT: value = ::gpk::OS_TIMEOUT; break;
  }
  return value;
}
#else
::gpk::error_t gpk::error_t_from_st_hal_status(int32_t status) { return status; }

#endif
