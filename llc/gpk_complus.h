#include "gpk_array_obj.h"

#ifdef GPK_WINDOWS

#include "gpk_windows.h"

#include <Unknwn.h>

#ifndef GPK_COMPLUS_H_23627
#define GPK_COMPLUS_H_23627

namespace gpk
{
	tplt<tpnm TUnknown> 
	error_t		safe_release	(TUnknown ** storage) { 
		IUnknown		* p				= *storage;
		if(p) { 
			(*storage)	= 0; 
			return (error_t)p->Release(); 
		} 
		return 0;
	}

	tplt<tpnm TUnknown> 
	error_t		safe_addref		(TUnknown ** storage, TUnknown * toAcquire) { 
		error_t			result			= 0;
		IUnknown		*p				= *storage;
		*storage	= toAcquire;
		if(toAcquire)
			result		= (error_t)toAcquire->AddRef();

		if(p)
			p->Release(); 

		return result; 
	}

	tplt<tpnm _tCOM>
	class ptr_com {
	protected:
		_tCOM				* Unknown	= 0;
	public:
		tydf				ptr_com<_tCOM>	TCOM;
		tydf				_tCOM			TRef;

		inln				~ptr_com	()							nxpt	{ clear(); }

		inxp				ptr_com		()							nxpt	= default;
		inln				ptr_com		(TRef		* other)		nxpt	{ Unknown = other;			if(Unknown) Unknown->AddRef(); }
		inln				ptr_com		(cnst TCOM	& other)		nxpt	{ Unknown = other.Unknown;	if(Unknown) Unknown->AddRef(); }
		inxp				ptr_com		(TCOM		&& other)		nxpt	{ Unknown = other.Unknown;	other.Unknown = 0; }

		inxp	oper		TRef *		()							nxpt	{ return Unknown;	}
		inxp	oper		cnst TRef *	()							csnx	{ return Unknown;	}

		inxp	bool		oper==		(cnst TCOM & other)			csnx	{ return Unknown == other.Unknown; }
		inxp	bool		oper!=		(cnst TCOM & other)			csnx	{ return !oper==(other); }

		inxp	TCOM		oper =		(TRef * other)				nxpt	{ TRef * oldInstance = Unknown; Unknown = other;			if(Unknown) Unknown->AddRef();	safe_release(&oldInstance); return *this; }
		inxp	TCOM		oper =		(cnst TCOM & other)			nxpt	{ TRef * oldInstance = Unknown; Unknown = other.Unknown;	if(Unknown) Unknown->AddRef();	safe_release(&oldInstance); return *this; }
		inxp	TCOM		oper =		(TCOM && other)				nxpt	{ TRef * oldInstance = Unknown; Unknown = other.Unknown;	other.Unknown = 0;				safe_release(&oldInstance); return *this; }

		inln	TRef*		oper->		()							nxpt	{ return Unknown; }
		inln	cnst TRef*	oper->		()							csnx	{ return Unknown; }

		inln	TRef**		oper &		()							nxpt	{ return &Unknown; }

		inln	u2_t		clear		()							nxpt	{ return safe_release(&Unknown); }
		inxp	cnst TRef*	get_ref		()							csnx	{ return Unknown; }
		inxp	cnst TRef*	set_ref		(TRef * ref)				nxpt	{ safe_addref(&Unknown, ref); return Unknown; }
		inxp	TRef*		get			()							csnx	{ return Unknown; }

		tplt<tpnm _tOther>
		inln	err_t		as			(_tOther* * other)			nxpt	{ 
			_tOther					* old		= *other;
			*other				= {};
			HRESULT					hr			= Unknown ? Unknown->QueryInterface(__uuidof(_tOther), (void**)other) : 0;
			if(old)
				old->Release();
			return hr;
		}
		tplt<tpnm _tOther>
		inln	err_t		as			(ptr_com<_tOther> & other)	nxpt	{ 
			ptr_com<_tOther>		old			= other;
			other				= {};
			HRESULT					hr			= Unknown ? Unknown->QueryInterface(__uuidof(_tOther), (void**)&other) : 0;
			return hr;
		}
	};
	tplTusng	pcom		= ptr_com<T>;
	tplTusng	acom		= aobj<pcom<T>>;
	tplTusng	vcom		= view<pcom<T>>;
	tplTusng	array_com	= acom<T>;
	tplTusng	view_com	= vcom<T>;
}

#endif // GPK_COMPLUS_H_23627

#endif // GPK_WINDOWS - ignore the whole thing if not a Windows build


