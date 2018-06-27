#ifndef GPK_PNG_H_0928374982374
#define GPK_PNG_H_0928374982374

namespace gpk
{
#pragma pack(push, 1)
	struct SPNGChunk {
						uint8_t													Type				[4]	= {};
						uint32_t												CRC						= {};
						::gpk::array_pod<uint8_t>								Data					= {};
	};

	struct SPNGIHDR {
						::gpk::SCoord2<uint32_t>								Size					= {};
						int8_t													BitDepth				= 0;
						int8_t													ColorType				= 0;
						int8_t													MethodCompression		= 0;
						int8_t													MethodFilter			= 0;
						int8_t													MethodInterlace			= 0;
	};

	struct SPNGFeature {
						int32_t													tEXt					= -1;
						int32_t													zTXt					= -1;
						int32_t													fcTL					= -1;
						int32_t													fdAT					= -1;
						int32_t													bKGD					= -1;
						int32_t													cHRM					= -1;
						int32_t													dSIG					= -1;
						int32_t													eXIf					= -1;
						int32_t													gAMA					= -1;
						int32_t													hIST					= -1;
						int32_t													iCCP					= -1;
						int32_t													iTXt					= -1;
						int32_t													pHYs					= -1;
						int32_t													sBIT					= -1;
						int32_t													sPLT					= -1;
						int32_t													sRGB					= -1;
						int32_t													sTER					= -1;
						int32_t													tIME					= -1;
						int32_t													tRNS					= -1;
						int32_t													acTL					= -1;
	};

	struct SPNGData {
						char													Signature			[8]	= {};
						::gpk::array_obj<SPNGChunk>								Chunks					;
						::gpk::array_pod<ubyte_t>								Deflated				;
						::gpk::array_pod<ubyte_t>								Inflated				;
						::gpk::array_pod<ubyte_t>								Filters					;
						::gpk::array_obj<::gpk::array_pod<ubyte_t>>				Scanlines				;
						::gpk::array_pod<::gpk::color_bgr<uint8_t>>				Palette					;
						::gpk::SCoord2<uint32_t>								Adam7Sizes			[7]	= {};
						::gpk::SPNGFeature										Feature					= {};
						::gpk::SPNGIHDR											Header					= {};
	};
#pragma pack(pop)
} // namespace

#endif // GPK_PNG_H_0928374982374
