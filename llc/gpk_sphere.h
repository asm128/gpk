#include "gpk_n3.h"

#ifndef GPK_SPHERE_H_23627
#define GPK_SPHERE_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm T> 
	struct sphere { 
		double			Radius;
		::gpk::n3<T>	Center;

		GPK_DEFAULT_OPERATOR(sphere<T>, Center == other.Center && Radius == other.Radius); 
	};
	typedef sphere<float>		spheref32, spheref;
	typedef sphere<double>		spheref64, sphered;
	typedef sphere<uint8_t>		sphereu8;
	typedef sphere<uint16_t>	sphereu16;
	typedef sphere<uint32_t>	sphereu32;
	typedef sphere<uint64_t>	sphereu64;
	typedef sphere<int8_t>		spherei8;
	typedef sphere<int16_t>		spherei16;
	typedef sphere<int32_t>		spherei32;
	typedef sphere<int64_t>		spherei64;

	// ---- Collision
	// Returns the volume of a sphere. This is used to calculate how to recurse into the bounding volume tree. For a bounding sphere it is a simple calculation.
	tplt<tpnm T> stacxpr double	sphereSize		(const sphere<T> & sphere)								noexcept	{ return 1.3333333333333333 * ::gpk::math_pi * sphere.Radius * sphere.Radius * sphere.Radius; }
	tplt<tpnm T> stacxpr bool	sphereOverlaps	(const sphere<T> & sphereA, const sphere<T> & sphereB)	noexcept	{
		const double	distanceSquared	= (sphereA.Center - sphereB.Center).LengthSquared();
		const double	radiiSum		= sphereA.Radius + sphereB.Radius;
		return distanceSquared < (radiiSum * radiiSum);	// check squared distance against squared radius
	}
#pragma pack(pop)
} // namespace

#endif // GPK_SPHERE_H_23627
