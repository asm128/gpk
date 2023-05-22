#include "gpk_ref.h"

#ifndef GPK_PTR_NCO_H_0297349827220523
#define GPK_PTR_NCO_H_0297349827220523

namespace gpk
{
	template<typename _tNCO>
	class ptr_nco {
	protected:
		::gpk::gpk_ref<_tNCO>						* Reference				= 0;
	public:
		typedef	::gpk::ptr_nco<_tNCO>				TNCOPtr;
		typedef	::gpk::gpk_ref<_tNCO>				TRef;

		inline										~ptr_nco				()												noexcept	{ ::gpk::ref_release(&Reference);													}
		inlcxpr										ptr_nco					()												noexcept	= default;
		inline										ptr_nco					(const TNCOPtr & other)							noexcept	{ Reference = ::gpk::ref_acquire(other.Reference);									}
		inlcxpr										ptr_nco					(TNCOPtr && other)								noexcept	{ Reference = other.Reference; other.Reference = 0;									}
		inline										ptr_nco					(TRef * other)									noexcept	{ Reference = other;																}

		inlcxpr	operator							_tNCO		*			()												noexcept	{ return Reference ? Reference->Instance : 0;										}
		inlcxpr	operator							const _tNCO	*			()										const	noexcept	{ return Reference ? Reference->Instance : 0;										}

		inlcxpr	bool								operator==				(const TNCOPtr & other)					const	noexcept	{ return Reference == other.Reference;												}
		inlcxpr	bool								operator!=				(const TNCOPtr & other)					const	noexcept	{ return !operator==(other);														}

		inlcxpr	TNCOPtr								operator =				(const TNCOPtr & other)							noexcept	{ TRef * oldInstance = Reference; Reference = ::gpk::ref_acquire(other.Reference);	::gpk::ref_release(&oldInstance); return *this; }
		inlcxpr	TNCOPtr								operator =				(TNCOPtr && other)								noexcept	{ TRef * oldInstance = Reference; Reference = other.Reference; other.Reference = 0;	::gpk::ref_release(&oldInstance); return *this; }
		inlcxpr	TNCOPtr								operator =				(TRef * other)									noexcept	{ TRef * oldInstance = Reference; Reference = other;									::gpk::ref_release(&oldInstance); return *this; }

		inline	_tNCO*								operator->				()												noexcept	{ return Reference->Instance; }
		inline	const _tNCO*						operator->				()										const	noexcept	{ return Reference->Instance; }

		inline	TRef **								operator &				()												noexcept	{ return &Reference;	}

		inlcxpr	const TRef*							get_ref					()										const	noexcept	{ return Reference;	}
		inlcxpr	const TRef*							set_ref					(TRef * ref)									noexcept	{ TRef * oldInstance = Reference; Reference = ref; ::gpk::ref_release(&oldInstance); return Reference;	}

		template<typename _tNCOOther>
		inline	_tNCO*								as						(_tNCOOther* * other)							noexcept	{ return *other = (Reference ? dynamic_cast<_tNCOOther*>(Reference->Instance) : 0);				}

		template<typename _tNCOOther>
		inline	_tNCO*								as						(::gpk::ptr_nco<_tNCOOther> & other)			noexcept	{ other = {(::gpk::gpk_ref<_tNCOOther>*)::gpk::ref_acquire(Reference)}; return 0;									}
	};
	template <typename T> using pnco		= ::gpk::ptr_nco<T>; 
	template <typename T> using pn			= ::gpk::pnco<T>; 
	template <typename T> using pi			= ::gpk::pnco<T>; 
} // namespace

#endif // GPK_PTR_NCO_H_0297349827220523
