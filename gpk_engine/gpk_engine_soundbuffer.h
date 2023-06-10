#include "gpk_datatype.h"
#include "gpk_enum.h"
#include "gpk_engine_container.h"

#ifndef GPK_ENGINE_SOUNDBUFFER_H_23_05_08
#define GPK_ENGINE_SOUNDBUFFER_H_23_05_08

namespace gpk
{
	GDEFINE_ENUM_TYPE(SOUND_BUFFER_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(SOUND_BUFFER_MODE, Loop				,   0);
	GDEFINE_ENUM_VALUE(SOUND_BUFFER_MODE, Discard			,   1);
	GDEFINE_ENUM_VALUE(SOUND_BUFFER_MODE, Bitfield			, 100);

#pragma pack(push, 1)
	struct SSoundBufferDescription {
		::gpk::SDataTypeID							Format				= {};
		SOUND_BUFFER_MODE							Usage				= {};
	};
#pragma pack(pop)

	struct SSoundBuffer {
		SSoundBufferDescription						Desc			= {};
		::gpk::au8									Data			= {};

		::gpk::error_t								Save			(::gpk::apod<uint8_t> & output)	const	{ 
			gpk_necs(::gpk::savePOD	(output, Desc));
			gpk_necs(::gpk::saveView(output, Data));
			return 0; 
		}

		::gpk::error_t								Load			(::gpk::vcu8 & input)	{ 
			gpk_necs(::gpk::loadPOD (input, Desc));
			gpk_necs(::gpk::loadView(input, Data));
			return 0;
		}
	};

	typedef	::gpk::SLinearNamedMap<::gpk::SSoundBuffer>	SSoundBufferManager;

} // namespace

#endif // GPK_ENGINE_SOUNDBUFFER_H_23_05_08
