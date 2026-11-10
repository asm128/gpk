#include "lsc.h"

sttc ::gpk::err_t test_llc_entry_point(::gpk::SRuntimeValues & runtimeValues) {
	(void)runtimeValues;
	return 0;
}
GPK_OS_ENTRY_POINT(::test_llc_entry_point);