#include "gpk_array_pod.h"

#ifndef SSIEGE_ID_H_23701
#define SSIEGE_ID_H_23701

namespace lls
{
#pragma pack(push, 1)
	GPK_USING_TYPEINT();
	tydf	u2_t							llsid_t;
	tydf	cnst ::lls::llsid_t			llsid_c;
	tydf	::gpk::apod<::lls::llsid_t>	allsid;
	tydf	::gpk::view<::lls::llsid_t>	vllsid;
	tydf	::gpk::view<llsid_c>			vllscid;

	stxp llsid_t	SSIEGEID_INVALID		= (llsid_t)-1;
#pragma pack(pop)
} // namespace

#endif // SSIEGE_ID_H_23701