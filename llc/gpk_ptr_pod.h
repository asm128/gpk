#include "gpk_ptr_nco.h"

#ifndef GPK_PTR_POD_H_23627
#define GPK_PTR_POD_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_tstct ppod : pblc pnco<_t> {
		tdfTTCnst(_t);
		tydf		pnco<T>	TNCOPtr;
		tydf		gref<T>	TRef;
		usng		TNCOPtr	::Reference;

					inln	TCnst*	oper->		()								csnx	{ rtrn Reference->Instance;														}
					inln	T*		oper->		()								nxpt	{ rtrn (0 == Reference) ? ::gpk::ref_allocate(&Reference) : Reference->Instance;	}
		tpl_vtArgs	inln	T*		create		(_tArgs &&... constructorArgs)	nxpt	{ rtrn ::gpk::ref_create(&Reference, constructorArgs...);							}
					inln	T*		allocate	()								nxpt	{ rtrn ::gpk::ref_allocate(&Reference);											}
	};
	tplT using pp	= ::gpk::ppod<T>;
#pragma pack(pop)
} // namespace

#endif // GPK_PTR_POD_H_23627


