#include "gpk_json.h"

#ifndef GPK_BLOCK_H_9823749283749823
#define GPK_BLOCK_H_9823749283749823

namespace gpk
{
	template<typename _tBlock>
	struct SMapBlock {
		::gpk::array_pod<uint64_t>					Offset;
		::gpk::array_obj<_tBlock>					Block;
	};

	struct SBlockConfig {
		::gpk::view_const_string					Key;
		uint32_t									BlockSize;
		bool										Deflate;
	};
	::gpk::error_t								blockConfigLoad				(::gpk::SBlockConfig& out_config, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault = {});
} // namespace

#endif // GPK_BLOCK_H_9823749283749823
