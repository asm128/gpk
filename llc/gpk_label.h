#include "gpk_view_array.h"

#ifndef GPK_LABEL_H_2908742983
#define GPK_LABEL_H_2908742983

namespace gpk
{
	struct label_statics;

	class CLabelManager;
	class label : public view_const_char {
				CLabelManager				* LabelManager				= 0;
	public:
		static	const ::gpk::label_statics	& statics					();

											label						()																			noexcept	= default;
											label						(const label& other)														noexcept	= default;

				label&						operator=					(const label& other)														noexcept	= default;

											label						(const char* str, uint32_t len)												noexcept;

		template<size_t _stringLength>
		inline 								label						(const char (&str)[_stringLength], uint32_t count = (uint32_t)-1)			noexcept	: label(&str[0], ::gpk::min((uint32_t)_stringLength, count))	{}
		inline 								label						(const ::gpk::view_const_string& other)										noexcept	: label(other.begin(), other.size())							{}

		inline	operator					::gpk::view_const_string	()																	const				{ return {Data, Count}; }

				bool						operator==					(const ::gpk::view_const_string& other)								const	noexcept;
				bool						operator==					(const label& other)												const	noexcept;
		inline	bool						operator!=					(const label& other)												const	noexcept	{ return !operator==(other); }
	};

	// clang seems to be not conformant with calling template constructors, so we have to initialize them manually.
	struct label_statics {
				::gpk::label				empty						= {""	, 1};
				::gpk::label				space						= {" "	, 1};
				::gpk::label				slash						= {"/"	, 1};
				::gpk::label				backslash					= {"\\"	, 1};
				::gpk::label				question_mark				= {"?"	, 1};
				::gpk::label				exclamation_mark			= {"!"	, 1};
				::gpk::label				dot							= {"."	, 1};
				::gpk::label				comma						= {","	, 1};
	};

	inline	const ::gpk::label_statics &	label::statics				()																						{ static const ::gpk::label_statics init_statics; return init_statics; }
}

#endif // GPK_LABEL_H_2908742983
