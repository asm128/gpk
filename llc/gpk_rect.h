#include "gpk_n2.h"

#ifndef GPK_RECT_H_928374982364923524
#define GPK_RECT_H_928374982364923524

namespace gpk
{
#pragma pack(push, 1)
	template<typename T>
	struct SRectLimits {
		T						Left, Top, Right, Bottom; 
		GPK_DEFAULT_OPERATOR_NE(SRectLimits<T>, Left == other.Left && Top == other.Top && Right == other.Right && Bottom == other.Bottom); 

		inlcxpr	T				Width		()	const { return Right - Left; }
		inlcxpr	T				Height		()	const { return Bottom - Top; }
		cnstxpr ::gpk::n2<T>	Dimensions	()	const { return {Width(), Height()}; }

		template <typename _t>
		inlcxpr	SRectLimits<_t>	Cast		()	const	noexcept	{ return {(_t)Left, (_t)Top, (_t)Right, (_t)Bottom}; }
	};
#pragma pack(pop)
} // namespace

#endif // GPK_RECT_H_928374982364923524
