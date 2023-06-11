/// Copyright 2017 - asm128
#include "gpk_label.h"

#ifndef GPK_TYPE_IDENTIFIER_H_02397492873498
#define GPK_TYPE_IDENTIFIER_H_02397492873498

namespace gpk
{
	struct STypeIdentifier {
		::gpk::vcc				NameSpace				= {};
		::gpk::vcc				Name					= {};

		inlcxpr					STypeIdentifier			()			= default;
		inlcxpr					STypeIdentifier			(const ::gpk::vcc & nameSpace, const ::gpk::vcc & name)	: NameSpace(nameSpace), Name(name)	{}

		inline	bool			operator==				(const STypeIdentifier & right)		const	noexcept	{ return NameSpace == right.NameSpace && Name == right.Name;	}
	};	// struct
} // namespace


#endif // GPK_TYPE_IDENTIFIER_H_02397492873498
