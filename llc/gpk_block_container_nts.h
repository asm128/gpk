#include "gpk_block_container.h"

#ifndef GPK_BLOCK_CONTAINER_NTS_261011
#define GPK_BLOCK_CONTAINER_NTS_261011

namespace gpk
{
	tplt<size_t _size>
	class block_container_nts {
		block_container<sc_t, _size>	Characters = {};

	public:
		err_t	clear			() { rtrn Characters.clear(); }
		err_t	Save			(au0_t & output) const { rtrn Characters.Save(output); }
		err_t	Load			(vcu0_t & input) { rtrn Characters.Load(input); }

		err_t	push_sequence	(sc_c * sequence, u2_t length, vcst_t & out_view) {
			if_true_fef(length >= _size, GPK_FMT_GE_U2, length, (u2_t)_size);
			if_true_fe(length && 0 == sequence);
			vs0_t	stored	= {};
			err_t	iBlock	= {};
			if_fail_fe(iBlock = Characters.reserve(length + 1, stored));
			if(length)
				memcpy(stored.begin(), sequence, length);
			stored[length]	= 0;
			out_view		= {stored.begin(), length};
			rtrn iBlock;
		}
	};
} // namespace

#endif // GPK_BLOCK_CONTAINER_NTS_261011


