/*
 * WARNING: USE AT YOUR OWN RISK
 *
 * This code demonstrates the use of undocumented Windows internals to trigger a system-level hard error.
 *
 * This may cause a BSOD, system crash, or shutdown.
 * This is intended for educational/research purposes only.
 * Do NOT run this on production systems or machines with unsaved work.
 *
 */

#include <Windows.h>
#include <winternl.h>
#include <iostream>

typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(
    NTSTATUS ErrorStatus,
    ULONG NumberOfParameters,
    ULONG UnicodeStringParameterMask,
    PULONG_PTR Parameters,
    ULONG ResponseOption,
    PULONG Response
);

typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(
    ULONG Privilege,
    BOOLEAN Enable,
    BOOLEAN CurrentThread,
    PBOOLEAN Enabled
);

int main() {
    BOOLEAN bEnabled;
    ULONG uResp;
    // Get function pointers
    pdef_RtlAdjustPrivilege RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)
        GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlAdjustPrivilege");
    pdef_NtRaiseHardError NtRaiseHardError = (pdef_NtRaiseHardError)
        GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtRaiseHardError");

    // Enable the shutdown privilege
    RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);

    // Trigger a hard error
    NtRaiseHardError(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);

    return 0;
}