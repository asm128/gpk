#include "gpk_n2.h"

#ifndef GPK_CIRCLE_H_23627
#define GPK_CIRCLE_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tpl_t struct circle { 
		tydf	_t		T;
		f3_t			Radius; 
		::gpk::n2<T>	Center; 

		GPK_DEFAULT_OPERATOR(circle<T>, Center == other.Center && Radius == other.Radius); 
	};
	tydf	circle<uc_t>	circleuc_t;	tdcs circleuc_t	circleuc_c;
	tydf	circle<sc_t>	circlesc_t;	tdcs circlesc_t	circlesc_c;
	tydf	circle<u0_t>	circleu0_t;	tdcs circleu0_t	circleu0_c;
	tydf	circle<u1_t>	circleu1_t;	tdcs circleu1_t	circleu1_c;
	tydf	circle<u2_t>	circleu2_t;	tdcs circleu2_t	circleu2_c;
	tydf	circle<u3_t>	circleu3_t;	tdcs circleu3_t	circleu3_c;
	tydf	circle<s0_t>	circles0_t;	tdcs circles0_t	circles0_c;
	tydf	circle<s1_t>	circles1_t;	tdcs circles1_t	circles1_c;
	tydf	circle<s2_t>	circles2_t;	tdcs circles2_t	circles2_c;
	tydf	circle<s3_t>	circles3_t;	tdcs circles3_t	circles3_c;
	tydf	circle<f2_t>	circlef2_t;	tdcs circlef2_t	circlef2_c;
	tydf	circle<f3_t>	circlef3_t;	tdcs circlef3_t	circlef3_c;
#pragma pack(pop)
} // namespace

#endif // GPK_CIRCLE_H_23627


