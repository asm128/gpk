#include "gpk_parse_core.h"
#include "gpk_range.h"

#ifndef GPK_CTTI_PARSE_H
#define GPK_CTTI_PARSE_H

namespace gpk
{
	inxp b8_t cttiNameStart	(sc_c character) nxpt { rtrn '_' == character || (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'); }
	inxp b8_t cttiNamePart	(sc_c character) nxpt { rtrn cttiNameStart(character) || (character >= '0' && character <= '9'); }
	tplt<tpnm T>
	inxp auto cttiFirstMember(cnst T & source) nxpt {
		cnst auto & [result] = source;
		rtrn result;
	}

	inxp err_t cttiIdentifierError(vcst_t text, cnst rangeu2_t span) nxpt {
		if(span.empty() || false == cttiNameStart(text[span.Offset]))
			rtrn span.Offset;
		for(u2_t iChar = 1; iChar < span.Count; ++iChar)
			if(false == cttiNamePart(text[span.Offset + iChar]))
				rtrn span.Offset + iChar;
		rtrn -1;
	}
}

#endif // GPK_CTTI_PARSE_H
