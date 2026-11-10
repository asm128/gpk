#include "gpk_array.h"

#ifdef GPK_WINDOWS

#include "gpk_windows.h"

#include <Unknwn.h>

#ifndef GPK_COMPLUS_H_23627
#define GPK_COMPLUS_H_23627

namespace gpk
{
	tplt<tpnm TUnknown> 
	gpk::err_t		safe_release	(TUnknown ** storage) { 
		IUnknown*				p				= *storage;
		if(p) { 
			(*storage)			= 0; 
			return (gpk::err_t)p->Release(); 
		} 
		return 0;
	}

	tplt<tpnm TUnknown> 
	gpk::err_t		safe_addref		(TUnknown ** storage, TUnknown * toAcquire) { 
		gpk::err_t			result			= 0;
		IUnknown*				p				= *storage;
		*storage			= toAcquire;
		if(toAcquire)
			result = (gpk::err_t)toAcquire->AddRef();

		if(p)
			p->Release(); 

		return result; 
	}

	tplt<tpnm _tNCO>
	class ptr_com {
	protected:
		_tNCO				* Unknown			= 0;
	public:
		typedef	::gpk::ptr_com<_tNCO>	TNCOPtr;
		typedef	_tNCO					TRef;

		inline					~ptr_com		()									noexcept	{ clear(); }

		inxp					ptr_com			()									noexcept	= default;
		inline					ptr_com			(const TNCOPtr& other)				noexcept	{ Unknown = other.Unknown; if(Unknown) Unknown->AddRef(); }
		inxp					ptr_com			(TNCOPtr&& other)					noexcept	{ Unknown = other.Unknown; other.Unknown = 0; }
		inline					ptr_com			(TRef* other)						noexcept	{ Unknown = other; }

		inxp	operator		TRef		*	()									noexcept	{ return Unknown;	}
		inxp	operator		const TRef	*	()							const	noexcept	{ return Unknown;	}

		inxp	bool			operator==		(const TNCOPtr& other)		const	noexcept	{ return Unknown == other.Unknown; }
		inxp	bool			operator!=		(const TNCOPtr& other)		const	noexcept	{ return !operator==(other); }

		inxp	TNCOPtr			operator =		(const TNCOPtr& other)				noexcept	{ TRef* oldInstance = Unknown; Unknown = other.Unknown; if(Unknown) Unknown->AddRef();	if(oldInstance) oldInstance->Release(); return *this; }
		inxp	TNCOPtr			operator =		(TNCOPtr&& other)					noexcept	{ TRef* oldInstance = Unknown; Unknown = other.Unknown; other.Unknown = 0;					if(oldInstance) oldInstance->Release(); return *this; }
		inxp	TNCOPtr			operator =		(TRef* other)						noexcept	{ TRef* oldInstance = Unknown; Unknown = other; if(Unknown) Unknown->AddRef();				if(oldInstance) oldInstance->Release(); return *this; }

		inline	TRef*			operator->		()									noexcept	{ return Unknown; }
		inline	const TRef*		operator->		()							const	noexcept	{ return Unknown; }

		inline	TRef**			operator &		()									noexcept	{ return &Unknown; }

		inline	uint32_t		clear			()									noexcept	{ return ::gpk::safe_release(&Unknown); }
		inxp	const TRef*		get_ref			()							const	noexcept	{ return Unknown; }
		inxp	const TRef*		set_ref			(TRef* ref)							noexcept	{ ::gpk::safe_addref(&Unknown, ref); return Unknown; }
		inxp	TRef*			get				()							const	noexcept	{ return Unknown; }

		tplt<tpnm _tNCOOther>
		inline	gpk::err_t	as				(_tNCOOther** other)				noexcept	{ 
			_tNCOOther*					old				= *other;
			*other					= {};
			HRESULT						hr				= Unknown ? Unknown->QueryInterface(__uuidof(_tNCOOther), (void**)other) : 0;
			if(old)
				old->Release();
			return hr;
		}

		tplt<tpnm _tNCOOther>
		inline	gpk::err_t	as				(::gpk::ptr_com<_tNCOOther>& other)	noexcept	{ 
			::gpk::ptr_com<_tNCOOther>	old				= other;
			other					= {};
			HRESULT						hr				= Unknown ? Unknown->QueryInterface(__uuidof(_tNCOOther), (void**)&other) : 0;
			return hr;
		}
	};
	tplt <tpnm T> using pcom		= ::gpk::ptr_com<T>;
	tplt <tpnm T> using acom		= ::gpk::aobj<::gpk::pcom<T>>;
	tplt <tpnm T> using vcom		= ::gpk::view<::gpk::pcom<T>>;
	tplt <tpnm T> using array_com	= ::gpk::acom<T>;
	tplt <tpnm T> using view_com	= ::gpk::vcom<T>;
}

#endif // GPK_COMPLUS_H_23627

#endif // GPK_WINDOWS - ignore the whole thing if not a Windows build
