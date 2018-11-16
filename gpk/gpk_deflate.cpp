#include "gpk_deflate.h"
#include "deflate.h"


		::gpk::error_t									gpk::arrayDeflate								(const ::gpk::view_array<const byte_t>	& inflated, ::gpk::array_pod<byte_t>& deflated)	{
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
    error_if(strm.avail_in != 0, "%s", "Not all of the input bytes were consumed.");	/* all input will be used */
    error_if(ret != Z_STREAM_END && ret != Z_OK, "%s", "Unknown error");				/* stream will be complete */
	deflated.resize((uint32_t)((ptrdiff_t)strm.next_out - (ptrdiff_t)deflated.begin()));
    /* clean up and return */
	info_printf("deflateEnd: %u.", (uint32_t)ret);
	return 0;
}

		::gpk::error_t									gpk::arrayInflate								(const ::gpk::view_array<byte_t> & deflated, ::gpk::array_pod<byte_t>& inflated)	{
	z_stream																	strm											= {};
	int																			ret												= inflateInit(&strm);	 // allocate inflate state 
	if (ret != Z_OK)
		return ret;

	strm.avail_in															= (uint32_t)deflated.size(); 
	strm.avail_out															= (uint32_t)inflated.size();
	strm.next_in															= (Bytef *)deflated.begin();
	strm.next_out															= (Bytef *)inflated.begin();
	ret																		= ::inflate(&strm, Z_NO_FLUSH);
	ree_if(ret == Z_STREAM_ERROR, "%s", "ZIP Error");  // state not clobbered 
	switch (ret) {
	case Z_NEED_DICT		:
		ret																			= Z_DATA_ERROR;     // and fall through 
	case Z_VERSION_ERROR	:
	case Z_STREAM_ERROR		:
	case Z_DATA_ERROR		:
	case Z_MEM_ERROR		:
		(void)inflateEnd(&strm);
		return -1;
	}
	if(ret != Z_STREAM_END && ret != Z_OK) {
		(void)inflateEnd(&strm);
		error_printf("%s", "Failed to decompress?");
		return -1;
	}
	inflated.resize((uint32_t)((ptrdiff_t)strm.next_out - (ptrdiff_t)inflated.begin()));
	ret																			= ::inflateEnd(&strm);
	info_printf("inflateEnd: %u.", (uint32_t)ret);
	return 0;
}
