#include "gpk_view_serialize.h"

#ifndef GPK_LABEL_H_23627
#define GPK_LABEL_H_23627

namespace gpk
{
	struct label_statics;

	class CLabelManager;
	class label : public vcst_t {
		CLabelManager	* LabelManager	= 0;
	public:
		tplt<size_t _len>
		inln 			label			(sc_c (&str)[_len], u2_t count = (u2_t)-1)	nxpt	: label(&str[0], ::gpk::min((u2_t)_len, count))	{}
		inln 			label			(vcst_c other)								nxpt	: label(other.begin(), other.size())			{}
		inxp			label			()											nxpt	= default;
						label			(cnst label & other)						nxpt	= default;
						label			(sc_c * str, u2_t len)						nxpt;

				label&	oper=			(cnst label  & other)						nxpt	= default;
				bool	oper==			(vcst_t other)				csnx;
				bool	oper==			(cnst label  & other)		csnx;
		inln	bool	oper!=			(cnst label  & other)		csnx	{ return !oper==(other); }

		inln	oper	vcst_t			()							cnst	{ return {Data, Count}; }
	};

	stin err_t		loadLabel		(vcu0_t & input, vcst_t & output) {
		vcs0_t				serialized		= {};
		u2_t				bytesRead		= 0;
		gpk_necs(bytesRead = viewRead(serialized, input));
		gpk_necs(input.slice(input, bytesRead));
		output					= label(vcst_t{serialized.begin(), serialized.size()});
		return bytesRead;
	}
}

#endif // GPK_LABEL_H_23627


