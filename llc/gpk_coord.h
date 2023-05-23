#include "gpk_n3.h"
#include "gpk_bit.h"
#include "gpk_log.h"
#include "gpk_enum.h"

#ifndef GPK_COORD_H_928374982364923322
#define GPK_COORD_H_928374982364923322

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack

	// ---- Geometric figures and other coord-related POD structs.
	template<typename _tUnit>	struct SOrigin			{ ::gpk::n3<_tUnit>	Front, Up, Right		; GPK_DEFAULT_OPERATOR_NE(SOrigin		<_tUnit>, Front		== other.Front	&& Up		== other.Up		&& Right == other.Right); };
	template<typename _tUnit>	struct SRectLimits		{
		_tUnit			Left, Top, Right, Bottom; 
		
		GPK_DEFAULT_OPERATOR_NE(SRectLimits	<_tUnit>, Left		== other.Left	&& Top		== other.Top	&& Right == other.Right && Bottom == other.Bottom); 

		inlcxpr	_tUnit				Width		()	const { return Right - Left; }
		inlcxpr	_tUnit				Height		()	const { return Bottom - Top; }
		cnstxpr ::gpk::n2<_tUnit>	Dimensions	()	const { return {Width(), Height()}; }
		template <typename _t>
		inlcxpr	SRectLimits<_t>		Cast		()	const	noexcept	{ return {(_t)Left, (_t)Top, (_t)Right, (_t)Bottom}; }
	};

	template<typename _tDim>	struct SSphere			{ double Radius; ::gpk::n3<_tDim>	Center		; GPK_DEFAULT_OPERATOR_NE(SSphere		<_tDim>, Center == other.Center && Radius == other.Radius); };
	template<typename _tDim>	struct SCircle			{ double Radius; ::gpk::n2<_tDim>	Center		; GPK_DEFAULT_OPERATOR_NE(SCircle		<_tDim>, Center == other.Center && Radius == other.Radius); };
	template<typename _tDim>	struct SRectangle3D		{ ::gpk::n3<_tDim>					Offset, Size; GPK_DEFAULT_OPERATOR_NE(SRectangle3D	<_tDim>, Offset == other.Offset && Size   == other.Size  ); };

	template<typename _tElement>	struct SRectangle2		{
		::gpk::n2<_tElement>			Offset, Size;

		GPK_DEFAULT_OPERATOR_NE(SRectangle2<_tElement>, Offset	== other.Offset	&& Size == other.Size);

		inline	::gpk::n2<_tElement>	Limit				()		const	noexcept	{ return Offset + Size; }
	};


#pragma pack(pop)


	// ---- Collision
	//template<typename _tElement>	static					bool					raySegmentIntersect		(n2<_tElement> r_d, n2<_tElement> r_p, n2<_tElement> s_d, n2<_tElement> s_p)								{
	//	const double																		t2						= (r_d.x * (s_p.y - r_p.y) + r_d.y * (r_p.x - s_p.x)) / (s_d.x * r_d.y - s_d.y * r_d.x);
	//	const double																		t1						= (s_p.x + s_d.x * t2 - r_p.x) / r_d.x;	// Plug the value of T2 to get T1
	//	return (t2 > 0 && 0 < t2 && t2 < 1);
	//}

	template<typename _tElement>	static					bool					sphereOverlaps			(const SSphere<_tElement> &sphereA, const SSphere<_tElement> &sphereB)	noexcept	{
		const double																		distanceSquared			= (sphereA.Center - sphereB.Center).LengthSquared();
		const double																		radiiSum				= sphereA.Radius + sphereB.Radius;
		return distanceSquared < (radiiSum * radiiSum);	// check squared distance against squared radius
	}

	// Returns the volume of a sphere. This is used to calculate how to recurse into the bounding volume tree. For a bounding sphere it is a simple calculation.
	template<typename _tElement>	static					double					sphereSize				(const SSphere<_tElement> &sphere)										noexcept	{ return 1.3333333333333333 * ::gpk::math_pi * sphere.Radius * sphere.Radius * sphere.Radius; }


#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(AXIS, uint8_t);
	GDEFINE_ENUM_VALUE(AXIS, ORIGIN		, 0x0);
	GDEFINE_ENUM_VALUE(AXIS, X_POSITIVE	, 0x1);
	GDEFINE_ENUM_VALUE(AXIS, X_NEGATIVE	, 0x2);
	GDEFINE_ENUM_VALUE(AXIS, Y_POSITIVE	, 0x4);
	GDEFINE_ENUM_VALUE(AXIS, Y_NEGATIVE	, 0x8);
	GDEFINE_ENUM_VALUE(AXIS, Z_POSITIVE	, 0x10);
	GDEFINE_ENUM_VALUE(AXIS, Z_NEGATIVE	, 0x20);

	GDEFINE_ENUM_TYPE(DIRECTION, uint8_t);
	GDEFINE_ENUM_VALUE(DIRECTION, ORIGIN, AXIS_ORIGIN);
	GDEFINE_ENUM_VALUE(DIRECTION, FRONT	, AXIS_X_POSITIVE);
	GDEFINE_ENUM_VALUE(DIRECTION, BACK	, AXIS_X_NEGATIVE);
	GDEFINE_ENUM_VALUE(DIRECTION, UP	, AXIS_Y_POSITIVE);
	GDEFINE_ENUM_VALUE(DIRECTION, DOWN	, AXIS_Y_NEGATIVE);
	GDEFINE_ENUM_VALUE(DIRECTION, RIGHT	, AXIS_Z_POSITIVE);
	GDEFINE_ENUM_VALUE(DIRECTION, LEFT	, AXIS_Z_NEGATIVE);

	GDEFINE_ENUM_TYPE(ALIGN, uint8_t);
	GDEFINE_ENUM_VALUE(ALIGN, ORIGIN		, 0);
	GDEFINE_ENUM_VALUE(ALIGN, RIGHT			, ((uint8_t)::gpk::AXIS_X_POSITIVE));
	GDEFINE_ENUM_VALUE(ALIGN, LEFT			, ((uint8_t)::gpk::AXIS_X_NEGATIVE));
	GDEFINE_ENUM_VALUE(ALIGN, BOTTOM		, ((uint8_t)::gpk::AXIS_Y_POSITIVE));
	GDEFINE_ENUM_VALUE(ALIGN, TOP			, ((uint8_t)::gpk::AXIS_Y_NEGATIVE));
	GDEFINE_ENUM_VALUE(ALIGN, FAR			, ((uint8_t)::gpk::AXIS_Z_POSITIVE));
	GDEFINE_ENUM_VALUE(ALIGN, NEAR			, ((uint8_t)::gpk::AXIS_Z_NEGATIVE));
	GDEFINE_ENUM_VALUE(ALIGN, HCENTER		, ALIGN_LEFT	| ALIGN_RIGHT);
	GDEFINE_ENUM_VALUE(ALIGN, VCENTER		, ALIGN_TOP		| ALIGN_BOTTOM);
	GDEFINE_ENUM_VALUE(ALIGN, DCENTER		, ALIGN_FAR		| ALIGN_NEAR);
	GDEFINE_ENUM_VALUE(ALIGN, XCENTER		, ALIGN_LEFT	| ALIGN_RIGHT);
	GDEFINE_ENUM_VALUE(ALIGN, YCENTER		, ALIGN_TOP		| ALIGN_BOTTOM);
	GDEFINE_ENUM_VALUE(ALIGN, ZCENTER		, ALIGN_FAR		| ALIGN_NEAR);
	GDEFINE_ENUM_VALUE(ALIGN, TOP_LEFT		, ALIGN_TOP		| ALIGN_LEFT);
	GDEFINE_ENUM_VALUE(ALIGN, TOP_RIGHT		, ALIGN_TOP		| ALIGN_RIGHT);
	GDEFINE_ENUM_VALUE(ALIGN, BOTTOM_LEFT	, ALIGN_BOTTOM	| ALIGN_LEFT);
	GDEFINE_ENUM_VALUE(ALIGN, BOTTOM_RIGHT	, ALIGN_BOTTOM	| ALIGN_RIGHT);
	GDEFINE_ENUM_VALUE(ALIGN, CENTER_TOP	, ALIGN_HCENTER	| ALIGN_TOP);
	GDEFINE_ENUM_VALUE(ALIGN, CENTER_BOTTOM	, ALIGN_HCENTER	| ALIGN_BOTTOM);
	GDEFINE_ENUM_VALUE(ALIGN, CENTER_LEFT	, ALIGN_VCENTER	| ALIGN_LEFT);
	GDEFINE_ENUM_VALUE(ALIGN, CENTER_RIGHT	, ALIGN_VCENTER	| ALIGN_RIGHT);
	GDEFINE_ENUM_VALUE(ALIGN, CENTER		, ALIGN_VCENTER	| ALIGN_HCENTER);
#pragma pack(pop)

	//------------------------------------------------------------------------------------------------------------
	template <typename _tCoord>
	::gpk::SRectangle2<_tCoord>&									realignRectangle
		(	const ::gpk::n2<uint32_t>			& targetSize
		,	const ::gpk::SRectangle2<_tCoord>	& rectangleToRealign
		,	::gpk::SRectangle2<_tCoord>			& rectangleRealigned
		,	ALIGN								align
		)																																					noexcept	{
		rectangleRealigned															= rectangleToRealign;
			 if(::gpk::bit_true(align, ::gpk::ALIGN_HCENTER	))	{ rectangleRealigned.Offset.x += (_tCoord)(targetSize.x >> 1)	- (rectangleRealigned.Size.x >> 1); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_RIGHT	))	{ rectangleRealigned.Offset.x =  (_tCoord) targetSize.x			- (rectangleRealigned.Size.x + rectangleRealigned.Offset.x); }
		else													{}

			 if(::gpk::bit_true(align, ::gpk::ALIGN_VCENTER	))	{ rectangleRealigned.Offset.y += (_tCoord)(targetSize.y >> 1)	- (rectangleRealigned.Size.y >> 1); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_BOTTOM	))	{ rectangleRealigned.Offset.y =  (_tCoord) targetSize.y			- (rectangleRealigned.Size.y + rectangleRealigned.Offset.y); }
		else													{}
		return rectangleRealigned;
	}

	template <typename _tCoord>
	::gpk::SRectangle2<_tCoord>&									realignRectangle
					(	const ::gpk::n2<uint32_t>			& targetSize
					,	const ::gpk::SRectangle2<double>		& rectangleToRealign
					,	::gpk::SRectangle2<double>				& rectangleRealigned
					,	ALIGN									align
					)																																					noexcept	{
		rectangleRealigned															= rectangleToRealign;
			 if(::gpk::bit_true(align, ::gpk::ALIGN_HCENTER	))	{ rectangleRealigned.Offset.x += (_tCoord)(targetSize.x >> 1)	- (rectangleRealigned.Size.x / 2); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_RIGHT	))	{ rectangleRealigned.Offset.x =  (_tCoord) targetSize.x			- (rectangleRealigned.Size.x + rectangleRealigned.Offset.x); }
		else													{}

			 if(::gpk::bit_true(align, ::gpk::ALIGN_VCENTER	))	{ rectangleRealigned.Offset.y += (_tCoord)(targetSize.y >> 1)	- (rectangleRealigned.Size.y / 2); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_BOTTOM	))	{ rectangleRealigned.Offset.y =  (_tCoord) targetSize.y			- (rectangleRealigned.Size.y + rectangleRealigned.Offset.y); }
		else													{}
		return rectangleRealigned;
	}

	template <typename _tCoord>
	::gpk::SRectangle2<_tCoord>&									dockRectangle
					(	const ::gpk::SRectangle2<_tCoord>		& rectangleToDockTo
					,	const ::gpk::SRectangle2<_tCoord>		& rectangleToDock
					,	::gpk::SRectangle2<_tCoord>			& rectangleDocked
					,	ALIGN									align
					)																																					noexcept	{
		rectangleDocked																= rectangleToDock;
			 /*if(::gpk::bit_true(align, ::gpk::ALIGN_HCENTER	))	{ rectangleDocked.Offset.x = (rectangleToDockTo.Size.x >> 1)	- (rectangleDocked.Size.x >> 1)	+ rectangleDocked.Offset.x; }
		else*/if(::gpk::bit_true(align, ::gpk::ALIGN_RIGHT	))	{ rectangleDocked.Offset.x = rectangleToDockTo.Size.x  - (rectangleDocked.Size.x		+ rectangleDocked.Offset.x); }
		else													{ rectangleDocked.Offset.x = rectangleToDockTo.Size.x; }

			 if(::gpk::bit_true(align, ::gpk::ALIGN_VCENTER	))	{ rectangleDocked.Offset.y = (rectangleToDockTo.Size.y >> 1)	- (rectangleDocked.Size.y >> 1)	+ rectangleDocked.Offset.y; }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_BOTTOM	))	{ rectangleDocked.Offset.y = rectangleToDockTo.Size.y			- (rectangleDocked.Size.y		+ rectangleDocked.Offset.y); }
		else													{}
		return rectangleDocked;
	}

	//------------------------------------------------------------------------------------------------------------
	template <typename _tCoord>
	::gpk::n2<_tCoord>&	realignCoord
					( const ::gpk::n2u32		& targetSize
					, const ::gpk::n2<_tCoord>	& coordToRealign
					, ::gpk::n2<_tCoord>		& coordRealigned
					, ::gpk::ALIGN				align
					)																																					noexcept	{
		coordRealigned		= coordToRealign;
 			 if gbit_true(align, ALIGN_CENTER	) coordRealigned.x	+= (targetSize.x >> 1);
		else if gbit_true(align, ALIGN_RIGHT	) coordRealigned.x	= targetSize.x - coordToRealign.x;
												  
			 if gbit_true(align, ALIGN_VCENTER	) coordRealigned.y	+= (targetSize.y >> 1);
		else if gbit_true(align, ALIGN_BOTTOM	) coordRealigned.y	= targetSize.y - coordToRealign.y;
		return coordRealigned;
	}

	template<typename _tValue>
	stacxpr	bool		in_range	(const ::gpk::n2<_tValue>& pointToTest, const ::gpk::SRectangle2<_tValue>& area)	noexcept	{
		return	::gpk::in_range(pointToTest.x, area.Offset.x, (_tValue)(area.Offset.x + area.Size.x))
			&&	::gpk::in_range(pointToTest.y, area.Offset.y, (_tValue)(area.Offset.y + area.Size.y))
			;
	}
}

#endif // GPK_COORD_H_928374982364923322
