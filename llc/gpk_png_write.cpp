#include "gpk_png.h"
#include "gpk_img_color.h"
#include "gpk_deflate.h"

static	unsigned long	g_crc_table[256]		= {};	// Table of CRCs of all 8-bit messages.
static	int32_t			g_crc_table_computed	= 0;	// Flag: has the table been computed? Initially false.
/* Make the table for a fast CRC. */
static	int32_t			make_crc_table			() {
    uint32_t					n, k;
    for (n = 0; n < 256; ++n) {
		uint32_t					c										= n;
		for (k = 0; k < 8; ++k)
			c						= (c & 1) ? 0xedb88320L ^ (c >> 1) : c >> 1;
		g_crc_table[n]			= c;
    }
    return g_crc_table_computed	= 1;
}

// Update a running CRC with the bytes buf[0..len-1]--the CRC should be initialized to all 1's, and the transmitted value is the 1's complement of the final running CRC (see the crc() routine below)).
uint32_t				gpk::update_crc			(const ::gpk::vcu8 & buf, uint32_t crc)										{
    uint32_t					c						= crc;
    if(0 == g_crc_table_computed) {
		static const int32_t		initedTable				= make_crc_table();
		(void)initedTable;
		info_printf("Initialized PNG CRC table: %i.", initedTable);
	}

	for (uint32_t n = 0, count = buf.size(); n < count; ++n)
		c						= g_crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);

	return c;
}

::gpk::error_t			gpk::pngFileWrite		(const ::gpk::gc8bgra & in_imageView, ::gpk::au8 & out_Bytes)		{
	stacxpr	const uint8_t		signature	[8]			= {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
	::gpk::au8					safe_Bytes				= {};
	safe_Bytes.append(signature);

	uint32_t					chunkSize				= sizeof(::gpk::SPNGIHDR);
	stacxpr	const uint8_t		typeIHDR	[4]			= {'I', 'H', 'D', 'R'};
	int32_t						crc						= 0;
	::gpk::SPNGIHDR				imageHeader				= {};
	imageHeader.Size				= in_imageView.metrics();
	imageHeader.BitDepth			= 8;
	imageHeader.ColorType			= COLOR_TYPE_RGBA;
	imageHeader.MethodCompression	= 0;
	imageHeader.MethodFilter		= 0;
	imageHeader.MethodInterlace		= 0;

	be2le_32(imageHeader.Size.x);
	be2le_32(imageHeader.Size.y);
	be2le_32(chunkSize);
	safe_Bytes.append((const uint8_t*)&chunkSize, 4);
	uint32_t					crcDataStart			= safe_Bytes.size();
	safe_Bytes.append(typeIHDR);
	safe_Bytes.append((const uint8_t*)&imageHeader, sizeof(::gpk::SPNGIHDR));
	crc						= ::gpk::get_crc({&safe_Bytes[crcDataStart], safe_Bytes.size() - crcDataStart});
	be2le_32(crc);
	safe_Bytes.append((const uint8_t*)&crc, 4);

	// Reverse RGB byte order
	::gpk::img8rgba				convertedScanlines		= {};
	convertedScanlines.resize(in_imageView.metrics());
	for(uint32_t y = 0; y < in_imageView.metrics().y; ++y)
	for(uint32_t x = 0; x < in_imageView.metrics().x; ++x) {
		::gpk::bgra					colorSrc				= in_imageView		[y][x];
		::gpk::rgba					& colorDst				= convertedScanlines[y][x];
		colorDst.r				= colorSrc.r;
		colorDst.g				= colorSrc.g;
		colorDst.b				= colorSrc.b;
		colorDst.a				= colorSrc.a;
	}
	::gpk::imgu8					filtered				= {};
	filtered.resize(::gpk::n2u32{convertedScanlines.View.metrics().x * 4 + 1, convertedScanlines.View.metrics().y});
	const uint32_t					scanlineWidthUnfiltered	= convertedScanlines.View.metrics().x * 4;
	for(uint32_t y = 0; y < in_imageView.metrics().y; ++y) {
		filtered[y][0]				= 0;
		memcpy(&filtered[y][1], &convertedScanlines[y][0], scanlineWidthUnfiltered);
	}

	::gpk::au8						deflated;
	gpk_necall(::gpk::arrayDeflate(filtered.Texels.cu8(), deflated), "%s", "Failed to compress! Out of memory?");

	chunkSize					= deflated.size();
	be2le_32(chunkSize);
	safe_Bytes.append((const uint8_t*)&chunkSize, 4);
	crcDataStart				= safe_Bytes.size();

	stacxpr	const uint8_t			typeIDAT	[4]			= {'I', 'D', 'A', 'T'};
	safe_Bytes.append(typeIDAT);
	safe_Bytes.append(deflated);
	crc							= ::gpk::get_crc({&safe_Bytes[crcDataStart], safe_Bytes.size() - crcDataStart});
	be2le_32(crc);
	safe_Bytes.append((const uint8_t*)&crc, 4);

	chunkSize					= 0;
	crc							= 0;
	stacxpr	const uint8_t			typeIEND	[4]			= {'I', 'E', 'N', 'D'};
	be2le_32(chunkSize);
	safe_Bytes.append((const uint8_t*)&chunkSize, 4);
	crcDataStart				= safe_Bytes.size();
	safe_Bytes.append(typeIEND);
	crc							= ::gpk::get_crc({&safe_Bytes[crcDataStart], safe_Bytes.size() - crcDataStart});
	be2le_32(crc);
	safe_Bytes.append((const uint8_t*)&crc, 4);

	int32_t							oldSize					= out_Bytes.size();
	gpk_necs(out_Bytes.resize(oldSize + safe_Bytes.size()));
	memcpy(&out_Bytes[oldSize], safe_Bytes.begin(), safe_Bytes.size());
	return 0;
}
