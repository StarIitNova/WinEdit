#include <iostream>
#include <string>
#include <unordered_map>

#include <windows.h>
#include <tlhelp32.h>

HWND findWindow(DWORD processID) {
    struct HandleData {
        DWORD processID;
        HWND hwnd;
    };

    HandleData data = {processID, nullptr};

    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        HandleData& data = *(HandleData*)lParam;
        DWORD windowProcessID;
        GetWindowThreadProcessId(hWnd, &windowProcessID);
        if (windowProcessID == data.processID && GetWindow(hWnd, GW_OWNER) == nullptr && IsWindowVisible(hWnd)) {
            data.hwnd = hWnd;
            return FALSE;  // Stop enumeration once the main window is found
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&data));

    return data.hwnd;
}

DWORD getProcIdByName(const std::string &procName) {
    DWORD procId = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry)) {
        do {
            if (procName == entry.szExeFile) {
                procId = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return procId;
}

std::unordered_map<std::string, DWORD> getStyleMap() {
    return {
        {"WS_EX_ACCEPTFILES", WS_EX_ACCEPTFILES},
        {"WS_EX_APPWINDOW", WS_EX_APPWINDOW},
        {"WS_EX_CLIENTEDGE", WS_EX_CLIENTEDGE},
        {"WS_EX_COMPOSITED", WS_EX_COMPOSITED},
        {"WS_EX_CONTEXTHELP", WS_EX_CONTEXTHELP},
        {"WS_EX_CONTROLPARENT", WS_EX_CONTROLPARENT},
        {"WS_EX_DLGMODALFRAME", WS_EX_DLGMODALFRAME},
        {"WS_EX_LAYERED", WS_EX_LAYERED},
        {"WS_EX_LAYOUTRTL", WS_EX_LAYOUTRTL},
        {"WS_EX_LEFT", WS_EX_LEFT},
        {"WS_EX_LEFTSCROLLBAR", WS_EX_LEFTSCROLLBAR},
        {"WS_EX_LTRREADING", WS_EX_LTRREADING},
        {"WS_EX_MDICHILD", WS_EX_MDICHILD},
        {"WS_EX_NOACTIVATE", WS_EX_NOACTIVATE},
        {"WS_EX_NOINHERITLAYOUT", WS_EX_NOINHERITLAYOUT},
        {"WS_EX_NOPARENTNOTIFY", WS_EX_NOPARENTNOTIFY},
        {"WS_EX_NOREDIRECTIONBITMAP", WS_EX_NOREDIRECTIONBITMAP},
        {"WS_EX_RIGHT", WS_EX_RIGHT},
        {"WS_EX_RIGHTSCROLLBAR", WS_EX_RIGHTSCROLLBAR},
        {"WS_EX_RTLREADING", WS_EX_RTLREADING},
        {"WS_EX_STATICEDGE", WS_EX_STATICEDGE},
        {"WS_EX_TOOLWINDOW", WS_EX_TOOLWINDOW},
        {"WS_EX_TOPMOST", WS_EX_TOPMOST},
        {"WS_EX_TRANSPARENT", WS_EX_TRANSPARENT},
        {"WS_EX_WINDOWEDGE", WS_EX_WINDOWEDGE}
    };
}

void modifyWindowStyle(HWND hwnd, const std::string& flag, bool remove = false) {
    if (!hwnd) {
        std::cerr << "Window parameter passed does not exist (Did you type the executable's name properly?)\n";
        return;
    }

    auto styleMap = getStyleMap();
    if (styleMap.find(flag.substr(1)) == styleMap.end()) {
        std::cerr << "Flag is not supported\n";
        return;
    }

    DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

    if (flag[0] == '+') {
        exStyle |= styleMap[flag.substr(1)];
    } else if (flag[0] == '-') {
        exStyle &= ~styleMap[flag.substr(1)];
    }

    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: WinEdit process.exe +FLAG ...\n";
        return 1;
    }

    std::string processName = argv[1];
    DWORD processID = getProcIdByName(processName);
    if (!processID) {
        std::cerr << "Process not found: " << processName << "\n";
        return 1;
    }

    HWND hwnd = findWindow(processID);
    for (int i = 2; i < argc; ++i) {
        modifyWindowStyle(hwnd, argv[i]);
    }

    std::cout << "Updated window styles successfully!\n";
    return 0;
}
