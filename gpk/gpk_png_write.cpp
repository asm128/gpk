#include "gpk_bitmap_file.h"
#include "gpk_png.h"


						::gpk::error_t							gpk::pngFileWrite						(const ::gpk::grid_view<::gpk::SColorBGRA>& in_imageView, ::gpk::array_pod<ubyte_t>& out_Bytes)		{
	static constexpr const ubyte_t										signature	[8]							= {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
	::gpk::array_pod<ubyte_t>											safe_Bytes								= {};
	safe_Bytes.append((ubyte_t*)signature, 8);

	uint32_t															chunkSize								= sizeof(::gpk::SPNGIHDR);
	static constexpr const char											typeIHDR	[4]							= {'I', 'H', 'D', 'R'};
	int32_t																crc										= 0;
	::gpk::SPNGIHDR														imageHeader								= {};
	imageHeader.Size												= in_imageView.metrics();
	imageHeader.BitDepth											= 8;
	imageHeader.ColorType											= 6;
	imageHeader.MethodCompression									= 0;
	imageHeader.MethodFilter										= 0;
	imageHeader.MethodInterlace										= 0;

	be2le_32(imageHeader.Size.x);
	be2le_32(imageHeader.Size.y);
	be2le_32(chunkSize);
	be2le_32(crc);
	safe_Bytes.append((const ubyte_t*)&chunkSize, 4);
	safe_Bytes.append((const ubyte_t*)typeIHDR, 4);
	safe_Bytes.append((const ubyte_t*)&imageHeader, sizeof(::gpk::SPNGIHDR));
	safe_Bytes.append((const ubyte_t*)&crc, 4);

	// Reverse RGB byte order
	::gpk::STexture<::gpk::color_rgba<uint8_t>>							convertedScanlines						= {};
	for(uint32_t y = 0; y < in_imageView.metrics().y; ++y)
	for(uint32_t x = 0; x < in_imageView.metrics().x; ++x) {
		::gpk::SColorBGRA													colorSrc								= in_imageView		[y][x];
		::gpk::color_rgba<uint8_t>											& colorDst								= convertedScanlines[y][x];
		colorDst.r														= colorSrc.r;
		colorDst.g														= colorSrc.g;
		colorDst.b														= colorSrc.b;
		colorDst.a														= colorSrc.a;
	}

	static constexpr const char											typeIDAT	[4]							= {'I', 'H', 'D', 'R'};
	chunkSize														= 0;
	crc																= 0;
	be2le_32(chunkSize);
	be2le_32(crc);
	safe_Bytes.append((const ubyte_t*)&chunkSize, 4);
	safe_Bytes.append((const ubyte_t*)typeIDAT, 4);
	safe_Bytes.append((const ubyte_t*)convertedScanlines.Texels.begin(), 4);
	safe_Bytes.append((const ubyte_t*)&crc, 4);

	chunkSize														= 0;
	crc																= 0;
	static constexpr const char											typeIEND	[4]							= {'I', 'E', 'N', 'D'};
	be2le_32(chunkSize);
	be2le_32(crc);
	safe_Bytes.append((const ubyte_t*)&chunkSize, 4);
	safe_Bytes.append((const ubyte_t*)typeIEND, 4);
	safe_Bytes.append((const ubyte_t*)&crc, 4);

	int32_t																oldSize									= out_Bytes.size();
	gpk_necall(out_Bytes.resize(oldSize + safe_Bytes.size()), "Out of memory?");
	memcpy(&out_Bytes[oldSize], safe_Bytes.begin(), safe_Bytes.size());
	return 0;
}


