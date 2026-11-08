#include "gpk_rect2.h"
#include "gpk_n3.h"
#include "gpk_enum.h"

#ifndef GPK_GRID_VIEW_H_23627
#define GPK_GRID_VIEW_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	tplt <tpnm _tCell>
	class grid {
	protected:
		// Properties / Member Variables
		_tCell						* Data			= 0;
		n2u2_t				Size			= {0, 0};
	public:
		tydf	_tCell				T;
		// Constructors
		inxp						grid			()									nxpt	= default;
		inln						grid			(T * data, u2_t width, u2_t height)		: Data{data}, Size{width, height} {
			if_true_tef(0 == data && Size.x && Size.y, "Invalid parameters. Size: {x:%u, y:%u}, pointer: %p.", width, height, data);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}
		inln						grid			(T * data, n2u2_c & gridMetrics)		: grid(data, gridMetrics.x, gridMetrics.y) {}
		tplt<size_t _tWidth, size_t _tHeight>
		inxp						grid			(T (&data)[_tHeight][_tWidth])		nxpt	: Data{&data[0][0]}, Size{_tWidth, _tHeight} {}

		inxp	oper				grid<const T>	()							const	nxpt	{ return {Data, Size}; }
		// Operators
		view<T>				operator[]		(u2_t row)									{ if_true_te(0 == Data); if_true_tef(row >= Size.y, "Invalid row: %i.", (s2_t)row); return {&Data[row * Size.x], Size.x}; }
		view<const T>		operator[]		(u2_t row)				const				{ if_true_te(0 == Data); if_true_tef(row >= Size.y, "Invalid row: %i.", (s2_t)row); return {&Data[row * Size.x], Size.x}; }
		T&							operator[]		(n2u2_c & cell)						{ if_true_te(0 == Data); if_true_tef(cell.y >= Size.y, "Invalid row: %i.", (s2_t)cell.y); if_true_tef(cell.x >= Size.x, "Invalid column: %i.", (s2_t)cell.x); return Data[cell.y * Size.x + cell.x]; }
		const T&					operator[]		(n2u2_c & cell)	const				{ if_true_te(0 == Data); if_true_tef(cell.y >= Size.y, "Invalid row: %i.", (s2_t)cell.y); if_true_tef(cell.x >= Size.x, "Invalid column: %i.", (s2_t)cell.x); return Data[cell.y * Size.x + cell.x]; }

		inxp	const T*			begin			()		const	nxpt	{ return Data; }
		inxp	const T*			end				()		const	nxpt	{ return Data + size(); }

		inxp	T*					begin			()				nxpt	{ return Data; }
		inxp	T*					end				()				nxpt	{ return Data + size(); }

		inxp	n2u2_c&		metrics			()		const	nxpt	{ return Size; }
		inxp	n2u1_t		metrics16		()		const	nxpt	{ return Size.u1(); }
		inxp	u2_t		size			()		const	nxpt	{ return area(); }
		inxp	u2_t 		byte_count		()		const	nxpt	{ return area() * sizeof(T); }
		inxp	u2_t		area			()		const	nxpt	{ return Size.x * Size.y; }

		inln	err_t 	fill			(const T & value, u2_t offset = 0, u2_t count = 0xFFFFFFFFU) {
			if_true_vef(-1, (count > size()) && (count != 0xFFFFFFFFU), "Count: %u", count);
			for(u2_t i = offset, actualCount = min(size(), count); i < actualCount; ++i)
				Data[i]						= value;
			return 0;
		}
	};

#pragma pack(pop)

	tplt<tpnm TDim, tpnm TColor>
	static	err_t 	drawRectangle			(grid<TColor> & target, const TColor & value, const rect2<TDim> & rectangle)		{
		s2_c						xCount					= min((s2_t)target.metrics().x - max((s2_t)rectangle.Offset.x, (s2_t)0), (s2_t)rectangle.Size.x + min((s2_t)rectangle.Offset.x, (s2_t)0));
		s2_c						yCount					= min((s2_t)target.metrics().y - max((s2_t)rectangle.Offset.y, (s2_t)0), (s2_t)rectangle.Size.y + min((s2_t)rectangle.Offset.y, (s2_t)0));
		s2_c						yStart					= (s2_t)max((s2_t)0, (s2_t)rectangle.Offset.y);
		s2_c						xStart					= (s2_t)max((s2_t)0, (s2_t)rectangle.Offset.x);
		if(xStart >= (s2_t)target.metrics().x || yStart >= (s2_t)target.metrics().y)
			return 0;
		if(xCount <= 0 || yCount <= 0)
			return 0;

		view<TColor>				firstRow				= target[yStart];
		if cxpr(1 == sizeof(TColor))
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
	tydf	grid<sc_t>		gsc_t;	tdcs gsc_t	gsc_c; 	
	tydf	grid<uc_t>		guc_t;	tdcs guc_t	guc_c; 	
	tydf	grid<f2_t>		gf2_t;	tdcs gf2_t	gf2_c; 	
	tydf	grid<f3_t>		gf3_t;	tdcs gf3_t	gf3_c; 	
	tydf	grid<u0_t>		gu0_t;	tdcs gu0_t	gu0_c; 	
	tydf	grid<u1_t>		gu1_t;	tdcs gu1_t	gu1_c; 	
	tydf	grid<u2_t>		gu2_t;	tdcs gu2_t	gu2_c; 	
	tydf	grid<u3_t>		gu3_t;	tdcs gu3_t	gu3_c; 	
	tydf	grid<s0_t>		gs0_t;	tdcs gs0_t	gs0_c; 	
	tydf	grid<s1_t>		gs1_t;	tdcs gs1_t	gs1_c; 	
	tydf	grid<s2_t>		gs2_t;	tdcs gs2_t	gs2_c; 	
	tydf	grid<s3_t>		gs3_t;	tdcs gs3_t	gs3_c; 	
	tydf	grid<n2sc_t>	g2sc_t;	tdcs g2sc_t	g2sc_c; 	
	tydf	grid<n2uc_t>	g2uc_t;	tdcs g2uc_t	g2uc_c; 	
	tydf	grid<n2f2_t>	g2f2_t;	tdcs g2f2_t	g2f2_c; 	
	tydf	grid<n2f3_t>	g2f3_t;	tdcs g2f3_t	g2f3_c; 	
	tydf	grid<n2u0_t>	g2u0_t;	tdcs g2u0_t	g2u0_c; 	
	tydf	grid<n2u1_t>	g2u1_t;	tdcs g2u1_t	g2u1_c; 	
	tydf	grid<n2u2_t>	g2u2_t;	tdcs g2u2_t	g2u2_c; 	
	tydf	grid<n2u3_t>	g2u3_t;	tdcs g2u3_t	g2u3_c; 	
	tydf	grid<n2s0_t>	g2s0_t;	tdcs g2s0_t	g2s0_c; 	
	tydf	grid<n2s1_t>	g2s1_t;	tdcs g2s1_t	g2s1_c; 	
	tydf	grid<n2s2_t>	g2s2_t;	tdcs g2s2_t	g2s2_c; 	
	tydf	grid<n2s3_t>	g2s3_t;	tdcs g2s3_t	g2s3_c; 	
	tydf	grid<n3sc_t>	g3sc_t;	tdcs g3sc_t	g3sc_c; 	
	tydf	grid<n3uc_t>	g3uc_t;	tdcs g3uc_t	g3uc_c; 	
	tydf	grid<n3f2_t>	g3f2_t;	tdcs g3f2_t	g3f2_c; 	
	tydf	grid<n3f3_t>	g3f3_t;	tdcs g3f3_t	g3f3_c; 	
	tydf	grid<n3u0_t>	g3u0_t;	tdcs g3u0_t	g3u0_c; 	
	tydf	grid<n3u1_t>	g3u1_t;	tdcs g3u1_t	g3u1_c; 	
	tydf	grid<n3u2_t>	g3u2_t;	tdcs g3u2_t	g3u2_c; 	
	tydf	grid<n3u3_t>	g3u3_t;	tdcs g3u3_t	g3u3_c; 	
	tydf	grid<n3s0_t>	g3s0_t;	tdcs g3s0_t	g3s0_c; 	
	tydf	grid<n3s1_t>	g3s1_t;	tdcs g3s1_t	g3s1_c; 	
	tydf	grid<n3s2_t>	g3s2_t;	tdcs g3s2_t	g3s2_c; 	
	tydf	grid<n3s3_t>	g3s3_t;	tdcs g3s3_t	g3s3_c; 	

	GDEFINE_ENUM_TYPE(GRID_ROTATION, u0_t);
	GDEFINE_ENUM_VALUE(GRID_ROTATION, DEFAULT	, 0x0);
	GDEFINE_ENUM_VALUE(GRID_ROTATION, 0			, 0x1);
	GDEFINE_ENUM_VALUE(GRID_ROTATION, 90		, 0x2);
	GDEFINE_ENUM_VALUE(GRID_ROTATION, 180		, 0x4);
	GDEFINE_ENUM_VALUE(GRID_ROTATION, 270		, 0x8);
}

#define GPK_USING_GRID()																			\
	usng	::gpk::gsc_t, ::gpk::gsc_c, ::gpk::g2sc_t, ::gpk::g2sc_c, ::gpk::g3sc_t, ::gpk::g3sc_c	\
		,	::gpk::guc_t, ::gpk::guc_c, ::gpk::g2uc_t, ::gpk::g2uc_c, ::gpk::g3uc_t, ::gpk::g3uc_c	\
		,	::gpk::gf2_t, ::gpk::gf2_c, ::gpk::g2f2_t, ::gpk::g2f2_c, ::gpk::g3f2_t, ::gpk::g3f2_c	\
		,	::gpk::gf3_t, ::gpk::gf3_c, ::gpk::g2f3_t, ::gpk::g2f3_c, ::gpk::g3f3_t, ::gpk::g3f3_c	\
		,	::gpk::gu0_t, ::gpk::gu0_c, ::gpk::g2u0_t, ::gpk::g2u0_c, ::gpk::g3u0_t, ::gpk::g3u0_c	\
		,	::gpk::gu1_t, ::gpk::gu1_c, ::gpk::g2u1_t, ::gpk::g2u1_c, ::gpk::g3u1_t, ::gpk::g3u1_c	\
		,	::gpk::gu2_t, ::gpk::gu2_c, ::gpk::g2u2_t, ::gpk::g2u2_c, ::gpk::g3u2_t, ::gpk::g3u2_c	\
		,	::gpk::gu3_t, ::gpk::gu3_c, ::gpk::g2u3_t, ::gpk::g2u3_c, ::gpk::g3u3_t, ::gpk::g3u3_c	\
		,	::gpk::gs0_t, ::gpk::gs0_c, ::gpk::g2s0_t, ::gpk::g2s0_c, ::gpk::g3s0_t, ::gpk::g3s0_c	\
		,	::gpk::gs1_t, ::gpk::gs1_c, ::gpk::g2s1_t, ::gpk::g2s1_c, ::gpk::g3s1_t, ::gpk::g3s1_c	\
		,	::gpk::gs2_t, ::gpk::gs2_c, ::gpk::g2s2_t, ::gpk::g2s2_c, ::gpk::g3s2_t, ::gpk::g3s2_c	\
		,	::gpk::gs3_t, ::gpk::gs3_c, ::gpk::g2s3_t, ::gpk::g2s3_c, ::gpk::g3s3_t, ::gpk::g3s3_c


#endif // GPK_GRID_VIEW_H_23627
