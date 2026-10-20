#include "gpk_n3.h"

#ifndef GPK_SPHERE_H_23627
#define GPK_SPHERE_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplT struct sphere {
		f3_t	Radius;
		n3<T>	Center;

		GPK_DEFAULT_OPERATOR(sphere<T>, Center == other.Center && Radius == other.Radius);
	};
#pragma pack(pop)

	tydf sphere<uc_t>	sphereuc_t;	tdcs sphereuc_t	sphereuc_c;
	tydf sphere<sc_t>	spheresc_t;	tdcs spheresc_t	spheresc_c;
	tydf sphere<u0_t>	sphereu0_t;	tdcs sphereu0_t	sphereu0_c;
	tydf sphere<u1_t>	sphereu1_t;	tdcs sphereu1_t	sphereu1_c;
	tydf sphere<u2_t>	sphereu2_t;	tdcs sphereu2_t	sphereu2_c;
	tydf sphere<u3_t>	sphereu3_t;	tdcs sphereu3_t	sphereu3_c;
	tydf sphere<s0_t>	spheres0_t;	tdcs spheres0_t	spheres0_c;
	tydf sphere<s1_t>	spheres1_t;	tdcs spheres1_t	spheres1_c;
	tydf sphere<s2_t>	spheres2_t;	tdcs spheres2_t	spheres2_c;
	tydf sphere<s3_t>	spheres3_t;	tdcs spheres3_t	spheres3_c;
	tydf sphere<f2_t>	spheref2_t;	tdcs spheref2_t	spheref2_c;
	tydf sphere<f3_t>	spheref3_t;	tdcs spheref3_t	spheref3_c;

	tplT stxp f3_t	sphereSize		(cnst sphere<T> & value) nxpt	{ return 1.3333333333333333 * math_pi * value.Radius * value.Radius * value.Radius; }
	tplT stxp bool	sphereOverlaps	(cnst sphere<T> & a, cnst sphere<T> & b) nxpt	{
		cnst f3_t distanceSquared	= (a.Center - b.Center).LengthSquared();
		cnst f3_t radiiSum			= a.Radius + b.Radius;
		return distanceSquared < radiiSum * radiiSum;
	}
}

#endif // GPK_SPHERE_H_23627


