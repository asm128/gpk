#include "gpk_n3.h"

#ifndef GPK_SPHERE_H_23627
#define GPK_SPHERE_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm T> 
	struct sphere { 
		f3_t			Radius;
		::gpk::n3<T>	Center;

		GPK_DEFAULT_OPERATOR(sphere<T>, Center == other.Center && Radius == other.Radius); 
	};
	typedef sphere<f2_t>		spheref2_t, spheref;
	typedef sphere<f3_t>		spheref64, sphered;
	typedef sphere<u0_t>		sphereu8;
	typedef sphere<u1_t>	sphereu1_t;
	typedef sphere<u2_t>	sphereu32;
	typedef sphere<u3_t>	sphereu64;
	typedef sphere<s0_t>		spherei8;
	typedef sphere<s1_t>		spherei16;
	typedef sphere<s2_t>		spherei32;
	typedef sphere<s3_t>		spherei64;

	// ---- Collision
	// Returns the volume of a sphere. This is used to calculate how to recurse into the bounding volume tree. For a bounding sphere it is a simple calculation.
	tplt<tpnm T> stxp f3_t	sphereSize		(const sphere<T> & sphere)								noexcept	{ return 1.3333333333333333 * ::gpk::math_pi * sphere.Radius * sphere.Radius * sphere.Radius; }
	tplt<tpnm T> stxp bool	sphereOverlaps	(const sphere<T> & sphereA, const sphere<T> & sphereB)	noexcept	{
		const f3_t	distanceSquared	= (sphereA.Center - sphereB.Center).LengthSquared();
		const f3_t	radiiSum		= sphereA.Radius + sphereB.Radius;
		return distanceSquared < (radiiSum * radiiSum);	// check squared distance against squared radius
	}
#pragma pack(pop)
} // namespace

#endif // GPK_SPHERE_H_23627
