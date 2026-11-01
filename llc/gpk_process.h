#include "gpk_array_obj.h"
#include "gpk_keyval.h"
#include "gpk_string.h"
#include "gpk_n2.h"

#ifndef GPK_PROCESS_H_23627
#define GPK_PROCESS_H_23627

namespace gpk
{
	tplt<tpnm TManager>
	stct SProcessScope {
		TManager		& Manager;
		u2_t			Id;
		bool			Inactive				= {};
		//
						~SProcessScope			()							{ if_fail_e(Close()); }
						SProcessScope			(TManager & manager, u2_t id) nxpt : Manager(manager), Id(id) {}
						SProcessScope			(cnst SProcessScope &)		= delete;
		SProcessScope & oper=					(cnst SProcessScope &)		= delete;
		//
		err_t			Close					() {
			if(Inactive)
				rtrn 0;
			Inactive		= true;
			rtrn Manager.Close(Id);
		}
	};

	typedef size_t					TWindowsHandle;
	typedef sc_t					* ntsc_t;
	stxp	cnst TWindowsHandle		INVALID_WINDOWS_HANDLE	 = (size_t)-1;
	stct SWindowsProcessInfo {
		TWindowsHandle		hProcess				= {};
		TWindowsHandle		hThread					= {};
		u2_t				Id						= {};
		u2_t				Thread					= {};
	};
	stct SWindowsStartupInfo {
		u2_t				SizeInBytes				= sizeof(SWindowsStartupInfo);
		ntsc_t				Reserved				= {};
		ntsc_t				Desktop					= {};
		ntsc_t				Title					= {};
		n2u2_t				WindowPosition			= {};
		n2u2_t				WindowSize				= {};
		n2u2_t				TextModeSize			= {};
		u2_t				TextModeFillAttribute	= {};
		u2_t				Flags					= {}; // ?? CreationFlags? ExecutionMode? What the fuck flags means? Why did't they call it "number" instead, or just plain "Bits". ... ... ... Retards.
		u1_t				ShowWindow				= {};
		u1_t				Reserved2ByteCount		= {};
		u0_t				* Reserved2				= {};
		TWindowsHandle		HandleStdInput			= {};
		TWindowsHandle		HandleStdOutput			= {};
		TWindowsHandle		HandleStdError			= {};
	};
	stct SProcessCreationFlags {
		u2_t				DebugProcess				: 1;
		u2_t				DebugOnlyThisProcess		: 1;
		u2_t				Suspended					: 1;
		u2_t				DetachedProcess				: 1;
		u2_t				NewConsole					: 1;
		u2_t				NormalPriority				: 1;
		u2_t				IdlePriority				: 1;
		u2_t				HighPriority				: 1;
		u2_t				RealtimePriority			: 1;
		u2_t				NewProcessGroup				: 1;
		u2_t				UnicodeEnvironment			: 1;
		u2_t				SeparateWOWVDM				: 1;
		u2_t				SharedWOWVDM				: 1;
		u2_t				ForceDOS					: 1;
		u2_t				BelowNormalPriority			: 1;
		u2_t				AboveNormalPriority			: 1;
		u2_t				InheritParentAffinity		: 1;
		u2_t				InheritCallerPriority		: 1;
		u2_t				ProtectedProcess			: 1;
		u2_t				ExtendedStartupInfoPresent	: 1;
		u2_t				BackgroundBegin				: 1;
		u2_t				BackgroundEnd				: 1;
		u2_t				SecureProcess				: 1;
		u2_t				Reserved23					: 1;
		u2_t				BreakawayFromJob			: 1;
		u2_t				PreserveCodeAuthzLevel		: 1;
		u2_t				DefaultErrorMode			: 1;
		u2_t				NoWindow					: 1;
		u2_t				ProfileUser					: 1;
		u2_t				ProfileKernel				: 1;
		u2_t				ProfileServer				: 1;
		u2_t				IgnoreSystemDefault			: 1;
	};
#ifdef GPK_WINDOWS
	static_assert(sizeof(SWindowsProcessInfo  ) == sizeof(PROCESS_INFORMATION));
	static_assert(sizeof(SWindowsStartupInfo  ) == sizeof(STARTUPINFOA));
	static_assert(sizeof(SProcessCreationFlags) == sizeof(DWORD));
#endif

#pragma pack(push, 1)
	stct SProcessWin32 {
		SWindowsProcessInfo		Info				= {};
		SWindowsStartupInfo		Startup				= {sizeof(SWindowsStartupInfo)};
		TWindowsHandle			StdinWrite			= INVALID_WINDOWS_HANDLE;
		TWindowsHandle			StdoutRead			= INVALID_WINDOWS_HANDLE;
	};
#pragma pack(pop)
	stct SProcessLinux {
		s2_t					PID					= -1;
		s2_t					StdinRead			= -1;
		s2_t					StdinWrite			= -1;
		s2_t					StdoutRead			= -1;
		s2_t					StdoutWrite			= -1;
	};

	stct SProcessState {
		b8_t					Started				: 1;
		b8_t					Finished			: 1;
		b8_t					Redirected			: 1;
		b8_t					Closed				: 1;
	};
	stct SProcess {
		au0_t					Output;
		SProcessState			State				= {};
		SProcessCreationFlags	CreationFlags		= {};
		u2_t					ExitCode			= {};
		string					Application			= {};
		string					CommandLine			= {};
		as0_t					Environment			= {};
	};

	tydf 
#ifdef GPK_WINDOWS
		SProcessWin32
#else
		SProcessLinux 
#endif
			SProcessPlatform;
	// IDs are local to this manager, never reused, and invalid after Close(). Calls on one manager must be serialized; Execute joins its own pipe worker.
	tplt<tpnm _t = SProcessPlatform>
	stct SProcessManager {
		tydf			_t					T, TProcess;
		tydf			SProcessScope<SProcessManager>	TProcessScope;

		apod<T>			PlatformState		= {};
		aobj<SProcess>	KnownProcesses		= {};

		stct SCreateScope {
			SProcessManager & Manager;
			u2_t			Id;
			bool			Inactive = {};
			SCreateScope(SProcessManager & manager, u2_t id) nxpt : Manager(manager), Id(id) {}
			SCreateScope(cnst SCreateScope &) = delete;
			SCreateScope & oper=(cnst SCreateScope &) = delete;
			~SCreateScope() { if_fail_e(Close()); }
			err_t Close() {
				if(Inactive) rtrn 0;
				Inactive = true;
				cnst err_t removed = Manager.KnownProcesses.size() > Id ? Manager.KnownProcesses.pop_back() : 0;
				cnst err_t platform = Manager.PlatformState.pop_back();
				rtrn failed(removed) ? removed : failed(platform) ? platform : 0;
			}
		};

						~SProcessManager	() {
			for(u2_t id = 0; id < KnownProcesses.size(); ++id) {
				if(false == KnownProcesses[id].State.Closed)
					if_fail_e(Close(id));
			}
		}
						 SProcessManager	() = default;
						 SProcessManager	(cnst SProcessManager &) = delete;
		SProcessManager & oper=				(cnst SProcessManager &) = delete;

		err_t			Validate			(u2_t id) cnst {
			if_true_fef(id >= KnownProcesses.size() || id >= PlatformState.size(), "Invalid process id: %u.", id);
			if_true_fef(KnownProcesses[id].State.Closed, "Closed process id: %u.", id);
			rtrn 0;
		}

		// Create retains the arguments and returns an ID; Launch starts the child.
		err_t			Create				(vcst_t application, vcst_t commandLine, vcs0_t environment = {}, u2_t creationFlags = 0) {
			if_true_fe(KnownProcesses.size() >= 0x7FFFFFFFU || KnownProcesses.size() != PlatformState.size());
			if_true_fef(application.size() && memchr(application.begin(), 0, application.size()), "%s", "Application contains an embedded zero.");
			if_true_fef(commandLine.size() && memchr(commandLine.begin(), 0, commandLine.size()), "%s", "Command line contains an embedded zero.");
			if_true_fef(environment.size() && environment[environment.size() - 1], "%s", "Unterminated process environment block.");
			cnst u2_t id = KnownProcesses.size();
			err_t result;
			if_fail_vef(-1, result = PlatformState.push_back({}), "(%" GPK_FMT_S2 ")", result);
			SCreateScope rollback{*this, id};
			if_fail_vef(result, result = KnownProcesses.emplace_back(), "(%" GPK_FMT_S2 ")", result);
			SProcess & record = KnownProcesses[id];
			if_fail_vef(result, result = record.Application.append(application.begin(), application.size()), "(%" GPK_FMT_S2 ")", result);
			if_fail_vef(result, result = record.CommandLine.append(commandLine.begin(), commandLine.size()), "(%" GPK_FMT_S2 ")", result);
			if_fail_vef(result, result = record.Environment.append(environment.begin(), environment.size()), "(%" GPK_FMT_S2 ")", result);
			// Windows requires two terminators even for an explicitly empty environment.
			if(environment.size()) {
				if_fail_vef(result, result = record.Environment.push_back(0), "(%" GPK_FMT_S2 ")", result);
			}
			::gpk::cpycst(record.CreationFlags, creationFlags);
			rollback.Inactive = true;
			rtrn (err_t)id;
		}
		err_t			Execute				(u2_t id) {
			if_fail_fe(Validate(id));
			rtrn Execute(id, {}, KnownProcesses[id].Output, KnownProcesses[id].ExitCode);
		}
		err_t			Execute				(u2_t processId, vcu0_t input, au0_t & output, u2_t & exitCode);
		err_t			Launch				(u2_t processId);
		err_t			Redirect			(u2_t processId);

		err_t			Write				(u2_t processId, vcu0_t input);
		err_t			Read				(u2_t processId, au0_t & output);
		// Already launched/redirected child: close stdin while draining output; does not launch or reap.
		err_t			Communicate			(u2_t processId, vcu0_t input, au0_t & output);

		err_t			Wait				(u2_t processId, u2_t & exitCode);// Linux: signal termination is 128 + signal; wait consumes the child id.
		err_t			Close				(u2_t processId); // Releases the record; does not kill the child.

		err_t			Execute				(vcst_t application, vcst_t commandLine, vcu0_t input, au0_t & output, u2_t & exitCode, vcs0_t environment) {
			err_t				id;
			if_fail_vef(-1, id = Create(application, commandLine, environment), "(%" GPK_FMT_S2 ")", id);
			TProcessScope scope{*this, (u2_t)id};
			err_t				result, closed;
			result = Execute((u2_t)id, input, output, exitCode);
			if_true_ef(result, "(%" GPK_FMT_S2 ")", result);
			if_fail_vef(-3, closed = scope.Close(), "(%" GPK_FMT_S2 ")", closed);
			if(result) rtrn -2;
			rtrn 0;
		}
		err_t			Launch				(vcst_t application, vcst_t commandLine, vcs0_t environment) {
			err_t				id;
			if_fail_vef(-1, id = Create(application, commandLine, environment), "(%" GPK_FMT_S2 ")", id);
			TProcessScope scope{*this, (u2_t)id};
			err_t				result, closed;
			result = Launch((u2_t)id);
			if_true_ef(result, "(%" GPK_FMT_S2 ")", result);
			if_fail_vef(-3, closed = scope.Close(), "(%" GPK_FMT_S2 ")", closed);
			if(result) rtrn -2;
			rtrn 0;
		}
	};

	tydf SProcessManager<SProcessWin32> SProcessManagerWin32;
	tydf SProcessManager<SProcessLinux> SProcessManagerLinux;
	//
	//err_t processExecute(vcst_t application, vcst_t commandLine, vcu0_t input, au0_t & output, u2_t & exitCode, vcs0_t environment = {});
	//err_t processLaunch(vcst_t application, vcst_t commandLine, vcs0_t environment = {});
} // namespace

#endif // GPK_PROCESS_H_23627
