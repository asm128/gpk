#include "gpk_png.h"
#include "zlib.h"

static			unsigned long									g_crc_table	[256]						= {};	// Table of CRCs of all 8-bit messages.
static			int32_t											g_crc_table_computed					= 0;	// Flag: has the table been computed? Initially false.
/* Make the table for a fast CRC. */
static			int32_t											make_crc_table							()																									{
    uint32_t															n, k;
    for (n = 0; n < 256; ++n) {
		uint32_t															c										= n;
		for (k = 0; k < 8; ++k)
			c																= (c & 1) ? 0xedb88320L ^ (c >> 1) : c >> 1;
		g_crc_table[n]													= c;
    }
    return g_crc_table_computed										= 1;
}

// Update a running CRC with the bytes buf[0..len-1]--the CRC should be initialized to all 1's, and the transmitted value is the 1's complement of the final running CRC (see the crc() routine below)).
				uint32_t										gpk::update_crc							(const ::gpk::view_array<const ubyte_t> & buf, uint32_t crc)										{
    uint32_t															c										= crc;
    if(0 == g_crc_table_computed) {
		static const int32_t												initedTable								= make_crc_table();
		(void)initedTable;
		info_printf("Initialized PNG CRC table: %i.", initedTable);
	}

	for (uint32_t n = 0, count = buf.size(); n < count; ++n)
		c																= g_crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);

	return c;
}

static			::gpk::error_t									pngDeflate								(const ::gpk::view_array<const ubyte_t>& inflated, ::gpk::apod<ubyte_t>& deflated)		{
    int				ret;
	z_stream		strm												= {};
    ret																	= deflateInit(&strm, Z_BEST_COMPRESSION);
    if (ret != Z_OK)
        return ret;
    strm.avail_in														= inflated.size();
    strm.next_in														= (Bytef*)inflated.begin();
    strm.avail_out														= deflated.size();
    strm.next_out														= (Bytef*)deflated.begin();
    ret																	= deflate(&strm, Z_FINISH);    /* no bad return value */
    if(ret == Z_STREAM_ERROR) {
	   (void)deflateEnd(&strm);
		error_printf("Failed to compress: 0x%x.", ret);  /* state not clobbered */
		return -1;
	}
    (void)deflateEnd(&strm);
    gerror_if(strm.avail_in != 0, "%s", "Not all of the input bytes were consumed.");	/* all input will be used */
    gerror_if(ret != Z_STREAM_END && ret != Z_OK, "%s", "Unknown error");				/* stream will be complete */
	deflated.resize((uint32_t)((ptrdiff_t)strm.next_out - (ptrdiff_t)deflated.begin()));
    /* clean up and return */
	info_printf("deflateEnd: %u.", (uint32_t)ret);
	return 0;
}

						::gpk::error_t							gpk::pngFileWrite						(const ::gpk::view2d<::gpk::SColorBGRA>& in_imageView, ::gpk::apod<ubyte_t>& out_Bytes)		{
	static constexpr const ubyte_t										signature	[8]							= {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
	::gpk::apod<ubyte_t>											safe_Bytes								= {};
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
	safe_Bytes.append((const ubyte_t*)&chunkSize, 4);
	uint32_t															crcDataStart							= safe_Bytes.size();
	safe_Bytes.append((const ubyte_t*)typeIHDR, 4);
	safe_Bytes.append((const ubyte_t*)&imageHeader, sizeof(::gpk::SPNGIHDR));
	crc																= ::gpk::get_crc({&safe_Bytes[crcDataStart], safe_Bytes.size() - crcDataStart});
	be2le_32(crc);
	safe_Bytes.append((const ubyte_t*)&crc, 4);

	// Reverse RGB byte order
	::gpk::SImage<::gpk::color_rgba<uint8_t>>							convertedScanlines						= {};
	convertedScanlines.resize(in_imageView.metrics());
	for(uint32_t y = 0; y < in_imageView.metrics().y; ++y)
	for(uint32_t x = 0; x < in_imageView.metrics().x; ++x) {
		::gpk::SColorBGRA													colorSrc								= in_imageView		[y][x];
		::gpk::color_rgba<uint8_t>											& colorDst								= convertedScanlines[y][x];
		colorDst.r														= colorSrc.r;
		colorDst.g														= colorSrc.g;
		colorDst.b														= colorSrc.b;
		colorDst.a														= colorSrc.a;
	}
	::gpk::apod<ubyte_t>											deflated;
	::gpk::SImage<uint8_t>											filtered								= {};
	filtered.resize({convertedScanlines.View.metrics().x * 4 + 1, convertedScanlines.View.metrics().y});
	const uint32_t														scanlineWidthUnfiltered					= convertedScanlines.View.metrics().x * 4;
	for(uint32_t y = 0; y < in_imageView.metrics().y; ++y) {
		filtered[y][0]													= 0;
		memcpy(&filtered[y][1], &convertedScanlines[y][0], scanlineWidthUnfiltered);
	}
	gpk_necall(deflated.resize(filtered.Texels.size() * 2 + 65535), "%s", "Out of memory?");
	gpk_necall(::pngDeflate({(const ubyte_t*)filtered.Texels.begin(), filtered.Texels.size()}, deflated), "%s", "Failed to compress! Out of memory?");

	static constexpr const char											typeIDAT	[4]							= {'I', 'D', 'A', 'T'};
	chunkSize														= deflated.size();
	be2le_32(chunkSize);
	safe_Bytes.append((const ubyte_t*)&chunkSize, 4);
	crcDataStart													= safe_Bytes.size();
	safe_Bytes.append((const ubyte_t*)typeIDAT, 4);
	safe_Bytes.append((const ubyte_t*)deflated.begin(), deflated.size());
	crc																= ::gpk::get_crc({&safe_Bytes[crcDataStart], safe_Bytes.size() - crcDataStart});
	be2le_32(crc);
	safe_Bytes.append((const ubyte_t*)&crc, 4);

	chunkSize														= 0;
	crc																= 0;
	static constexpr const char											typeIEND	[4]							= {'I', 'E', 'N', 'D'};
	be2le_32(chunkSize);
	safe_Bytes.append((const ubyte_t*)&chunkSize, 4);
	crcDataStart													= safe_Bytes.size();
	safe_Bytes.append((const ubyte_t*)typeIEND, 4);
	crc																= ::gpk::get_crc({&safe_Bytes[crcDataStart], safe_Bytes.size() - crcDataStart});
	be2le_32(crc);
	safe_Bytes.append((const ubyte_t*)&crc, 4);

	int32_t																oldSize									= out_Bytes.size();
	gpk_necall(out_Bytes.resize(oldSize + safe_Bytes.size()), "%s", "Out of memory?");
	memcpy(&out_Bytes[oldSize], safe_Bytes.begin(), safe_Bytes.size());
	return 0;
}
