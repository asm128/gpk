#include "gpk_array.h"

#ifndef GPK_DEFLATE_H_92173498234
#define GPK_DEFLATE_H_92173498234

namespace gpk 
{
	::gpk::error_t									arrayDeflate								(const ::gpk::view_array<const byte_t>	& inflated, ::gpk::array_pod<byte_t> & deflated);
	::gpk::error_t									arrayInflate								(const ::gpk::view_array<byte_t>		& deflated, ::gpk::array_pod<byte_t> & inflated);
} // namespace

#endif // GPK_DEFLATE_H_92173498234
