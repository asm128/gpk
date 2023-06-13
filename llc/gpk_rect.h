#include "gpk_n2.h"

#ifndef GPK_RECT_H_928374982364923524
#define GPK_RECT_H_928374982364923524

namespace gpk
{
#pragma pack(push, 1)
	template<typename _tDimension>
	struct rect {
		typedef	_tDimension		T;
		T						Left, Top, Right, Bottom;

		GPK_DEFAULT_OPERATOR(rect<T>, Left == other.Left && Top == other.Top && Right == other.Right && Bottom == other.Bottom); 

		inlcxpr	T				Width		()	const { return Right - Left; }
		inlcxpr	T				Height		()	const { return Bottom - Top; }
		cnstxpr ::gpk::n2<T>	Dimensions	()	const { return {Width(), Height()}; }

		template <typename _t>
		inlcxpr	rect<_t>		Cast		()	const	noexcept	{ return {(_t)Left, (_t)Top, (_t)Right, (_t)Bottom}; }

		inlcxpr	rect<uint8_t>	u8			()	const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	rect<uint16_t>	u16			()	const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	rect<uint32_t>	u32			()	const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	rect<uint64_t>	u64			()	const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	rect<int8_t>	i8			()	const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	rect<int16_t>	i16			()	const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	rect<int32_t>	i32			()	const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	rect<int64_t>	i64			()	const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	rect<float>		f32			()	const	noexcept	{ return Cast<float		>(); }
		inlcxpr	rect<double>	f64			()	const	noexcept	{ return Cast<double	>(); }
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

#endif // GPK_RECT_H_928374982364923524
