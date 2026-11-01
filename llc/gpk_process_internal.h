#include "gpk_process.h"

#ifndef GPK_PROCESS_INTERNAL_H
#define GPK_PROCESS_INTERNAL_H
namespace gpk::process_detail {

	err_t processClose(SProcessWin32 & process);
	err_t processRedirect(SProcessWin32 & process);
	err_t processCreate(SProcessWin32 & process, vcst_t application, vcst_t commandLine, vcs0_t environment, u2_t creationFlags);
	err_t processWrite(SProcessWin32 & process, vcu0_t input);
	err_t processRead(SProcessWin32 & process, au0_t & output);
	err_t processWait(SProcessWin32 & process, u2_t & exitCode);
	err_t processCommunicate(SProcessWin32 & process, vcu0_t input, au0_t & output);

	err_t processClose(SProcessLinux & process);
	err_t processRedirect(SProcessLinux & process);
	err_t processCreate(SProcessLinux & process, vcst_t application, vcst_t commandLine, vcs0_t environment, u2_t creationFlags);
	err_t processWrite(SProcessLinux & process, vcu0_t input);
	err_t processRead(SProcessLinux & process, au0_t & output);
	err_t processWait(SProcessLinux & process, u2_t & exitCode);
	err_t processCommunicate(SProcessLinux & process, vcu0_t input, au0_t & output);

	tplt<tpnm TProcess>
	stct SProcessNativeScope {
		TProcess		& Process;
		SProcessState	* State;
		bool			Inactive = {};
		SProcessNativeScope(TProcess & process, SProcessState * state = 0) nxpt : Process(process), State(state) {}
		SProcessNativeScope(cnst SProcessNativeScope &) = delete;
		SProcessNativeScope & oper=(cnst SProcessNativeScope &) = delete;
		~SProcessNativeScope() { if_fail_e(Close()); }
		err_t Close() {
			if(Inactive) rtrn 0;
			Inactive = true;
			cnst err_t result = processClose(Process);
			if(State) State->Redirected = false;
			rtrn result;
		}
	};
}
#endif

