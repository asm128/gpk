#include "gpk_line.h"
#include "gpk_n2.h"

#ifndef GPK_LINE2_H_9283749823649230523
#define GPK_LINE2_H_9283749823649230523

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename _tDimension>	struct line2 : public line<::gpk::n2<_tDimension>>		{
		typedef	_tDimension		T;	
		typedef	::gpk::n2<T>	TVertex;	
		using line<TVertex>::	A;
		using line<TVertex>::	B;
		using line<TVertex>::	line;

		template<typename _tOther>
		line2<_tOther>			Cast		()		const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				};
		}
	};
	typedef line2<float>	line2f32, line2f;
	typedef line2<double>	line2f64, line2d;
	typedef line2<uint8_t>	line2u8;
	typedef line2<uint16_t>	line2u16;
	typedef line2<uint32_t>	line2u32;
	typedef line2<uint64_t>	line2u64;
	typedef line2<int8_t>	line2i8;
	typedef line2<int16_t>	line2i16;
	typedef line2<int32_t>	line2i32;
	typedef line2<int64_t>	line2i64;

	template<typename T> stincxp	T		rise		(const ::gpk::line2<T> & line)									noexcept	{ return line.B.y - line.A.y;		}
	template<typename T> stincxp	T		run			(const ::gpk::line2<T> & line)									noexcept	{ return line.B.x - line.A.x;		}
	template<typename T> stincxp	T		slope		(const ::gpk::line2<T> & line)												{ return rise(line) / run(line);	}
	template<typename T> stacxpr	T		orient2d	(const ::gpk::line2<T> & segment, const ::gpk::n2<T> & point)	noexcept	{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
	template<typename T> stainli	double	determinant	(const ::gpk::line2<T>& line)									noexcept	{ return ::gpk::determinant((double)line.A.x, (double)line.A.y, (double)line.B.x, (double)line.B.y); }
#pragma pack(pop)
	// ---- Collision
	//template<typename T> static	bool	raySegmentIntersect	(n2<_tElement> r_d, n2<_tElement> r_p, n2<_tElement> s_d, n2<_tElement> s_p)								{
	//	const double	t2		= (r_d.x * (s_p.y - r_p.y) + r_d.y * (r_p.x - s_p.x)) / (s_d.x * r_d.y - s_d.y * r_d.x);
	//	const double	t1		= (s_p.x + s_d.x * t2 - r_p.x) / r_d.x;	// Plug the value of T2 to get T1
	//	return (t2 > 0 && 0 < t2 && t2 < 1);
	//}
} // namespace

#endif // GPK_LINE2_H_9283749823649230523
