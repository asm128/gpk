#include "gpk_ptr_nco.h"

#ifndef GPK_PTR_POD_H_0297349827220523
#define GPK_PTR_POD_H_0297349827220523

namespace gpk
{
	template<typename _tPOD>
	class ptr_pod : public ::gpk::ptr_nco<_tPOD> {
	public:
		typedef	::gpk::ptr_nco<_tPOD>		TNCOPtr;
		typedef	::gpk::gpk_ref<_tPOD>		TRef;
		typedef	::gpk::ptr_pod<_tPOD>		TPODPtr;
		using	TNCOPtr						::Reference;

		inline	const _tPOD*				operator->				()										const	noexcept	{ return Reference->Instance;														}
		inline	_tPOD*						operator->				()												noexcept	{ return (0 == Reference) ? ::gpk::ref_allocate(&Reference) : Reference->Instance;	}

		template<typename... _tArgsConstructor>
		inline	_tPOD*						create					(_tArgsConstructor &&... argsConstructor)		noexcept	{ return ::gpk::ref_create(&Reference, argsConstructor...);							}
		inline	_tPOD*						allocate				()												noexcept	{ return ::gpk::ref_allocate(&Reference);											}
	};
	template<typename T> using ppod		= ::gpk::ptr_pod<T>;
	template<typename T> using pp		= ::gpk::ppod<T>;
} // namespace

#endif // GPK_PTR_POD_H_0297349827220523
