#include "gpk_n3.h"
#include "gpk_bit.h"
#include "gpk_log.h"

#ifndef GPK_COORD_H_928374982364923322
#define GPK_COORD_H_928374982364923322

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	typedef SMinMax<float> SNearFar;

	// ---- Geometric figures and other coord-related POD structs.
	template<typename _tUnit>	struct SRange			{ _tUnit			Offset, Count			; GPK_DEFAULT_OPERATOR_NE(SRange		<_tUnit>, Offset	== other.Offset	&& Count	== other.Count	); };
	template<typename _tUnit>	struct SSlice			{ _tUnit			Begin, End				; GPK_DEFAULT_OPERATOR_NE(SSlice		<_tUnit>, Begin		== other.Begin	&& End		== other.End	); };
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

	template<typename _tVertex>		
	struct SLine { 
		_tVertex					A, B; 

		cnstxpr						SLine		()											= default;
		cnstxpr						SLine		(const SLine<_tVertex> & other)				= default;
		cnstxpr						SLine		(const _tVertex & a, const _tVertex & b)	: A(a), B(b)	{}

		GPK_DEFAULT_OPERATOR_NE(SLine<_tVertex>, A == other.A && B == other.B);
	};

	template<typename _tDimension>	struct SLine2 : public SLine<::gpk::n2<_tDimension>>		{
		typedef	::gpk::n2<_tDimension>	TVertex;	
		using SLine<TVertex>::		A;
		using SLine<TVertex>::		B;
		using SLine<TVertex>::		SLine;

		template<typename _tOther>
		SLine2<_tOther>				Cast							()		const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				};
		}
	};

	template<typename _tDimension>	struct SLine3 : public SLine<::gpk::n3<_tDimension>>		{
		typedef	::gpk::n3<_tDimension>	TVertex;	
		using SLine<TVertex>::		A;
		using SLine<TVertex>::		B;
		using SLine<TVertex>::		SLine;

		template<typename _tOther>
		SLine3<_tOther>				Cast							()		const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				};
		}
	};

	template<typename _tElement>	struct SCircle			{ double Radius; ::gpk::n2<_tElement>	Center			; GPK_DEFAULT_OPERATOR_NE(SCircle		<_tElement>, Center	== other.Center	&& Radius	== other.Radius					); };

	template<typename _tElement>	struct SRectangle3D		{ ::gpk::n3<_tElement>					Offset, Size	; GPK_DEFAULT_OPERATOR_NE(SRectangle3D	<_tElement>, Offset	== other.Offset	&& Size		== other.Size					); };
	template<typename _tDim>		struct SQuad			{ _tDim									A, B, C, D		; GPK_DEFAULT_OPERATOR_NE(SQuad			<_tDim>		, A == other.A	&& B == other.B	&& C == other.C	&& D == other.D); };
	template<typename _tElement>	struct SQuad2			{ ::gpk::n2<_tElement>					A, B, C, D		; GPK_DEFAULT_OPERATOR_NE(SQuad2		<_tElement>	, A == other.A	&& B == other.B	&& C == other.C	&& D == other.D); };
	template<typename _tElement>	struct SQuad3			{ ::gpk::n3<_tElement>					A, B, C, D		; GPK_DEFAULT_OPERATOR_NE(SQuad3		<_tElement>	, A == other.A	&& B == other.B	&& C == other.C	&& D == other.D); };
	template<typename _tElement>	struct SSphere			{ double Radius; ::gpk::n3<_tElement>	Center			; GPK_DEFAULT_OPERATOR_NE(SSphere		<_tElement>, Center	== other.Center	&& Radius	== other.Radius					); };

	template<typename _tElement>	struct SRectangle2		{
		::gpk::n2<_tElement>			Offset, Size;

		GPK_DEFAULT_OPERATOR_NE(SRectangle2<_tElement>, Offset	== other.Offset	&& Size == other.Size);

		inline	::gpk::n2<_tElement>	Limit				()		const	noexcept	{ return Offset + Size; }
	};


#pragma pack(pop)

	// ---- Line
	template<typename _tElement>	stainli	cnstxpr	_tElement				rise					(const SLine2<_tElement> & line)														noexcept	{ return line.B.y - line.A.y;		}
	template<typename _tElement>	stainli	cnstxpr	_tElement				run						(const SLine2<_tElement> & line)														noexcept	{ return line.B.x - line.A.x;		}
	template<typename _tElement>	stainli	cnstxpr	_tElement				slope					(const SLine2<_tElement> & line)																	{ return rise(line) / run(line);	}
	template<typename _tElement>	static	cnstxpr	_tElement				orient2d				(const ::gpk::SLine2<_tElement> & segment, const ::gpk::n2<_tElement> & point)	noexcept	{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
	template<typename _tElement>	static	cnstxpr	_tElement				orient2d3d				(const ::gpk::SLine3<_tElement> & segment, const ::gpk::n2<_tElement> & point)	noexcept	{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }

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
	enum AXIS : uint8_t
		{	AXIS_ORIGIN				= 0x0
		,	AXIS_X_POSITIVE			= 0x1
		,	AXIS_X_NEGATIVE			= 0x2
		,	AXIS_Y_POSITIVE			= 0x4
		,	AXIS_Y_NEGATIVE			= 0x8
		,	AXIS_Z_POSITIVE			= 0x10
		,	AXIS_Z_NEGATIVE			= 0x20
		};

	enum DIRECTION : uint8_t
		{	DIRECTION_ORIGIN		= AXIS_ORIGIN	
		,	DIRECTION_FRONT			= AXIS_X_POSITIVE
		,	DIRECTION_BACK			= AXIS_X_NEGATIVE
		,	DIRECTION_UP			= AXIS_Y_POSITIVE
		,	DIRECTION_DOWN			= AXIS_Y_NEGATIVE
		,	DIRECTION_RIGHT			= AXIS_Z_POSITIVE
		,	DIRECTION_LEFT			= AXIS_Z_NEGATIVE
		};

	enum ALIGN : uint8_t
		{	ALIGN_ORIGIN			= 0
		,	ALIGN_RIGHT				= ((uint8_t)::gpk::AXIS_X_POSITIVE)
		,	ALIGN_LEFT				= ((uint8_t)::gpk::AXIS_X_NEGATIVE)
		,	ALIGN_BOTTOM			= ((uint8_t)::gpk::AXIS_Y_POSITIVE)
		,	ALIGN_TOP				= ((uint8_t)::gpk::AXIS_Y_NEGATIVE)
		,	ALIGN_FAR				= ((uint8_t)::gpk::AXIS_Z_POSITIVE)
		,	ALIGN_NEAR				= ((uint8_t)::gpk::AXIS_Z_NEGATIVE)
		,	ALIGN_HCENTER			= ALIGN_LEFT	| ALIGN_RIGHT
		,	ALIGN_VCENTER			= ALIGN_TOP		| ALIGN_BOTTOM
		,	ALIGN_DCENTER			= ALIGN_FAR		| ALIGN_NEAR
		,	ALIGN_XCENTER			= ALIGN_LEFT	| ALIGN_RIGHT
		,	ALIGN_YCENTER			= ALIGN_TOP		| ALIGN_BOTTOM
		,	ALIGN_ZCENTER			= ALIGN_FAR		| ALIGN_NEAR
		,	ALIGN_TOP_LEFT			= ALIGN_TOP		| ALIGN_LEFT
		,	ALIGN_TOP_RIGHT			= ALIGN_TOP		| ALIGN_RIGHT
		,	ALIGN_BOTTOM_LEFT		= ALIGN_BOTTOM	| ALIGN_LEFT
		,	ALIGN_BOTTOM_RIGHT		= ALIGN_BOTTOM	| ALIGN_RIGHT
		,	ALIGN_CENTER_TOP		= ALIGN_HCENTER	| ALIGN_TOP
		,	ALIGN_CENTER_BOTTOM		= ALIGN_HCENTER	| ALIGN_BOTTOM
		,	ALIGN_CENTER_LEFT		= ALIGN_VCENTER	| ALIGN_LEFT
		,	ALIGN_CENTER_RIGHT		= ALIGN_VCENTER	| ALIGN_RIGHT
		,	ALIGN_CENTER			= ALIGN_VCENTER	| ALIGN_HCENTER
		};
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

	template<typename _tCoord>
	stainli	double					determinant			(const ::gpk::SLine2<_tCoord>& line)	noexcept	{ return ::gpk::determinant((double)line.A.x, (double)line.A.y, (double)line.B.x, (double)line.B.y); }

}

#endif // GPK_COORD_H_928374982364923322
