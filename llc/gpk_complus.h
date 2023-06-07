#include "gpk_array.h"

#ifdef GPK_WINDOWS

#include "gpk_windows.h"

#include <Unknwn.h>

#ifndef GPK_COMPLUS_H_982734
#define GPK_COMPLUS_H_982734

namespace gpk
{
	template<typename _tNCO>
	class ptr_com {
	protected:
		_tNCO				* Reference			= 0;
	public:
		typedef	::gpk::ptr_com<_tNCO>	TNCOPtr;
		typedef	_tNCO					TRef;

		inline					~ptr_com		()									noexcept	{ if(Reference) Reference->Release();								}
		inlcxpr					ptr_com			()									noexcept	= default;
		inline					ptr_com			(const TNCOPtr& other)				noexcept	{ Reference = other.Reference; if(Reference) Reference->AddRef();	}
		inlcxpr					ptr_com			(TNCOPtr&& other)					noexcept	{ Reference = other.Reference; other.Reference = 0;					}
		inline					ptr_com			(TRef* other)						noexcept	{ Reference = other;												}

		inlcxpr	operator		TRef		*	()									noexcept	{ return Reference;	}
		inlcxpr	operator		const TRef	*	()							const	noexcept	{ return Reference;	}

		inlcxpr	bool			operator==		(const TNCOPtr& other)		const	noexcept	{ return Reference == other.Reference;												}
		inlcxpr	bool			operator!=		(const TNCOPtr& other)		const	noexcept	{ return !operator==(other);														}

		inlcxpr	TNCOPtr			operator =		(const TNCOPtr& other)				noexcept	{ TRef* oldInstance = Reference; Reference = other.Reference; if(Reference) Reference->AddRef();	if(oldInstance) oldInstance->Release(); return *this; }
		inlcxpr	TNCOPtr			operator =		(TNCOPtr&& other)					noexcept	{ TRef* oldInstance = Reference; Reference = other.Reference; other.Reference = 0;					if(oldInstance) oldInstance->Release(); return *this; }
		inlcxpr	TNCOPtr			operator =		(TRef* other)						noexcept	{ TRef* oldInstance = Reference; Reference = other; if(Reference) Reference->AddRef();				if(oldInstance) oldInstance->Release(); return *this; }

		inline	TRef*			operator->		()									noexcept	{ return Reference; }
		inline	const TRef*		operator->		()							const	noexcept	{ return Reference; }

		inline	TRef**			operator &		()									noexcept	{ return &Reference;	}

		inlcxpr	const TRef*		get_ref			()							const	noexcept	{ return Reference;	}
		inlcxpr	const TRef*		set_ref			(TRef* ref)							noexcept	{ TRef* oldInstance = Reference; Reference = ref; if(oldInstance) oldInstance->Release(); return Reference;	}
		inlcxpr	TRef*			get				()							const	noexcept	{ return Reference;	}

		template<typename _tNCOOther>
		inline	::gpk::error_t	as				(_tNCOOther** other)				noexcept	{ 
			_tNCOOther*					old				= *other;
			*other					= {};
			HRESULT						hr				= Reference ? Reference->QueryInterface(__uuidof(_tNCOOther), (void**)other) : 0;
			if(old)
				old->Release();
			return hr;
		}

		template<typename _tNCOOther>
		inline	::gpk::error_t	as				(::gpk::ptr_com<_tNCOOther>& other)	noexcept	{ 
			::gpk::ptr_com<_tNCOOther>	old				= other;
			other					= {};
			HRESULT						hr				= Reference ? Reference->QueryInterface(__uuidof(_tNCOOther), (void**)&other) : 0;
			return hr;
		}
	};
	template <typename T> using pcom		= ::gpk::ptr_com<T>;
	template <typename T> using acom		= ::gpk::aobj<::gpk::pcom<T>>;
	template <typename T> using vcom		= ::gpk::view<::gpk::pcom<T>>;
	template <typename T> using array_com	= ::gpk::acom<T>;
	template <typename T> using view_com	= ::gpk::vcom<T>;
}

#endif // GPK_WINDOWS

#endif // GPK_COMPLUS_H_982734