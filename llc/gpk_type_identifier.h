/// Copyright 2017 - asm128
#include "gpk_label.h"

#ifndef GPK_TYPE_IDENTIFIER_H_02397492873498
#define GPK_TYPE_IDENTIFIER_H_02397492873498

namespace gpk
{
	struct STypeIdentifier {
		::gpk::label			NameSpace				= ::gpk::label::statics().empty;
		::gpk::label			Name					= ::gpk::label::statics().empty;

								STypeIdentifier			()			= default;
								STypeIdentifier			(const ::gpk::label& nameSpace, const ::gpk::label& name)						: NameSpace(nameSpace), Name(name)	{}

		const bool				operator==				(const STypeIdentifier& right)								const	noexcept	{ return NameSpace == right.NameSpace && Name == right.Name;	}
	};	// struct
} // namespace


#endif // GPK_TYPE_IDENTIFIER_H_02397492873498
