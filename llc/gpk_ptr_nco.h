#include "gpk_ref.h"

#ifndef GPK_PTR_NCO_H_23627
#define GPK_PTR_NCO_H_23627

namespace gpk
{
	template<typename _tNCO>
	class pnco {
	protected:
		::gpk::gref<_tNCO>		* Reference	= 0;
	public:
		typedef	_tNCO			T;
		typedef	pnco<T>			TNCOPtr;
		typedef	::gpk::gref<T>	TRef;

		inline					~pnco		()									noexcept	{ clear(); }
		inlcxpr					pnco		()									noexcept	= default;
		inline					pnco		(const TNCOPtr & other)				noexcept	{ Reference = ::gpk::ref_acquire(other.Reference);	}
		inlcxpr					pnco		(TNCOPtr && other)					noexcept	{ Reference = other.Reference; other.Reference = 0;	}
		inline					pnco		(TRef * other)						noexcept	{ Reference = other; }

		inlcxpr	operator		T*			()									noexcept	{ return Reference ? Reference->Instance : 0; }
		inlcxpr	operator		const T*	()							const	noexcept	{ return Reference ? Reference->Instance : 0; }

		inlcxpr	bool			operator==	(const TNCOPtr & other)		const	noexcept	{ return Reference == other.Reference; }
		inlcxpr	bool			operator!=	(const TNCOPtr & other)		const	noexcept	{ return !operator==(other); }

		inlcxpr	TNCOPtr			operator =	(const TNCOPtr & other)				noexcept	{ TRef * oldInstance = Reference; Reference = ::gpk::ref_acquire(other.Reference);	::gpk::ref_release(&oldInstance); return *this; }
		inlcxpr	TNCOPtr			operator =	(TNCOPtr && other)					noexcept	{ TRef * oldInstance = Reference; Reference = other.Reference; other.Reference = 0;	::gpk::ref_release(&oldInstance); return *this; }
		inlcxpr	TNCOPtr			operator =	(TRef * other)						noexcept	{ TRef * oldInstance = Reference; Reference = other; ::gpk::ref_release(&oldInstance); return *this; }

		inline	T*				operator->	()									noexcept	{ return Reference->Instance; }
		inline	const T*		operator->	()							const	noexcept	{ return Reference->Instance; }

		inline	TRef **			operator &	()									noexcept	{ return &Reference; }

		inlcxpr	const TRef*		get_ref		()							const	noexcept	{ return Reference; }
		inlcxpr	const TRef*		set_ref		(TRef * ref)						noexcept	{ TRef * oldInstance = Reference; Reference = ref; ::gpk::ref_release(&oldInstance); return Reference; }
		inline	::gpk::error_t	clear		()									noexcept	{ return ::gpk::ref_release(&Reference); }

		template<typename _tNCOOther>
		inline	_tNCO*			as			(_tNCOOther* * other)				noexcept	{ return *other = (Reference ? dynamic_cast<_tNCOOther*>(Reference->Instance) : 0); }

		template<typename _tNCOOther>
		inline	_tNCO*			as			(::gpk::pnco<_tNCOOther> & other)	noexcept	{ other = {(::gpk::gref<_tNCOOther>*)::gpk::ref_acquire(Reference)}; return 0; }
	};
	template <typename T> using pn		= ::gpk::pnco<T>; 
	template <typename T> using pi		= ::gpk::pnco<T>; 
} // namespace

#endif // GPK_PTR_NCO_H_23627
