#include "gpk_view.h"
#include "gpk_view_serialize.h"

#ifndef GPK_LABEL_H_23627
#define GPK_LABEL_H_23627

namespace gpk
{
	struct label_statics;

	class CLabelManager;
	class label : public vcsc_t {
		CLabelManager		* LabelManager	= 0;
	public:
		inxp				label			()								noexcept	= default;
							label			(const label & other)			noexcept	= default;
							label			(const char* str, uint32_t len)	noexcept;

		label&				oper=		(const label & other)			noexcept	= default;

		tplt<size_t _stringLength>
		inline 				label			(const char (&str)[_stringLength], uint32_t count = (uint32_t)-1)	noexcept	: label(&str[0], ::gpk::min((uint32_t)_stringLength, count))	{}
		inline 				label			(const ::gpk::vcst_t & other)											noexcept	: label(other.begin(), other.size())							{}

		inline	oper	::gpk::vcst_t		()								const				{ return {Data, Count}; }

				bool		oper==		(const ::gpk::vcst_t & other)		const	noexcept;
				bool		oper==		(const label & other)			const	noexcept;
		inline	bool		oper!=		(const label & other)			const	noexcept	{ return !oper==(other); }
	};

	stin ::gpk::error_t	loadLabel		(::gpk::vcu0_t & input, ::gpk::vcsc_t & output) { 
		uint32_t					bytesRead		= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(output, input)); 
		gpk_necs(input.slice(input, bytesRead));
		output					= ::gpk::label(output);
		return bytesRead;
	}
}

#endif // GPK_LABEL_H_23627
