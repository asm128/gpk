/// Copyright 2017 - asm128
#include "gpk_label.h"

#ifndef GPK_TYPE_IDENTIFIER_H_23627
#define GPK_TYPE_IDENTIFIER_H_23627

namespace gpk
{
	struct STypeIdentifier {
		vcst_t					NameSpace				= {};
		vcst_t					Name					= {};

		inlcxpr					STypeIdentifier			()			= default;
		inlcxpr					STypeIdentifier			(vcst_c & nameSpace, vcst_c & name)	: NameSpace(nameSpace), Name(name)	{}

		inline	bool			operator==				(const STypeIdentifier & right)		const	noexcept	{ return NameSpace == right.NameSpace && Name == right.Name; }
	};	// struct
} // namespace


#endif // GPK_TYPE_IDENTIFIER_H_23627


