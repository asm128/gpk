#include "gpk_view.h"		// for gpk::view<>
#include "gpk_rect2.h"
#include "gpk_n3.h"

#ifndef GPK_GRID_VIEW_H_3423423
#define GPK_GRID_VIEW_H_3423423

namespace gpk
{
#pragma pack(push, 1)
	template <typename _tCell>
	class grid {
	protected:
		// Properties / Member Variables
		_tCell						* Data			= 0;
		::gpk::n2u32				Size			= {0, 0};
	public:
		typedef	_tCell				T;
		// Constructors
		inlcxpr						grid			()									noexcept	= default;
		inline						grid			(T* data, uint32_t width, uint32_t height)		: Data{data}, Size{width, height} {
			gthrow_if(0 == data && Size.x && Size.y, "Invalid parameters. Size: {x:%u, y:%u}, pointer: %p.", width, height, data);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}
		inline						grid			(T* data, const ::gpk::n2u32 & gridMetrics)		: grid(data, gridMetrics.x, gridMetrics.y) {}
		template<size_t _tWidth, size_t _tHeight>
		inlcxpr						grid			(T (&data)[_tHeight][_tWidth])		noexcept	: Data{&data[0][0]}, Size{_tWidth, _tHeight} {}

		inlcxpr	operator			grid<const T>	()							const	noexcept	{ return {Data, Size}; }
		// Operators
		::gpk::view<T>				operator[]		(uint32_t row)									{ gsthrow_if(0 == Data); gthrow_if(row >= Size.y, "Invalid row: %i.", (int32_t)row); return {&Data[row * Size.x], Size.x}; }
		::gpk::view<const T>		operator[]		(uint32_t row)				const				{ gsthrow_if(0 == Data); gthrow_if(row >= Size.y, "Invalid row: %i.", (int32_t)row); return {&Data[row * Size.x], Size.x}; }
		T&							operator[]		(const ::gpk::n2u32 & cell)						{ gsthrow_if(0 == Data); gthrow_if(cell.y >= Size.y, "Invalid row: %i.", (int32_t)cell.y); gthrow_if(cell.x >= Size.x, "Invalid column: %i.", (int32_t)cell.x); return Data[cell.y * Size.x + cell.x]; }
		const T&					operator[]		(const ::gpk::n2u32 & cell)	const				{ gsthrow_if(0 == Data); gthrow_if(cell.y >= Size.y, "Invalid row: %i.", (int32_t)cell.y); gthrow_if(cell.x >= Size.x, "Invalid column: %i.", (int32_t)cell.x); return Data[cell.y * Size.x + cell.x]; }

		inlcxpr	const T*			begin			()		const	noexcept	{ return Data; }
		inlcxpr	const T*			end				()		const	noexcept	{ return Data + size(); }

		inlcxpr	T*					begin			()				noexcept	{ return Data; }
		inlcxpr	T*					end				()				noexcept	{ return Data + size(); }

		inlcxpr	const ::gpk::n2u32&	metrics			()		const	noexcept	{ return Size; }
		inlcxpr	::gpk::n2u16		metrics16		()		const	noexcept	{ return Size.u16(); }
		inlcxpr	uint32_t			size			()		const	noexcept	{ return area(); }
		inlcxpr	uint32_t 			byte_count		()		const	noexcept	{ return area() * sizeof(T); }
		inlcxpr	uint32_t			area			()		const	noexcept	{ return Size.x * Size.y; }

		inline	::gpk::error_t		fill			(const T & value, uint32_t offset = 0, uint32_t count = 0xFFFFFFFFU) {
			ree_if((count > size()) && (count != 0xFFFFFFFFU), "Count: %u", count);
			for(uint32_t i = offset, actualCount = ::gpk::min(size(), count); i < actualCount; ++i)
				Data[i]						= value;
			return 0;
		}
	};

#pragma pack(pop)

	template<typename TDim, typename TColor>
	static	::gpk::error_t		drawRectangle			(::gpk::grid<TColor> & target, const TColor & value, const ::gpk::rect2<TDim> & rectangle)		{
		const int32_t					xCount					= ::gpk::min((int32_t)target.metrics().x - ::gpk::max((int32_t)rectangle.Offset.x, (int32_t)0), (int32_t)rectangle.Size.x + ::gpk::min((int32_t)rectangle.Offset.x, (int32_t)0));
		const int32_t					yCount					= ::gpk::min((int32_t)target.metrics().y - ::gpk::max((int32_t)rectangle.Offset.y, (int32_t)0), (int32_t)rectangle.Size.y + ::gpk::min((int32_t)rectangle.Offset.y, (int32_t)0));
		const int32_t					yStart					= (int32_t)::gpk::max((int32_t)0, (int32_t)rectangle.Offset.y);
		const int32_t					xStart					= (int32_t)::gpk::max((int32_t)0, (int32_t)rectangle.Offset.x);
		if(xStart >= (int32_t)target.metrics().x || yStart >= (int32_t)target.metrics().y)
			return 0;
		if(xCount <= 0 || yCount <= 0)
			return 0;

		::gpk::view<TColor>				firstRow				= target[yStart];
		if constexpr(1 == sizeof(TColor))
			memset(&firstRow[xStart], *((uint8_t*)&value), xCount);
		else {
			for(int32_t x = 0; x < xCount; ++x)
				firstRow[xStart + x]		= value;
		}
		for(int32_t y = 1; y < yCount; ++y)
			memcpy(&target[yStart + y][xStart], &target[yStart][xStart], xCount * sizeof(TColor));

		return 0;
	}

	// view common typedefs
	typedef	::gpk::grid<char	>				gc		, gchar;
	typedef	::gpk::grid<uchar_t	>				guc		, guchar;
	typedef	::gpk::grid<uint8_t	>				gub		;
	typedef	::gpk::grid<int8_t	>				gb		;
	typedef	::gpk::grid<float	>				gf32	;
	typedef	::gpk::grid<double	>				gf64	;
	typedef	::gpk::grid<uint8_t	>				gu8		;
	typedef	::gpk::grid<uint16_t>				gu16	;
	typedef	::gpk::grid<uint32_t>				gu32	;
	typedef	::gpk::grid<uint64_t>				gu64	;
	typedef	::gpk::grid<int8_t	>				gi8		;
	typedef	::gpk::grid<int16_t	>				gi16	;
	typedef	::gpk::grid<int32_t	>				gi32	;
	typedef	::gpk::grid<int64_t	>				gi64	;
	typedef	::gpk::grid<const ::gpk::n2<char>>	g2c		;
	typedef	::gpk::grid<const ::gpk::n2u8>		g2ub	;
	typedef	::gpk::grid<const ::gpk::n2i8>		g2b		;
	typedef	::gpk::grid<const ::gpk::n2f32>		g2f32	;
	typedef	::gpk::grid<const ::gpk::n2f64>		g2f64	;
	typedef	::gpk::grid<const ::gpk::n2u8>		g2ui8	;
	typedef	::gpk::grid<const ::gpk::n2u16>		g2ui16	;
	typedef	::gpk::grid<const ::gpk::n2u32>		g2ui32	;
	typedef	::gpk::grid<const ::gpk::n2u64>		g2ui64	;
	typedef	::gpk::grid<const ::gpk::n2i8>		g2i8	;
	typedef	::gpk::grid<const ::gpk::n2i16>		g2i16	;
	typedef	::gpk::grid<const ::gpk::n2i32>		g2i32	;
	typedef	::gpk::grid<const ::gpk::n2i64>		g2i64	;


	typedef	::gpk::grid<::gpk::n2<char>>		gn2c	;
	typedef	::gpk::grid<::gpk::n2u8>			gn2ub	;
	typedef	::gpk::grid<::gpk::n2i8>			gn2b	;
	typedef	::gpk::grid<::gpk::n2f32>			gn2f32	;
	typedef	::gpk::grid<::gpk::n2f64>			gn2f64	;
	typedef	::gpk::grid<::gpk::n2u8>			gn2ui8	;
	typedef	::gpk::grid<::gpk::n2u16>			gn2ui16	;
	typedef	::gpk::grid<::gpk::n2u32>			gn2ui32	;
	typedef	::gpk::grid<::gpk::n2u64>			gn2ui64	;
	typedef	::gpk::grid<::gpk::n2i8>			gn2i8	;
	typedef	::gpk::grid<::gpk::n2i16>			gn2i16	;
	typedef	::gpk::grid<::gpk::n2i32>			gn2i32	;
	typedef	::gpk::grid<::gpk::n2i64>			gn2i64	;
	typedef	::gpk::grid<const ::gpk::n2<char>>	gc2c	;
	typedef	::gpk::grid<const ::gpk::n2u8>		gc2ub	;
	typedef	::gpk::grid<const ::gpk::n2i8>		gc2b	;
	typedef	::gpk::grid<const ::gpk::n2f32>		gc2f32	;
	typedef	::gpk::grid<const ::gpk::n2f64>		gc2f64	;
	typedef	::gpk::grid<const ::gpk::n2u8>		gc2ui8	;
	typedef	::gpk::grid<const ::gpk::n2u16>		gc2ui16	;
	typedef	::gpk::grid<const ::gpk::n2u32>		gc2ui32	;
	typedef	::gpk::grid<const ::gpk::n2u64>		gc2ui64	;
	typedef	::gpk::grid<const ::gpk::n2i8>		gc2i8	;
	typedef	::gpk::grid<const ::gpk::n2i16>		gc2i16	;
	typedef	::gpk::grid<const ::gpk::n2i32>		gc2i32	;
	typedef	::gpk::grid<const ::gpk::n2i64>		gc2i64	;


	typedef	::gpk::grid<::gpk::n3<char>>		gn3c	;
	typedef	::gpk::grid<::gpk::n3u8>			gn3ub	;
	typedef	::gpk::grid<::gpk::n3i8>			gn3b	;
	typedef	::gpk::grid<::gpk::n3f32>			gn3f32	;
	typedef	::gpk::grid<::gpk::n3f64>			gn3f64	;
	typedef	::gpk::grid<::gpk::n3u8>			gn3ui8	;
	typedef	::gpk::grid<::gpk::n3u16>			gn3ui16	;
	typedef	::gpk::grid<::gpk::n3u32>			gn3ui32	;
	typedef	::gpk::grid<::gpk::n3u64>			gn3ui64	;
	typedef	::gpk::grid<::gpk::n3i8>			gn3i8	;
	typedef	::gpk::grid<::gpk::n3i16>			gn3i16	;
	typedef	::gpk::grid<::gpk::n3i32>			gn3i32	;
	typedef	::gpk::grid<::gpk::n3i64>			gn3i64	;
	typedef	::gpk::grid<const ::gpk::n3<char>>	gc3c	;
	typedef	::gpk::grid<const ::gpk::n3u8>		gc3ub	;
	typedef	::gpk::grid<const ::gpk::n3i8>		gc3b	;
	typedef	::gpk::grid<const ::gpk::n3f32>		gc3f32	;
	typedef	::gpk::grid<const ::gpk::n3f64>		gc3f64	;
	typedef	::gpk::grid<const ::gpk::n3u8>		gc3ui8	;
	typedef	::gpk::grid<const ::gpk::n3u16>		gc3ui16	;
	typedef	::gpk::grid<const ::gpk::n3u32>		gc3ui32	;
	typedef	::gpk::grid<const ::gpk::n3u64>		gc3ui64	;
	typedef	::gpk::grid<const ::gpk::n3i8>		gc3i8	;
	typedef	::gpk::grid<const ::gpk::n3i16>		gc3i16	;
	typedef	::gpk::grid<const ::gpk::n3i32>		gc3i32	;
	typedef	::gpk::grid<const ::gpk::n3i64>		gc3i64	;

	enum GRID_ROTATION : uint8_t {
		GRID_ROTATION_DEFAULT	= 0,
		GRID_ROTATION_0			= 0x1,
		GRID_ROTATION_90		= 0x2,
		GRID_ROTATION_180		= 0x4,
		GRID_ROTATION_270		= 0x8,
	};
}

#endif // GPK_GRID_VIEW_H_3423423
