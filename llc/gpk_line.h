#include "gpk_n3.h"

#ifndef GPK_LINE_H_9283749823649230523
#define GPK_LINE_H_9283749823649230523

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename _tVertex>		
	struct SLine { 
		_tVertex					A, B; 

		cnstxpr						SLine		()											= default;
		cnstxpr						SLine		(const SLine<_tVertex> & other)				= default;
		cnstxpr						SLine		(const _tVertex & a, const _tVertex & b)	: A(a), B(b)	{}

		GPK_DEFAULT_OPERATOR_NE(SLine<_tVertex>, A == other.A && B == other.B);
	};

	template<typename _tDimension>	struct SLine2 : public SLine<::gpk::n2<_tDimension>>		{
		typedef	::gpk::n2<_tDimension>	TVertex;	
		using SLine<TVertex>::		A;
		using SLine<TVertex>::		B;
		using SLine<TVertex>::		SLine;

		template<typename _tOther>
		SLine2<_tOther>				Cast							()		const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				};
		}
	};
	template<typename T> stincxp	T	rise			(const ::gpk::SLine2<T> & line)									noexcept	{ return line.B.y - line.A.y;		}
	template<typename T> stincxp	T	run				(const ::gpk::SLine2<T> & line)									noexcept	{ return line.B.x - line.A.x;		}
	template<typename T> stincxp	T	slope			(const ::gpk::SLine2<T> & line)												{ return rise(line) / run(line);	}
	template<typename T> stacxpr	T	orient2d		(const ::gpk::SLine2<T> & segment, const ::gpk::n2<T> & point)	noexcept	{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }

	template<typename _tDimension>	struct SLine3 : public SLine<::gpk::n3<_tDimension>>		{
		typedef	::gpk::n3<_tDimension>	TVertex;	
		using SLine<TVertex>::		A;
		using SLine<TVertex>::		B;
		using SLine<TVertex>::		SLine;

		template<typename _tOther>
		SLine3<_tOther>				Cast							()		const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				};
		}
	};
	template<typename T> stacxpr	T	orient2d3d		(const ::gpk::SLine3<T> & segment, const ::gpk::n2<T> & point)	noexcept	{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
#pragma pack(pop)

	// ---- Line

	// ---- Collision
	//template<typename T>	static	bool	raySegmentIntersect	(n2<_tElement> r_d, n2<_tElement> r_p, n2<_tElement> s_d, n2<_tElement> s_p)								{
	//	const double							t2						= (r_d.x * (s_p.y - r_p.y) + r_d.y * (r_p.x - s_p.x)) / (s_d.x * r_d.y - s_d.y * r_d.x);
	//	const double							t1						= (s_p.x + s_d.x * t2 - r_p.x) / r_d.x;	// Plug the value of T2 to get T1
	//	return (t2 > 0 && 0 < t2 && t2 < 1);
	//}
	template<typename T> stainli	double	determinant	(const ::gpk::SLine2<T>& line)	noexcept	{ return ::gpk::determinant((double)line.A.x, (double)line.A.y, (double)line.B.x, (double)line.B.y); }
}

#endif // GPK_LINE_H_9283749823649230523
