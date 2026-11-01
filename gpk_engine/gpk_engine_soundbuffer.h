#include "gpk_datatype.h"
#include "gpk_enum.h"
#include "gpk_linear_map_pobj.h"

#ifndef GPK_ENGINE_SOUNDBUFFER_H_23_05_08
#define GPK_ENGINE_SOUNDBUFFER_H_23_05_08

namespace gpk
{
	GDEFINE_ENUM_TYPE(SOUND_BUFFER_MODE, u0_t);
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
		::gpk::au0_t									Data			= {};

		gpk::err_t 							Save			(::gpk::apod<u0_t> & output)	const	{ 
			if_fail_fe(gpk::savePOD	(output, Desc));
			if_fail_fe(gpk::saveView(output, Data));
			return 0; 
		}

		gpk::err_t 							Load			(::gpk::vcu0_t & input)	{ 
			if_fail_fe(gpk::loadPOD (input, Desc));
			if_fail_fe(gpk::loadView(input, Data));
			return 0;
		}
	};

	typedef	::gpk::gpk_linear_map_pobj<::gpk::SSoundBuffer>	SSoundBufferManager;

} // namespace

#endif // GPK_ENGINE_SOUNDBUFFER_H_23_05_08
