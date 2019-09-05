#include "gpk_json.h"

#ifndef GPK_BLOCK_H_9823749283749823
#define GPK_BLOCK_H_9823749283749823

namespace gpk
{
	struct SBlockConfig {
		::gpk::view_const_string					Key;
		uint32_t									BlockSize;
		bool										Deflate;
	};

	template<typename _tBlock>
	struct SMapBlock {
		::gpk::array_pod<uint32_t>					Id;
		::gpk::array_obj<_tBlock>					Block;

		::gpk::SBlockConfig							BlockConfig;
		::gpk::view_const_string					DBName;
	};

	::gpk::error_t								blockConfigLoad				(::gpk::SBlockConfig& out_config, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault = {{}, 65535, 0});
	::gpk::error_t								blockFileName				(const uint32_t idBlock, const ::gpk::view_const_string & dbName, ::gpk::array_pod<char_t> & fileName);
} // namespace

#endif // GPK_BLOCK_H_9823749283749823
