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
} // namespace

#endif // GPK_BLOCK_H_9823749283749823
