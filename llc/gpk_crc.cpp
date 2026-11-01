#include "llc_crc.h"
#include "llc_noise.h"

LLC_USING_TYPEINT();

stxp u2_c			LLC_CRC_CRC_SEED			= 18973;
llc::err_t			llc::crcGenerate			(vcu0_t bytes, u3_t & crc)	{
	crc						= 0;
	u2_c				lastPos						= bytes.size() - 1;
	for(u2_t i=0; i < bytes.size(); ++i) {
		crc						+= noise1DBase(bytes[i], ::LLC_CRC_CRC_SEED);
		crc						+= noise1DBase(bytes[lastPos - i], ::LLC_CRC_CRC_SEED);
	}
	return 0;
}
llc::err_t			llc::crcGenerateAndAppend	(au0_t & bytes)	{
	u3_t					crcToStore					= 0;
	crcGenerate(bytes, crcToStore);
	if_fail_fe(bytes.append((const u0_t*)&crcToStore, szof(u3_t)));;
	return 0;
}
llc::err_t			llc::crcVerifyAndRemove		(au0_t & bytes)	{
	if_true_fef(bytes.size() < 8, "Invalid input. No CRC can be found in an array of %u bytes.", bytes.size());
	u3_t					check						= 0;
	u2_c				startOfCRC					= bytes.size() - 8;
	crcGenerate({bytes.begin(), startOfCRC}, check);
	const u3_t				found						= *(u3_t*)&bytes[startOfCRC];
	if_true_fef(check != found, "CRC Check failed: Stored: %llu. Calculated: %llu.", found, check);
	if_fail_fe(bytes.resize(bytes.size() - 8));
	return 0;
}
