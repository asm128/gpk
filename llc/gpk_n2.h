#include "gpk_math.h"

#include "gpk_functional.h"

#ifndef GPK_N2_H_23627
#define GPK_N2_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	tpl_t struct n2 {
		tydf	_t			T;
		T					x, y;

		tydf	n2<T>		Tn2;

		sinx	Tn2			from				(T value)		nxpt			{ return {value, value}; }

		//
		GPK_DEFAULT_OPERATOR(Tn2, x == other.x && y == other.y);

		cxpr	Tn2			oper+			(cnst Tn2 & other)	csnx	{ return {T(x + other.x), T(y + other.y)}; }
		cxpr	Tn2			oper-			(cnst Tn2 & other)	csnx	{ return {T(x - other.x), T(y - other.y)}; }
		cxpr	Tn2			oper*			(f3_t	scalar)		csnx	{ return {T(x * scalar), T(y * scalar)}; }
		cxpr	Tn2			oper/			(f3_t	scalar)		cnst	{ return {T(x / scalar), T(y / scalar)}; }
		cxpr	Tn2			oper*			(s3_t	scalar)		csnx	{ return {T(x * scalar), T(y * scalar)}; }
		cxpr	Tn2			oper/			(s3_t	scalar)		cnst	{ return {T(x / scalar), T(y / scalar)}; }
		cxpr	Tn2			oper*			(u3_t	scalar)		csnx	{ return {T(x * scalar), T(y * scalar)}; }
		cxpr	Tn2			oper/			(u3_t	scalar)		cnst	{ return {T(x / scalar), T(y / scalar)}; }
		cxpr	Tn2			oper*			(s2_t	scalar)		csnx	{ return {T(x * scalar), T(y * scalar)}; }
		cxpr	Tn2			oper/			(s2_t	scalar)		cnst	{ return {T(x / scalar), T(y / scalar)}; }
		cxpr	Tn2			oper*			(u2_t	scalar)		csnx	{ return {T(x * scalar), T(y * scalar)}; }
		cxpr	Tn2			oper/			(u2_t	scalar)		cnst	{ return {T(x / scalar), T(y / scalar)}; }
		//
		Tn2&				oper+=			(cnst Tn2 & other)	nxpt	{ x += other.x; y += other.y; return *this; }
		Tn2&				oper-=			(cnst Tn2 & other)	nxpt	{ x -= other.x; y -= other.y; return *this; }
		Tn2&				oper*=			(f3_t	scalar)		nxpt	{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				oper/=			(f3_t	scalar)				{ x = T(x / scalar); y = T(y / scalar);	return *this; }
		Tn2&				oper*=			(s3_t	scalar)		nxpt	{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				oper/=			(s3_t	scalar)				{ x = T(x / scalar); y = T(y / scalar);	return *this; }
		Tn2&				oper*=			(s2_t	scalar)		nxpt	{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				oper/=			(s2_t	scalar)				{ x = T(x / scalar); y = T(y / scalar);	return *this; }
		Tn2&				oper*=			(u2_t	scalar)		nxpt	{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				oper/=			(u2_t	scalar)				{ x = T(x / scalar); y = T(y / scalar);	return *this; }
		Tn2&				oper*=			(u3_t	scalar)		nxpt	{ x = T(x * scalar); y = T(y * scalar);	return *this; }
		Tn2&				oper/=			(u3_t	scalar)				{ x = T(x / scalar); y = T(y / scalar);	return *this; }

		cxpr	Tn2			oper-			()					csnx	{ return {x * -1, y * -1}; }

		//
		inxp	Tn2			xx					()				csnx	{ return {x, x}; }
		inxp	Tn2			xy					()				csnx	{ return {x, y}; }
		inxp	Tn2			yx					()				csnx	{ return {y, x}; }
		inxp	Tn2			yy					()				csnx	{ return {y, y}; }

		//
		tpl_t2
		inxp	n2<_t2>		Cast				()				csnx	{ return {(_t2)x, (_t2)y}; }
		inxp	n2<uc_t> 	uc_t				()				csnx	{ return Cast<::gpk::uc_t>(); }
		inxp	n2<sc_t> 	sc_t				()				csnx	{ return Cast<::gpk::sc_t>(); }
		inxp	n2<u0_t> 	u0_t				()				csnx	{ return Cast<::gpk::u0_t>(); }
		inxp	n2<u1_t> 	u1_t				()				csnx	{ return Cast<::gpk::u1_t>(); }
		inxp	n2<u2_t> 	u2_t				()				csnx	{ return Cast<::gpk::u2_t>(); }
		inxp	n2<u3_t> 	u3_t				()				csnx	{ return Cast<::gpk::u3_t>(); }
		inxp	n2<s0_t> 	s0_t				()				csnx	{ return Cast<::gpk::s0_t>(); }
		inxp	n2<s1_t> 	s1_t				()				csnx	{ return Cast<::gpk::s1_t>(); }
		inxp	n2<s2_t> 	s2_t				()				csnx	{ return Cast<::gpk::s2_t>(); }
		inxp	n2<s3_t> 	s3_t				()				csnx	{ return Cast<::gpk::s3_t>(); }
		inxp	n2<f2_t> 	f2_t				()				csnx	{ return Cast<::gpk::f2_t>(); }
		inxp	n2<f3_t> 	f3_t				()				csnx	{ return Cast<::gpk::f3_t>(); }

		inline	Tn2&		Set					(T value)							nxpt	{ x = y = value; return *this; }
		inline	Tn2&		From				(T value)							nxpt	{ x = y = value; return *this; }

		int					SetAxis				(uint8_t axisIndex, T value)		nxpt	{
			switch(axisIndex) { 
			case 0: x = value; return 0; 
			case 1: y = value; return 1; 
			}
			return -1;
		}
		int					SetOther			(uint8_t axisExcluded, T value)		nxpt	{
			switch(axisExcluded) { 
			case 0: y = value; return 0;
			case 1: x = value; return 1;
			}
			return -1;
		}

		inxp	uint32_t	Index				(uint32_t width) 							csnx	{ return y * width + x; }
		inxp	T			Area				()											csnx	{ return x * y; }
		Tn2					Clamp				(cnst Tn2 & min, cnst Tn2 & max)			csnx	{ return {::gpk::clamped(x, min.x, max.x), ::gpk::clamped(y, min.y, max.y)}; }
		inxp	Tn2			GetScaled			(::gpk::f3_t	scalar)						csnx	{ return {(T)(x * scalar), (T)(y * scalar)}; }
		inxp	Tn2			GetScaled			(::gpk::f3_t scalarx, ::gpk::f3_t scalary)	csnx	{ return {(T)(x * scalarx), (T)(y * scalary)}; }
		inxp	Tn2			GetScaled			(cnst Tn2 & scales)							csnx	{ return {(T)(x * scales.x), (T)(y * scales.y)}; }
		inline	Tn2			GetNormalized		()											csnx	{ cnst T sqLen = LengthSquared(); if(sqLen) { f3_c len = ::sqrt(sqLen); return {(T)(x / len), (T)(y / len)}; } else return {x, y};	}
		cxpr	::gpk::f3_t	Dot					(cnst Tn2 & other)							csnx	{ return x * other.x + y * other.y; }
		cxpr	T			LengthSquared		()											csnx	{ return x * x + y * y; }
		cxpr	::gpk::f3_t	Length				()											csnx	{ cnst T sqLen = LengthSquared(); return sqLen ? ::sqrt(sqLen) : 0; }
		cxpr	::gpk::f3_t	AngleWith			(cnst Tn2 & other)							csnx	{ ::gpk::f3_t lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos(Dot(other) / lengthsProduct) : 0; }
		void				AddScaled			(cnst Tn2 & vectorToScaleAndAdd, ::gpk::f2_t scale)	nxpt	{
			x					+= vectorToScaleAndAdd.x * scale;
			y					+= vectorToScaleAndAdd.y * scale;
		}
		Tn2&				Rotate				(::gpk::f3_t theta)								nxpt	{
			cnst ::gpk::SSinCos	pairSinCos			= ::gpk::getSinCos(theta);
			f3_c			px					= x * pairSinCos.Cos - y * pairSinCos.Sin;
			y					= T(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x					= (T)px;
			return *this;	
		}
		inline	Tn2&		InPlaceScale		(::gpk::f3_t scalar)							nxpt	{ return *this *= scalar; }
		tpl_t2
		inline	Tn2&		InPlaceScale		(_t2 scalarx, _t2 scalary)						nxpt	{ return *this = {(T)(x * scalarx), (T)(y * scalary)}; }
		tpl_t2
		inline	Tn2&		InPlaceScale		(cnst n2<_t2>& other)							nxpt	{ return InPlaceScale(other.x, other.y); }
		inline	Tn2&		InPlaceNormalize	()														{ cnst T sqLen = LengthSquared(); return sqLen ? *this /= ::sqrt(sqLen) : *this; }
		inline	Tn2&		InPlaceClamp		(cnst Tn2 & min, cnst Tn2 & max)				nxpt	{
			x					= ::gpk::clamped(x, min.x, max.x);
			y					= ::gpk::clamped(y, min.y, max.y);
			return *this;
		}

		tydf	FVoid<Tn2&>				TFuncForEach;
		tydf	FVoid<uint32_t&, Tn2&>	TFuncEnumerate;

		T					for_each			(cnst TFuncForEach & funcForEach, T xStart = 0)			cnst	{
			Tn2						offset				= {};
			for(; offset.y < y; ++offset.y)
			for(offset.x = xStart; offset.x < x; ++offset.x) 
				funcForEach(offset);

			return T(offset.x * offset.y);
		}
		uint32_t			enumerate			(cnst TFuncEnumerate & funcForEach)	cnst {
			uint32_t				index				= 0;
			Tn2						offset				= {};
			for(; offset.y < y; ++offset.y)
			for(offset.x = 0; offset.x < x; ++offset.x, ++index) 
				funcForEach(index, offset);

			return index;
		}
		uint32_t			enumerate			(cnst TFuncEnumerate & funcForEachX, cnst TFuncEnumerate & funcForEachY)	cnst {
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
	tydf	n2<uc_t>	n2uc_t;	tdcs	n2uc_t	n2uc_c;
	tydf	n2<sc_t>	n2sc_t;	tdcs	n2sc_t	n2sc_c;
	tydf	n2<u0_t>	n2u0_t;	tdcs	n2u0_t	n2u0_c;
	tydf	n2<u1_t>	n2u1_t;	tdcs	n2u1_t	n2u1_c;
	tydf	n2<u2_t>	n2u2_t;	tdcs	n2u2_t	n2u2_c;
	tydf	n2<u3_t>	n2u3_t;	tdcs	n2u3_t	n2u3_c;
	tydf	n2<s0_t>	n2s0_t;	tdcs	n2s0_t	n2s0_c;
	tydf	n2<s1_t>	n2s1_t;	tdcs	n2s1_t	n2s1_c;
	tydf	n2<s2_t>	n2s2_t;	tdcs	n2s2_t	n2s2_c;
	tydf	n2<s3_t>	n2s3_t;	tdcs	n2s3_t	n2s3_c;
	tydf	n2<f2_t>	n2f2_t;	tdcs	n2f2_t	n2f2_c;
	tydf	n2<f3_t>	n2f3_t;	tdcs	n2f3_t	n2f3_c;

	tplT	stxp	bool	in_range	(cnst ::gpk::n2<T> & valueToTest, cnst ::gpk::n2<T> & rangeStart, cnst ::gpk::n2<T> & rangeStop)	nxpt	{
		return	::gpk::in_range(valueToTest.x, rangeStart.x, rangeStop.x)
			&&	::gpk::in_range(valueToTest.y, rangeStart.y, rangeStop.y)
			;
	}
#pragma pack(pop)

#define GPK_USING_N2()					\
	using ::gpk::n2uc_t, ::gpk::n2uc_c	\
		, ::gpk::n2sc_t, ::gpk::n2sc_c	\
		, ::gpk::n2u0_t, ::gpk::n2u0_c	\
		, ::gpk::n2u1_t, ::gpk::n2u1_c	\
		, ::gpk::n2u2_t, ::gpk::n2u2_c	\
		, ::gpk::n2u3_t, ::gpk::n2u3_c	\
		, ::gpk::n2s0_t, ::gpk::n2s0_c	\
		, ::gpk::n2s1_t, ::gpk::n2s1_c	\
		, ::gpk::n2s2_t, ::gpk::n2s2_c	\
		, ::gpk::n2s3_t, ::gpk::n2s3_c	\
		, ::gpk::n2f2_t, ::gpk::n2f2_c	\
		, ::gpk::n2f3_t, ::gpk::n2f3_c
		
} // namespace

#define N2_F2	"%f, %f"
#define N2_F3	"%f, %f"
#define N2_S0	"%" GPK_FMT_S0 ", %" GPK_FMT_S0
#define N2_U0	"%" GPK_FMT_U0 ", %" GPK_FMT_U0
#define N2_I16	"%" GPK_FMT_S1 ", %" GPK_FMT_S1
#define N2_U1	"%" GPK_FMT_U1 ", %" GPK_FMT_U1
#define N2_S2	"%" GPK_FMT_S2 ", %" GPK_FMT_S2
#define N2_U2	"%" GPK_FMT_U2 ", %" GPK_FMT_U2
#define N2_S3	"%ll" GPK_FMT_S3 ", %ll" GPK_FMT_S3
#define N2_U3	"%ll" GPK_FMT_U3 ", %ll" GPK_FMT_U3

#define gpk_xy(n2var) n2var.x, n2var.y

#endif // GPK_N2_H_23627
