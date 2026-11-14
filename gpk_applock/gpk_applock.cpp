#include "gpk_runtime.h"
#include "gpk_string_compose.h"

GPK_USING_TYPEINT();

stxp gpk::vcst_c	ROOT			= "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\";
stxp gpk::vcst_c	BLOCKED[]		= { "node.exe", "node-repl.exe", "node_repl.exe", "nodejs.exe", "python.exe", "python3.exe", "pythonw.exe", "python3w.exe", "py.exe", "pyw.exe", "pip.exe", "pip3.exe"};

sttc gpk::err_t         uninstallKeys   (gpk::vcst_t keyPath, gpk::vcst_t debugger) {
        HKEY                                    key                             = {};
    LSTATUS                                     result;
        if_true_ef((result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &key)) && result != ERROR_FILE_NOT_FOUND, "0x%X", result);
        if_true_if(ERROR_FILE_NOT_FOUND == result, "keyPath:\"%s\" dbugger:\"%s\" doesn't exist", keyPath.begin(), debugger.begin());

        gpk::astsc_t<4096>              existing                = {};
        DWORD                                   bytes                   = existing.size();
        result                          = RegGetValue(key, 0, "Debugger", RRF_RT_REG_SZ, 0, &existing[0], &bytes);
        if(ERROR_SUCCESS == result && debugger == gpk::vcst_t{&existing[0], (u2_t)-1})
                result                          = RegDeleteValue(key, "Debugger");
        RegCloseKey(key);
        if_true_ef(result && result != ERROR_FILE_NOT_FOUND, "0x%X", result);
        return 0;
}
sttc gpk::err_t         installKeys             (gpk::vcst_t keyPath, gpk::vcst_t debugger) {
    HKEY                                        key                             = {};
        gpk::astsc_t<4096>              existing                = {};
        DWORD                                   bytes                   = existing.size();
    LSTATUS                                     result;
        if_true_ef(result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, &keyPath[0], 0, 0, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, 0, &key, 0), "0x%X-0x%X", result, GetLastError());
    result                              = RegGetValue(key, 0, "Debugger", RRF_RT_REG_SZ, 0, &existing[0], &bytes);
        //if(ERROR_SUCCESS == result && debugger != gpk::vcst_t{&existing[0], (u2_t)-1}) {
        //        RegCloseKey(key);
        //        error_printf("Refusing to replace the existing IFEO Debugger for %s.", keyPath.begin());
        //        return ERROR_ALREADY_EXISTS;
        //}
        if_true_ef(result && result != ERROR_FILE_NOT_FOUND, "0x%X", result);
        result                          = RegSetValueEx(key, "Debugger", 0, REG_SZ, debugger.bytes().begin(), debugger.size() + 1);
        RegCloseKey(key);
        if_true_ef(result, "RegSetValueEx() --> 0x%X", result);
        return 0;
}
sttc gpk::err_t configure(bool installLocker) {
        always_printf("%s Gatewatch's App Locker", installLocker ? "installing" : "uninstalling");

        gpk::sstring<4096>      executable = {};
    if_zero_ef(GetModuleFileName(nullptr, &executable[0], executable.size() - 1), "executable:\"%.*s\"->(%i)", GPK_VIEW_ARGS(executable), GetLastError());

    gpk::string                 keyPath, debugger;
    for (const auto name : BLOCKED) {
                keyPath = debugger = {};
                if_fail_e(gpk::append_strings(keyPath, ROOT, name));
                if_fail_e(gpk::append_strings(debugger, '"', executable, "\" --blocked"));

        if_fail_e(installLocker ? installKeys(keyPath, debugger) : uninstallKeys(keyPath, debugger));
    }
    return 0;
}
//
int WINAPI app_blocker_entry_point(gpk::SRuntimeValues & runtimeValues) {
        if_true_fef(-1 != gpk::argsOptionIndex(runtimeValues.EntryPointArgs, "blocked"), "%s", "This runtime is prohibited by the computer's security policy.");

        if(-1 != gpk::argsOptionIndex(runtimeValues.EntryPointArgs, "install"))
        return configure(true);

        if(-1 != gpk::argsOptionIndex(runtimeValues.EntryPointArgs, "uninstall"))
        return configure(false);

    error_printf("-- Gatewatch Runtime Guard -- Run as administrator with --install or --uninstall.");
    return -1;
}

GPK_OS_ENTRY_POINT(app_blocker_entry_point);
