#include "gpk_error.h"

#ifndef GPK_BLOCK_H_23627
#define GPK_BLOCK_H_23627

namespace gpk
{
	stct SRecordMap {
		::gpk::u2_t	IdBlock		= {};
		::gpk::u2_t	IndexRecord	= {};
	};

	gpk::err_t blockRecordIndices	(::gpk::u3_c idRecord, ::gpk::u2_c blockSize, ::gpk::SRecordMap & indices)	nxpt;
	gpk::err_t blockRecordId		(cnst ::gpk::SRecordMap & indices, ::gpk::u2_c blockSize, ::gpk::u3_t & idRecord)	nxpt;
} // namespace

#endif // GPK_BLOCK_H_23627
