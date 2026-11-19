#include "gpk_n2.h"

#ifndef GPK_RECT2_H
#define GPK_RECT2_H

nmsp gpk
{
#pragma pack(push, 1)
	tplt<tpnm T>
	struct rect2 {
		n2<T>			Offset, Size;

		GPK_DEFAULT_OPERATOR(rect2<T>, Offset	== other.Offset	&& Size == other.Size);

		tpl_t	
		inxp	rect2<_t>	Cast		()	csnx	{ return {Offset.tplt Cast<_t>(), Size.tplt Cast<_t>()}; }

		inxp	rect2<uc_t>	uc			()	csnx	{ return Cast<uc_t>(); }
		inxp	rect2<sc_t>	sc			()	csnx	{ return Cast<sc_t>(); }
		inxp	rect2<u0_t>	u0			()	csnx	{ return Cast<u0_t>(); }
		inxp	rect2<u1_t>	u1			()	csnx	{ return Cast<u1_t>(); }
		inxp	rect2<u2_t>	u2			()	csnx	{ return Cast<u2_t>(); }
		inxp	rect2<u3_t>	u3			()	csnx	{ return Cast<u3_t>(); }
		inxp	rect2<s0_t>	s0			()	csnx	{ return Cast<s0_t>(); }
		inxp	rect2<s1_t>	s1			()	csnx	{ return Cast<s1_t>(); }
		inxp	rect2<s2_t>	s2			()	csnx	{ return Cast<s2_t>(); }
		inxp	rect2<s3_t>	s3			()	csnx	{ return Cast<s3_t>(); }
		inxp	rect2<f2_t>	f2			()	csnx	{ return Cast<f2_t>(); }
		inxp	rect2<f3_t>	f3			()	csnx	{ return Cast<f3_t>(); }

		inln	n2<T>		Limit		()	csnx	{ return Offset + Size; }
	};
	CTTI_STCT_TEMPLATE_EXISTING(rect2, T, struct rect2 { n2<T> Offset; n2<T> Size; };, n2<T>, n2<T>);
	tydf rect2<uc_t>	rect2uc_t;	tydf	cnst rect2uc_t	rect2uc_c;	
	tydf rect2<sc_t>	rect2sc_t;	tydf	cnst rect2sc_t	rect2sc_c;	
	tydf rect2<u0_t>	rect2u0_t;	tydf	cnst rect2u0_t	rect2u0_c;	
	tydf rect2<u1_t>	rect2u1_t;	tydf	cnst rect2u1_t	rect2u1_c;	
	tydf rect2<u2_t>	rect2u2_t;	tydf	cnst rect2u2_t	rect2u2_c;	
	tydf rect2<u3_t>	rect2u3_t;	tydf	cnst rect2u3_t	rect2u3_c;	
	tydf rect2<s0_t>	rect2s0_t;	tydf	cnst rect2s0_t	rect2s0_c;	
	tydf rect2<s1_t>	rect2s1_t;	tydf	cnst rect2s1_t	rect2s1_c;	
	tydf rect2<s2_t>	rect2s2_t;	tydf	cnst rect2s2_t	rect2s2_c;	
	tydf rect2<s3_t>	rect2s3_t;	tydf	cnst rect2s3_t	rect2s3_c;	
	tydf rect2<f2_t>	rect2f2_t;	tydf	cnst rect2f2_t	rect2f2_c;	
	tydf rect2<f3_t>	rect2f3_t;	tydf	cnst rect2f3_t	rect2f3_c;	

#pragma pack(pop)

	tplt<tpnm T>
	stxp	b8_t	in_range	(cnst n2<T>& pointToTest, cnst rect2<T>& area)	nxpt	{
		return	in_range(pointToTest.x, area.Offset.x, (T)(area.Offset.x + area.Size.x))
			&&	in_range(pointToTest.y, area.Offset.y, (T)(area.Offset.y + area.Size.y))
			;
	}
} // nmsp

#define GPK_USING_RECT2()					\
    usng  ::gpk::rect2uc_t, ::gpk::rect2uc_c \
        , ::gpk::rect2sc_t, ::gpk::rect2sc_c \
        , ::gpk::rect2u0_t, ::gpk::rect2u0_c \
        , ::gpk::rect2u1_t, ::gpk::rect2u1_c \
        , ::gpk::rect2u2_t, ::gpk::rect2u2_c \
        , ::gpk::rect2u3_t, ::gpk::rect2u3_c \
        , ::gpk::rect2s0_t, ::gpk::rect2s0_c \
        , ::gpk::rect2s1_t, ::gpk::rect2s1_c \
        , ::gpk::rect2s2_t, ::gpk::rect2s2_c \
        , ::gpk::rect2s3_t, ::gpk::rect2s3_c \
        , ::gpk::rect2f2_t, ::gpk::rect2f2_c \
        , ::gpk::rect2f3_t, ::gpk::rect2f3_c 
#endif // GPK_RECT2_H
