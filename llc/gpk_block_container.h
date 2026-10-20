#include "gpk_array_ptr.h"
#include "gpk_array_static.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_BLOCK_CONTAINER_H_261011
#define GPK_BLOCK_CONTAINER_H_261011

namespace gpk
{
	tplt<tpnm T, size_t _size>
	class block_container {
		appod<astatic<T, _size>>	Blocks					= {};
		au2_t						RemainingSpace			= {};

	public:
		err_t						clear					()															{ rtrn clear(Blocks, RemainingSpace); }
		err_t						push_sequence			(cnst T * sequence, u2_t length, view<cnst T> & out_view)	{
			if_true_fe(length && 0 == sequence);
			view<T>							stored	= {};
			err_t							iBlock	= {};
			if_fail_fe(iBlock = reserve(length, stored));
			if(length)
				memcpy(stored.begin(), sequence, stored.byte_count());
			out_view					= {stored.begin(), stored.size()};
			rtrn iBlock;
		}
		err_t						reserve					(u2_t length, view<T> & out_view) {
			if_true_fef(length > _size, GPK_FMT_GT_U2, length, (u2_t)_size);
			for(u2_t iBlock = 0; iBlock < Blocks.size(); ++iBlock) {
				u2_t							& blockRemaining	= RemainingSpace[iBlock];
				if(blockRemaining >= length) {
					out_view					= {&Blocks[iBlock]->oper[]((u2_t)_size - blockRemaining), length};
					blockRemaining				-= length;
					rtrn iBlock;
				}
			}
			ppod<astatic<T, _size>>			newBlock = {};
			if_null_fe(newBlock.allocate());
			memset(newBlock->Storage, 0, newBlock->byte_count());

			err_t							iBlock;
			if_fail_fe(iBlock = Blocks.push_back(newBlock));
			cnst err_t iRemaining = RemainingSpace.push_back((u2_t)_size - length);
			if(0 > iRemaining) {
				Blocks.pop_back();
				rtrn iRemaining;
			}
			out_view					= {&newBlock->oper[](0), length};
			rtrn iBlock;
		}
		err_t						Save					(au0_t & output) cnst {
			if_true_fe(Blocks.size() != RemainingSpace.size());
			if_fail_fe(saveView(output, RemainingSpace));
			for(u2_t iBlock = 0; iBlock < Blocks.size(); ++iBlock) {
				if_fail_fe(saveView(output, view<cnst T>{Blocks[iBlock]->begin(), Blocks[iBlock]->size()}));
			}
			rtrn 0;
		}
		err_t						Load					(vcu0_t & input) {
			vcu0_t							inputToRead				= input;
			au2_t							remainingSpace			= {};
			appod<astatic<T, _size>>		blocks					= {};

			if_fail_fe(loadView(inputToRead, remainingSpace));
			if_fail_fe(blocks.resize(remainingSpace.size()));
			for(u2_t iBlock = 0; iBlock < blocks.size(); ++iBlock) {
				if_true_fef(remainingSpace[iBlock] > _size, GPK_FMT_GT_U2, remainingSpace[iBlock], (u2_t)_size);
				if_null_fe(blocks[iBlock].allocate());

				view<cnst T>					serializedBlock	= {};
				err_t							bytesRead		= {};
				if_fail_fe(bytesRead = viewRead(serializedBlock, inputToRead));
				if_true_fef(serializedBlock.size() != _size, "%u != %u", serializedBlock.size(), (u2_t)_size);
				memcpy(blocks[iBlock]->Storage, serializedBlock.begin(), serializedBlock.byte_count());

				if_fail_fe(inputToRead.slice(inputToRead, bytesRead));
			}
			Blocks						= blocks;
			RemainingSpace				= remainingSpace;
			input						= inputToRead;
			rtrn 0;
		}
	};
} // namespace

#endif // GPK_BLOCK_CONTAINER_H_23627


