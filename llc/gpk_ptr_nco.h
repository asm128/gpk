#include "gpk_ref.h"

#ifndef GPK_PTR_NCO_H_23627
#define GPK_PTR_NCO_H_23627

namespace gpk
{
	tpl_t class pnco {
	protected:
		gref<_t>			* Reference	= 0;
	public:
		tydf	_t			T;
		tydf	pnco<T>		TNCOPtr;
		tydf	gref<T>		TRef;

		inln				~pnco		()						nxpt	{ clear(); }
		inxp				pnco		()						nxpt	= default;
		inln				pnco		(cnst TNCOPtr & other)	nxpt	{ Reference = ref_acquire(other.Reference);	}
		inxp				pnco		(TNCOPtr && other)		nxpt	{ Reference = other.Reference; other.Reference = 0;	}
		inln				pnco		(TRef * other)			nxpt	{ Reference = other; }

		ndix	oper		T*			()						nxpt	{ return Reference ? Reference->Instance : 0; }
		ndix	oper		cnst T*		()						csnx	{ return Reference ? Reference->Instance : 0; }

		ndix	bool		oper==		(cnst TNCOPtr & other)	csnx	{ return Reference == other.Reference; }
		ndix	bool		oper!=		(cnst TNCOPtr & other)	csnx	{ return !oper==(other); }

		inxp	TNCOPtr		oper =		(cnst TNCOPtr & other)	nxpt	{ TRef * oldInstance = Reference; Reference = ref_acquire(other.Reference);	ref_release(&oldInstance); return *this; }
		inxp	TNCOPtr		oper =		(TNCOPtr && other)		nxpt	{ TRef * oldInstance = Reference; Reference = other.Reference; other.Reference = 0;	ref_release(&oldInstance); return *this; }
		inxp	TNCOPtr		oper =		(TRef * other)			nxpt	{ TRef * oldInstance = Reference; Reference = other; ref_release(&oldInstance); return *this; }

		inln	T*			oper->		()						nxpt	{ return Reference->Instance; }
		inln	cnst T*		oper->		()						csnx	{ return Reference->Instance; }

		inln	TRef**		oper &		()						nxpt	{ return &Reference; }

		ndix	cnst TRef*	get_ref		()						csnx	{ return Reference; }
		inxp	cnst TRef*	set_ref		(TRef * ref)			nxpt	{ TRef * oldInstance = Reference; Reference = ref; ref_release(&oldInstance); return Reference; }
		inln	err_t		clear		()						nxpt	{ return ref_release(&Reference); }
		tpl_t2	inln	T*	as			(_t2* * other)			nxpt	{ return *other = Reference ? dynamic_cast<_t2*>(Reference->Instance) : 0; }
		tpl_t2	inln	T*	as			(pnco<_t2> & other)		nxpt	{ return *(other = ref_acquire(Reference)); }
	};
	tplT	using	pn		= pnco<T>; 
	tplT	using	pi		= pnco<T>; 
} // namespace

#endif // GPK_PTR_NCO_H_23627


