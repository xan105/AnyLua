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

std::wstring GetLastErrorMessage() {
    DWORD code = ::GetLastError();
    LPWSTR buffer = nullptr;
    size_t size = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&buffer,
        0,
        NULL
    );
    std::wstring message(buffer, size);
    LocalFree(buffer);
    return L"Error " + std::to_wstring(code) + L": " + message;
}

void EnableConsole() {
    if (AllocConsole()) {
        HWND consoleWindow = GetConsoleWindow();
        if (consoleWindow) {
            LONG style = GetWindowLong(consoleWindow, GWL_EXSTYLE);
            SetWindowLong(consoleWindow, GWL_EXSTYLE, style | WS_EX_LAYERED);
            SetLayeredWindowAttributes(consoleWindow, 0, 225, LWA_COLORKEY);
            ShowWindow(consoleWindow, SW_SHOW);
        }
    }

    // Explicitly open console handles instead of relying on GetStdHandle
    HANDLE hOut = CreateFileW(L"CONOUT$", GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    HANDLE hIn = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

    if (hOut != INVALID_HANDLE_VALUE) {
        SetStdHandle(STD_OUTPUT_HANDLE, hOut);
        SetStdHandle(STD_ERROR_HANDLE, hOut);
        FILE* fDummy;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
    }
    if (hIn != INVALID_HANDLE_VALUE) {
        SetStdHandle(STD_INPUT_HANDLE, hIn);
        FILE* fDummy;
        freopen_s(&fDummy, "CONIN$", "r", stdin);
    }

    // Enable ANSI escape sequence processing
    DWORD mode = 0;
    if (GetConsoleMode(hOut, &mode)) {
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, mode);
    }
}