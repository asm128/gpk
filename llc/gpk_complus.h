#include "gpk_array.h"

#ifdef GPK_WINDOWS

#include "gpk_windows.h"

#include <Unknwn.h>

#ifndef GPK_COMPLUS_H_23627
#define GPK_COMPLUS_H_23627

namespace gpk
{
	template<typename TUnknown> 
	::gpk::error_t		safe_release	(TUnknown ** storage) { 
		IUnknown*				p				= *storage;
		if(p) { 
			(*storage)			= 0; 
			return (::gpk::error_t)p->Release(); 
		} 
		return 0;
	}

	template<typename TUnknown> 
	::gpk::error_t		safe_addref		(TUnknown ** storage, TUnknown * toAcquire) { 
		::gpk::error_t			result			= 0;
		IUnknown*				p				= *storage;
		*storage			= toAcquire;
		if(toAcquire)
			result = (::gpk::error_t)toAcquire->AddRef();

		if(p)
			p->Release(); 

		return result; 
	}

	template<typename _tNCO>
	class ptr_com {
	protected:
		_tNCO				* Unknown			= 0;
	public:
		typedef	::gpk::ptr_com<_tNCO>	TNCOPtr;
		typedef	_tNCO					TRef;

		inline					~ptr_com		()									noexcept	{ clear(); }

		inlcxpr					ptr_com			()									noexcept	= default;
		inline					ptr_com			(const TNCOPtr& other)				noexcept	{ Unknown = other.Unknown; if(Unknown) Unknown->AddRef(); }
		inlcxpr					ptr_com			(TNCOPtr&& other)					noexcept	{ Unknown = other.Unknown; other.Unknown = 0; }
		inline					ptr_com			(TRef* other)						noexcept	{ Unknown = other; }

		inlcxpr	operator		TRef		*	()									noexcept	{ return Unknown;	}
		inlcxpr	operator		const TRef	*	()							const	noexcept	{ return Unknown;	}

		inlcxpr	bool			operator==		(const TNCOPtr& other)		const	noexcept	{ return Unknown == other.Unknown; }
		inlcxpr	bool			operator!=		(const TNCOPtr& other)		const	noexcept	{ return !operator==(other); }

		inlcxpr	TNCOPtr			operator =		(const TNCOPtr& other)				noexcept	{ TRef* oldInstance = Unknown; Unknown = other.Unknown; if(Unknown) Unknown->AddRef();	if(oldInstance) oldInstance->Release(); return *this; }
		inlcxpr	TNCOPtr			operator =		(TNCOPtr&& other)					noexcept	{ TRef* oldInstance = Unknown; Unknown = other.Unknown; other.Unknown = 0;					if(oldInstance) oldInstance->Release(); return *this; }
		inlcxpr	TNCOPtr			operator =		(TRef* other)						noexcept	{ TRef* oldInstance = Unknown; Unknown = other; if(Unknown) Unknown->AddRef();				if(oldInstance) oldInstance->Release(); return *this; }

		inline	TRef*			operator->		()									noexcept	{ return Unknown; }
		inline	const TRef*		operator->		()							const	noexcept	{ return Unknown; }

		inline	TRef**			operator &		()									noexcept	{ return &Unknown; }

		inline	uint32_t		clear			()									noexcept	{ return ::gpk::safe_release(&Unknown); }
		inlcxpr	const TRef*		get_ref			()							const	noexcept	{ return Unknown; }
		inlcxpr	const TRef*		set_ref			(TRef* ref)							noexcept	{ ::gpk::safe_addref(&Unknown, ref); return Unknown; }
		inlcxpr	TRef*			get				()							const	noexcept	{ return Unknown; }

		template<typename _tNCOOther>
		inline	::gpk::error_t	as				(_tNCOOther** other)				noexcept	{ 
			_tNCOOther*					old				= *other;
			*other					= {};
			HRESULT						hr				= Unknown ? Unknown->QueryInterface(__uuidof(_tNCOOther), (void**)other) : 0;
			if(old)
				old->Release();
			return hr;
		}

		template<typename _tNCOOther>
		inline	::gpk::error_t	as				(::gpk::ptr_com<_tNCOOther>& other)	noexcept	{ 
			::gpk::ptr_com<_tNCOOther>	old				= other;
			other					= {};
			HRESULT						hr				= Unknown ? Unknown->QueryInterface(__uuidof(_tNCOOther), (void**)&other) : 0;
			return hr;
		}
	};
	template <typename T> using pcom		= ::gpk::ptr_com<T>;
	template <typename T> using acom		= ::gpk::aobj<::gpk::pcom<T>>;
	template <typename T> using vcom		= ::gpk::view<::gpk::pcom<T>>;
	template <typename T> using array_com	= ::gpk::acom<T>;
	template <typename T> using view_com	= ::gpk::vcom<T>;
}

#endif // GPK_COMPLUS_H_23627

#endif // GPK_WINDOWS - ignore the whole thing if not a Windows build
