#include "gpk_datatype.h"
#include "gpk_enum.h"
#include "gpk_engine_container.h"

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

	GDEFINE_ENUM_TYPE(ELEMENT_FREQUENCY, uint8_t);
	GDEFINE_ENUM_VALUE(ELEMENT_FREQUENCY, Element	,  0);
	GDEFINE_ENUM_VALUE(ELEMENT_FREQUENCY, Instance	,  1);

#pragma pack(push, 1)
	struct SRenderBufferDescription {
		::gpk::SDataTypeID							Format				= {};
		BUFFER_USAGE								Usage				= {};
		uint8_t										PitchStride			= {};
		uint16_t									PitchRow			: 15;
		uint16_t									Frequency			: 1;
	};
#pragma pack(pop)

	struct SRenderBuffer {
		SRenderBufferDescription					Desc			= {};
		::gpk::apod<uint8_t>						Data			= {};

		::gpk::error_t								Save			(::gpk::apod<ubyte_t> & output)	const	{ 			
			gpk_necs(::gpk::savePOD	(output, Desc));
			gpk_necs(::gpk::viewSave(output, Data));
			return 0; 
		}

		::gpk::error_t								Load			(::gpk::vcub & input)	{ 
			gpk_necs(::gpk::loadPOD (input, Desc));
			gpk_necs(::gpk::loadView(input, Data));
			return 0;
		}
	};

	typedef		::gpk::SLinearMap<::gpk::SRenderBuffer>		SRenderBufferManager;
} // namespace

#endif
