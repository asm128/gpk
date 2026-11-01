#include "gpk_ptr_nco.h"

#ifndef GPK_PTR_OBJ_H_23627
#define GPK_PTR_OBJ_H_23627

namespace gpk
{
	tplt<tpnm _tOBJ>
	class pobj : public pnco<_tOBJ> {
	public:
		tydf	_tOBJ			T;
		tydf	pnco<T>			TNCOPtr;
		tydf	pobj<T>			TOBJPtr;
		tydf	gref<T>			TRef;

		usng	TNCOPtr			::Reference;

		inln	cnst T*			oper->	()												csnx	{ return Reference->Instance;														}
		inln	T*				oper->	()												nxpt	{ return (0 == Reference) ? ref_create(&Reference) : Reference->Instance;	}

		tplt<tpnm... _tArgsConstructor>
		inln	T*				create		(_tArgsConstructor &&... argsConstructor)	nxpt	{ return ref_create(&Reference, argsConstructor...);							}
		inln	T*				allocate	()											nxpt	{ return ref_allocate(&Reference);											}
	};
	tplt<tpnm T> usng po	= pobj<T>; 
} // namespace

#endif // GPK_PTR_OBJ_H_23627
