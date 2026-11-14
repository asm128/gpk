#include "gpk_block.h"
#include "gpk_log.h"

gpk::err_t gpk::blockRecordIndices(u3_c idRecord, u2_c blockSize, SRecordMap & indices) nxpt {
	if_zero_fe(blockSize);
	cnst u3_t idBlock = idRecord / blockSize;
	if_true_fef(idBlock > u2_t(-1), "%" GPK_FMT_S3, idBlock);
	indices = {u2_t(idBlock), u2_t(idRecord % blockSize)};
	rtrn 0;
}

gpk::err_t gpk::blockRecordId(cnst SRecordMap & indices, u2_c blockSize, u3_t & idRecord) nxpt {
	if_zero_fe(blockSize);
	if_true_fef(indices.IndexRecord >= blockSize, GPK_FMT_GE_S2, indices.IndexRecord, blockSize);
	idRecord = u3_t(indices.IdBlock) * blockSize + indices.IndexRecord;
	rtrn 0;
}
