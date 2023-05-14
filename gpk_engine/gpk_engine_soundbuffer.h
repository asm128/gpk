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

	GDEFINE_ENUM_TYPE(SOUND_FREQUENCY, uint8_t);
	GDEFINE_ENUM_VALUED(SOUND_FREQUENCY, Element			,   0, "Each element of the buffer matches to an element of the indexed buffer");
	GDEFINE_ENUM_VALUED(SOUND_FREQUENCY, Instance			,   1, "Each elements of the buffer matches to a whole primitive instance of the indexed buffer. "
		"This means that for drawing a triangle, three indexed elements will be used while only one of the instance buffer will be used.");
	GDEFINE_ENUM_VALUED(SOUND_FREQUENCY, Bitfield			,   2, "Each element of the buffer contains a smaller-than-byte value that will be used for either each indexed element or each primitive instance, "
		"depending on the value of first bit");
	GDEFINE_ENUM_VALUED(SOUND_FREQUENCY, BitfieldSizeMask	,  12, "This 2-bit mask grabs the size in bits to read for each element inside the byte. The value represents the size in bits of the element less one (sizeInBits - 1). "
		"This means it could be 0 for 1-bit values, 1 for 2-bit values, 2 for 3-bit values and 3 for 4-bit values");

#pragma pack(push, 1)
	struct SSoundBufferDescription {
		::gpk::SDataTypeID							Format				= {};
		SOUND_BUFFER_MODE							Usage				= {};
		uint8_t										PitchStride			= {};
		uint16_t									PitchRow			: 15;
		uint16_t									Frequency			: 1;
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

	typedef	::gpk::SLinearMap<::gpk::SSoundBuffer>	SSoundBufferManager;

} // namespace

#endif // GPK_ENGINE_SOUNDBUFFER_H_23_05_08
