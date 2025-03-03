#include "gpk_math.h"

#include "gpk_functional.h"

#ifndef GPK_N2_H_23627
#define GPK_N2_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	tplt<tpnm _tBase>
	struct n2 {
		typedef	_tBase		T;
		T					x, y;

		typedef	n2<T>		Tn2;

		stincxp	Tn2			from				(T value)							noexcept			{ return {value, value}; }

		//
		GPK_DEFAULT_OPERATOR(Tn2, x == other.x && y == other.y);

		cnstxpr	Tn2			operator+			(const Tn2 & other)					const	noexcept	{ return {T(x + other.x), T(y + other.y)}; }
		cnstxpr	Tn2			operator-			(const Tn2 & other)					const	noexcept	{ return {T(x - other.x), T(y - other.y)}; }
		cnstxpr	Tn2			operator*			(double		scalar)					const	noexcept	{ return {T(x * scalar), T(y * scalar)}; }
		cnstxpr	Tn2			operator/			(double		scalar)					const				{ return {T(x / scalar), T(y / scalar)}; }
		cnstxpr	Tn2			operator*			(int64_t	scalar)					const	noexcept	{ return {T(x * scalar), T(y * scalar)}; }
		cnstxpr	Tn2			operator/			(int64_t	scalar)					const				{ return {T(x / scalar), T(y / scalar)}; }
		cnstxpr	Tn2			operator*			(uint64_t	scalar)					const	noexcept	{ return {T(x * scalar), T(y * scalar)}; }
		cnstxpr	Tn2			operator/			(uint64_t	scalar)					const				{ return {T(x / scalar), T(y / scalar)}; }
		cnstxpr	Tn2			operator*			(int32_t	scalar)					const	noexcept	{ return {T(x * scalar), T(y * scalar)}; }
		cnstxpr	Tn2			operator/			(int32_t	scalar)					const				{ return {T(x / scalar), T(y / scalar)}; }
		cnstxpr	Tn2			operator*			(uint32_t	scalar)					const	noexcept	{ return {T(x * scalar), T(y * scalar)}; }
		cnstxpr	Tn2			operator/			(uint32_t	scalar)					const				{ return {T(x / scalar), T(y / scalar)}; }
		//
		Tn2&				operator+=			(const Tn2 & other)					noexcept			{ x += other.x; y += other.y; return *this; }
		Tn2&				operator-=			(const Tn2 & other)					noexcept			{ x -= other.x; y -= other.y; return *this; }
		Tn2&				operator*=			(double		scalar)					noexcept			{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				operator/=			(double		scalar)										{ x = T(x / scalar); y = T(y / scalar);	return *this; }
		Tn2&				operator*=			(int64_t	scalar)					noexcept			{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				operator/=			(int64_t	scalar)										{ x = T(x / scalar); y = T(y / scalar);	return *this; }
		Tn2&				operator*=			(int32_t	scalar)					noexcept			{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				operator/=			(int32_t	scalar)										{ x = T(x / scalar); y = T(y / scalar);	return *this; }
		Tn2&				operator*=			(uint32_t	scalar)					noexcept			{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				operator/=			(uint32_t	scalar)										{ x = T(x / scalar); y = T(y / scalar);	return *this; }
		Tn2&				operator*=			(uint64_t	scalar)					noexcept			{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				operator/=			(uint64_t	scalar)										{ x = T(x / scalar); y = T(y / scalar);	return *this; }

		cnstxpr	Tn2			operator-			()									const	noexcept	{ return {x * -1, y * -1}; }

		//
		inlcxpr	Tn2			xx					()									const	noexcept	{ return {x, x}; }
		inlcxpr	Tn2			xy					()									const	noexcept	{ return {x, y}; }
		inlcxpr	Tn2			yx					()									const	noexcept	{ return {y, x}; }
		inlcxpr	Tn2			yy					()									const	noexcept	{ return {y, y}; }

		//
		tplt<tpnm _t>
		inlcxpr	n2<_t>		Cast				()									const	noexcept	{ return {(_t)x, (_t)y}; }

		inlcxpr	n2<uint8_t>	u0_t					()									const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	n2<uint16_t>u1_t					()									const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	n2<uint32_t>u2_t					()									const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	n2<uint64_t>u3_t					()									const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	n2<int8_t>	s0_t					()									const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	n2<int16_t>	s1_t					()									const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	n2<int32_t>	s2_t					()									const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	n2<int64_t>	s3_t					()									const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	n2<float>	f2_t					()									const	noexcept	{ return Cast<float		>(); }
		inlcxpr	n2<double>	f3_t					()									const	noexcept	{ return Cast<double	>(); }

		inline	Tn2&		Set					(T value)							noexcept	{ x = y = value; return *this; }
		inline	Tn2&		From				(T value)							noexcept	{ x = y = value; return *this; }

		int					SetAxis				(uint8_t axisIndex, T value)		noexcept	{
			switch(axisIndex) { 
			case 0: x = value; return 0; 
			case 1: y = value; return 1; 
			}
			return -1;
		}
		int					SetOther			(uint8_t axisExcluded, T value)		noexcept	{
			switch(axisExcluded) { 
			case 0: y = value; return 0;
			case 1: x = value; return 1;
			}
			return -1;
		}

		inlcxpr	uint32_t	Index				(uint32_t width) 						const	noexcept	{ return y * width + x; }
		inlcxpr	T			Area				()										const	noexcept	{ return x * y; }
		Tn2					Clamp				(const Tn2 & min, const Tn2 & max)		const	noexcept	{ return {::gpk::clamped(x, min.x, max.x), ::gpk::clamped(y, min.y, max.y)}; }
		inlcxpr	Tn2			GetScaled			(double	scalar)							const	noexcept	{ return {(T)(x * scalar), (T)(y * scalar)}; }
		inlcxpr	Tn2			GetScaled			(double scalarx, double scalary)		const	noexcept	{ return {(T)(x * scalarx), (T)(y * scalary)}; }
		inlcxpr	Tn2			GetScaled			(const Tn2 & scales)					const	noexcept	{ return {(T)(x * scales.x), (T)(y * scales.y)}; }
		inline	Tn2			GetNormalized		()										const	noexcept	{ const T sqLen = LengthSquared(); if(sqLen) { const double len = ::sqrt(sqLen); return {(T)(x / len), (T)(y / len)}; } else return {x, y};	}
		cnstxpr	double		Dot					(const Tn2 & other)						const	noexcept	{ return x * other.x + y * other.y; }
		cnstxpr	T			LengthSquared		()										const	noexcept	{ return x * x + y * y; }
		cnstxpr	double		Length				()										const	noexcept	{ const T sqLen = LengthSquared(); return sqLen ? ::sqrt(sqLen) : 0; }
		cnstxpr	double		AngleWith			(const Tn2 & other)						const	noexcept	{ double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos(Dot(other) / lengthsProduct) : 0; }
		void				AddScaled			(const Tn2 & vectorToScaleAndAdd, float scale)	noexcept	{
			x					+= vectorToScaleAndAdd.x * scale;
			y					+= vectorToScaleAndAdd.y * scale;
		}
		Tn2&				Rotate				(double theta)									noexcept	{
			const ::gpk::SSinCos	pairSinCos			= ::gpk::getSinCos(theta);
			const double			px					= x * pairSinCos.Cos - y * pairSinCos.Sin;
			y					= T(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x					= (T)px;
			return *this;	
		}
		inline	Tn2&		InPlaceScale		(double scalar)									noexcept	{ return *this *= scalar; }
		tplt<tpnm _t>
		inline	Tn2&		InPlaceScale		(_t scalarx, _t scalary)						noexcept	{ return *this = {(T)(x * scalarx), (T)(y * scalary)}; }
		tplt<tpnm _t>
		inline	Tn2&		InPlaceScale		(const n2<_t>& other)							noexcept	{ return InPlaceScale(other.x, other.y); }
		inline	Tn2&		InPlaceNormalize	()															{ const T sqLen = LengthSquared(); return sqLen ? *this /= ::sqrt(sqLen) : *this; }
		inline	Tn2&		InPlaceClamp		(const Tn2 & min, const Tn2 & max)				noexcept	{
			x					= ::gpk::clamped(x, min.x, max.x);
			y					= ::gpk::clamped(y, min.y, max.y);
			return *this;
		}

		typedef	FVoid<Tn2&>				TFuncForEach;
		typedef	FVoid<uint32_t&, Tn2&>	TFuncEnumerate;

		T					for_each			(const TFuncForEach & funcForEach, T xStart = 0)			const	{
			Tn2						offset				= {};
			for(; offset.y < y; ++offset.y)
			for(offset.x = xStart; offset.x < x; ++offset.x) 
				funcForEach(offset);

			return T(offset.x * offset.y);
		}
		uint32_t			enumerate			(const TFuncEnumerate & funcForEach)	const {
			uint32_t				index				= 0;
			Tn2						offset				= {};
			for(; offset.y < y; ++offset.y)
			for(offset.x = 0; offset.x < x; ++offset.x, ++index) 
				funcForEach(index, offset);

			return index;
		}
		uint32_t			enumerate			(const TFuncEnumerate & funcForEachX, const TFuncEnumerate & funcForEachY)	const {
			uint32_t				index				= 0;
			Tn2						offset				= {};
			for(; offset.y < y; ++offset.y) {
				for(offset.x = 0; offset.x < x; ++offset.x, ++index) 
					funcForEachX(index, offset);
				
				funcForEachY(index, offset);
			}
			return index;
		}

	};	// struct n2
	typedef	n2<sc_t>		n2char;
	typedef	n2<uc_t>		n2uchar;
	typedef	n2<float>		n2f32;
	typedef	n2<double>		n2f64;
	typedef	n2<uint8_t>		n2u8;
	typedef	n2<uint16_t>	n2u16;
	typedef	n2<uint32_t>	n2u32;
	typedef	n2<uint64_t>	n2u64;
	typedef	n2<int8_t>		n2i8;
	typedef	n2<int16_t>		n2i16;
	typedef	n2<int32_t>		n2i32;
	typedef	n2<int64_t>		n2i64;

	typedef	const n2char	cn2char		;
	typedef	const n2uchar	cn2uchar	;
	typedef	const n2f32		cn2f32		;
	typedef	const n2f64		cn2f64		;
	typedef	const n2u8		cn2u8		;
	typedef	const n2u16		cn2u16		;
	typedef	const n2u32		cn2u32		;
	typedef	const n2u64		cn2u64		;
	typedef	const n2i8		cn2i8		;
	typedef	const n2i16		cn2i16		;
	typedef	const n2i32		cn2i32		;
	typedef	const n2i64		cn2i64		;

	tplt<tpnm _tValue>
	stacxpr	bool	in_range	(const ::gpk::n2<_tValue>& valueToTest, const ::gpk::n2<_tValue>& rangeStart, const ::gpk::n2<_tValue>& rangeStop)	noexcept	{
		return	::gpk::in_range(valueToTest.x, rangeStart.x, rangeStop.x)
			&&	::gpk::in_range(valueToTest.y, rangeStart.y, rangeStop.y)
			;
	}
#pragma pack(pop)
} // namespace

#define N2_F32	"%f, %f"
#define N2_F64	"%f, %f"
#define N2_I8	"%" GPK_FMT_I8 ", %" GPK_FMT_I8
#define N2_U8	"%" GPK_FMT_U8 ", %" GPK_FMT_U8
#define N2_I16	"%" GPK_FMT_I16 ", %" GPK_FMT_I16
#define N2_U16	"%" GPK_FMT_U16 ", %" GPK_FMT_U16
#define N2_I32	"%" GPK_FMT_I32 ", %" GPK_FMT_I32
#define N2_U32	"%" GPK_FMT_U32 ", %" GPK_FMT_U32
#define N2_I64	"%ll" GPK_FMT_I64 ", %ll" GPK_FMT_I64
#define N2_U64	"%ll" GPK_FMT_U64 ", %ll" GPK_FMT_U64

#define gpk_xy(n2var) n2var.x, n2var.y

#endif // GPK_N2_H_23627
