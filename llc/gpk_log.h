#include "gpk_log_level.h"

#ifndef GPK_LOG_H_23627
#define GPK_LOG_H_23627

#ifndef es_if
#	define es_if				e_if
#	define ws_if				w_if				
#	define is_if				i_if				
#	define break_gserror_if		be_if
#	define break_gswarn_if		bw_if
#	define break_gsinfo_if		bi_if
#	define break_gsverbose_if	bv_if
#	define break_gerror_if		bef_if
#	define break_gwarn_if		bwf_if
#	define break_ginfo_if		bif_if
#	define break_gverbose_if	bvf_if
#	define continue_gserror_if	ce_if		
#	define continue_gswarn_if	cw_if		
#	define continue_gsinfo_if	ci_if		
#	define retval_gsalways_if	va_if		
#	define retval_gserror_if	ve_if		
#	define retval_gswarn_if		vw_if		
#	define retval_gsinfo_if		vi_if		
#	define retval_gsverbose_if	vv_if		
#	define continue_gerror_if	cef_if		
#	define continue_gwarn_if	cwf_if		
#	define continue_ginfo_if	cif_if		
#	define retval_galways_if	vaf_if		
#	define retval_gerror_if		vef_if		
#	define retval_gwarn_if		vwf_if	
#	define retval_ginfo_if		vif_if	
#	define retval_gverbose_if	vvf_if		
#	define ret_gserror_if(condition)					if_true_log_and_return(error_printf, condition)
#	define ret_gswarn_if(condition)						if_true_log_and_return(warning_printf, condition)
#	define ret_gsinfo_if(condition)						if_true_log_and_return(info_printf, condition)
#	define ret_gerror_if(condition, format, ...)		if_true_logf_and_return(error_printf, condition, format, __VA_ARGS__)
#	define ret_gwarn_if(condition, format, ...)			if_true_logf_and_return(warning_printf, condition, format, __VA_ARGS__)
#	define ret_ginfo_if(condition, format, ...)			if_true_logf_and_return(info_printf, condition, format, __VA_ARGS__)


#	define retnul_gserror_if(condition)				ve_if(0, condition)
#	define retnul_gswarn_if(condition)				vw_if(0, condition)
#	define retnul_gsinfo_if(condition)				vi_if(0, condition)
#	define reterr_gserror_if(condition)				ve_if(-1, condition)
#	define reterr_gswarn_if(condition)				vw_if(-1, condition)
#	define reterr_gsinfo_if(condition)				vi_if(-1, condition)
#	define retwarn_gserror_if(condition)			ve_if(1, condition)
#	define retwarn_gswarn_if(condition)				vw_if(1, condition)
#	define retwarn_gsinfo_if(condition)				vi_if(1, condition)
#	define retnul_gerror_if(condition, format, ...)	vef_if(0, condition, format, __VA_ARGS__)
#	define retnul_gwarn_if(condition, format, ...)	vwf_if(0, condition, format, __VA_ARGS__)
#	define retnul_ginfo_if(condition, format, ...)	vif_if(0, condition, format, __VA_ARGS__)
#	define reterr_gerror_if(condition, format, ...)	vef_if(-1, condition, format, __VA_ARGS__)
#	define reterr_gwarn_if(condition, format, ...)	vwf_if(-1, condition, format, __VA_ARGS__)
#	define reterr_ginfo_if(condition, format, ...)	vif_if(-1, condition, format, __VA_ARGS__)
#	define retwarn_gerror_if(condition, format, ...)vef_if(1, condition, format, __VA_ARGS__)
#	define retwarn_gwarn_if(condition, format, ...)	vwf_if(1, condition, format, __VA_ARGS__)
#	define retwarn_ginfo_if(condition, format, ...)	vif_if(1, condition, format, __VA_ARGS__)
#endif

#ifndef re_if
#	define re_if							ret_gerror_if
#	define rw_if							ret_gwarn_if
#	define ri_if							ret_ginfo_if
#	define res_if							ret_gserror_if
#	define rws_if							ret_gswarn_if
#	define ris_if							ret_gsinfo_if
#endif


#ifndef bes_if
#	define bes_if							break_gserror_if
#	define bws_if							break_gswarn_if
#	define bis_if							break_gsinfo_if
#	define ces_if							continue_gserror_if
#	define cws_if							continue_gswarn_if
#	define cis_if							continue_gsinfo_if
#	define rva_if			vaf_if		
#	define rve_if			vef_if		
#	define rvw_if			vwf_if		
#	define rvi_if			vif_if		
#	define rvv_if			vvf_if		
#	define rva_if_failed	if_fail_vaf	
#	define rve_if_failed	if_fail_vef	
#	define rvw_if_failed	if_fail_vwf	
#	define rvi_if_failed	if_fail_vif	
#	define rvv_if_failed	if_fail_vvf	
#	define rvas_if			va_if
#	define rves_if			ve_if
#	define rvws_if			vw_if
#	define rvis_if			vi_if
#	define rvas_if_failed	if_fail_va
#	define rves_if_failed	if_fail_ve
#	define rvws_if_failed	if_fail_vw
#	define rvis_if_failed	if_fail_vi
#	define rne_if							retnul_gerror_if
#	define rnw_if							retnul_gwarn_if
#	define rni_if							retnul_ginfo_if
#	define rnes_if							retnul_gserror_if
#	define rnws_if							retnul_gswarn_if
#	define rnis_if							retnul_gsinfo_if
#	define ree_if							reterr_gerror_if
#	define rew_if							reterr_gwarn_if
#	define rei_if							reterr_ginfo_if
#	define rees_if							reterr_gserror_if
#	define rews_if							reterr_gswarn_if
#	define reis_if							reterr_gsinfo_if
#	define ree_if_failed(gpkl_call, ...)			if_fail_vef(-1, (gpkl_call), __VA_ARGS__)	
#	define rew_if_failed(gpkl_call, ...)			if_fail_vef(-1, (gpkl_call), __VA_ARGS__)	
#	define rei_if_failed(gpkl_call, ...)			if_fail_vef(-1, (gpkl_call), __VA_ARGS__)	
#	define rea_if_failed(gpkl_call, ...)			if_fail_vef(-1, (gpkl_call), __VA_ARGS__)	
#	define rees_if_failed(gpkl_call)				if_fail_ve(-1, (gpkl_call))
#	define rews_if_failed(gpkl_call)				if_fail_ve(-1, (gpkl_call))
#	define reis_if_failed(gpkl_call)				if_fail_ve(-1, (gpkl_call))
#	define reas_if_failed(gpkl_call)				if_fail_ve(-1, (gpkl_call))
#	define re_if_failed(condition, format, ...)	re_if (0 > gpk::error_t(condition), format, __VA_ARGS__)
#	define rw_if_failed(condition, format, ...)	rw_if (0 > gpk::error_t(condition), format, __VA_ARGS__)
#	define ri_if_failed(condition, format, ...)	ri_if (0 > gpk::error_t(condition), format, __VA_ARGS__)
#	define ra_if_failed(condition, format, ...)	ra_if (0 > gpk::error_t(condition), format, __VA_ARGS__)
#	define res_if_failed(condition)				res_if(0 > gpk::error_t(condition))
#	define rws_if_failed(condition)				rws_if(0 > gpk::error_t(condition))
#	define ris_if_failed(condition)				ris_if(0 > gpk::error_t(condition))
#	define ras_if_failed(condition)				ras_if(0 > gpk::error_t(condition))
#	define e_if_failed   if_fail_ef
#	define w_if_failed   if_fail_wf
#	define i_if_failed   if_fail_if
#	define a_if_failed   if_fail_af
#	define es_if_failed  if_fail_e
#	define ws_if_failed  if_fail_w
#	define is_if_failed  if_fail_i
#	define as_if_failed  if_fail_a
#	define ce_if_failed  if_fail_cef
#	define cw_if_failed  if_fail_cwf
#	define ci_if_failed  if_fail_cif
#	define ces_if_failed if_fail_ce
#	define cws_if_failed if_fail_cw
#	define cis_if_failed if_fail_ci
#	define be_if_failed  if_fail_bef
#	define bw_if_failed  if_fail_bwf
#	define bi_if_failed  if_fail_bif
#	define bes_if_failed if_fail_be
#	define bws_if_failed if_fail_bw
#	define bis_if_failed if_fail_bi
#	define ree_if_succeeded(condition, format, ...)	ree_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define rew_if_succeeded(condition, format, ...)	rew_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define rei_if_succeeded(condition, format, ...)	rei_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define rea_if_succeeded(condition, format, ...)	rea_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define rees_if_succeeded(condition)				rees_if	(0 <= gpk::error_t(condition))
#	define rews_if_succeeded(condition)				rews_if	(0 <= gpk::error_t(condition))
#	define reis_if_succeeded(condition)				reis_if	(0 <= gpk::error_t(condition))
#	define reas_if_succeeded(condition)				reas_if	(0 <= gpk::error_t(condition))
#	define re_if_succeeded(condition, format, ...)	re_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define rw_if_succeeded(condition, format, ...)	rw_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define ri_if_succeeded(condition, format, ...)	ri_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define ra_if_succeeded(condition, format, ...)	ra_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define res_if_succeeded(condition)				res_if	(0 <= gpk::error_t(condition))
#	define rws_if_succeeded(condition)				rws_if	(0 <= gpk::error_t(condition))
#	define ris_if_succeeded(condition)				ris_if	(0 <= gpk::error_t(condition))
#	define ras_if_succeeded(condition)				ras_if	(0 <= gpk::error_t(condition))
#	define e_if_succeeded(condition, format, ...)	ef_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define w_if_succeeded(condition, format, ...)	wf_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define i_if_succeeded(condition, format, ...)	if_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define a_if_succeeded(condition, format, ...)	af_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define es_if_succeeded(condition)				e_if	(0 <= gpk::error_t(condition))
#	define ws_if_succeeded(condition)				w_if	(0 <= gpk::error_t(condition))
#	define is_if_succeeded(condition)				i_if	(0 <= gpk::error_t(condition))
#	define as_if_succeeded(condition)				a_if	(0 <= gpk::error_t(condition))
#	define ce_if_succeeded(condition, format, ...)	ce_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define cw_if_succeeded(condition, format, ...)	cw_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define ci_if_succeeded(condition, format, ...)	ci_if	(0 <= gpk::error_t(condition), format, __VA_ARGS__)
#	define ces_if_succeeded(condition)				ces_if	(0 <= gpk::error_t(condition))
#	define cws_if_succeeded(condition)				cws_if	(0 <= gpk::error_t(condition))
#	define cis_if_succeeded(condition)				cis_if	(0 <= gpk::error_t(condition))
#	define rwe_if							retwarn_gerror_if
#	define rww_if							retwarn_gwarn_if
#	define rwi_if							retwarn_ginfo_if
#	define rwes_if							retwarn_gserror_if
#	define rwws_if							retwarn_gswarn_if
#	define rwis_if							retwarn_gsinfo_if
#endif

#endif // GPK_LOG_H_23627
