#ifndef GPK_PNG_H_0928374982374
#define GPK_PNG_H_0928374982374

namespace gpk
{
#pragma pack(push, 1)
	struct SPNGChunk {
						uint8_t													Type				[4]			= {};
						uint32_t												CRC								= {};
						::gpk::array_pod<uint8_t>								Data							= {};
	};

	struct SPNGIHDR {
						::gpk::SCoord2<uint32_t>								Size							= {};
						int8_t													BitDepth						= 0;
						int8_t													ColorType						= 0;
						int8_t													MethodCompression				= 0;
						int8_t													MethodFilter					= 0;
						int8_t													MethodInterlace					= 0;
	};

	struct SPNGFeature {
						int32_t													tEXt							= -1;
						int32_t													zTXt							= -1;
						int32_t													bKGD							= -1;
						int32_t													cHRM							= -1;
						int32_t													dSIG							= -1;
						int32_t													eXIf							= -1;
						int32_t													gAMA							= -1;
						int32_t													hIST							= -1;
						int32_t													iCCP							= -1;
						int32_t													iTXt							= -1;
						int32_t													pHYs							= -1;
						int32_t													sBIT							= -1;
						int32_t													sPLT							= -1;
						int32_t													sRGB							= -1;
						int32_t													sTER							= -1;
						int32_t													tIME							= -1;
						int32_t													tRNS							= -1;
						int32_t													fcTL							= -1;
						int32_t													fdAT							= -1;
						int32_t													acTL							= -1;
	};

	struct SPNGData {
						char													Signature			[8]			= {};
						::gpk::array_obj<SPNGChunk>								Chunks							;
						::gpk::array_pod<ubyte_t>								Deflated						;
						::gpk::array_pod<ubyte_t>								Inflated						;
						::gpk::array_pod<ubyte_t>								Filters							;
						::gpk::array_obj<::gpk::array_pod<ubyte_t>>				Scanlines						;
						::gpk::array_pod<::gpk::color_bgr<uint8_t>>				Palette							;
						::gpk::SCoord2<uint32_t>								Adam7Sizes			[7]			= {};
						::gpk::SPNGFeature										Feature							= {};
						::gpk::SPNGIHDR											Header							= {};
	};
#pragma pack(pop)

					uint32_t												update_crc						(const ::gpk::view_array<const ubyte_t>& buf, uint32_t crc)		;
	static inline	uint32_t												get_crc							(const ::gpk::view_array<const ubyte_t>& buf)					{ return update_crc(buf, 0xffffffffL) ^ 0xffffffffL; }

					::gpk::error_t											pngFileLoad						(::gpk::SPNGData& pngCache, const ::gpk::view_const_string				& filename		, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)	;
					::gpk::error_t											pngFileLoad						(::gpk::SPNGData& pngCache, const ::gpk::view_array<const ubyte_t>		& source		, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)	;
					::gpk::error_t											pngFileWrite					(const ::gpk::view_grid<::gpk::SColorBGRA> & out_ImageView, ::gpk::array_pod<ubyte_t>& out_Bytes);

	static inline	::gpk::error_t											pngFileLoad						(const ::gpk::view_const_string			& filename	, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)	{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, filename	, out_Texture); }
	static inline	::gpk::error_t											pngFileLoad						(const ::gpk::view_array<const ubyte_t>	& source	, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)	{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, source	, out_Texture); }
} // namespace

#endif // GPK_PNG_H_0928374982374
