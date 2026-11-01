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
		::gpk::n3<u2_t>	Size			= {0, 0, 0};
		u2_t			LayerSize		= 0;
	public:
		// Constructors
		inxp				view_layered	()			noexcept	= default;
		inline				view_layered	(_tElement * dataElements, u2_t layerWidth, u2_t layerHeight, u2_t layerCount)	: Data{dataElements}, Size{layerWidth, layerHeight, layerCount}, LayerSize(layerWidth * layerHeight) {
			if_true_tef(0 == dataElements && 0 != Size.x && 0 != Size.y && 0 != Size.z, "{%" GPK_FMT_U32 ", %" GPK_FMT_U32 ", %" GPK_FMT_U32 "}", Size.x, Size.y, Size.z);	// Throw if we received invalid parameters in order to prevent further malfunctioning.
		}

		inline				view_layered	(_tElement * dataElements, const ::gpk::n3<u2_t> viewSize)							: Data{dataElements}, Size{viewSize}, LayerSize(viewSize.x * viewSize.y) {
			if_true_tef(0 == dataElements && 0 != Size.x && 0 != Size.y && 0 != Size.z, "{%" GPK_FMT_U32 ", %" GPK_FMT_U32 ", %" GPK_FMT_U32 "}", Size.x, Size.y, Size.z);	// Throw if we received invalid parameters in order to prevent further malfunctioning.
		}

		// Operators
		::gpk::grid<_tElement>	operator[]		(u2_t layer)																			{
			if_true_te(0 == Data);
			if_true_tef(layer >= Size.z, "%" GPK_FMT_U32 " > %" GPK_FMT_U32 ".", layer, Size.z);
			return ::gpk::grid<_tElement		>(&Data[layer * LayerSize], Size.x, Size.y);
		}
		::gpk::grid<const _tElement>		operator[]		(u2_t layer)														const				{
			if_true_te(0 == Data);
			if_true_tef(layer >= Size.z, "%" GPK_FMT_U32 " > %" GPK_FMT_U32 ".", layer, Size.z);
			return ::gpk::grid<const _tElement>(&Data[layer * LayerSize], Size.x, Size.y);
		}

		// Methods
		inxp	const _tElement*						begin			()																		const	noexcept	{ return Data;						}
		inxp	const _tElement*						end				()																		const	noexcept	{ return Data + size();				}

		inxp	_tElement*								begin			()			noexcept	{ return Data;						}
		inxp	_tElement*								end				()			noexcept	{ return Data + size();				}

		inxp	const ::gpk::n3<u2_t>&			metrics			()																		const	noexcept	{ return Size;						}
		inxp	u2_t								size			()																		const	noexcept	{ return Size.x * Size.y * Size.z;	}
};
#pragma pack(pop)

	// grid common typedefs
	typedef				::gpk::view_layered<sc_t			>	view3d_char				;
	typedef				::gpk::view_layered<u0_t			>	view3d_ubyte			;
	typedef				::gpk::view_layered<s0_t			>	view3d_byte				;
	typedef				::gpk::view_layered<f2_t			>	view3d_float32			;
	typedef				::gpk::view_layered<f3_t			>	view3d_float64			;
	typedef				::gpk::view_layered<u0_t			>	view3d_uint8			;
	typedef				::gpk::view_layered<u1_t		>	view3d_uint16			;
	typedef				::gpk::view_layered<u2_t		>	view3d_uint32			;
	typedef				::gpk::view_layered<u3_t		>	view3d_uint64			;
	typedef				::gpk::view_layered<s0_t			>	view3d_int8				;
	typedef				::gpk::view_layered<s1_t			>	view3d_int16			;
	typedef				::gpk::view_layered<s2_t			>	view3d_int32			;
	typedef				::gpk::view_layered<s3_t			>	view3d_int64			;

	// grid<const> common typedefs
	typedef				::gpk::view_layered<const sc_t	>	view3d_const_char		;
	typedef				::gpk::view_layered<const u0_t	>	view3d_const_ubyte		;
	typedef				::gpk::view_layered<const s0_t	>	view3d_const_byte		;
	typedef				::gpk::view_layered<const f2_t		>	view3d_const_float32	;
	typedef				::gpk::view_layered<const f3_t	>	view3d_const_float64	;
	typedef				::gpk::view_layered<const u0_t	>	view3d_const_uint8		;
	typedef				::gpk::view_layered<const u1_t	>	view3d_const_uint16		;
	typedef				::gpk::view_layered<const u2_t	>	view3d_const_uint32		;
	typedef				::gpk::view_layered<const u3_t	>	view3d_const_uint64		;
	typedef				::gpk::view_layered<const s0_t	>	view3d_const_int8		;
	typedef				::gpk::view_layered<const s1_t	>	view3d_const_int16		;
	typedef				::gpk::view_layered<const s2_t	>	view3d_const_int32		;
	typedef				::gpk::view_layered<const s3_t	>	view3d_const_int64		;

}

#endif // GPK_LAYERED_VIEW_H_23627
