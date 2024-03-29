#include "gpk_ptr_nco.h"

#ifndef GPK_PTR_POD_H_23627
#define GPK_PTR_POD_H_23627

namespace gpk
{
	tplt<tpnm _tPOD>
	class ppod : public ::gpk::pnco<_tPOD> {
	public:
		typedef _tPOD			T;
		typedef	::gpk::pnco<T>	TNCOPtr;
		typedef	::gpk::gref<T>	TRef;

		using	TNCOPtr			::Reference;
								
		inline	const T*		operator->	()		const	noexcept	{ return Reference->Instance;														}
		inline	T*				operator->	()				noexcept	{ return (0 == Reference) ? ::gpk::ref_allocate(&Reference) : Reference->Instance;	}

		tplt<tpnm... _tArgsConstructor>
		inline	T*				create		(_tArgsConstructor &&... argsConstructor)	noexcept	{ return ::gpk::ref_create(&Reference, argsConstructor...);							}
		inline	T*				allocate	()											noexcept	{ return ::gpk::ref_allocate(&Reference);											}
	};
	tplt<tpnm T> using pp	= ::gpk::ppod<T>;
} // namespace

#endif // GPK_PTR_POD_H_23627
