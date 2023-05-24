#include "gpk_n2.h"

#ifndef GPK_RECT2_H_928374982364923524
#define GPK_RECT2_H_928374982364923524

namespace gpk
{
#pragma pack(push, 1)
	template<typename T>	
	struct rect2 {
		::gpk::n2<T>			Offset, Size;
		GPK_DEFAULT_OPERATOR_NE(rect2<T>, Offset	== other.Offset	&& Size == other.Size);

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

	template<typename T>
	stacxpr	bool	in_range	(const ::gpk::n2<T>& pointToTest, const ::gpk::rect2<T>& area)	noexcept	{
		return	::gpk::in_range(pointToTest.x, area.Offset.x, (T)(area.Offset.x + area.Size.x))
			&&	::gpk::in_range(pointToTest.y, area.Offset.y, (T)(area.Offset.y + area.Size.y))
			;
	}
} // namespace

#endif // GPK_RECT2_H_928374982364923524
