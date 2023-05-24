#include "gpk_n3.h"

#ifndef GPK_CIRCLE_H_9283749823649230524
#define GPK_CIRCLE_H_9283749823649230524

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename T> struct SSphere { double Radius; ::gpk::n3<T> Center; GPK_DEFAULT_OPERATOR_NE(SSphere<T>, Center == other.Center && Radius == other.Radius); };
	template<typename T> struct SCircle { double Radius; ::gpk::n2<T> Center; GPK_DEFAULT_OPERATOR_NE(SCircle<T>, Center == other.Center && Radius == other.Radius); };
	// ---- Collision
	// Returns the volume of a sphere. This is used to calculate how to recurse into the bounding volume tree. For a bounding sphere it is a simple calculation.
	template<typename T> stacxpr double	sphereSize			(const SSphere<T> &sphere)								noexcept	{ return 1.3333333333333333 * ::gpk::math_pi * sphere.Radius * sphere.Radius * sphere.Radius; }
	template<typename T> stacxpr bool	sphereOverlaps		(const SSphere<T> &sphereA, const SSphere<T> &sphereB)	noexcept	{
		const double							distanceSquared		= (sphereA.Center - sphereB.Center).LengthSquared();
		const double							radiiSum			= sphereA.Radius + sphereB.Radius;
		return distanceSquared < (radiiSum * radiiSum);	// check squared distance against squared radius
	}
#pragma pack(pop)
} // namespace

#endif // GPK_CIRCLE_H_9283749823649230524
