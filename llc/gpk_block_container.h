#include "gpk_array_static.h"
#include "gpk_array_ptr.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_BLOCK_CONTAINER_H_23627
#define GPK_BLOCK_CONTAINER_H_23627

namespace gpk
{
	tplt<tpnm T, size_t _size>
	class block_container {
		::gpk::appod<::gpk::astatic<T, _size>>	Blocks;
		::gpk::au32								RemainingSpace;
	public:
		::gpk::error_t							clear						()										{ return ::gpk::clear(Blocks, RemainingSpace); }

		::gpk::error_t							Save						(::gpk::apod<uint8_t> & output)	const	{
			gpk_necs(gpk::saveView(output, RemainingSpace));
			for(uint32_t iArray = 0; iArray < RemainingSpace.size(); ++iArray)
				gpk_necs(gpk::saveView(output, *Blocks[iArray]));

			return 0;
		}

		::gpk::error_t							Load						(::gpk::vcu8 & input) {
			clear();
			gpk_necs(gpk::loadView(input, RemainingSpace));
			gpk_necs(Blocks.resize(RemainingSpace.size()));
			for(uint32_t iArray = 0; iArray < Blocks.size(); ++iArray)
				gpk_necs(gpk::loadView(input, Blocks[iArray]->Storage));

			return 0;
		}

		::gpk::error_t							push_sequence				(const T* sequence, uint32_t length, ::gpk::view<const T>& out_view)	{
			for(uint32_t iBlock = 0; iBlock < Blocks.size(); ++iBlock) {
				uint32_t										& blkRemainingSpace			= RemainingSpace[iBlock];
				if(blkRemainingSpace >= length) {
					T												* sequenceStart				= &Blocks[iBlock]->operator[](_size - blkRemainingSpace);
					out_view									= {sequenceStart, length};
					memcpy(sequenceStart, sequence, length * sizeof(T));
					blkRemainingSpace									-= length;
					return iBlock;
				}
			}
			int32_t											indexNewBlock				= Blocks.size();
			gpk_necs(Blocks			.resize(Blocks.size() + 1));
			gpk_necs(RemainingSpace	.resize(Blocks.size(), _size - length));
			out_view									= {&Blocks[indexNewBlock]->operator[](0), length};
			memcpy(Blocks[indexNewBlock]->Storage, sequence, length * sizeof(T));
			return indexNewBlock;
		}
	};
} // namespace

#endif // GPK_BLOCK_CONTAINER_H_230522