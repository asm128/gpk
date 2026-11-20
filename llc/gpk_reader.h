#include "gpk_reader_core.h"
#include "gpk_array_obj.h"
#include "gpk_array_pod.h"

#ifndef GPK_READER_H
#define GPK_READER_H

nmsp gpk
{
#pragma pack(push, 1)
	tplt<tpnm TToken>
	stct SReader {
		view<sc_c>		Source	= {};
		apod<TToken>	Token	= {};
		aobj<vcst_t>	View	= {};

		inln oper SReaderView<TToken>() cnst { rtrn {Source, Token, View}; }
	};

#pragma pack(pop)
} // nmsp

#endif // GPK_READER_H
