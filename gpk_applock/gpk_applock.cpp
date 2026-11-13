#include "gpk_runtime.h"
#include "gpk_string_compose.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();

using gpk::aobj;

struct auto_hkey : public gpk::auto_handler<HKEY, nullptr> { using TWrapper::auto_handler; ~auto_hkey() nxpt { if(Handle) RegCloseKey(Handle); } };
struct SAppLockEntry {
	vcst_t		Name;
	vcst_t		Path;
};
struct SAppLockTable {
	vcst_c				ROOT			= "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\";
	aobj<SAppLockEntry>	BLOCKED			=
		{ {"node.exe", ""}, {"node-repl.exe", ""}, {"node_repl.exe", ""}, {"nodejs.exe", ""}, {"python.exe", ""}, {"python3.exe", ""}, {"pythonw.exe", ""}, {"python3w.exe", ""}, {"py.exe", ""}, {"pyw.exe", ""}, {"pip.exe", ""}, {"pip3.exe"}
		, {"MSBuild.exe", "C:\\Program Files\\Microsoft Visual Studio\\18\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe"}
		};
	vcst_c				FILTER_PATH		= "\\Gatewatch";
};

static  SAppLockTable appLock;
static const auto configureStatus = [](LSTATUS result, b8_t error) -> LSTATUS {
	if_true_vef(result, error, "%u", result);
	if_true_vif(0, result, "%u", result);
	return 0;
};

sttc LSTATUS	configureValue			(b8_t install, HKEY key, vcst_t name, DWORD type, const void * data, DWORD bytes) {
	LSTATUS result = install
		? RegSetValueEx(key, name.begin(), 0, type, (u0_c*)data, bytes)
		: RegDeleteValue(key, name.begin());
	return configureStatus(result, result && (install || ERROR_FILE_NOT_FOUND != result));
}
sttc gpk::err_t		configureFilter	(b8_t install, gpk::vcst_t keyPath, gpk::vcst_t filterKeyPath) {
	DWORD useFilter = 1;
	LSTATUS result = install
			? RegSetKeyValue(HKEY_LOCAL_MACHINE, keyPath.begin(), "UseFilter", REG_DWORD, &useFilter, sizeof(useFilter))
			: RegDeleteKey(HKEY_LOCAL_MACHINE, filterKeyPath.begin());
	if_fail_fe(configureStatus(result, result && (install || ERROR_FILE_NOT_FOUND != result)));
	if(false == install && (ERROR_SUCCESS == result || ERROR_FILE_NOT_FOUND == result)) {
		result = RegDeleteKeyValue(HKEY_LOCAL_MACHINE, keyPath.begin(), "UseFilter");
		if_fail_fe(configureStatus(result, result && ERROR_FILE_NOT_FOUND != result));
	}
	return 0;
}
sttc gpk::err_t		configureKey	(HKEY & key, b8_t install, vcst_t filterKeyPath, vcst_t debugger) {
	LSTATUS result = install
			? RegCreateKeyEx(HKEY_LOCAL_MACHINE, filterKeyPath.begin(), 0, 0, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, 0, &key, 0)
			: RegOpenKeyEx(HKEY_LOCAL_MACHINE, filterKeyPath.begin(), 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &key);
	if_true_vif(result, ERROR_FILE_NOT_FOUND == result, "keyPath:\"%s\" debugger:\"%s\" doesn't exist", filterKeyPath.begin(), debugger.begin());
	return configureStatus(result, result);
}
sttc gpk::err_t		configureLock	(b8_t install, vcst_t keyPath, vcst_t filterPath, vcst_t debugger) {
	gpk::string				filterKeyPath;
	if_fail_fe(gpk::append_strings(filterKeyPath, keyPath, filterPath.size() ? appLock.FILTER_PATH : vcst_t{}));
	if(0 == filterPath.size()) {
		gpk::string			staleFilterKeyPath;
		if_fail_fe(gpk::append_strings(staleFilterKeyPath, keyPath, appLock.FILTER_PATH));
		if_fail_fe(configureFilter(false, keyPath, staleFilterKeyPath));
	}
	auto_hkey				key;
	gpk::astsc_t<4096>		existing		= {};
	DWORD					bytes			= existing.size();
	LRESULT result;
	if_fail_fe(configureKey(key.Handle, install, filterKeyPath, debugger));
	if_true_fe((result = RegGetValue(key, 0, "Debugger", RRF_RT_REG_SZ, 0, existing.Storage, &bytes)) && ERROR_FILE_NOT_FOUND != result);
	if(false == install && ERROR_FILE_NOT_FOUND == result)
		return 0;
	if_fail_fe(configureValue(install, key, "Debugger", REG_SZ, debugger.begin(), debugger.size() + 1));
	if(filterPath.size())
		if_fail_fe(configureValue(install, key, "FilterFullPath", REG_SZ, filterPath.begin(), filterPath.size() + 1));
	if(filterPath.size())
		if_fail_fe(configureFilter(install, keyPath, filterKeyPath));
	return 0;
}
sttc gpk::err_t configure(bool installLocker) {
	always_printf("%s Gatewatch's App Locker", installLocker ? "installing" : "uninstalling");

	gpk::astsc_t<4096>	executable = {};
    if_zero_fef(GetModuleFileName(nullptr, &executable[0], executable.size() - 1), "executable:\"%.*s\"->(%i)", GPK_VIEW_ARGS(executable), GetLastError());

    gpk::string	keyPath, debugger;
	if_fail_fe(gpk::append_strings(debugger, '"', executable.Storage, "\" --blocked"));
    for (const SAppLockEntry & entry : appLock.BLOCKED) {
		if_fail_fe(gpk::append_strings(keyPath = {}, appLock.ROOT, entry.Name));
		if_fail_fe(configureLock(installLocker, keyPath, entry.Path, debugger));
    }
    return 0;
}
//
int WINAPI app_blocker_entry_point(gpk::SRuntimeValues & runtimeValues) {
	if_true_fef(-1 != gpk::argsOptionIndex(runtimeValues.EntryPointArgs, "blocked"), "%s", "This runtime is prohibited by the computer's security policy.")
	else if(-1 != gpk::argsOptionIndex(runtimeValues.EntryPointArgs, "install"))
        return configure(true);
	else if(-1 != gpk::argsOptionIndex(runtimeValues.EntryPointArgs, "uninstall"))
        return configure(false);

    error_printf("-- Gatewatch Runtime Guard -- Run as administrator with --install or --uninstall.");
    return -1;
}

GPK_OS_ENTRY_POINT(app_blocker_entry_point);
