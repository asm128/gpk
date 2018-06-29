#include "gpk_view_array.h"

#ifndef GPK_LABEL_H_2908742983
#define GPK_LABEL_H_2908742983

namespace gpk
{
	class CLabelManager;
	class label : public view_array<const char> {
				CLabelManager*					LabelManager;
	public:
												label						()																			noexcept	= default;
												label						(const label& other)														noexcept	= default;

				label&							operator=					(const label& other)														noexcept	= default;

												label						(const char* str, uint32_t len)												noexcept;

		template<size_t _stringLength>
		inline 									label						(const char (&str)[_stringLength], uint32_t count = (uint32_t)-1)			noexcept	: label(str, ::gpk::min((uint32_t)_stringLength, count))	{}

		inline	operator						::gpk::view_const_string	()																	const	{ return {Data, Count}; }

				bool							operator==					(const label& other)												const	noexcept;
		inline	bool							operator!=					(const label& other)												const	noexcept	{ return !operator==(other); }
	};
}

#endif // GPK_LABEL_H_2908742983
