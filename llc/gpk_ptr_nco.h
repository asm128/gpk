#include "gpk_ref.h"

#ifndef GPK_PTR_NCO_H_23627
#define GPK_PTR_NCO_H_23627

namespace gpk
{
	tplt<tpnm _tNCO>
	class pnco {
	protected:
		gref<_tNCO>		* Reference	= 0;
	public:
		typedef	_tNCO			T;
		typedef	pnco<T>			TNCOPtr;
		typedef	gref<T>	TRef;

		inln					~pnco		()								nxpt	{ clear(); }
		inxp					pnco		()								nxpt	= dflt;
		inln					pnco		(cnst TNCOPtr & other)			nxpt	{ Reference = ref_acquire(other.Reference);	}
		inxp					pnco		(TNCOPtr && other)				nxpt	{ Reference = other.Reference; other.Reference = 0;	}
		inln					pnco		(TRef * other)					nxpt	{ Reference = other; }

		inxp	oper			T*			()								nxpt	{ return Reference ? Reference->Instance : 0; }
		inxp	oper			cnst T*	()									csnx	{ return Reference ? Reference->Instance : 0; }

		inxp	b8_t			oper==	(cnst TNCOPtr & other)				csnx	{ return Reference == other.Reference; }
		inxp	b8_t			oper!=	(cnst TNCOPtr & other)				csnx	{ return !oper==(other); }

		inxp	TNCOPtr			oper =	(cnst TNCOPtr & other)				nxpt	{ TRef * oldInstance = Reference; Reference = ref_acquire(other.Reference);	ref_release(&oldInstance); return *this; }
		inxp	TNCOPtr			oper =	(TNCOPtr && other)					nxpt	{ TRef * oldInstance = Reference; Reference = other.Reference; other.Reference = 0;	ref_release(&oldInstance); return *this; }
		inxp	TNCOPtr			oper =	(TRef * other)						nxpt	{ TRef * oldInstance = Reference; Reference = other; ref_release(&oldInstance); return *this; }

		inln	T*				oper->	()									nxpt	{ return Reference->Instance; }
		inln	cnst T*			oper->	()									csnx	{ return Reference->Instance; }

		inln	TRef **			oper &	()									nxpt	{ return &Reference; }

		inxp	cnst TRef*		get_ref		()								csnx	{ return Reference; }
		inxp	cnst TRef*		set_ref		(TRef * ref)					nxpt	{ TRef * oldInstance = Reference; Reference = ref; ref_release(&oldInstance); return Reference; }
		inln	err_t			clear		()								nxpt	{ return ref_release(&Reference); }

		tplt<tpnm _tNCOOther>
		inln	_tNCO*			as			(_tNCOOther* * other)			nxpt	{ return *other = (Reference ? dynamic_cast<_tNCOOther*>(Reference->Instance) : 0); }

		tplt<tpnm _tNCOOther>
		inln	_tNCO*			as			(pnco<_tNCOOther> & other)		nxpt	{ other = {(gref<_tNCOOther>*)ref_acquire(Reference)}; return 0; }
	};
	tplt <tpnm T> usng pn		= pnco<T>; 
	tplt <tpnm T> usng pi		= pnco<T>; 
} // namespace

#endif // GPK_PTR_NCO_H_23627
