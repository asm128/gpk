#include "gpk_view_array.h"		// for gpk::view_array<>
#include "gpk_coord.h"

#ifndef GPK_GRID_VIEW_H_3423423
#define GPK_GRID_VIEW_H_3423423

namespace gpk
{
#pragma pack(push, 1)
	template <typename _tElement>
	class view_grid {
	protected:
		// Properties / Member Variables
							_tElement								* Data						= 0;
							::gpk::SCoord2<uint32_t>				Size						= {0, 0};
	public:
		typedef				_tElement								TElement;
		// Constructors
		inline constexpr											view_grid					()																			noexcept	= default;
		inline														view_grid					(_tElement* dataElements, uint32_t gridWidth, uint32_t gridHeight)						: Data{dataElements}, Size{gridWidth, gridHeight}			{
			gthrow_if(0 == dataElements && 0 != Size.x && 0 != Size.y, "Invalid parameters. Size: %u, %u, pointer: %p.", gridWidth, gridHeight, dataElements);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}
		inline														view_grid					(_tElement* dataElements, const ::gpk::SCoord2<uint32_t>& gridMetrics)					: view_grid(dataElements, gridMetrics.x, gridMetrics.y)		{}
		template<size_t _tWidth, size_t _tHeight>
		inline constexpr											view_grid					(_tElement (&dataElements)[_tHeight][_tWidth])											: Data{&dataElements[0][0]}, Size{_tWidth, _tHeight}				{}

		inline constexpr	operator								view_grid<const _tElement>	()																	const	noexcept	{ return {Data, Size}; }
		// Operators
							::gpk::view_array<_tElement>			operator[]					(uint32_t row)																			{ gthrow_if(0 == Data, "Uninitialized array pointer. Invalid row: %u.", row); gthrow_if(row >= Size.y, "Invalid row: %i.", (int32_t)row); return ::gpk::view_array<_tElement		>(&Data[row * Size.x], Size.x); }
							::gpk::view_array<const _tElement>		operator[]					(uint32_t row)														const				{ gthrow_if(0 == Data, "Uninitialized array pointer. Invalid row: %u.", row); gthrow_if(row >= Size.y, "Invalid row: %i.", (int32_t)row); return ::gpk::view_array<const _tElement	>(&Data[row * Size.x], Size.x); }
							_tElement		&						operator[]					(const ::gpk::SCoord2<uint32_t> & cell)													{ gthrow_if(0 == Data, "Uninitialized array pointer. Invalid cell: {%u, %u}.", cell.x, cell.y); gthrow_if(cell.y >= Size.y, "Invalid row: %i.", (int32_t)cell.y); gthrow_if(cell.x >= Size.x, "Invalid column: %i.", (int32_t)cell.x); return Data[cell.y * Size.x + cell.x]; }
							const _tElement	&						operator[]					(const ::gpk::SCoord2<uint32_t> & cell)								const				{ gthrow_if(0 == Data, "Uninitialized array pointer. Invalid cell: {%u, %u}.", cell.x, cell.y); gthrow_if(cell.y >= Size.y, "Invalid row: %i.", (int32_t)cell.y); gthrow_if(cell.x >= Size.x, "Invalid column: %i.", (int32_t)cell.x); return Data[cell.y * Size.x + cell.x]; }

		// Methods
#if !defined(GPK_ATMEL)
		inline constexpr	const _tElement*						begin						()																	const	noexcept	{ return Data;							}
		inline constexpr	const _tElement*						end							()																	const	noexcept	{ return Data + size();					}
#endif
		inline constexpr	_tElement*								begin						()																			noexcept	{ return Data;							}
		inline constexpr	_tElement*								end							()																			noexcept	{ return Data + size();					}

		inline constexpr	const ::gpk::SCoord2<uint32_t>&			metrics						()																	const	noexcept	{ return Size;							}
		inline constexpr	uint32_t								size						()																	const	noexcept	{ return area();						}
		inline constexpr	uint32_t 								byte_count					()																	const	noexcept	{ return area() * sizeof(_tElement);	}
		inline constexpr	uint32_t								area						()																	const	noexcept	{ return Size.x * Size.y;				}

		inline				::gpk::error_t							fill						(const _tElement& value, uint32_t offset = 0, uint32_t count = 0xFFFFFFFFU) {
			ree_if((count > size()) && (count != 0xFFFFFFFFU), "Count: %u", count);
			for(uint32_t i = offset, actualCount = ::gpk::min(size(), count); i < actualCount; ++i)
				Data[i]									= value;
			return 0;
		}
	};
#pragma pack(pop)

	template<typename _tCoord, typename _tColor>
	static					::gpk::error_t						drawRectangle				(::gpk::view_grid<_tColor>& target, const _tColor& value, const ::gpk::SRectangle2<_tCoord>& rectangle)		{
		const int32_t													xCount						= ::gpk::min((int32_t)target.metrics().x - ::gpk::max((int32_t)rectangle.Offset.x, (int32_t)0), (int32_t)rectangle.Size.x + ::gpk::min((int32_t)rectangle.Offset.x, (int32_t)0));
		const int32_t													yCount						= ::gpk::min((int32_t)target.metrics().y - ::gpk::max((int32_t)rectangle.Offset.y, (int32_t)0), (int32_t)rectangle.Size.y + ::gpk::min((int32_t)rectangle.Offset.y, (int32_t)0));
		const int32_t													yStart						= (int32_t)::gpk::max((int32_t)0, (int32_t)rectangle.Offset.y);
		const int32_t													xStart						= (int32_t)::gpk::max((int32_t)0, (int32_t)rectangle.Offset.x);
		if(xStart >= (int32_t)target.metrics().x || yStart >= (int32_t)target.metrics().y)
			return 0;
		if(xCount <= 0 || yCount <= 0)
			return 0;

		::gpk::view_array<_tColor>										firstRow					= target[yStart];
		if constexpr(1 == sizeof(_tColor))
			memset(&firstRow[xStart], *((ubyte_t*)&value), xCount);
		else
			for(int32_t x = 0; x < xCount; ++x)
				firstRow[xStart + x]										= value;

		for(int32_t y = 1; y < yCount; ++y)
			memcpy(&target[yStart + y][xStart], &target[yStart][xStart], xCount * sizeof(_tColor));

		return 0;
	}

	// view_array common typedefs
	typedef				::gpk::view_grid<char_t			>		view2d_char			;
	typedef				::gpk::view_grid<ubyte_t		>		view2d_ubyte		;
	typedef				::gpk::view_grid<byte_t			>		view2d_byte			;
	typedef				::gpk::view_grid<float			>		view2d_float32		;
	typedef				::gpk::view_grid<double			>		view2d_float64		;
	typedef				::gpk::view_grid<uint8_t		>		view2d_uint8		;
	typedef				::gpk::view_grid<uint16_t		>		view2d_uint16		;
	typedef				::gpk::view_grid<uint32_t		>		view2d_uint32		;
	typedef				::gpk::view_grid<uint64_t		>		view2d_uint64		;
	typedef				::gpk::view_grid<int8_t			>		view2d_int8			;
	typedef				::gpk::view_grid<int16_t		>		view2d_int16		;
	typedef				::gpk::view_grid<int32_t		>		view2d_int32		;
	typedef				::gpk::view_grid<int64_t		>		view2d_int64		;

	// view_array<const> common typedefs
	typedef				::gpk::view_grid<const char_t	>		view2d_const_char	;
	typedef				::gpk::view_grid<const ubyte_t	>		view2d_const_ubyte	;
	typedef				::gpk::view_grid<const byte_t	>		view2d_const_byte	;
	typedef				::gpk::view_grid<const float	>		view2d_const_float32;
	typedef				::gpk::view_grid<const double	>		view2d_const_float64;
	typedef				::gpk::view_grid<const uint8_t	>		view2d_const_uint8	;
	typedef				::gpk::view_grid<const uint16_t	>		view2d_const_uint16	;
	typedef				::gpk::view_grid<const uint32_t	>		view2d_const_uint32	;
	typedef				::gpk::view_grid<const uint64_t	>		view2d_const_uint64	;
	typedef				::gpk::view_grid<const int8_t	>		view2d_const_int8	;
	typedef				::gpk::view_grid<const int16_t	>		view2d_const_int16	;
	typedef				::gpk::view_grid<const int32_t	>		view2d_const_int32	;
	typedef				::gpk::view_grid<const int64_t	>		view2d_const_int64	;
}

#endif // GPK_GRID_VIEW_H_3423423
