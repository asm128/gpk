#include "gpk_view.h"		// for gpk::view<>
#include "gpk_rect2.h"
#include "gpk_n3.h"

#ifndef GPK_GRID_VIEW_H_3423423
#define GPK_GRID_VIEW_H_3423423

namespace gpk
{
#pragma pack(push, 1)
	template <typename _tElement>
	class view2d {
	protected:
		// Properties / Member Variables
		_tElement					* Data					= 0;
		::gpk::n2u32				Size					= {0, 0};
	public:
		typedef	_tElement			T;
		// Constructors
		inlcxpr						view2d					()																			noexcept	= default;
		inline						view2d					(_tElement* dataElements, uint32_t gridWidth, uint32_t gridHeight)						: Data{dataElements}, Size{gridWidth, gridHeight}			{
			gthrow_if(0 == dataElements && 0 != Size.x && 0 != Size.y, "Invalid parameters. Size: %u, %u, pointer: %p.", gridWidth, gridHeight, dataElements);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}
		inline						view2d					(_tElement* dataElements, const ::gpk::n2<uint32_t>& gridMetrics)						: view2d(dataElements, gridMetrics.x, gridMetrics.y)		{}
		template<size_t _tWidth, size_t _tHeight>
		inlcxpr						view2d					(_tElement (&dataElements)[_tHeight][_tWidth])											: Data{&dataElements[0][0]}, Size{_tWidth, _tHeight}				{}

		inlcxpr	operator			view2d<const _tElement>	()																	const	noexcept	{ return {Data, Size}; }
		// Operators
		::gpk::view<T>				operator[]				(uint32_t row)																			{ gthrow_if(0 == Data, "Uninitialized array pointer. Invalid row: %u.", row); gthrow_if(row >= Size.y, "Invalid row: %i.", (int32_t)row); return ::gpk::v1<_tElement		>(&Data[row * Size.x], Size.x); }
		::gpk::view<const T>		operator[]				(uint32_t row)														const				{ gthrow_if(0 == Data, "Uninitialized array pointer. Invalid row: %u.", row); gthrow_if(row >= Size.y, "Invalid row: %i.", (int32_t)row); return ::gpk::v1<const _tElement	>(&Data[row * Size.x], Size.x); }
		T&							operator[]				(const ::gpk::n2<uint32_t> & cell)														{ gthrow_if(0 == Data, "Uninitialized array pointer. Invalid cell: {%u, %u}.", cell.x, cell.y); gthrow_if(cell.y >= Size.y, "Invalid row: %i.", (int32_t)cell.y); gthrow_if(cell.x >= Size.x, "Invalid column: %i.", (int32_t)cell.x); return Data[cell.y * Size.x + cell.x]; }
		const T&					operator[]				(const ::gpk::n2<uint32_t> & cell)									const				{ gthrow_if(0 == Data, "Uninitialized array pointer. Invalid cell: {%u, %u}.", cell.x, cell.y); gthrow_if(cell.y >= Size.y, "Invalid row: %i.", (int32_t)cell.y); gthrow_if(cell.x >= Size.x, "Invalid column: %i.", (int32_t)cell.x); return Data[cell.y * Size.x + cell.x]; }

		// Methods
#if !defined(GPK_ATMEL)
		inlcxpr	const T*			begin					()																	const	noexcept	{ return Data;							}
		inlcxpr	const T*			end						()																	const	noexcept	{ return Data + size();					}
#endif
		inlcxpr	T*					begin					()																			noexcept	{ return Data;							}
		inlcxpr	T*					end						()																			noexcept	{ return Data + size();					}

		inlcxpr	const ::gpk::n2u32&	metrics					()																	const	noexcept	{ return Size;							}
		inlcxpr	uint32_t			size					()																	const	noexcept	{ return area();						}
		inlcxpr	uint32_t 			byte_count				()																	const	noexcept	{ return area() * sizeof(_tElement);	}
		inlcxpr	uint32_t			area					()																	const	noexcept	{ return Size.x * Size.y;				}

		inline	::gpk::error_t		fill					(const _tElement & value, uint32_t offset = 0, uint32_t count = 0xFFFFFFFFU) {
			ree_if((count > size()) && (count != 0xFFFFFFFFU), "Count: %u", count);
			for(uint32_t i = offset, actualCount = ::gpk::min(size(), count); i < actualCount; ++i)
				Data[i]						= value;
			return 0;
		}
	};

	template<typename T>	using grid			= ::gpk::view2d<T>;
	template<typename T>	using view_grid		= ::gpk::view2d<T>;
	template<typename T>	using v2			= ::gpk::view2d<T>;

#pragma pack(pop)

	template<typename T, typename _tColor>
	static	::gpk::error_t		drawRectangle			(::gpk::view2d<_tColor>& target, const _tColor& value, const ::gpk::rect2<T>& rectangle)		{
		const int32_t					xCount					= ::gpk::min((int32_t)target.metrics().x - ::gpk::max((int32_t)rectangle.Offset.x, (int32_t)0), (int32_t)rectangle.Size.x + ::gpk::min((int32_t)rectangle.Offset.x, (int32_t)0));
		const int32_t					yCount					= ::gpk::min((int32_t)target.metrics().y - ::gpk::max((int32_t)rectangle.Offset.y, (int32_t)0), (int32_t)rectangle.Size.y + ::gpk::min((int32_t)rectangle.Offset.y, (int32_t)0));
		const int32_t					yStart					= (int32_t)::gpk::max((int32_t)0, (int32_t)rectangle.Offset.y);
		const int32_t					xStart					= (int32_t)::gpk::max((int32_t)0, (int32_t)rectangle.Offset.x);
		if(xStart >= (int32_t)target.metrics().x || yStart >= (int32_t)target.metrics().y)
			return 0;
		if(xCount <= 0 || yCount <= 0)
			return 0;

		::gpk::v1<_tColor>				firstRow				= target[yStart];
		if constexpr(1 == sizeof(_tColor))
			memset(&firstRow[xStart], *((uint8_t*)&value), xCount);
		else {
			for(int32_t x = 0; x < xCount; ++x)
				firstRow[xStart + x]		= value;
		}
		for(int32_t y = 1; y < yCount; ++y)
			memcpy(&target[yStart + y][xStart], &target[yStart][xStart], xCount * sizeof(_tColor));

		return 0;
	}


	// view common typedefs
	typedef	::gpk::view2d<	char	>				v2c			;
	typedef	::gpk::view2d<	uint8_t	>				v2ub		;
	typedef	::gpk::view2d<	int8_t	>				v2b			;
	typedef	::gpk::view2d<	float	>				v2f32		, v2f;
	typedef	::gpk::view2d<	double	>				v2f64		;
	typedef	::gpk::view2d<	uint8_t	>				v2u8		;
	typedef	::gpk::view2d<	uint16_t>				v2u16		;
	typedef	::gpk::view2d<	uint32_t>				v2u32		;
	typedef	::gpk::view2d<	uint64_t>				v2u64		;
	typedef	::gpk::view2d<	int8_t	>				v2i8		;
	typedef	::gpk::view2d<	int16_t	>				v2i16		;
	typedef	::gpk::view2d<	int32_t	>				v2i32		;
	typedef	::gpk::view2d<	int64_t	>				v2i64		;
	typedef	::gpk::view2d<const	char		>		v2cc	;
	typedef	::gpk::view2d<const	uint8_t		>		v2cub	;
	typedef	::gpk::view2d<const	int8_t		>		v2cb	;
	typedef	::gpk::view2d<const	float		>		v2cf32	, v2cf;
	typedef	::gpk::view2d<const	double		>		v2cf64	;
	typedef	::gpk::view2d<const	uint8_t		>		v2cui8	;
	typedef	::gpk::view2d<const	uint16_t	>		v2cui16	;
	typedef	::gpk::view2d<const	uint32_t	>		v2cui32	;
	typedef	::gpk::view2d<const	uint64_t	>		v2cui64	;
	typedef	::gpk::view2d<const	int8_t		>		v2ci8	;
	typedef	::gpk::view2d<const	int16_t		>		v2ci16	;
	typedef	::gpk::view2d<const	int32_t		>		v2ci32	;
	typedef	::gpk::view2d<const	int64_t		>		v2ci64	;

	// n2
	typedef	::gpk::view<::gpk::n2<char		>>		v1n2c		;
	typedef	::gpk::view<::gpk::n2<uint8_t	>>		v1n2ub		;
	typedef	::gpk::view<::gpk::n2<int8_t	>>		v1n2b		;
	typedef	::gpk::view<::gpk::n2f32		>		v1n2f32		, v1n2f;
	typedef	::gpk::view<::gpk::n2f64		>		v1n2f64		;
	typedef	::gpk::view<::gpk::n2u8			>		v1n2ui8		;
	typedef	::gpk::view<::gpk::n2u16		>		v1n2ui16	;
	typedef	::gpk::view<::gpk::n2u32		>		v1n2ui32	;
	typedef	::gpk::view<::gpk::n2u64		>		v1n2ui64	;
	typedef	::gpk::view<::gpk::n2i8			>		v1n2i8		;
	typedef	::gpk::view<::gpk::n2i16		>		v1n2i16		;
	typedef	::gpk::view<::gpk::n2i32		>		v1n2i32		;
	typedef	::gpk::view<::gpk::n2i64		>		v1n2i64		;
	typedef	::gpk::view<const ::gpk::n2<char>	>	v1c2c	;
	typedef	::gpk::view<const ::gpk::n2u8		>	v1c2ub	;
	typedef	::gpk::view<const ::gpk::n2i8		>	v1c2b	;
	typedef	::gpk::view<const ::gpk::n2f32		>	v1c2f32	, v1c2f;
	typedef	::gpk::view<const ::gpk::n2f64		>	v1c2f64	;
	typedef	::gpk::view<const ::gpk::n2u8		>	v1c2ui8	;
	typedef	::gpk::view<const ::gpk::n2u16		>	v1c2ui16;
	typedef	::gpk::view<const ::gpk::n2u32		>	v1c2ui32;
	typedef	::gpk::view<const ::gpk::n2u64		>	v1c2ui64;
	typedef	::gpk::view<const ::gpk::n2i8		>	v1c2i8	;
	typedef	::gpk::view<const ::gpk::n2i16		>	v1c2i16	;
	typedef	::gpk::view<const ::gpk::n2i32		>	v1c2i32	;
	typedef	::gpk::view<const ::gpk::n2i64		>	v1c2i64	;
	typedef	::gpk::view2d<::gpk::n2<char	>>		v2n2c		;
	typedef	::gpk::view2d<::gpk::n2<uint8_t	>>		v2n2ub		;
	typedef	::gpk::view2d<::gpk::n2<int8_t	>>		v2n2b		;
	typedef	::gpk::view2d<::gpk::n2f32		>		v2n2f32		, v2n2f;
	typedef	::gpk::view2d<::gpk::n2f64		>		v2n2f64		;
	typedef	::gpk::view2d<::gpk::n2u8		>		v2n2ui8		;
	typedef	::gpk::view2d<::gpk::n2u16		>		v2n2ui16	;
	typedef	::gpk::view2d<::gpk::n2u32		>		v2n2ui32	;
	typedef	::gpk::view2d<::gpk::n2u64		>		v2n2ui64	;
	typedef	::gpk::view2d<::gpk::n2i8		>		v2n2i8		;
	typedef	::gpk::view2d<::gpk::n2i16		>		v2n2i16		;
	typedef	::gpk::view2d<::gpk::n2i32		>		v2n2i32		;
	typedef	::gpk::view2d<::gpk::n2i64		>		v2n2i64		;
	typedef	::gpk::view2d<const	::gpk::n2<char>>	v2c2c	;
	typedef	::gpk::view2d<const	::gpk::n2u8	>		v2c2ub	;
	typedef	::gpk::view2d<const	::gpk::n2i8	>		v2c2b	;
	typedef	::gpk::view2d<const	::gpk::n2f32>		v2c2f32	, v2c2f;
	typedef	::gpk::view2d<const	::gpk::n2f64>		v2c2f64	;
	typedef	::gpk::view2d<const	::gpk::n2u8	>		v2c2ui8	;
	typedef	::gpk::view2d<const	::gpk::n2u16>		v2c2ui16;
	typedef	::gpk::view2d<const	::gpk::n2u32>		v2c2ui32;
	typedef	::gpk::view2d<const	::gpk::n2u64>		v2c2ui64;
	typedef	::gpk::view2d<const	::gpk::n2i8	>		v2c2i8	;
	typedef	::gpk::view2d<const	::gpk::n2i16>		v2c2i16	;
	typedef	::gpk::view2d<const	::gpk::n2i32>		v2c2i32	;
	typedef	::gpk::view2d<const	::gpk::n2i64>		v2c2i64	;

	// n3
	typedef	::gpk::view<::gpk::n3<char			>>	v1n3c		;
	typedef	::gpk::view<::gpk::n3<uint8_t		>>	v1n3ub		;
	typedef	::gpk::view<::gpk::n3<int8_t		>>	v1n3b		;
	typedef	::gpk::view<::gpk::n3f32			>	v1n3f32		, v1n3f;
	typedef	::gpk::view<::gpk::n3f64			>	v1n3f64		;
	typedef	::gpk::view<::gpk::n3u8				>	v1n3ui8		;
	typedef	::gpk::view<::gpk::n3u16			>	v1n3ui16	;
	typedef	::gpk::view<::gpk::n3u32			>	v1n3ui32	;
	typedef	::gpk::view<::gpk::n3u64			>	v1n3ui64	;
	typedef	::gpk::view<::gpk::n3i8				>	v1n3i8		;
	typedef	::gpk::view<::gpk::n3i16			>	v1n3i16		;
	typedef	::gpk::view<::gpk::n3i32			>	v1n3i32		;
	typedef	::gpk::view<::gpk::n3i64			>	v1n3i64		;
	typedef	::gpk::view<const ::gpk::n3<char	>>	v1c3c	;
	typedef	::gpk::view<const ::gpk::n3<uint8_t	>>	v1c3ub	;
	typedef	::gpk::view<const ::gpk::n3<int8_t	>>	v1c3b	;
	typedef	::gpk::view<const ::gpk::n3f32		>	v1c3f32	, v1c3f;
	typedef	::gpk::view<const ::gpk::n3f64		>	v1c3f64	;
	typedef	::gpk::view<const ::gpk::n3u8		>	v1c3ui8	;
	typedef	::gpk::view<const ::gpk::n3u16		>	v1c3ui16;
	typedef	::gpk::view<const ::gpk::n3u32		>	v1c3ui32;
	typedef	::gpk::view<const ::gpk::n3u64		>	v1c3ui64;
	typedef	::gpk::view<const ::gpk::n3i8		>	v1c3i8	;
	typedef	::gpk::view<const ::gpk::n3i16		>	v1c3i16	;
	typedef	::gpk::view<const ::gpk::n3i32		>	v1c3i32	;
	typedef	::gpk::view<const ::gpk::n3i64		>	v1c3i64	;
	typedef	::gpk::view2d<::gpk::n3<char		>>	v2n3c		;
	typedef	::gpk::view2d<::gpk::n3<uint8_t		>>	v2n3ub		;
	typedef	::gpk::view2d<::gpk::n3<int8_t		>>	v2n3b		;
	typedef	::gpk::view2d<::gpk::n3f32			>	v2n3f32		, v2n3f;
	typedef	::gpk::view2d<::gpk::n3f64			>	v2n3f64		;
	typedef	::gpk::view2d<::gpk::n3u8			>	v2n3ui8		;
	typedef	::gpk::view2d<::gpk::n3u16			>	v2n3ui16	;
	typedef	::gpk::view2d<::gpk::n3u32			>	v2n3ui32	;
	typedef	::gpk::view2d<::gpk::n3u64			>	v2n3ui64	;
	typedef	::gpk::view2d<::gpk::n3i8			>	v2n3i8		;
	typedef	::gpk::view2d<::gpk::n3i16			>	v2n3i16		;
	typedef	::gpk::view2d<::gpk::n3i32			>	v2n3i32		;
	typedef	::gpk::view2d<::gpk::n3i64			>	v2n3i64		;
	typedef	::gpk::view2d<const	::gpk::n3<char>	>	v2c3c	;
	typedef	::gpk::view2d<const	::gpk::n3u8>		v2c3ub	;
	typedef	::gpk::view2d<const	::gpk::n3i8>		v2c3b	;
	typedef	::gpk::view2d<const	::gpk::n3f32>		v2c3f32	, v2c3f;
	typedef	::gpk::view2d<const	::gpk::n3f64>		v2c3f64	;
	typedef	::gpk::view2d<const	::gpk::n3u8	>		v2c3ui8	;
	typedef	::gpk::view2d<const	::gpk::n3u16>		v2c3ui16;
	typedef	::gpk::view2d<const	::gpk::n3u32>		v2c3ui32;
	typedef	::gpk::view2d<const	::gpk::n3u64>		v2c3ui64;
	typedef	::gpk::view2d<const	::gpk::n3i8	>		v2c3i8	;
	typedef	::gpk::view2d<const	::gpk::n3i16>		v2c3i16	;
	typedef	::gpk::view2d<const	::gpk::n3i32>		v2c3i32	;
	typedef	::gpk::view2d<const	::gpk::n3i64>		v2c3i64	;

	enum GRID_ROTATION : uint8_t {
		GRID_ROTATION_DEFAULT	= 0,
		GRID_ROTATION_0			= 0x1,
		GRID_ROTATION_90		= 0x2,
		GRID_ROTATION_180		= 0x4,
		GRID_ROTATION_270		= 0x8,
	};
}

#endif // GPK_GRID_VIEW_H_3423423
