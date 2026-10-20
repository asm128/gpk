#include "gpk_ptr_nco.h"

#ifndef GPK_PTR_OBJ_H_23627
#define GPK_PTR_OBJ_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_t class pobj : public ::gpk::pnco<_t> {
	public:
		tdfTTCnst(_t);
		tydf		pnco<T>	TNCOPtr;
		tydf		gref<T>	TRef;
		using		TNCOPtr	::Reference;
	
					inxp	cnst T*	oper->		()								csnx	{ return Reference->Instance;														}
					inln	T*		oper->		()								nxpt	{ return (0 == Reference) ? ::gpk::ref_create(&Reference) : Reference->Instance;	}
		tpl_vtArgs	inln	T*		create		(_tArgs &&... constructorArgs)	nxpt	{ return ::gpk::ref_create(&Reference, constructorArgs...);							}
					inln	T*		allocate	()								nxpt	{ return ::gpk::ref_allocate(&Reference);											}
	};
	tplT	using	po	= ::gpk::pobj<T>; 
#pragma pack(pop)
} // namespace

#endif // GPK_PTR_OBJ_H_23627


