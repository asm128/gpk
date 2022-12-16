#include "gpk_datatype.h"
#include "gpk_enum.h"

#ifndef GPK_ENGINE_RENDERBUFFER_H
#define GPK_ENGINE_RENDERBUFFER_H

namespace gpk 
{
	GDEFINE_ENUM_TYPE(BUFFER_USAGE, uint8_t);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Position			,  0);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Index				,  1);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Color				,  2);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, ColorIndex			,  3);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Normal				,  4);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, UV					,  5);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Shadow				,  6);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Light				,  7);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, BlendIndex			,  8);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, BlendWeight		,  9);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, Palette			, 10);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, RasterSource		, 11);
	GDEFINE_ENUM_VALUE(BUFFER_USAGE, RasterDestination	, 12);

#pragma pack(push, 1)
	struct SRenderBufferDescription {
		::gpk::DATA_TYPE							Format			= {};
		BUFFER_USAGE								Usage			= {};
		uint8_t										Stride			= {};
		uint16_t									Pitch			= {};
	};
#pragma pack(pop)

	struct SRenderBuffer {
		SRenderBufferDescription					Desc			= {};
		::gpk::array_pod<uint8_t>					Data			= {};

		::gpk::error_t								Save			(::gpk::array_pod<byte_t> & output)		{ 			
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const ::gpk::SRenderBufferDescription>{&Desc, 1}, output));
			gpk_necs(::gpk::viewWrite(::gpk::view_const_uint8{Data}, output));
			return 0; 
		}

		::gpk::error_t								Load			(::gpk::view_array<const byte_t> & input)	{ 
			gpk_necs(::gpk::loadPOD (input, Desc));
			gpk_necs(::gpk::loadView(input, Data));
			return 0;
		}
	};
} // namespace

#endif
