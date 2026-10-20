#include "gpk_png.h"
#include "gpk_img_color.h"
#include "gpk_deflate.h"

using gpk::u0_t, gpk::u2_t;

::gpk::err_t			gpk::pngFileWrite		(const ::gpk::gc8bgra & in_imageView, ::gpk::au0_t & out_Bytes)		{
	stacxpr	const u0_t		signature	[8]			= {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
	::gpk::au0_t				safe_Bytes				= {};
	safe_Bytes.append(signature);

	u2_t					chunkSize				= sizeof(::gpk::SPNGIHDR);
	stacxpr	const u0_t		typeIHDR	[4]			= {'I', 'H', 'D', 'R'};
	u2_t						crc						= 0;
	::gpk::SPNGIHDR				imageHeader				= {};
	imageHeader.Size				= in_imageView.metrics();
	imageHeader.BitDepth			= 8;
	imageHeader.ColorType			= COLOR_TYPE_RGBA;
	imageHeader.MethodCompression	= 0;
	imageHeader.MethodFilter		= 0;
	imageHeader.MethodInterlace		= 0;

	be2le(imageHeader.Size.x);
	be2le(imageHeader.Size.y);
	be2le(chunkSize);
	safe_Bytes.append((const u0_t*)&chunkSize, 4);
	u2_t					crcDataStart			= safe_Bytes.size();
	safe_Bytes.append(typeIHDR);
	safe_Bytes.append((const u0_t*)&imageHeader, sizeof(::gpk::SPNGIHDR));
	crc						= ::gpk::get_crc({&safe_Bytes[crcDataStart], safe_Bytes.size() - crcDataStart});
	be2le(crc);
	safe_Bytes.append((const u0_t*)&crc, 4);

	// Reverse RGB byte order
	::gpk::img8rgba				convertedScanlines		= {};
	convertedScanlines.resize(in_imageView.metrics());
	for(u2_t y = 0; y < in_imageView.metrics().y; ++y)
	for(u2_t x = 0; x < in_imageView.metrics().x; ++x) {
		::gpk::bgra					colorSrc				= in_imageView		[y][x];
		::gpk::rgba					& colorDst				= convertedScanlines[y][x];
		colorDst.r				= colorSrc.r;
		colorDst.g				= colorSrc.g;
		colorDst.b				= colorSrc.b;
		colorDst.a				= colorSrc.a;
	}
	::gpk::imgu0_t					filtered				= {};
	filtered.resize(::gpk::n2u2_t{convertedScanlines.View.metrics().x * 4 + 1, convertedScanlines.View.metrics().y});
	const u2_t					scanlineWidthUnfiltered	= convertedScanlines.View.metrics().x * 4;
	for(u2_t y = 0; y < in_imageView.metrics().y; ++y) {
		filtered[y][0]				= 0;
		memcpy(&filtered[y][1], &convertedScanlines[y][0], scanlineWidthUnfiltered);
	}

	::gpk::au0_t					deflated;
	gpk_necall(gpk::arrayDeflate(filtered.Texels.cu8(), deflated), "%s", "Failed to compress! Out of memory?");

	chunkSize					= deflated.size();
	be2le(chunkSize);
	safe_Bytes.append((const u0_t*)&chunkSize, 4);
	crcDataStart				= safe_Bytes.size();

	stacxpr	const u0_t			typeIDAT	[4]			= {'I', 'D', 'A', 'T'};
	safe_Bytes.append(typeIDAT);
	safe_Bytes.append(deflated);
	crc							= ::gpk::get_crc({&safe_Bytes[crcDataStart], safe_Bytes.size() - crcDataStart});
	be2le(crc);
	safe_Bytes.append((const u0_t*)&crc, 4);

	chunkSize					= 0;
	crc							= 0;
	stacxpr	const u0_t			typeIEND	[4]			= {'I', 'E', 'N', 'D'};
	be2le(chunkSize);
	safe_Bytes.append((const u0_t*)&chunkSize, 4);
	crcDataStart				= safe_Bytes.size();
	safe_Bytes.append(typeIEND);
	crc							= ::gpk::get_crc({&safe_Bytes[crcDataStart], safe_Bytes.size() - crcDataStart});
	be2le(crc);
	safe_Bytes.append((const u0_t*)&crc, 4);

	u2_t							oldSize					= out_Bytes.size();
	gpk_necs(out_Bytes.resize(oldSize + safe_Bytes.size()));
	memcpy(&out_Bytes[oldSize], safe_Bytes.begin(), safe_Bytes.size());
	return 0;
}


