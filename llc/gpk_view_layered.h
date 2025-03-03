#include "gpk_grid.h"

#ifndef GPK_LAYERED_VIEW_H_23627
#define GPK_LAYERED_VIEW_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt <tpnm _tElement>
	class view_layered {
	protected:
		// Properties / Member Variables
		_tElement			* Data			= 0;
		::gpk::n3<uint32_t>	Size			= {0, 0, 0};
		uint32_t			LayerSize		= 0;
	public:
		// Constructors
		inlcxpr				view_layered	()			noexcept	= default;
		inline				view_layered	(_tElement * dataElements, uint32_t layerWidth, uint32_t layerHeight, uint32_t layerCount)	: Data{dataElements}, Size{layerWidth, layerHeight, layerCount}, LayerSize(layerWidth * layerHeight) {
			gthrow_if(0 == dataElements && 0 != Size.x && 0 != Size.y && 0 != Size.z, "{%" GPK_FMT_U32 ", %" GPK_FMT_U32 ", %" GPK_FMT_U32 "}", Size.x, Size.y, Size.z);	// Throw if we received invalid parameters in order to prevent further malfunctioning.
		}

		inline				view_layered	(_tElement * dataElements, const ::gpk::n3<uint32_t> viewSize)							: Data{dataElements}, Size{viewSize}, LayerSize(viewSize.x * viewSize.y) {
			gthrow_if(0 == dataElements && 0 != Size.x && 0 != Size.y && 0 != Size.z, "{%" GPK_FMT_U32 ", %" GPK_FMT_U32 ", %" GPK_FMT_U32 "}", Size.x, Size.y, Size.z);	// Throw if we received invalid parameters in order to prevent further malfunctioning.
		}

		// Operators
		::gpk::grid<_tElement>	operator[]		(uint32_t layer)																			{
			gsthrow_if(0 == Data);
			gthrow_if(layer >= Size.z, "%" GPK_FMT_U32 " > %" GPK_FMT_U32 ".", layer, Size.z);
			return ::gpk::grid<_tElement		>(&Data[layer * LayerSize], Size.x, Size.y);
		}
		::gpk::grid<const _tElement>		operator[]		(uint32_t layer)														const				{
			gsthrow_if(0 == Data);
			gthrow_if(layer >= Size.z, "%" GPK_FMT_U32 " > %" GPK_FMT_U32 ".", layer, Size.z);
			return ::gpk::grid<const _tElement>(&Data[layer * LayerSize], Size.x, Size.y);
		}

		// Methods
		inlcxpr	const _tElement*						begin			()																		const	noexcept	{ return Data;						}
		inlcxpr	const _tElement*						end				()																		const	noexcept	{ return Data + size();				}

		inlcxpr	_tElement*								begin			()			noexcept	{ return Data;						}
		inlcxpr	_tElement*								end				()			noexcept	{ return Data + size();				}

		inlcxpr	const ::gpk::n3<uint32_t>&			metrics			()																		const	noexcept	{ return Size;						}
		inlcxpr	uint32_t								size			()																		const	noexcept	{ return Size.x * Size.y * Size.z;	}
};
#pragma pack(pop)

	// grid common typedefs
	typedef				::gpk::view_layered<sc_t			>	view3d_char				;
	typedef				::gpk::view_layered<uint8_t			>	view3d_ubyte			;
	typedef				::gpk::view_layered<int8_t			>	view3d_byte				;
	typedef				::gpk::view_layered<float			>	view3d_float32			;
	typedef				::gpk::view_layered<double			>	view3d_float64			;
	typedef				::gpk::view_layered<uint8_t			>	view3d_uint8			;
	typedef				::gpk::view_layered<uint16_t		>	view3d_uint16			;
	typedef				::gpk::view_layered<uint32_t		>	view3d_uint32			;
	typedef				::gpk::view_layered<uint64_t		>	view3d_uint64			;
	typedef				::gpk::view_layered<int8_t			>	view3d_int8				;
	typedef				::gpk::view_layered<int16_t			>	view3d_int16			;
	typedef				::gpk::view_layered<int32_t			>	view3d_int32			;
	typedef				::gpk::view_layered<int64_t			>	view3d_int64			;

	// grid<const> common typedefs
	typedef				::gpk::view_layered<const sc_t	>	view3d_const_char		;
	typedef				::gpk::view_layered<const uint8_t	>	view3d_const_ubyte		;
	typedef				::gpk::view_layered<const int8_t	>	view3d_const_byte		;
	typedef				::gpk::view_layered<const float		>	view3d_const_float32	;
	typedef				::gpk::view_layered<const double	>	view3d_const_float64	;
	typedef				::gpk::view_layered<const uint8_t	>	view3d_const_uint8		;
	typedef				::gpk::view_layered<const uint16_t	>	view3d_const_uint16		;
	typedef				::gpk::view_layered<const uint32_t	>	view3d_const_uint32		;
	typedef				::gpk::view_layered<const uint64_t	>	view3d_const_uint64		;
	typedef				::gpk::view_layered<const int8_t	>	view3d_const_int8		;
	typedef				::gpk::view_layered<const int16_t	>	view3d_const_int16		;
	typedef				::gpk::view_layered<const int32_t	>	view3d_const_int32		;
	typedef				::gpk::view_layered<const int64_t	>	view3d_const_int64		;

}

#endif // GPK_LAYERED_VIEW_H_23627
