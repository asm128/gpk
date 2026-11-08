#include "gpk_n3.h"

#ifndef GPK_SPHERE_H_23627
#define GPK_SPHERE_H_23627

nmsp gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm T> 
	struct sphere { 
		f3_t	Radius;
		n3<T>	Center;

		GPK_DEFAULT_OPERATOR(sphere<T>, Center == other.Center && Radius == other.Radius); 
	};
	tydf sphere<f2_t> spheref2_t; tdcs spheref2_t spheref2_c;
	tydf sphere<f3_t> spheref3_t; tdcs spheref3_t spheref3_c;
	tydf sphere<u0_t> sphereu0_t; tdcs sphereu0_t sphereu0_c;
	tydf sphere<u1_t> sphereu1_t; tdcs sphereu1_t sphereu1_c;
	tydf sphere<u2_t> sphereu2_t; tdcs sphereu2_t sphereu2_c;
	tydf sphere<u3_t> sphereu3_t; tdcs sphereu3_t sphereu3_c;
	tydf sphere<s0_t> spheres0_t; tdcs spheres0_t spheres0_c;
	tydf sphere<s1_t> spheres1_t; tdcs spheres1_t spheres1_c;
	tydf sphere<s2_t> spheres2_t; tdcs spheres2_t spheres2_c;
	tydf sphere<s3_t> spheres3_t; tdcs spheres3_t spheres3_c;

	// ---- Collision
	// Returns the volume of a sphere. This is used to calculate how to recurse into the bounding volume tree. For a bounding sphere it is a simple calculation.
	tplt<tpnm T> stxp f3_t	sphereSize		(const sphere<T> & sphere)								nxpt	{ return 1.3333333333333333 * math_pi * sphere.Radius * sphere.Radius * sphere.Radius; }
	tplt<tpnm T> stxp bool	sphereOverlaps	(const sphere<T> & sphereA, const sphere<T> & sphereB)	nxpt	{
		const f3_t	distanceSquared	= (sphereA.Center - sphereB.Center).LengthSquared();
		const f3_t	radiiSum		= sphereA.Radius + sphereB.Radius;
		return distanceSquared < (radiiSum * radiiSum);	// check squared distance against squared radius
	}
#pragma pack(pop)
} // nmsp

#endif // GPK_SPHERE_H_23627
