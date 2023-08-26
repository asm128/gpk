#include "gpk_view.h"
#include "gpk_view_serialize.h"

#ifndef GPK_LABEL_H_23627
#define GPK_LABEL_H_23627

namespace gpk
{
	struct label_statics;

	class CLabelManager;
	class label : public vcc {
		CLabelManager		* LabelManager	= 0;
	public:
		inlcxpr				label			()								noexcept	= default;
							label			(const label & other)			noexcept	= default;
							label			(const char* str, uint32_t len)	noexcept;

		label&				operator=		(const label & other)			noexcept	= default;

		tplt<size_t _stringLength>
		inline 				label			(const char (&str)[_stringLength], uint32_t count = (uint32_t)-1)	noexcept	: label(&str[0], ::gpk::min((uint32_t)_stringLength, count))	{}
		inline 				label			(const ::gpk::vcs & other)											noexcept	: label(other.begin(), other.size())							{}

		inline	operator	::gpk::vcs		()								const				{ return {Data, Count}; }

				bool		operator==		(const ::gpk::vcs & other)		const	noexcept;
				bool		operator==		(const label & other)			const	noexcept;
		inline	bool		operator!=		(const label & other)			const	noexcept	{ return !operator==(other); }
	};

	stainli ::gpk::error_t	loadLabel		(::gpk::vcu8 & input, ::gpk::vcc & output) { 
		uint32_t					bytesRead		= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(output, input)); 
		gpk_necs(input.slice(input, bytesRead));
		output					= ::gpk::label(output);
		return bytesRead;
	}
}

#endif // GPK_LABEL_H_23627
