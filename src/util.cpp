/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "util.h"

std::wstring Getenv(LPCWSTR name) {
    std::wstring buffer(65535, L'\0');
    DWORD size = GetEnvironmentVariableW(name, &buffer[0], static_cast<DWORD>(buffer.size()));

    if (size) {
        buffer.resize(size);
        return buffer;
    }
    else {
        return L"";
    }
}

std::wstring GetCurrentProcessDir() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);

    std::wstring path(buffer);
    std::wstring::size_type pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        path = path.substr(0, pos + 1);  // Keep trailing slash for easy path concat
    }
    return path;
}

std::wstring GetCurrentProcessName() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);

    std::wstring path(buffer);
    std::wstring::size_type pos = path.find_last_of(L"\\");
    if (pos != std::wstring::npos) {
        path = path.substr(pos + 1);
    }
    return path;
}

std::wstring toWString(const std::string& s) {
    int size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.length(), nullptr, 0);
    std::wstring buf(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.length(), &buf[0], size);
    return buf;
}

int toInt(size_t val) { //64bits
    return (val <= INT_MAX) ? (int)((size_t)val) : 0;
}

std::string toString(const std::wstring& s) {
    int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, s.c_str(), toInt(s.length()), NULL, 0, NULL, NULL);
    std::string buf = std::string(size, '\0');
    WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, s.c_str(), toInt(s.length()), &buf[0], size, NULL, NULL);
    return buf;
}

void enableConsole() {

    if (AllocConsole()) {
        HWND consoleWindow = GetConsoleWindow();

        if (consoleWindow) {
            // Set the console window to be layered
            LONG style = GetWindowLong(consoleWindow, GWL_EXSTYLE);
            SetWindowLong(consoleWindow, GWL_EXSTYLE, style | WS_EX_LAYERED);
            SetLayeredWindowAttributes(consoleWindow, 0, 225, LWA_COLORKEY);

            // Show the console window
            ShowWindow(consoleWindow, SW_SHOW);
        }

        // Redirect stdio to the console
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONIN$", "r", stdin);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
    }
}