#include <windows.h>
#include <winhttp.h>
#include <iostream>

#pragma comment(lib, "winhttp.lib")

void OpenHttpsConnection(const wchar_t* domain) {
    // Specify the target as HTTPS.
    const int port = INTERNET_DEFAULT_HTTPS_PORT;

    // Initialize WinHTTP session.
    HINTERNET hSession = WinHttpOpen(L"Karma-Harness",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::wcerr << L"WinHttpOpen failed with error: " << GetLastError() << std::endl;
        return;
    }

    // Create a connection handle.
    HINTERNET hConnect = WinHttpConnect(hSession, domain, port, 0);
    if (!hConnect) {
        std::wcerr << L"WinHttpConnect failed with error: " << GetLastError() << std::endl;
        WinHttpCloseHandle(hSession);
        return;
    }

    std::wcout << L"Opened HTTPS connection to " << domain << std::endl;
}
