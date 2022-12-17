#include "gpk_windows.h"

#include <Unknwn.h>

#ifndef GPK_COMPLUS_H_982734
#define GPK_COMPLUS_H_982734

namespace gpk
{
	template<typename _tNCO>
	class ptr_com {
	protected:
							_tNCO								* Reference							= 0;
	public:
		typedef				::gpk::ptr_com<_tNCO>				TNCOPtr;
		typedef				_tNCO								TRef;

		inline													~ptr_com							()															noexcept	{ if(Reference) Reference->Release();								}
		inline constexpr										ptr_com								()															noexcept	= default;
		inline													ptr_com								(const TNCOPtr& other)										noexcept	{ Reference = other.Reference; if(Reference) Reference->AddRef();	}
		inline constexpr										ptr_com								(TNCOPtr&& other)											noexcept	{ Reference = other.Reference; other.Reference = 0;					}
		inline													ptr_com								(TRef* other)												noexcept	{ Reference = other;												}

		inline constexpr	operator							_tNCO		*						()															noexcept	{ return Reference;	}
		inline constexpr	operator							const _tNCO	*						()													const	noexcept	{ return Reference;	}

		inline constexpr	bool								operator==							(const TNCOPtr& other)								const	noexcept	{ return Reference == other.Reference;												}
		inline constexpr	bool								operator!=							(const TNCOPtr& other)								const	noexcept	{ return !operator==(other);														}

		inline constexpr	TNCOPtr								operator =							(const TNCOPtr& other)										noexcept	{ TRef* oldInstance = Reference; Reference = other.Reference; if(Reference) Reference->AddRef();	if(oldInstance) oldInstance->Release(); return *this; }
		inline constexpr	TNCOPtr								operator =							(TNCOPtr&& other)											noexcept	{ TRef* oldInstance = Reference; Reference = other.Reference; other.Reference = 0;					if(oldInstance) oldInstance->Release(); return *this; }
		inline constexpr	TNCOPtr								operator =							(TRef* other)												noexcept	{ TRef* oldInstance = Reference; Reference = other;													if(oldInstance) oldInstance->Release(); return *this; }

		inline				_tNCO*								operator->							()															noexcept	{ return Reference; }
		inline				const _tNCO*						operator->							()													const	noexcept	{ return Reference; }

		inline				TRef**								operator &							()															noexcept	{ return &Reference;	}

		inline constexpr	const TRef*							get_ref								()													const	noexcept	{ return Reference;	}
		inline constexpr	const TRef*							set_ref								(TRef* ref)													noexcept	{ TRef* oldInstance = Reference; Reference = ref; if(oldInstance) oldInstance->Release(); return Reference;	}
		inline constexpr	TRef*								get									()													const	noexcept	{ return Reference;	}

		template<typename _tNCOOther>
		inline				_tNCOOther*							as									(_tNCOOther** other)										noexcept	{ 
			_tNCOOther*													old									= *other;
			*other													= {};
			if(Reference)
				Reference->QueryInterface(__uuidof(_tNCOOther), (void**)other);
			if(old)
				old->Release();
			return *other;
		}

		template<typename _tNCOOther>
		inline				_tNCOOther*							as									(::gpk::ptr_com<_tNCOOther>& other)							noexcept	{ 
			::gpk::ptr_com<_tNCOOther>									old									= other;
			other													= {};
			if(Reference)
				Reference->QueryInterface(__uuidof(_tNCOOther), (void**)&other);
			return other.get();
		}
	};
}

#endif // GPK_COMPLUS_H_982734