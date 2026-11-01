#include "gpk_view.h"		// for gpk::view<>
#include "gpk_rect2.h"
#include "gpk_n3.h"

#ifndef GPK_GRID_VIEW_H_23627
#define GPK_GRID_VIEW_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt <tpnm _tCell>
	class grid {
	protected:
		// Properties / Member Variables
		_tCell						* Data			= 0;
		::gpk::n2u2_t				Size			= {0, 0};
	public:
		tydf	_tCell				T;
		// Constructors
		inxp						grid			()									nxpt	= default;
		inln						grid			(T * data, u2_t width, u2_t height)		: Data{data}, Size{width, height} {
			if_true_tef(0 == data && Size.x && Size.y, "Invalid parameters. Size: {x:%u, y:%u}, pointer: %p.", width, height, data);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}
		inln						grid			(T * data, ::gpk::n2u2_c & gridMetrics)		: grid(data, gridMetrics.x, gridMetrics.y) {}
		tplt<size_t _tWidth, size_t _tHeight>
		inxp						grid			(T (&data)[_tHeight][_tWidth])		nxpt	: Data{&data[0][0]}, Size{_tWidth, _tHeight} {}

		inxp	oper				grid<const T>	()							const	nxpt	{ return {Data, Size}; }
		// Operators
		::gpk::view<T>				operator[]		(u2_t row)									{ if_true_te(0 == Data); if_true_tef(row >= Size.y, "Invalid row: %i.", (s2_t)row); return {&Data[row * Size.x], Size.x}; }
		::gpk::view<const T>		operator[]		(u2_t row)				const				{ if_true_te(0 == Data); if_true_tef(row >= Size.y, "Invalid row: %i.", (s2_t)row); return {&Data[row * Size.x], Size.x}; }
		T&							operator[]		(::gpk::n2u2_c & cell)						{ if_true_te(0 == Data); if_true_tef(cell.y >= Size.y, "Invalid row: %i.", (s2_t)cell.y); if_true_tef(cell.x >= Size.x, "Invalid column: %i.", (s2_t)cell.x); return Data[cell.y * Size.x + cell.x]; }
		const T&					operator[]		(::gpk::n2u2_c & cell)	const				{ if_true_te(0 == Data); if_true_tef(cell.y >= Size.y, "Invalid row: %i.", (s2_t)cell.y); if_true_tef(cell.x >= Size.x, "Invalid column: %i.", (s2_t)cell.x); return Data[cell.y * Size.x + cell.x]; }

		inxp	const T*			begin			()		const	nxpt	{ return Data; }
		inxp	const T*			end				()		const	nxpt	{ return Data + size(); }

		inxp	T*					begin			()				nxpt	{ return Data; }
		inxp	T*					end				()				nxpt	{ return Data + size(); }

		inxp	::gpk::n2u2_c&		metrics			()		const	nxpt	{ return Size; }
		inxp	::gpk::n2u1_t		metrics16		()		const	nxpt	{ return Size.u1(); }
		inxp	u2_t				size			()		const	nxpt	{ return area(); }
		inxp	u2_t 				byte_count		()		const	nxpt	{ return area() * sizeof(T); }
		inxp	u2_t				area			()		const	nxpt	{ return Size.x * Size.y; }

		inln	gpk::err_t 	fill			(const T & value, u2_t offset = 0, u2_t count = 0xFFFFFFFFU) {
			if_true_vef(-1, (count > size()) && (count != 0xFFFFFFFFU), "Count: %u", count);
			for(u2_t i = offset, actualCount = ::gpk::min(size(), count); i < actualCount; ++i)
				Data[i]						= value;
			return 0;
		}
	};

#pragma pack(pop)

	tplt<tpnm TDim, tpnm TColor>
	static	gpk::err_t 	drawRectangle			(::gpk::grid<TColor> & target, const TColor & value, const ::gpk::rect2<TDim> & rectangle)		{
		s2_c						xCount					= ::gpk::min((s2_t)target.metrics().x - ::gpk::max((s2_t)rectangle.Offset.x, (s2_t)0), (s2_t)rectangle.Size.x + ::gpk::min((s2_t)rectangle.Offset.x, (s2_t)0));
		s2_c						yCount					= ::gpk::min((s2_t)target.metrics().y - ::gpk::max((s2_t)rectangle.Offset.y, (s2_t)0), (s2_t)rectangle.Size.y + ::gpk::min((s2_t)rectangle.Offset.y, (s2_t)0));
		s2_c						yStart					= (s2_t)::gpk::max((s2_t)0, (s2_t)rectangle.Offset.y);
		s2_c						xStart					= (s2_t)::gpk::max((s2_t)0, (s2_t)rectangle.Offset.x);
		if(xStart >= (s2_t)target.metrics().x || yStart >= (s2_t)target.metrics().y)
			return 0;
		if(xCount <= 0 || yCount <= 0)
			return 0;

		::gpk::view<TColor>				firstRow				= target[yStart];
		if constexpr(1 == sizeof(TColor))
			memset(&firstRow[xStart], *((u0_t*)&value), xCount);
		else {
			for(s2_t x = 0; x < xCount; ++x)
				firstRow[xStart + x]		= value;
		}
		for(s2_t y = 1; y < yCount; ++y)
			memcpy(&target[yStart + y][xStart], &target[yStart][xStart], xCount * sizeof(TColor));

		return 0;
	}

	// view common typedefs
	typedef	::gpk::grid<sc_t>					gc		, gchar;
	typedef	::gpk::grid<uc_t>					guc		, guchar;
	typedef	::gpk::grid<u0_t>					gub		;
	typedef	::gpk::grid<s0_t>					gb		;
	typedef	::gpk::grid<f2_t>					gf2_t	;
	typedef	::gpk::grid<f3_t>					gf64	;
	typedef	::gpk::grid<u0_t>					gu8		;
	typedef	::gpk::grid<u1_t>					gu1_t	;
	typedef	::gpk::grid<u2_t>					gu2_t	;
	typedef	::gpk::grid<u3_t>					gu64	;
	typedef	::gpk::grid<s0_t>					gi8		;
	typedef	::gpk::grid<s1_t>					gi16	;
	typedef	::gpk::grid<s2_t>					gi32	;
	typedef	::gpk::grid<s3_t>					gi64	;
	typedef	::gpk::grid<const ::gpk::n2<sc_t>>	g2c		;
	typedef	::gpk::grid<::gpk::n2u0_c>			g2ub	;
	typedef	::gpk::grid<::gpk::n2s0_c>			g2b		;
	typedef	::gpk::grid<::gpk::n2f2_c>			g2f2_t	;
	typedef	::gpk::grid<::gpk::n2f3_c>			g2f64	;
	typedef	::gpk::grid<::gpk::n2u0_c>			g2u8	;
	typedef	::gpk::grid<::gpk::n2u1_c>			g2u1_t	;
	typedef	::gpk::grid<::gpk::n2u2_c>			g2u32	;
	typedef	::gpk::grid<::gpk::n2u3_c>			g2u64	;
	typedef	::gpk::grid<::gpk::n2s0_c>			g2i8	;
	typedef	::gpk::grid<::gpk::n2s1_c>			g2s1_t	;
	typedef	::gpk::grid<::gpk::n2s2_c>			g2i32	;
	typedef	::gpk::grid<::gpk::n2s3_c>			g2i64	;

	typedef	::gpk::grid<::gpk::n2<sc_t>>		gn2c	;
	typedef	::gpk::grid<::gpk::n2u0_t>			gn2ub	;
	typedef	::gpk::grid<::gpk::n2s0_t>			gn2b	;
	typedef	::gpk::grid<::gpk::n2f2_t>			gn2f2_t	;
	typedef	::gpk::grid<::gpk::n2f3_t>			gn2f64	;
	typedef	::gpk::grid<::gpk::n2u0_t>			gn2u8	;
	typedef	::gpk::grid<::gpk::n2u1_t>			gn2u1_t	;
	typedef	::gpk::grid<::gpk::n2u2_t>			gn2u32	;
	typedef	::gpk::grid<::gpk::n2u3_t>			gn2u64	;
	typedef	::gpk::grid<::gpk::n2s0_t>			gn2i8	;
	typedef	::gpk::grid<::gpk::n2s1_t>			gn2s1_t	;
	typedef	::gpk::grid<::gpk::n2s2_t>			gn2i32	;
	typedef	::gpk::grid<::gpk::n2s3_t>			gn2i64	;
	typedef	::gpk::grid<const ::gpk::n2<sc_t>>	gc2c	;
	typedef	::gpk::grid<::gpk::n2u0_c>		gc2ub	;
	typedef	::gpk::grid<::gpk::n2s0_c>		gc2b	;
	typedef	::gpk::grid<::gpk::n2f2_c>		gc2f2_t	;
	typedef	::gpk::grid<::gpk::n2f3_c>		gc2f64	;
	typedef	::gpk::grid<::gpk::n2u0_c>		gc2u8	;
	typedef	::gpk::grid<::gpk::n2u1_c>		gc2u1_t	;
	typedef	::gpk::grid<::gpk::n2u2_c>		gc2u32	;
	typedef	::gpk::grid<::gpk::n2u3_c>		gc2u64	;
	typedef	::gpk::grid<::gpk::n2s0_c>		gc2i8	;
	typedef	::gpk::grid<::gpk::n2s1_c>		gc2s1_t	;
	typedef	::gpk::grid<::gpk::n2s2_c>		gc2i32	;
	typedef	::gpk::grid<::gpk::n2s3_c>		gc2i64	;

	typedef	::gpk::grid<::gpk::n3<sc_t>>		gn3c	;
	typedef	::gpk::grid<::gpk::n3u0_t>			gn3ub	;
	typedef	::gpk::grid<::gpk::n3s0_t>			gn3b	;
	typedef	::gpk::grid<::gpk::n3f2_t>			gn3f2_t	;
	typedef	::gpk::grid<::gpk::n3f3_t>			gn3f64	;
	typedef	::gpk::grid<::gpk::n3u0_t>			gn3u8	;
	typedef	::gpk::grid<::gpk::n3u1_t>			gn3u1_t	;
	typedef	::gpk::grid<::gpk::n3u2_t>			gn3u32	;
	typedef	::gpk::grid<::gpk::n3u3_t>			gn3u64	;
	typedef	::gpk::grid<::gpk::n3s0_t>			gn3s0_t	;
	typedef	::gpk::grid<::gpk::n3s1_t>			gn3i16	;
	typedef	::gpk::grid<::gpk::n3s2_t>			gn3i32	;
	typedef	::gpk::grid<::gpk::n3s3_t>			gn3i64	;
	typedef	::gpk::grid<const ::gpk::n3<sc_t>>	gc3c	;
	typedef	::gpk::grid<::gpk::n3u0_c>		gc3ub	;
	typedef	::gpk::grid<::gpk::n3s0_c>		gc3b	;
	typedef	::gpk::grid<::gpk::n3f2_c>		gc3f2_t	;
	typedef	::gpk::grid<::gpk::n3f3_c>		gc3f64	;
	typedef	::gpk::grid<::gpk::n3u0_c>		gc3u8	;
	typedef	::gpk::grid<::gpk::n3u1_c>		gc3u1_t	;
	typedef	::gpk::grid<::gpk::n3u2_c>		gc3u32	;
	typedef	::gpk::grid<::gpk::n3u3_c>		gc3u64	;
	typedef	::gpk::grid<::gpk::n3s0_c>		gc3s0_t	;
	typedef	::gpk::grid<::gpk::n3s1_c>		gc3i16	;
	typedef	::gpk::grid<::gpk::n3s2_c>		gc3i32	;
	typedef	::gpk::grid<::gpk::n3s3_c>		gc3i64	;

	enum GRID_ROTATION : u0_t {
		GRID_ROTATION_DEFAULT	= 0,
		GRID_ROTATION_0			= 0x1,
		GRID_ROTATION_90		= 0x2,
		GRID_ROTATION_180		= 0x4,
		GRID_ROTATION_270		= 0x8,
	};
}

#endif // GPK_GRID_VIEW_H_23627
