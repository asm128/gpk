#include "gpk_ptr_nco.h"

#ifndef GPK_PTR_POD_H_23627
#define GPK_PTR_POD_H_23627

namespace gpk
{
	tplt<tpnm _tPOD>
	class ppod : public pnco<_tPOD> {
	public:
		tydf _tPOD			T;
		tydf	pnco<T>	TNCOPtr;
		tydf	gref<T>	TRef;

		usng	TNCOPtr			::Reference;
								
		inln	cnst T*			oper->	()												csnx	{ return Reference->Instance;														}
		inln	T*				oper->	()												nxpt	{ return (0 == Reference) ? ref_allocate(&Reference) : Reference->Instance;	}

		tplt<tpnm... _tArgsConstructor>
		inln	T*				create		(_tArgsConstructor &&... argsConstructor)	nxpt		{ return ref_create(&Reference, argsConstructor...);		}
		inln	T*				allocate	()											nxpt		{ return ref_allocate(&Reference);						}
	};
	tplt<tpnm T> usng	pp	= ppod<T>;
} // namespace

#endif // GPK_PTR_POD_H_23627
