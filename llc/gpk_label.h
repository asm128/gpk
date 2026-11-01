#include "gpk_view.h"
#include "gpk_view_serialize.h"

#ifndef GPK_LABEL_H_23627
#define GPK_LABEL_H_23627

namespace gpk
{
	struct label_statics;

	class CLabelManager;
	class label : public vcst_t {
		CLabelManager		* LabelManager	= 0;
	public:
		usng	vcst_t		::oper==	;

		inxp				label		()										nxpt	= dflt;
							label		(cnst label & other)					nxpt	= dflt;
							label		(vcst_c & other)						;
		tplN2uinln 			label		(sc_c (&str)[N], u2_t count = (u2_t)-1)	: label(vcst_t(str, count))	{}

				label &		oper=		(cnst label & other)					= dflt;

				b8_t		oper==		(cnst label & other)					csnx;
		inln	b8_t		oper!=		(cnst label & other)					csnx	{ rtrn !oper==(other); }
	};

	stin err_t			loadLabel	(vcu0_t & input, vcst_t & output) { 
		u2_t					bytesRead		= 0;
		if_fail_fe(bytesRead = viewRead(output, input)); 
		if_fail_fe(input.slice(input, bytesRead));
		output					= label(output);
		rtrn bytesRead;
	}
}

#endif // GPK_LABEL_H_23627
