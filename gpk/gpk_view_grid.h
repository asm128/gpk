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
			throw_if(0 == dataElements && 0 != Size.x && 0 != Size.y, ::std::exception(""), "Invalid parameters.");	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}
		inline														view_grid					(_tElement* dataElements, const ::gpk::SCoord2<uint32_t>& gridMetrics)					: view_grid(dataElements, gridMetrics.x, gridMetrics.y)		{}

		// Operators
							::gpk::view_array<_tElement>			operator[]					(uint32_t row)																			{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(row >= Size.y, ::std::exception(""), "Invalid row: %i.", (int32_t)row); return ::gpk::view_array<_tElement		>(&Data[row * Size.x], Size.x); }
							::gpk::view_array<const _tElement>		operator[]					(uint32_t row)														const				{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(row >= Size.y, ::std::exception(""), "Invalid row: %i.", (int32_t)row); return ::gpk::view_array<const _tElement	>(&Data[row * Size.x], Size.x); }

		// Methods
		inline constexpr	const _tElement*						begin						()																	const	noexcept	{ return Data;						}
		inline constexpr	const _tElement*						end							()																	const	noexcept	{ return Data + size();				}

		inline constexpr	_tElement*								begin						()																			noexcept	{ return Data;						}
		inline constexpr	_tElement*								end							()																			noexcept	{ return Data + size();				}

		inline constexpr	const ::gpk::SCoord2<uint32_t>&			metrics						()																	const	noexcept	{ return Size;						}
		inline constexpr	uint32_t								size						()																	const	noexcept	{ return Size.x * Size.y;			}
		//inline constexpr	uint32_t								width						()																	const	noexcept	{ return Size.x;					}
		//inline constexpr	uint32_t								height						()																	const	noexcept	{ return Size.y;					}
	};
#pragma pack(pop)

	// view_array common typedefs
	typedef				::gpk::view_grid<char_t				>	view2d_char				;
	typedef				::gpk::view_grid<ubyte_t			>	view2d_ubyte			;
	typedef				::gpk::view_grid<byte_t				>	view2d_byte				;
	typedef				::gpk::view_grid<float				>	view2d_float32			;
	typedef				::gpk::view_grid<double				>	view2d_float64			;
	typedef				::gpk::view_grid<uint8_t			>	view2d_uint8			;
	typedef				::gpk::view_grid<uint16_t			>	view2d_uint16			;
	typedef				::gpk::view_grid<uint32_t			>	view2d_uint32			;
	typedef				::gpk::view_grid<uint64_t			>	view2d_uint64			;
	typedef				::gpk::view_grid<int8_t				>	view2d_int8				;
	typedef				::gpk::view_grid<int16_t			>	view2d_int16			;
	typedef				::gpk::view_grid<int32_t			>	view2d_int32			;
	typedef				::gpk::view_grid<int64_t			>	view2d_int64			;

	// view_array<const> common typedefs
	typedef				::gpk::view_grid<const char_t		>	view2d_const_char		;
	typedef				::gpk::view_grid<const ubyte_t		>	view2d_const_ubyte		;
	typedef				::gpk::view_grid<const byte_t		>	view2d_const_byte		;
	typedef				::gpk::view_grid<const float		>	view2d_const_float32	;
	typedef				::gpk::view_grid<const double		>	view2d_const_float64	;
	typedef				::gpk::view_grid<const uint8_t		>	view2d_const_uint8		;
	typedef				::gpk::view_grid<const uint16_t		>	view2d_const_uint16		;
	typedef				::gpk::view_grid<const uint32_t		>	view2d_const_uint32		;
	typedef				::gpk::view_grid<const uint64_t		>	view2d_const_uint64		;
	typedef				::gpk::view_grid<const int8_t		>	view2d_const_int8		;
	typedef				::gpk::view_grid<const int16_t		>	view2d_const_int16		;
	typedef				::gpk::view_grid<const int32_t		>	view2d_const_int32		;
	typedef				::gpk::view_grid<const int64_t		>	view2d_const_int64		;

}

#endif // GPK_GRID_VIEW_H_3423423
