#include "gpk_block.h"

::gpk::err_t gpk::blockRecordIndices(::gpk::u3_c idRecord, ::gpk::u2_c blockSize, ::gpk::SRecordMap & indices) nxpt {
	if_zero(blockSize)
		rtrn ::gpk::OS_INVALID_PARAMETER;
	cnst ::gpk::u3_t idBlock = idRecord / blockSize;
	if(idBlock > ::gpk::u2_t(-1))
		rtrn ::gpk::OS_OVERRUN;
	indices = {::gpk::u2_t(idBlock), ::gpk::u2_t(idRecord % blockSize)};
	rtrn 0;
}

::gpk::err_t gpk::blockRecordId(cnst ::gpk::SRecordMap & indices, ::gpk::u2_c blockSize, ::gpk::u3_t & idRecord) nxpt {
	if_zero(blockSize)
		rtrn ::gpk::OS_INVALID_PARAMETER;
	if(indices.IndexRecord >= blockSize)
		rtrn ::gpk::OS_INVALID_PARAMETER;
	idRecord = ::gpk::u3_t(indices.IdBlock) * blockSize + indices.IndexRecord;
	rtrn 0;
}
