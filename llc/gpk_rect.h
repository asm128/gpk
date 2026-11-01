#include "gpk_n2.h"

#ifndef GPK_RECT_H_23627
#define GPK_RECT_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplT struct rect {
		T	Left, Top, Right, Bottom;

		GPK_DEFAULT_OPERATOR(rect<T>, Left == other.Left && Top == other.Top && Right == other.Right && Bottom == other.Bottom);

		cxpr	rect<T>	oper+		(cnst rect<T> & other)	csnx	{ return {T(Left + other.Left), T(Top + other.Top), T(Right + other.Right), T(Bottom + other.Bottom)}; }
		inxp	T			Width		()							csnx	{ return Right - Left; }
		inxp	T			Height		()							csnx	{ return Bottom - Top; }
		inxp	n2<T>		Dimensions	()							csnx	{ return {Width(), Height()}; }

		tpl_t2
		inxp	rect<_t2>	Cast		()							csnx	{ return {(_t2)Left, (_t2)Top, (_t2)Right, (_t2)Bottom}; }

		inxp	rect<uc_t>	uc			()							csnx	{ return Cast<uc_t>(); }
		inxp	rect<sc_t>	sc			()							csnx	{ return Cast<sc_t>(); }
		inxp	rect<u0_t>	u0			()							csnx	{ return Cast<u0_t>(); }
		inxp	rect<u1_t>	u1			()							csnx	{ return Cast<u1_t>(); }
		inxp	rect<u2_t>	u2			()							csnx	{ return Cast<u2_t>(); }
		inxp	rect<u3_t>	u3			()							csnx	{ return Cast<u3_t>(); }
		inxp	rect<s0_t>	s0			()							csnx	{ return Cast<s0_t>(); }
		inxp	rect<s1_t>	s1			()							csnx	{ return Cast<s1_t>(); }
		inxp	rect<s2_t>	s2			()							csnx	{ return Cast<s2_t>(); }
		inxp	rect<s3_t>	s3			()							csnx	{ return Cast<s3_t>(); }
		inxp	rect<f2_t>	f2			()							csnx	{ return Cast<f2_t>(); }
		inxp	rect<f3_t>	f3			()							csnx	{ return Cast<f3_t>(); }
	};
#pragma pack(pop)

	tydf rect<uc_t>	rectuc_t;	tdcs rectuc_t	rectuc_c;
	tydf rect<sc_t>	rectsc_t;	tdcs rectsc_t	rectsc_c;
	tydf rect<u0_t>	rectu0_t;	tdcs rectu0_t	rectu0_c;
	tydf rect<u1_t>	rectu1_t;	tdcs rectu1_t	rectu1_c;
	tydf rect<u2_t>	rectu2_t;	tdcs rectu2_t	rectu2_c;
	tydf rect<u3_t>	rectu3_t;	tdcs rectu3_t	rectu3_c;
	tydf rect<s0_t>	rects0_t;	tdcs rects0_t	rects0_c;
	tydf rect<s1_t>	rects1_t;	tdcs rects1_t	rects1_c;
	tydf rect<s2_t>	rects2_t;	tdcs rects2_t	rects2_c;
	tydf rect<s3_t>	rects3_t;	tdcs rects3_t	rects3_c;
	tydf rect<f2_t>	rectf2_t;	tdcs rectf2_t	rectf2_c;
	tydf rect<f3_t>	rectf3_t;	tdcs rectf3_t	rectf3_c;
}

#endif // GPK_RECT_H_23627
