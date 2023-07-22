#include "gpk_ptr_nco.h"

#ifndef GPK_PTR_OBJ_H_23627
#define GPK_PTR_OBJ_H_23627

namespace gpk
{
	tplt<tpnm _tOBJ>
	class pobj : public ::gpk::pnco<_tOBJ> {
	public:
		typedef _tOBJ			T;
		typedef	::gpk::pnco<T>	TNCOPtr;
		typedef	::gpk::pobj<T>	TOBJPtr;
		typedef	::gpk::gref<T>	TRef;

		using	TNCOPtr			::Reference;

		inline	const T*		operator->	()		const	noexcept	{ return Reference->Instance;														}
		inline	T*				operator->	()				noexcept	{ return (0 == Reference) ? ::gpk::ref_create(&Reference) : Reference->Instance;	}

		tplt<tpnm... _tArgsConstructor>
		inline	T*				create		(_tArgsConstructor &&... argsConstructor)	noexcept	{ return ::gpk::ref_create(&Reference, argsConstructor...);							}
		inline	T*				allocate	()											noexcept	{ return ::gpk::ref_allocate(&Reference);											}
	};
	tplt<tpnm T> using po	= ::gpk::pobj<T>; 
} // namespace

#endif // GPK_PTR_OBJ_H_23627
