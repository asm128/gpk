#include "gpk_typeint.h"

#ifndef GPK_RANGE_H
#define GPK_RANGE_H

namespace gpk
{
#pragma pack(push, 1)
	tpl_t struct range {	
		tydf		_t	T;
		T							Offset, Count;	
		GPK_DEFAULT_OPERATOR(range <T>, Offset == other.Offset && Count == other.Count); 

		tpl_t2
		inxp	range<_t2>			Cast		()		const	noexcept	{ return{(_t2)Offset, (_t2)Count}; } 
		inxp	range<uc_t>			uc			()		const	noexcept	{ return Cast<uc_t>(); }
		inxp	range<sc_t>			sc			()		const	noexcept	{ return Cast<sc_t>(); }
		inxp	range<u0_t>			u0			()		const	noexcept	{ return Cast<u0_t>(); }
		inxp	range<u1_t>			u1			()		const	noexcept	{ return Cast<u1_t>(); }
		inxp	range<u2_t>			u2			()		const	noexcept	{ return Cast<u2_t>(); }
		inxp	range<u3_t>			u3			()		const	noexcept	{ return Cast<u3_t>(); }
		inxp	range<s0_t>			s0			()		const	noexcept	{ return Cast<s0_t>(); }
		inxp	range<s1_t>			s1			()		const	noexcept	{ return Cast<s1_t>(); }
		inxp	range<s2_t>			s2			()		const	noexcept	{ return Cast<s2_t>(); }
		inxp	range<s3_t>			s3			()		const	noexcept	{ return Cast<s3_t>(); }
		inxp	range<f2_t>			f2			()		const	noexcept	{ return Cast<f2_t>(); }
		inxp	range<f3_t>			f3			()		const	noexcept	{ return Cast<f3_t>(); }
	};
#pragma pack(pop)

	tydf	range<uc_t>	rangeuc_t;	tydf	const rangeuc_t	rangeuc_c;
	tydf	range<sc_t>	rangesc_t;	tydf	const rangesc_t	rangesc_c;
	tydf	range<u0_t>	rangeu0_t;	tydf	const rangeu0_t	rangeu0_c;
	tydf	range<u1_t>	rangeu1_t;	tydf	const rangeu1_t	rangeu1_c;
	tydf	range<u2_t>	rangeu2_t;	tydf	const rangeu2_t	rangeu2_c;
	tydf	range<u3_t>	rangeu3_t;	tydf	const rangeu3_t	rangeu3_c;
	tydf	range<s0_t>	ranges0_t;	tydf	const ranges0_t	ranges0_c;
	tydf	range<s1_t>	ranges1_t;	tydf	const ranges1_t	ranges1_c;
	tydf	range<s2_t>	ranges2_t;	tydf	const ranges2_t	ranges2_c;
	tydf	range<s3_t>	ranges3_t;	tydf	const ranges3_t	ranges3_c;
	tydf	range<f2_t>	rangef2_t;	tydf	const rangef2_t	rangef2_c;
	tydf	range<f3_t>	rangef3_t;	tydf	const rangef3_t	rangef3_c;
}

#endif // GPK_RANGE_H
