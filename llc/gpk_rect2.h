#include "gpk_n2.h"

#ifndef GPK_RECT2_H
#define GPK_RECT2_H

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T>
	struct rect2 {
		::gpk::n2<T>			Offset, Size;

		GPK_DEFAULT_OPERATOR(rect2<T>, Offset	== other.Offset	&& Size == other.Size);

		tplt <tpnm _t>
		inlcxpr	rect2<_t>		Cast		()	const	noexcept	{ return {Offset.tplt Cast<_t>(), Size.tplt Cast<_t>()}; }

		inlcxpr	rect2<uint8_t>	u0_t			()	const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	rect2<uint16_t>	u1_t			()	const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	rect2<uint32_t>	u2_t			()	const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	rect2<uint64_t>	u3_t			()	const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	rect2<int8_t>	s0_t			()	const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	rect2<int16_t>	s1_t			()	const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	rect2<int32_t>	s2_t			()	const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	rect2<int64_t>	s3_t			()	const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	rect2<float>	f2_t			()	const	noexcept	{ return Cast<float		>(); }
		inlcxpr	rect2<double>	f3_t			()	const	noexcept	{ return Cast<double	>(); }

		inline	::gpk::n2<T>	Limit		()	const	noexcept	{ return Offset + Size; }
	};
	typedef rect2<float>	rect2f32, rect2f;
	typedef rect2<double>	rect2f64, rect2d;
	typedef rect2<uint8_t>	rect2u8;
	typedef rect2<uint16_t>	rect2u16;
	typedef rect2<uint32_t>	rect2u32;
	typedef rect2<uint64_t>	rect2u64;
	typedef rect2<int8_t>	rect2i8;
	typedef rect2<int16_t>	rect2i16;
	typedef rect2<int32_t>	rect2i32;
	typedef rect2<int64_t>	rect2i64;
#pragma pack(pop)

	tplt<tpnm T>
	stacxpr	bool	in_range	(const ::gpk::n2<T>& pointToTest, const ::gpk::rect2<T>& area)	noexcept	{
		return	::gpk::in_range(pointToTest.x, area.Offset.x, (T)(area.Offset.x + area.Size.x))
			&&	::gpk::in_range(pointToTest.y, area.Offset.y, (T)(area.Offset.y + area.Size.y))
			;
	}
} // namespace

#endif // GPK_RECT2_H
