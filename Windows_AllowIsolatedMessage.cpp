#include "Windows_AllowIsolatedMessage.hpp"

/* Windows AllowIsolatedMessage 
// Windows_AllowIsolatedMessage.cpp
//
// Author: Jan Ringos, http://Tringi.MX-3.cz, jan@ringos.cz
// Version: 0.1
//
// Changelog:
//      11.04.2013 - initial version
*/

#include "Windows_Symbol.hpp"

#ifndef MSGFLT_ADD
#define MSGFLT_ADD                              1
#define MSGFLT_REMOVE                           2
#endif
#ifndef MSGFLT_ALLOW
#define MSGFLT_RESET                            0
#define MSGFLT_ALLOW                            1
#define MSGFLT_DISALLOW                         2
#endif

namespace {
    bool initialized = false;
    BOOL (WINAPI * pfnChangeWindowMessageFilter) (UINT, DWORD) = NULL;
    BOOL (WINAPI * pfnChangeWindowMessageFilterEx) (HWND, UINT, DWORD, LPVOID) = NULL;
};

bool Windows::AllowIsolatedMessage (HWND hWnd, UINT message, bool allow) {
    if (!initialized) {
        initialized = true;
        Windows::Symbol (L"USER32", pfnChangeWindowMessageFilter, "ChangeWindowMessageFilter");
        Windows::Symbol (L"USER32", pfnChangeWindowMessageFilterEx, "ChangeWindowMessageFilterEx");
    };
    
    // ChangeWindowMessageFilterEx is available on Windows7+
    
    if (hWnd && pfnChangeWindowMessageFilterEx)
        return pfnChangeWindowMessageFilterEx (hWnd, message,
                                               allow ? MSGFLT_ALLOW : MSGFLT_DISALLOW, NULL);
    
    // global op is requested or hwnd-local function is missing, set globally
    //  - except when attempting to disallow for specific window
    //    (which would incorrectly disallow the message for whole process)
    
    if (pfnChangeWindowMessageFilter && !(hWnd && !allow))
        return pfnChangeWindowMessageFilter (message, allow ? MSGFLT_ADD : MSGFLT_REMOVE);
    
    // if neither of ChangeWindowMessageFilter(Ex) is available then messages
    // are always allowed, and isolation is not supported on this system
    
    if (!allow)
        SetLastError (ERROR_NOT_SUPPORTED);
    
    return allow;
};

