#include "gpk_view_grid.h"

#ifndef GPK_LAYERED_VIEW_H_0923782
#define GPK_LAYERED_VIEW_H_0923782

namespace gpk
{
#pragma pack(push, 1)
	template <typename _tElement>
	class view_layered {
	protected:
		// Properties / Member Variables
							_tElement									* Data						= 0;
							::gpk::SCoord3<uint32_t>					Size						= {0, 0, 0};
							uint32_t									LayerSize					= 0;
	public:
		// Constructors
		inline constexpr												view_layered				()																				noexcept	= default;
		inline															view_layered				(_tElement* dataElements, uint32_t layerWidth, uint32_t layerHeight, uint32_t layerCount)	: Data{dataElements}, Size{layerWidth, layerHeight, layerCount}, LayerSize(layerWidth * layerHeight) {
			throw_if(0 == dataElements && 0 != Size.x && 0 != Size.y && 0 != Size.z, "Invalid parameters.");	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		inline															view_layered				(_tElement* dataElements, const ::gpk::SCoord3<uint32_t> viewSize)							: Data{dataElements}, Size{viewSize}, LayerSize(viewSize.x * viewSize.y) {
			throw_if(0 == dataElements && 0 != Size.x && 0 != Size.y && 0 != Size.z, "Invalid parameters.");	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}
		// Operators
							::gpk::view_grid<_tElement>					operator[]					(uint32_t layer)																			{ throw_if(0 == Data, "Uninitialized array pointer."); throw_if(layer >= Size.z, "Invalid layer: %i.", layer); return ::gpk::view_grid<_tElement		>(&Data[layer * LayerSize], Size.x, Size.y); }
							::gpk::view_grid<const _tElement>			operator[]					(uint32_t layer)														const				{ throw_if(0 == Data, "Uninitialized array pointer."); throw_if(layer >= Size.z, "Invalid layer: %i.", layer); return ::gpk::view_grid<const _tElement>(&Data[layer * LayerSize], Size.x, Size.y); }

		// Methods
		inline constexpr	const _tElement*							begin						()																		const	noexcept	{ return Data;						}
		inline constexpr	const _tElement*							end							()																		const	noexcept	{ return Data + size();				}

		inline constexpr	_tElement*									begin						()																				noexcept	{ return Data;						}
		inline constexpr	_tElement*									end							()																				noexcept	{ return Data + size();				}

		inline constexpr	const ::gpk::SCoord3<uint32_t>&				metrics						()																		const	noexcept	{ return Size;						}
		inline constexpr	uint32_t									size						()																		const	noexcept	{ return Size.x * Size.y * Size.z;	}
};
#pragma pack(pop)

	// view_grid common typedefs
	typedef				::gpk::view_layered<char_t				>	view3d_char				;
	typedef				::gpk::view_layered<ubyte_t				>	view3d_ubyte			;
	typedef				::gpk::view_layered<byte_t				>	view3d_byte				;
	typedef				::gpk::view_layered<float				>	view3d_float32			;
	typedef				::gpk::view_layered<double				>	view3d_float64			;
	typedef				::gpk::view_layered<uint8_t				>	view3d_uint8			;
	typedef				::gpk::view_layered<uint16_t			>	view3d_uint16			;
	typedef				::gpk::view_layered<uint32_t			>	view3d_uint32			;
	typedef				::gpk::view_layered<uint64_t			>	view3d_uint64			;
	typedef				::gpk::view_layered<int8_t				>	view3d_int8				;
	typedef				::gpk::view_layered<int16_t				>	view3d_int16			;
	typedef				::gpk::view_layered<int32_t				>	view3d_int32			;
	typedef				::gpk::view_layered<int64_t				>	view3d_int64			;

	// view_grid<const> common typedefs
	typedef				::gpk::view_layered<const char_t		>	view3d_const_char		;
	typedef				::gpk::view_layered<const ubyte_t		>	view3d_const_ubyte		;
	typedef				::gpk::view_layered<const byte_t		>	view3d_const_byte		;
	typedef				::gpk::view_layered<const float			>	view3d_const_float32	;
	typedef				::gpk::view_layered<const double		>	view3d_const_float64	;
	typedef				::gpk::view_layered<const uint8_t		>	view3d_const_uint8		;
	typedef				::gpk::view_layered<const uint16_t		>	view3d_const_uint16		;
	typedef				::gpk::view_layered<const uint32_t		>	view3d_const_uint32		;
	typedef				::gpk::view_layered<const uint64_t		>	view3d_const_uint64		;
	typedef				::gpk::view_layered<const int8_t		>	view3d_const_int8		;
	typedef				::gpk::view_layered<const int16_t		>	view3d_const_int16		;
	typedef				::gpk::view_layered<const int32_t		>	view3d_const_int32		;
	typedef				::gpk::view_layered<const int64_t		>	view3d_const_int64		;

}

#endif // GPK_LAYERED_VIEW_H_0923782
