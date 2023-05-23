#include "gpk_math.h"

#ifndef GPK_N2_H_230515
#define GPK_N2_H_230515

namespace gpk 
{
#pragma pack(push, 1)
	template<typename _tBase>
	struct n2 {
		typedef	_tBase		T;
		T					x, y;

		typedef	n2<T>		Tn2;

		cnstxpr	Tn2			operator+			(const Tn2 & other)					const	noexcept	{ return {(T)(x + other.x), (T)(y + other.y)}; }
		cnstxpr	Tn2			operator-			(const Tn2 & other)					const	noexcept	{ return {(T)(x - other.x), (T)(y - other.y)}; }
		cnstxpr	Tn2			operator*			(double		scalar)					const	noexcept	{ return {(T)(x * scalar), (T)(y * scalar)}; }
		cnstxpr	Tn2			operator/			(double		scalar)					const				{ return {(T)(x / scalar), (T)(y / scalar)}; }
		cnstxpr	Tn2			operator*			(int64_t	scalar)					const	noexcept	{ return {(T)(x * scalar), (T)(y * scalar)}; }
		cnstxpr	Tn2			operator/			(int64_t	scalar)					const				{ return {(T)(x / scalar), (T)(y / scalar)}; }
		cnstxpr	Tn2			operator*			(uint64_t	scalar)					const	noexcept	{ return {(T)(x * scalar), (T)(y * scalar)}; }
		cnstxpr	Tn2			operator/			(uint64_t	scalar)					const				{ return {(T)(x / scalar), (T)(y / scalar)}; }
		cnstxpr	Tn2			operator*			(int32_t	scalar)					const	noexcept	{ return {(T)(x * scalar), (T)(y * scalar)}; }
		cnstxpr	Tn2			operator/			(int32_t	scalar)					const				{ return {(T)(x / scalar), (T)(y / scalar)}; }
		cnstxpr	Tn2			operator*			(uint32_t	scalar)					const	noexcept	{ return {(T)(x * scalar), (T)(y * scalar)}; }
		cnstxpr	Tn2			operator/			(uint32_t	scalar)					const				{ return {(T)(x / scalar), (T)(y / scalar)}; }
		//
		Tn2&				operator+=			(const Tn2 & other)							noexcept	{ x += other.x; y += other.y; return *this; }
		Tn2&				operator-=			(const Tn2 & other)							noexcept	{ x -= other.x; y -= other.y; return *this; }
		Tn2&				operator*=			(double		scalar)							noexcept	{ x = (T)(x * scalar); y = (T)(y * scalar);	return *this; }
		Tn2&				operator/=			(double		scalar)										{ x = (T)(x / scalar); y = (T)(y / scalar);	return *this; }
		Tn2&				operator*=			(int64_t	scalar)							noexcept	{ x = (T)(x * scalar); y = (T)(y * scalar);	return *this; }
		Tn2&				operator/=			(int64_t	scalar)										{ x = (T)(x / scalar); y = (T)(y / scalar);	return *this; }
		Tn2&				operator*=			(int32_t	scalar)							noexcept	{ x = (T)(x * scalar); y = (T)(y * scalar);	return *this; }
		Tn2&				operator/=			(int32_t	scalar)										{ x = (T)(x / scalar); y = (T)(y / scalar);	return *this; }
		Tn2&				operator*=			(uint32_t	scalar)							noexcept	{ x = (T)(x * scalar); y = (T)(y * scalar);	return *this; }
		Tn2&				operator/=			(uint32_t	scalar)										{ x = (T)(x / scalar); y = (T)(y / scalar);	return *this; }
		Tn2&				operator*=			(uint64_t	scalar)							noexcept	{ x = (T)(x * scalar); y = (T)(y * scalar);	return *this; }
		Tn2&				operator/=			(uint64_t	scalar)										{ x = (T)(x / scalar); y = (T)(y / scalar);	return *this; }
		//
		cnstxpr	bool		operator==			(const Tn2 & other)					const	noexcept	{ return x == other.x && y == other.y; }
		inlcxpr	bool		operator!=			(const Tn2 & other)					const	noexcept	{ return !operator==(other); }
		cnstxpr	Tn2			operator-			()									const	noexcept	{ return {x*-1, y*-1}; }

		//
		inlcxpr	Tn2			xx					()									const	noexcept	{ return {x, x}; }
		inlcxpr	Tn2			xy					()									const	noexcept	{ return {x, y}; }
		inlcxpr	Tn2			yx					()									const	noexcept	{ return {y, x}; }
		inlcxpr	Tn2			yy					()									const	noexcept	{ return {y, y}; }

		//
		template<typename _t>
		inlcxpr	n2<_t>		Cast				()									const	noexcept	{ return {(_t)x, (_t)y}; }
		inlcxpr	T			Area				()									const				{ return x * y; }
		Tn2					Clamp				(const Tn2 & min, const Tn2 & max)	const	noexcept	{ return {::gpk::clamp(x, min.x, max.x), ::gpk::clamp(y, min.y, max.y)}; }
		inlcxpr	Tn2			GetScaled			(double	scalar)						const	noexcept	{ return {(T)(x * scalar), (T)(y * scalar)}; }
		inlcxpr	Tn2			GetScaled			(double scalarx, double scalary)	const	noexcept	{ return {(T)(x * scalarx), (T)(y * scalary)}; }
		inline	Tn2			GetNormalized		()									const				{ const T sqLen = LengthSquared(); if(sqLen) { const double len = ::sqrt(sqLen); return {(T)(x / len), (T)(y / len)}; } else return {x, y};	}
		cnstxpr	double		Dot					(const Tn2 & other)					const	noexcept	{ return x * other.x + y * other.y; }
		cnstxpr	T			LengthSquared		()									const	noexcept	{ return x * x + y * y; }
		cnstxpr	double		Length				()									const				{ const T sqLen = LengthSquared(); return sqLen ? ::sqrt(sqLen) : 0; }
		cnstxpr	double		AngleWith			(const Tn2 & other)					const				{ double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos(Dot(other) / lengthsProduct) : 0; }
		void				AddScaled			(const Tn2 & vectorToScaleAndAdd, float scale)			{
			x					+= vectorToScaleAndAdd.x * scale;
			y					+= vectorToScaleAndAdd.y * scale;
		}
		Tn2&				Rotate				(double theta)											{
			const ::gpk::SSinCos	pairSinCos			= ::gpk::getSinCos(theta);
			const double			px					= x * pairSinCos.Cos - y * pairSinCos.Sin;
			y					= (T)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x					= (T)px;
			return *this;
		}
		inline	Tn2&		InPlaceScale		(double scalar)								noexcept	{ return *this *= scalar; }
		template<typename _t>
		inline	Tn2&		InPlaceScale		(_t scalarx, _t scalary)					noexcept	{ return *this = {(T)(x * scalarx), (T)(y * scalary)}; }
		template<typename _t>
		inline	Tn2&		InPlaceScale		(const n2<_t>& other)						noexcept	{ return InPlaceScale(other.x, other.y); }
		inline	Tn2&		InPlaceNormalize	()														{ const T sqLen = LengthSquared(); return sqLen ? *this /= ::sqrt(sqLen) : *this; }
		inline	Tn2&		InPlaceClamp		(const Tn2 & min, const Tn2 & max)			noexcept	{
			x					= ::gpk::clamp(x, min.x, max.x);
			y					= ::gpk::clamp(y, min.y, max.y);
			return *this;
		}
	};	// struct n2

	typedef n2<float>		n2f32, n2f;
	typedef n2<double>		n2f64, n2d;
	typedef n2<uint8_t>		n2u8;
	typedef n2<uint16_t>	n2u16;
	typedef n2<uint32_t>	n2u32;
	typedef n2<uint64_t>	n2u64;
	typedef n2<int8_t>		n2i8;
	typedef n2<int16_t>		n2i16;
	typedef n2<int32_t>		n2i32;
	typedef n2<int64_t>		n2i64;

	template<typename _tValue>
	stacxpr	bool	in_range	(const ::gpk::n2<_tValue>& valueToTest, const ::gpk::n2<_tValue>& rangeStart, const ::gpk::n2<_tValue>& rangeStop)	noexcept	{
		return	::gpk::in_range(valueToTest.x, rangeStart.x, rangeStop.x)
			&&	::gpk::in_range(valueToTest.y, rangeStart.y, rangeStop.y)
			;
	}
#pragma pack(pop)
} // namespace

#define N2_F32 "{%f, %f}"
#define N2_F64 "{%g, %g}"
#define N2_I32 "{%i, %i}"
#define N2_U32 "{%u, %u}"
#define N2_I64 "{%lli, %lli}"
#define N2_U64 "{%llu, %llu}"

#define gpk_xy(n2var) n2var.x, n2var.y

#endif // GPK_N2_H_230515
