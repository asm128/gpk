/// Copyright 2017 - asm128
#include "gpk_label.h"

#ifndef GPK_TYPE_IDENTIFIER_H_23627
#define GPK_TYPE_IDENTIFIER_H_23627

namespace gpk
{
	struct STypeIdentifier {
		::gpk::vcsc_t			NameSpace				= {};
		::gpk::vcsc_t			Name					= {};

		inlcxpr					STypeIdentifier			()			= default;
		inlcxpr					STypeIdentifier			(::gpk::vcsc_c & nameSpace, ::gpk::vcsc_c & name)	: NameSpace(nameSpace), Name(name)	{}

		inline	bool			operator==				(const STypeIdentifier & right)		const	noexcept	{ return NameSpace == right.NameSpace && Name == right.Name; }
	};	// struct
} // namespace


#endif // GPK_TYPE_IDENTIFIER_H_23627
