#include "gpk_tri.h"
#include "gpk_n3.h"
#include "gpk_bit.h"
#include "gpk_eval.h"
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

	template<typename _tDimension>	struct STriangle2 : public STriangle<::gpk::n2<_tDimension>>		{
		typedef	::gpk::n2<_tDimension>	TVertex;	
		using STriangle<TVertex>::		A;
		using STriangle<TVertex>::		B;
		using STriangle<TVertex>::		C;
		using STriangle<TVertex>::		STriangle;

		template<typename _tOther>
		STriangle2<_tOther>				Cast							()											const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				, C.template Cast<_tOther>()
				};
		}
		::gpk::error_t					CulledX							(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.x  < minMax.Min) && (B.x  < minMax.Min) && (C.x  < minMax.Min))
				|| ((A.x >= minMax.Max) && (B.x >= minMax.Max) && (C.x >= minMax.Max))
				? 1 : 0;
		}
		::gpk::error_t					CulledY							(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.y  < minMax.Min) && (B.y  < minMax.Min) && (C.y  < minMax.Min))
				|| ((A.y >= minMax.Max) && (B.y >= minMax.Max) && (C.y >= minMax.Max))
				? 1 : 0;
		}
	};
	template<typename _tDimension>	
	using	tri2			= STriangle2<_tDimension>;
	typedef tri2<float>		tri2f32, tri2f;
	typedef tri2<double>	tri2f64, tri2d;
	typedef tri2<uint8_t>	tri2u8;
	typedef tri2<uint16_t>	tri2u16;
	typedef tri2<uint32_t>	tri2u32;
	typedef tri2<uint64_t>	tri2u64;
	typedef tri2<int8_t>	tri2i8;
	typedef tri2<int16_t>	tri2i16;
	typedef tri2<int32_t>	tri2i32;
	typedef tri2<int64_t>	tri2i64;

	template<typename _tDimension>	struct STriangle3 : public STriangle<::gpk::n3<_tDimension>>		{
		typedef	::gpk::n3<_tDimension>	TVertex;	
		using STriangle<TVertex>::		A;
		using STriangle<TVertex>::		B;
		using STriangle<TVertex>::		C;
		using STriangle<TVertex>::		STriangle;

		template<typename _tOther>
		STriangle3<_tOther>				Cast							()		const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				, C.template Cast<_tOther>()
				};
		}
		::gpk::error_t					CulledX							(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.x  < minMax.Min) && (B.x  < minMax.Min) && (C.x  < minMax.Min))
				|| ((A.x >= minMax.Max) && (B.x >= minMax.Max) && (C.x >= minMax.Max))
				? 1 : 0;
		}
		::gpk::error_t					CulledY							(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.y  < minMax.Min) && (B.y  < minMax.Min) && (C.y  < minMax.Min))
				|| ((A.y >= minMax.Max) && (B.y >= minMax.Max) && (C.y >= minMax.Max))
				? 1 : 0;
		}
		::gpk::error_t					CulledZ							(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.z  < minMax.Min) && (B.z  < minMax.Min) && (C.z  < minMax.Min))
				|| ((A.z >= minMax.Max) && (B.z >= minMax.Max) && (C.z >= minMax.Max))
				? 1 : 0;
		}
		::gpk::error_t					CulledZSpecial					(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.z <= minMax.Min) || (B.z <= minMax.Min) || (C.z <= minMax.Min))
				|| ((A.z >= minMax.Max) && (B.z >= minMax.Max) && (C.z >= minMax.Max))
				? 1 : 0;
		}
		bool							ClipZ							()		const	noexcept		{
			if(A.z < 0 || A.z >= 1) return true;
			if(B.z < 0 || B.z >= 1) return true;
			if(C.z < 0 || C.z >= 1) return true;
			return false;
		}
		STriangle3<_tDimension>&		Scale							(const ::gpk::n3<_tDimension> & scale)		noexcept		{
			A.Scale(scale);
			B.Scale(scale);
			C.Scale(scale);
			return *this;
		}
		STriangle3<_tDimension>&		Translate				(const ::gpk::n3<_tDimension> & translation)	noexcept		{
			A								+= translation;
			B								+= translation;
			C								+= translation;
			return *this;
		}
	};
	template<typename _tDimension>	
	using	tri3			= STriangle3<_tDimension>;
	typedef tri3<float>		tri3f32, tri3f;
	typedef tri3<double>	tri3f64, tri3d;
	typedef tri3<uint8_t>	tri3u8;
	typedef tri3<uint16_t>	tri3u16;
	typedef tri3<uint32_t>	tri3u32;
	typedef tri3<uint64_t>	tri3u64;
	typedef tri3<int8_t>	tri3i8;
	typedef tri3<int16_t>	tri3i16;
	typedef tri3<int32_t>	tri3i32;
	typedef tri3<int64_t>	tri3i64;

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

	template<typename _tElement>
		STriangle2<_tElement> &			translate			(::gpk::STriangle2<_tElement> & triangle, const ::gpk::n2<_tElement> & translation)	{
		triangle.A							+= translation;
		triangle.B							+= translation;
		triangle.C							+= translation;
		return triangle;
	}

	template<typename _tElement>
		STriangle3<_tElement> &			translate			(::gpk::STriangle3<_tElement> & triangle, const ::gpk::n3<_tElement> & translation)	{
		triangle.A							+= translation;
		triangle.B							+= translation;
		triangle.C							+= translation;
		return triangle;
	}
	template<typename _tElement>
		STriangle3<_tElement> &			scale				(::gpk::STriangle3<_tElement> & triangle, const ::gpk::n3<_tElement> & scale)		{
		triangle.A.Scale(scale);
		triangle.B.Scale(scale);
		triangle.C.Scale(scale);
		return triangle;
	}
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
	::gpk::n2<_tCoord>&										realignCoord
					(	const ::gpk::n2<uint32_t>		& targetSize
					,	const ::gpk::n2<_tCoord>		& coordToRealign
					,	::gpk::n2<_tCoord>				& coordRealigned
					,	::gpk::ALIGN						align
					)																																					noexcept	{
		coordRealigned																	= coordToRealign;
 			 if gbit_true(align, ALIGN_CENTER	)	coordRealigned.x						+= (targetSize.x >> 1);
		else if gbit_true(align, ALIGN_RIGHT	)	coordRealigned.x						= targetSize.x - coordToRealign.x;

			 if gbit_true(align, ALIGN_VCENTER	)	coordRealigned.y						+= (targetSize.y >> 1);
		else if gbit_true(align, ALIGN_BOTTOM	)	coordRealigned.y						= targetSize.y - coordToRealign.y;
		return coordRealigned;
	}

	template<typename _tValue>
	stacxpr	bool														in_range		(const ::gpk::n2<_tValue>& valueToTest, const ::gpk::n2<_tValue>& rangeStart, const ::gpk::n2<_tValue>& rangeStop)	noexcept	{
		return	::gpk::in_range(valueToTest.x, rangeStart.x, rangeStop.x)
			&&	::gpk::in_range(valueToTest.y, rangeStart.y, rangeStop.y)
			;
	}

	template<typename _tValue>
	stacxpr	bool														in_range		(const ::gpk::n2<_tValue>& pointToTest, const ::gpk::SRectangle2<_tValue>& area)	noexcept	{
		return	::gpk::in_range(pointToTest.x, area.Offset.x, (_tValue)(area.Offset.x + area.Size.x))
			&&	::gpk::in_range(pointToTest.y, area.Offset.y, (_tValue)(area.Offset.y + area.Size.y))
			;
	}

	template<typename _tCoord>
	stainli	double														determinant		(const ::gpk::SLine2<_tCoord>& line)													noexcept	{ return ::gpk::determinant((double)line.A.x, (double)line.A.y, (double)line.B.x, (double)line.B.y); }

	template<typename _tValue>
	::gpk::n3<_tValue>				triangleWeight		(const STriangle<_tValue> & weights, const STriangle3<_tValue> & values)	{ return values.A * weights.A + values.B * weights.B + values.C * weights.C; }
	template<typename _tValue>
	::gpk::n2<_tValue>				triangleWeight		(const STriangle<_tValue> & weights, const STriangle2<_tValue> & values)	{ return values.A * weights.A + values.B * weights.B + values.C * weights.C; }

}

#endif // GPK_COORD_H_928374982364923322
