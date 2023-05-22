#include "gpk_ptr_nco.h"

#ifndef GPK_PTR_OBJ_H_0297349827220523
#define GPK_PTR_OBJ_H_0297349827220523

namespace gpk
{
	template<typename _tOBJ>
	class ptr_obj : public ::gpk::ptr_nco<_tOBJ> {
	public:
		typedef _tOBJ				T;
		typedef	::gpk::ptr_nco<T>	TNCOPtr;
		typedef	::gpk::ptr_obj<T>	TOBJPtr;
		typedef	::gpk::gpk_ref<T>	TRef;
		using	TNCOPtr				::Reference;

		inline	const T*			operator->				()										const	noexcept	{ return Reference->Instance;														}
		inline	T*					operator->				()												noexcept	{ return (0 == Reference) ? ::gpk::ref_create(&Reference) : Reference->Instance;	}

		template<typename... _tArgsConstructor>
		inline	T*					create					(_tArgsConstructor &&... argsConstructor)		noexcept	{ return ::gpk::ref_create(&Reference, argsConstructor...);							}
		inline	T*					allocate				()												noexcept	{ return ::gpk::ref_allocate(&Reference);											}
	};

	template<typename T> using pobj	= ::gpk::ptr_obj<T>; 
	template<typename T> using po	= ::gpk::pobj<T>; 
} // namespace

#endif // GPK_PTR_OBJ_H_0297349827220523
