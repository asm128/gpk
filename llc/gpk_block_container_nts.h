#include "gpk_array_ptr.h"
#include "gpk_array_static.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_BLOCK_CONTAINER_NTS_230522
#define GPK_BLOCK_CONTAINER_NTS_230522

namespace gpk
{
	tplt<size_t _size>
	class block_container_nts {
		::gpk::apobj<::gpk::astaticc<_size>>	Blocks;
		::gpk::au32								RemainingSpace;

	public:	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		::gpk::error_t						clear						()								{ return ::gpk::clear(Blocks, RemainingSpace); }
		::gpk::error_t						Save						(::gpk::au8 & output)	const	{
			gpk_necs(::gpk::saveView(output, RemainingSpace));
			for(uint32_t iArray = 0; iArray < RemainingSpace.size(); ++iArray)
				gpk_necs(::gpk::saveView(output, *Blocks[iArray]));
			return 0;
		}

		::gpk::error_t						Load						(::gpk::vcu8 & input)			{
			clear();
			gpk_necs(::gpk::loadView(input, RemainingSpace));
			gpk_necs(Blocks.resize(RemainingSpace.size()));
			for(uint32_t iArray = 0; iArray < Blocks.size(); ++iArray)
				gpk_necs(::gpk::loadView(input, Blocks[iArray]->Storage));
			return 0;
		}

		::gpk::error_t						push_sequence				(const char* sequence, uint32_t length, ::gpk::vcc & out_view)	{
			const uint32_t							lengthPlusOne				= length + 1;
			for(uint32_t iBlock = 0; iBlock < Blocks.size(); ++iBlock) {
				uint32_t								& blkRemainingSpace			= RemainingSpace[iBlock];
				if(blkRemainingSpace >= lengthPlusOne) {
					char									* sequenceStart				= &Blocks[iBlock]->operator[](_size - blkRemainingSpace);
					out_view							= {sequenceStart, length};
					memcpy(sequenceStart, sequence, length);
					sequenceStart[lengthPlusOne]		= 0;
					blkRemainingSpace					-= lengthPlusOne;
					return iBlock;
				}
			}
			int32_t									indexNewBlock				= Blocks.size();
			gpk_necs(Blocks			.resize(Blocks.size() + 1));
			gpk_necs(RemainingSpace	.resize(Blocks.size() + 1, _size - length - 1));
			out_view							= {&Blocks[indexNewBlock]->operator[](0), length};
			memcpy(Blocks[indexNewBlock]->Storage, sequence, length);
			return indexNewBlock;
		}
	};
} // namespace

#endif // GPK_BLOCK_CONTAINER_NTS_230522