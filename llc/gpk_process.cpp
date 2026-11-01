#include "gpk_process_internal.h"

#if defined(GPK_WINDOWS) || defined(GPK_LINUX)
namespace gpk {
tplt<tpnm T>
err_t SProcessManager<T>::Redirect(u2_t id) {
	err_t result;
	if_fail_vef(-1, result = Validate(id), "(%" GPK_FMT_S2 ")", result);
	SProcess & record = KnownProcesses[id];
	if_true_fe(record.State.Started);
	if(record.State.Redirected) rtrn 0;
	if_fail_vef(-1, result = process_detail::processRedirect(PlatformState[id]), "(%" GPK_FMT_S2 ")", result);
	record.State.Redirected = true;
	rtrn 0;
}

tplt<tpnm T>
err_t SProcessManager<T>::Launch(u2_t id) {
	err_t result;
	if_fail_vef(-1, result = Validate(id), "(%" GPK_FMT_S2 ")", result);
	SProcess & record = KnownProcesses[id];
	if_true_fe(record.State.Started);
	u2_t creationFlags = 0;
	::gpk::cpycst(creationFlags, record.CreationFlags);
	// Native launch may have succeeded before a parent-end cleanup failed.
	process_detail::SProcessNativeScope<T> rollback{PlatformState[id], &record.State};
	if_fail_vef(result, result = process_detail::processCreate(PlatformState[id], record.Application, record.CommandLine, record.Environment, creationFlags), "(%" GPK_FMT_S2 ")", result);
	record.State.Started = true;
	rollback.Inactive = true;
	rtrn 0;
}

tplt<tpnm T>
err_t SProcessManager<T>::Write(u2_t id, vcu0_t input) {
	err_t result;
	if_fail_vef(-1, result = Validate(id), "(%" GPK_FMT_S2 ")", result);
	if_true_fe(!KnownProcesses[id].State.Started || !KnownProcesses[id].State.Redirected);
	if_fail_vef(result, result = process_detail::processWrite(PlatformState[id], input), "(%" GPK_FMT_S2 ")", result);
	rtrn result;
}

tplt<tpnm T>
err_t SProcessManager<T>::Read(u2_t id, au0_t & output) {
	err_t result;
	if_fail_vef(-1, result = Validate(id), "(%" GPK_FMT_S2 ")", result);
	if_true_fe(!KnownProcesses[id].State.Started || !KnownProcesses[id].State.Redirected);
	if_fail_vef(result, result = process_detail::processRead(PlatformState[id], output), "(%" GPK_FMT_S2 ")", result);
	rtrn result;
}

tplt<tpnm T>
err_t SProcessManager<T>::Communicate(u2_t id, vcu0_t input, au0_t & output) {
	err_t result;
	if_fail_vef(-1, result = Validate(id), "(%" GPK_FMT_S2 ")", result);
	cnst auto & state = KnownProcesses[id].State;
	if_true_fe(!state.Started || !state.Redirected || state.Finished);
	if_fail_vef(result, result = process_detail::processCommunicate(PlatformState[id], input, output), "(%" GPK_FMT_S2 ")", result);
	rtrn result;
}

tplt<tpnm T>
err_t SProcessManager<T>::Wait(u2_t id, u2_t & exitCode) {
	err_t result;
	if_fail_vef(-1, result = Validate(id), "(%" GPK_FMT_S2 ")", result);
	SProcess & record = KnownProcesses[id];
	if_true_fe(!record.State.Started);
	if(!record.State.Finished) {
		if_fail_vef(-1, result = process_detail::processWait(PlatformState[id], record.ExitCode), "(%" GPK_FMT_S2 ")", result);
		record.State.Finished = true;
	}
	exitCode = record.ExitCode;
	rtrn 0;
}

tplt<tpnm T>
err_t SProcessManager<T>::Execute(u2_t id, vcu0_t input, au0_t & output, u2_t & exitCode) {
	err_t result;
	if_fail_vef(-1, result = Validate(id), "(%" GPK_FMT_S2 ")", result);
	if_true_fe(KnownProcesses[id].State.Started);
	if_fail_vef(-1, result = Redirect(id), "(%" GPK_FMT_S2 ")", result);
	if_fail_vef(-1, result = Launch(id), "(%" GPK_FMT_S2 ")", result);
	if_fail_vef(result, result = Communicate(id, input, output), "(%" GPK_FMT_S2 ")", result);
	if_fail_vef(result, result = Wait(id, exitCode), "(%" GPK_FMT_S2 ")", result);
	rtrn result;
}

tplt<tpnm T>
err_t SProcessManager<T>::Close(u2_t id) {
	err_t result;
	if_fail_vef(-1, result = Validate(id), "(%" GPK_FMT_S2 ")", result);
	if_fail_vef(result, result = process_detail::processClose(PlatformState[id]), "(%" GPK_FMT_S2 ")", result);
	// Keep the slot as a tombstone: an old id must never identify a later child.
	KnownProcesses[id] = {};
	KnownProcesses[id].State.Closed = true;
	rtrn result;
}

template struct SProcessManager<SProcessPlatform>;
}
#endif
