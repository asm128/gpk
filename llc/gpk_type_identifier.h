/// Copyright 2017 - asm128
#include "gpk_view.h"

#ifndef GPK_TYPE_IDENTIFIER_H_23627
#define GPK_TYPE_IDENTIFIER_H_23627

namespace gpk
{
	struct STypeIdentifier {
		vcst_t			NameSpace		= {};
		vcst_t			Name			= {};

		inxp			STypeIdentifier	()								nxpt	= dflt;
		inxp			STypeIdentifier	(cnst STypeIdentifier &)		nxpt	= dflt;
		inxp			STypeIdentifier	(vcst_t nameSpace, vcst_t name)	nxpt	: NameSpace(nameSpace), Name(name)	{}
		inxp	b8_t	oper==			(cnst STypeIdentifier & right)	csnx	{ return NameSpace == right.NameSpace && Name == right.Name; }
	};	// struct
} // namespace


#endif // GPK_TYPE_IDENTIFIER_H_23627
