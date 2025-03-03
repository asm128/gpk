#include "gpk_n2.h"

#ifndef GPK_RECT_H_23627
#define GPK_RECT_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tDimension>
	struct rect {
		typedef	_tDimension		T;
		typedef	rect<T>			TRect;

		T						Left, Top, Right, Bottom;

		GPK_DEFAULT_OPERATOR(rect<T>, Left == other.Left && Top == other.Top && Right == other.Right && Bottom == other.Bottom); 

		cnstxpr	TRect			operator+	(const TRect & other)					const	noexcept	{ return {T(Left + other.Left), T(Top + other.Top), T(Right + other.Right), T(Bottom + other.Bottom)}; }

		inlcxpr	T				Width		()	const { return Right - Left; }
		inlcxpr	T				Height		()	const { return Bottom - Top; }
		cnstxpr ::gpk::n2<T>	Dimensions	()	const { return {Width(), Height()}; }

		tplt <tpnm _t>
		inlcxpr	rect<_t>		Cast		()	const	noexcept	{ return {(_t)Left, (_t)Top, (_t)Right, (_t)Bottom}; }

		inlcxpr	rect<uint8_t>	u0_t			()	const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	rect<uint16_t>	u1_t			()	const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	rect<uint32_t>	u2_t			()	const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	rect<uint64_t>	u3_t			()	const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	rect<int8_t>	s0_t			()	const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	rect<int16_t>	s1_t			()	const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	rect<int32_t>	s2_t			()	const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	rect<int64_t>	s3_t			()	const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	rect<float>		f2_t			()	const	noexcept	{ return Cast<float		>(); }
		inlcxpr	rect<double>	f3_t			()	const	noexcept	{ return Cast<double	>(); }
	};
	typedef rect<float>		rectf32;
	typedef rect<double>	rectf64;
	typedef rect<uint8_t>	rectu8;
	typedef rect<uint16_t>	rectu16;
	typedef rect<uint32_t>	rectu32;
	typedef rect<uint64_t>	rectu64;
	typedef rect<int8_t>	recti8;
	typedef rect<int16_t>	recti16;
	typedef rect<int32_t>	recti32;
	typedef rect<int64_t>	recti64;
#pragma pack(pop)
} // namespace

#endif // GPK_RECT_H_23627
