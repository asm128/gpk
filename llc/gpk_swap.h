#include "gpk_platform_globals.h"

#ifndef GPK_ATMEL
#	include <memory> // this is required for ::std::swap()
#endif // GPK_ATMEL

#ifndef GPK_SWAP_H
#define GPK_SWAP_H

nmsp gpk
{
#ifndef GPK_ATMEL
	usng ::std::swap;
#else // GPK_ATMEL
	tplt<class _Ty>
	inxp	void	swap(_Ty & _Left, _Ty & _Right) nxpt {
		_Ty _Tmp = _Left;
		_Left    = _Right;
		_Right   = _Tmp;
	}
#endif // GPK_ATMEL
} // nmsp

#endif // GPK_SWAP_H
