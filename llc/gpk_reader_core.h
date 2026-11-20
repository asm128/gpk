#include "gpk_view.h"

#ifndef GPK_READER_CORE_H
#define GPK_READER_CORE_H

nmsp gpk
{
#pragma pack(push, 1)
	tplt<tpnm TToken, tpnm TIndex = s2_t, tpnm TNest = s2_t>
	stct SReaderState {
		u2_t		IndexCurrentChar		= 0;
		TIndex		IndexCurrentElement	= (TIndex)-1;
		TToken		* CurrentElement		= 0;
		TNest		NestLevel			= 0;
		sc_t		CharCurrent			= 0;
	};

	tplt<tpnm TToken>
	stct SReaderView {
		view<sc_c>			Source	= {};
		view<cnst TToken>	Token	= {};
		view<vcst_c>		View	= {};
	};
#pragma pack(pop)
} // nmsp

#endif // GPK_READER_CORE_H
